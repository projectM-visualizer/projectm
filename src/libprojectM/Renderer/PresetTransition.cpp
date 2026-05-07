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

    // Render the transition quad
    m_mesh.Draw();

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
