/**
 * @file PresetTransition.cpp
 * @brief Implements Milkdrop-style preset transitions using a dual-FBO pipeline.
 *
 * Transition architecture:
 * - MilkdropPreset owns a ping-pong framebuffer pair (m_framebuffer[0/1]).
 * - Each preset renders into its own FBO; the final composite texture is sampled
 *   during the transition.
 * - PresetTransition binds the old preset to iChannel0 (unit 0) and the new
 *   preset to iChannel1 (unit 1), then draws a full-screen quad with a GLSL
 *   transition shader.
 * - Progress is passed via durationParams:
 *   .x = linear progress (raw)
 *   .y = cosine progress (legacy, backward-compatible)
 *   .z = bicubic progress (legacy, backward-compatible)
 *   .w = easing type selector (0=linear, 1=smoothstep, 2=ease-in, 3=ease-out)
 * - Shaders use iProgressEased, which applies the selected easing curve to the
 *   raw linear progress in GLSL.
 * - Aspect ratio uniforms (iAspectX/Y, iInvAspectX/Y) ensure geometry-sensitive
 *   effects look correct at non-square resolutions.
 * - High-quality linear filtering and optional anisotropic filtering are applied
 *   to both transition textures for smooth, Milkdrop-quality results.
 */

#include "Renderer/PresetTransition.hpp"

#include "Renderer/TextureManager.hpp"

#include <Logging.hpp>

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace Renderer {

constexpr double PI = 3.14159265358979323846;

PresetTransition::PresetTransition(const std::shared_ptr<Shader>& transitionShader, double durationSeconds, double transitionStartTime)
    : m_mesh(VertexBufferUsage::StaticDraw)
    , m_transitionShader(transitionShader)
    , m_durationSeconds(durationSeconds)
    , m_transitionStartTime(transitionStartTime)
{
    m_mesh.SetRenderPrimitiveType(Mesh::PrimitiveType::TriangleStrip);

    m_mesh.Vertices().Set({{-1.0f, 1.0f},
                           {1.0f, 1.0f},
                           {-1.0f, -1.0f},
                           {1.0f, -1.0f}});

    m_mesh.Indices().Set({0, 1, 2, 3});

    m_mesh.Update();

    std::mt19937 rand32(m_randomDevice());
    m_staticRandomValues = {rand32(), rand32(), rand32(), rand32()};

    // Pick a random easing curve for this transition (default smoothstep).
    m_easingType = static_cast<EasingType>(rand32() % static_cast<int>(EasingType::Count));
}

auto PresetTransition::IsDone(double currentFrameTime) const -> bool
{
    const auto secondsSinceStart = currentFrameTime - m_transitionStartTime;
    return m_durationSeconds <= 0.0 || secondsSinceStart >= m_durationSeconds;
}

auto PresetTransition::Progress(double currentFrameTime) const -> double
{
    return std::min(std::max((currentFrameTime - m_transitionStartTime) / m_durationSeconds, 0.0), 1.0);
}

void PresetTransition::SetPassCount(int passCount)
{
    m_passCount = std::max(1, std::min(passCount, 2));
}

auto PresetTransition::PassCount() const -> int
{
    return m_passCount;
}

void PresetTransition::BeginPass(int passNumber, int viewportWidth, int viewportHeight)
{
    if (passNumber < 0 || passNumber >= m_passCount)
    {
        return;
    }

    m_currentPass = passNumber;

    if (m_passCount > 1)
    {
        if (passNumber == 0)
        {
            if (!m_passFramebuffer)
            {
                m_passFramebuffer = std::make_shared<Framebuffer>(1);
                m_passFramebuffer->CreateColorAttachment(0, 0);
            }
            m_passFramebuffer->SetSize(viewportWidth, viewportHeight);
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_originalDrawFbo);
            m_passFramebuffer->Bind(0);
        }
        else if (passNumber == 1)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(m_originalDrawFbo));
        }
    }

    glViewport(0, 0, viewportWidth, viewportHeight);
    m_transitionShader->SetUniformInt("iPass", passNumber);
}

void PresetTransition::EndPass()
{
    if (m_currentPass == 0 && m_passCount > 1)
    {
        // Restore the original draw framebuffer so pass 1 (and subsequent code)
        // can safely sample the intermediate texture.
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(m_originalDrawFbo));
    }

    m_currentPass = -1;
}

auto PresetTransition::GetCurrentPass() const -> int
{
    return m_currentPass;
}

auto PresetTransition::GetPassTexture(int passNumber) const -> std::shared_ptr<class Texture>
{
    if (passNumber == 0 && m_passFramebuffer)
    {
        return m_passFramebuffer->GetColorAttachmentTexture(0, 0);
    }
    return nullptr;
}

void PresetTransition::SetBlendMode(TransitionBlendMode mode)
{
    m_blendMode = mode;
}

auto PresetTransition::GetBlendMode() const -> TransitionBlendMode
{
    return m_blendMode;
}

/**
 * @brief Renders the transition blend between two presets.
 *
 * Dual-FBO flow:
 * - The old preset's final composite is bound to texture unit 0 as iChannel0.
 * - The new preset's final composite is bound to texture unit 1 as iChannel1.
 * - Noise textures (if available) are bound starting at unit 2.
 * - A full-screen triangle strip is drawn with the transition shader.
 *
 * If either preset texture is missing (e.g. during rapid preset changes),
 * the transition is forced to a zero-duration hard cut to avoid crashing.
 */
void PresetTransition::Draw(const Preset& oldPreset,
                            const Preset& newPreset,
                            const RenderContext& context,
                            const libprojectM::Audio::FrameAudioData& audioData,
                            double currentFrameTime)
{
    if (m_transitionShader == nullptr)
    {
        // Shader failed to compile — force an instant hard-cut so the transition
        // completes on the next IsDone() check instead of stalling.
        m_durationSeconds = 0.0;
        return;
    }

    // Validate preset output textures before binding.
    auto oldTex = oldPreset.OutputTexture();
    auto newTex = newPreset.OutputTexture();
    if (!oldTex || !newTex)
    {
        LOG_WARN("PresetTransition: missing output texture, forcing hard cut.");
        m_durationSeconds = 0.0;
        return;
    }

    std::mt19937 rand32(m_randomDevice());

    // Calculate progress values
    const auto secondsSinceStart = currentFrameTime - m_transitionStartTime;

    // If duration is zero, all progress values snap to 1.0 (transition complete).
    double linearProgress{1.0};
    double cosineProgress{1.0};
    double bicubicProgress{1.0};

    if (m_durationSeconds > 0.0)
    {
        linearProgress = secondsSinceStart / m_durationSeconds;
        // Pre-computed easing curves for backward compatibility with custom shaders
        // that may still reference iProgressCosine or iProgressBicubic.
        cosineProgress = (-std::cos(linearProgress * PI) + 1.0) * 0.5;
        bicubicProgress = linearProgress < 0.5 ? 4.0 * linearProgress * linearProgress * linearProgress : 1.0 - pow(-2.0 * linearProgress + 2.0, 3.0) / 2.0;
    }

    m_transitionShader->Bind();

    // Numerical parameters
    m_transitionShader->SetUniformFloat3("iResolution", {static_cast<float>(context.viewportSizeX),
                                                         static_cast<float>(context.viewportSizeY),
                                                         0.0f});

    m_transitionShader->SetUniformFloat4("durationParams", {linearProgress,
                                                            cosineProgress,
                                                            bicubicProgress,
                                                            static_cast<float>(m_easingType)});

    m_transitionShader->SetUniformFloat2("timeParams", {secondsSinceStart,
                                                        currentFrameTime - m_lastFrameTime});

    m_transitionShader->SetUniformInt4("iRandStatic", m_staticRandomValues);

    m_transitionShader->SetUniformInt4("iRandFrame", {rand32(),
                                                      rand32(),
                                                      rand32(),
                                                      rand32()});

    m_transitionShader->SetUniformFloat3("iBeatValues", {audioData.bass,
                                                         audioData.mid,
                                                         audioData.treb});

    m_transitionShader->SetUniformFloat3("iBeatAttValues", {audioData.bassAtt,
                                                            audioData.midAtt,
                                                            audioData.trebAtt});

    // Aspect ratio correction uniforms for geometry-sensitive transitions.
    m_transitionShader->SetUniformFloat("iAspectX",    context.aspectX);
    m_transitionShader->SetUniformFloat("iAspectY",    context.aspectY);
    m_transitionShader->SetUniformFloat("iInvAspectX", context.invAspectX);
    m_transitionShader->SetUniformFloat("iInvAspectY", context.invAspectY);

    m_transitionShader->SetUniformInt("iBlendMode", static_cast<int>(m_blendMode));

    // Texture samplers
    // Explicitly activate each texture unit before binding to ensure correct
    // state on all drivers, including Emscripten/WebGL where implicit state
    // assumptions can cause issues.
    //
    // High-quality linear filtering is set on both transition textures.
    // This makes wipes, curls, and other effects look much smoother,
    // matching the quality of classic Milkdrop transitions.
    glActiveTexture(GL_TEXTURE0);
    m_transitionShader->SetUniformInt("iChannel0", 0);
    oldTex->Bind(0, m_presetSampler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    m_transitionShader->SetUniformInt("iChannel1", 1);
    newTex->Bind(1, m_presetSampler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Optional anisotropic filtering on desktop for even sharper transitions.
    // Safe to skip on Emscripten/WebGL where the extension may not be present.
#ifdef GL_EXT_texture_filter_anisotropic
    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    if (maxAniso > 1.0f)
    {
        glActiveTexture(GL_TEXTURE0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        glActiveTexture(GL_TEXTURE1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    }
#endif

    int textureUnit = 2;
    std::vector<TextureSamplerDescriptor> noiseDescriptors(m_noiseTextureNames.size());
    for (const auto& noiseTextureName : m_noiseTextureNames)
    {
        noiseDescriptors[textureUnit - 2] = context.textureManager->GetTexture(noiseTextureName);
        noiseDescriptors[textureUnit - 2].Bind(textureUnit, *m_transitionShader);
        textureUnit++;
    }

    // ------------------------------------------------------------------
    // Render the transition quad (single-pass or multi-pass)
    // ------------------------------------------------------------------
    if (m_passCount > 1)
    {
        // === Multi-pass rendering (2 passes) ===
        // Pass 0 renders to an intermediate FBO.
        // Pass 1 samples the intermediate result via iLastPassTex.

        BeginPass(0, context.viewportSizeX, context.viewportSizeY);
        m_mesh.Draw();
        EndPass();

        BeginPass(1, context.viewportSizeX, context.viewportSizeY);

        auto pass0Tex = GetPassTexture(0);
        if (pass0Tex)
        {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            m_transitionShader->SetUniformInt("iLastPassTex", textureUnit);
            pass0Tex->Bind(textureUnit, m_presetSampler);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        m_mesh.Draw();

        // Unbind pass texture if it was bound.
        if (pass0Tex)
        {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            pass0Tex->Unbind(textureUnit);
        }
    }
    else
    {
        // Single-pass (default): set iPass to 0 so single-pass shaders work unchanged.
        BeginPass(0, context.viewportSizeX, context.viewportSizeY);
        m_mesh.Draw();
        EndPass();
    }

    // Clean up — explicitly unbind each texture unit to leave the driver in a
    // predictable state. This prevents texture leakage during rapid preset
    // switches, which is especially important on Emscripten/WebGL.
    glActiveTexture(GL_TEXTURE0);
    oldTex->Unbind(0);

    glActiveTexture(GL_TEXTURE1);
    newTex->Unbind(1);

    for (int i = 2; i < textureUnit; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        noiseDescriptors[i - 2].Unbind(i);
    }

    glActiveTexture(GL_TEXTURE0);

    Mesh::Unbind();
    Shader::Unbind();

    // Update last frame time.
    m_lastFrameTime = currentFrameTime;
}

} // namespace Renderer
} // namespace libprojectM
