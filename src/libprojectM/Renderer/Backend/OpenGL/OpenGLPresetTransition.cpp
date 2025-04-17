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
    // This would be the OpenGL-specific draw logic, similar to the commented code in PresetTransition.cpp
    // For now, you can move the OpenGL draw code here from PresetTransition.cpp if needed.
}

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM
