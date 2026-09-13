#include "Renderer/PresetTransition.hpp"

#include "Renderer/TextureManager.hpp"

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
    , m_frameRandomEngine(m_randomDevice()) // Seed once at instantiation
{
    m_mesh.SetRenderPrimitiveType(Mesh::PrimitiveType::TriangleStrip);

    m_mesh.Vertices().Set({{-1.0f, 1.0f},
                           {1.0f, 1.0f},
                           {-1.0f, -1.0f},
                           {1.0f, -1.0f}});

    m_mesh.Indices().Set({0, 1, 2, 3});

    m_mesh.Update();

    m_staticRandomValues = {static_cast<int>(m_frameRandomEngine()),
                            static_cast<int>(m_frameRandomEngine()),
                            static_cast<int>(m_frameRandomEngine()),
                            static_cast<int>(m_frameRandomEngine())};
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

void PresetTransition::Draw(const Preset& oldPreset,
                            const Preset& newPreset,
                            const RenderContext& context,
                            const libprojectM::Audio::FrameAudioData& audioData,
                            double currentFrameTime)
{
    if (m_transitionShader == nullptr)
    {
        return;
    }

    // Calculate progress values
    const auto secondsSinceStart = currentFrameTime - m_transitionStartTime;

    // If duration is zero,
    double linearProgress{1.0};
    double cosineProgress{1.0};
    double bicubicProgress{1.0};

    if (m_durationSeconds > 0.0)
    {
        linearProgress = secondsSinceStart / m_durationSeconds;
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
                                                            m_durationSeconds});

    m_transitionShader->SetUniformFloat2("timeParams", {secondsSinceStart,
                                                        currentFrameTime - m_lastFrameTime});

    m_transitionShader->SetUniformInt4("iRandStatic", m_staticRandomValues);

    m_transitionShader->SetUniformInt4("iRandFrame", {static_cast<int>(m_frameRandomEngine()),
                                                      static_cast<int>(m_frameRandomEngine()),
                                                      static_cast<int>(m_frameRandomEngine()),
                                                      static_cast<int>(m_frameRandomEngine())});

    m_transitionShader->SetUniformFloat3("iBeatValues", {audioData.bass,
                                                         audioData.mid,
                                                         audioData.treb});

    m_transitionShader->SetUniformFloat3("iBeatAttValues", {audioData.bassAtt,
                                                            audioData.midAtt,
                                                            audioData.trebAtt});

    // Texture samplers
    m_transitionShader->SetUniformInt("iChannel0", 0);
    oldPreset.OutputTexture()->Bind(0, m_presetSampler);
    m_transitionShader->SetUniformInt("iChannel1", 1);
    newPreset.OutputTexture()->Bind(1, m_presetSampler);

    if (m_cachedNoiseDescriptors.empty() && context.textureManager != nullptr)
    {
        m_cachedNoiseDescriptors.reserve(m_noiseTextureNames.size());
        for (const auto& noiseTextureName : m_noiseTextureNames)
        {
            m_cachedNoiseDescriptors.push_back(context.textureManager->GetTexture(noiseTextureName));
        }
    }

    int textureUnit = 2;
    for (auto& descriptor : m_cachedNoiseDescriptors)
    {
        descriptor.Bind(textureUnit, *m_transitionShader);
        textureUnit++;
    }

    // Render the transition quad
    m_mesh.Draw();

    // Clean up
    oldPreset.OutputTexture()->Unbind(0);
    newPreset.OutputTexture()->Unbind(1);

    for (size_t i = 0; i < m_cachedNoiseDescriptors.size(); ++i)
    {
        m_cachedNoiseDescriptors[i].Unbind(static_cast<GLint>(i + 2));
    }

    Mesh::Unbind();
    Shader::Unbind();

    // Update last frame time.
    m_lastFrameTime = currentFrameTime;
}

} // namespace Renderer
} // namespace libprojectM
