#include "OpenGLPresetTransition.hpp"

namespace libprojectM {
namespace Renderer {
namespace Backend {
namespace OpenGL {

OpenGLPresetTransition::OpenGLPresetTransition(const std::shared_ptr<Shader>& transitionShader,
                                               double durationSeconds,
                                               double transitionStartTime)
    : PresetTransition(transitionShader, durationSeconds, transitionStartTime)
{
    Init();
}

void OpenGLPresetTransition::InitVertexAttrib()
{
    static const std::array<RenderItem::Point, 4> points{{{-1.0f, 1.0f},
                                                          {1.0f, 1.0f},
                                                          {-1.0f, -1.0f},
                                                          {1.0f, -1.0f}}};

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), reinterpret_cast<void*>(offsetof(Point, x))); // Position
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);
}

void OpenGLPresetTransition::Draw(const Preset& oldPreset,
                                  const Preset& newPreset,
                                  const RenderContext& context,
                                  const libprojectM::Audio::FrameAudioData& audioData,
                                  double currentFrameTime)
{
    if (m_transitionShader == nullptr)
    {
        return;
    }

    std::mt19937 rand32(m_randomDevice);

    constexpr double PI = 3.14159265358979323846;
    const auto secondsSinceStart = currentFrameTime - m_transitionStartTime;

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

    m_transitionShader->SetUniformInt("iChannel0", 0);
    oldPreset.OutputTexture()->Bind(0, m_presetSampler);
    m_transitionShader->SetUniformInt("iChannel1", 1);
    newPreset.OutputTexture()->Bind(1, m_presetSampler);

    int textureUnit = 2;
    std::vector<Renderer::TextureSamplerDescriptor> noiseDescriptors(m_noiseTextureNames.size());
    for (const auto& noiseTextureName : m_noiseTextureNames)
    {
        noiseDescriptors[textureUnit - 2] = context.textureManager->GetTexture(noiseTextureName);
        noiseDescriptors[textureUnit - 2].Bind(textureUnit, *m_transitionShader);
        textureUnit++;
    }

    // Render the transition quad
    InitVertexAttrib();
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Clean up
    oldPreset.OutputTexture()->Unbind(0);
    newPreset.OutputTexture()->Unbind(1);

    for (int i = 2; i < textureUnit; i++)
    {
        noiseDescriptors[i - 2].Unbind(textureUnit);
    }

    Renderer::Shader::Unbind();

    m_lastFrameTime = currentFrameTime;
}

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM
