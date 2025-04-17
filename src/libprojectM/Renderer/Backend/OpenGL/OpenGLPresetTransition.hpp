#pragma once

#include <Renderer/PresetTransition.hpp>
#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

namespace libprojectM {
namespace Renderer {
namespace Backend {
namespace OpenGL {

/**
 * @brief OpenGL backend implementation of PresetTransition.
 */
class OpenGLPresetTransition : public PresetTransition, public OpenGLRenderItem
{
public:
    OpenGLPresetTransition(const std::shared_ptr<Shader>& transitionShader,
                           double durationSeconds,
                           double transitionStartTime);

    void InitVertexAttrib() override;

    void Draw(const Preset& oldPreset,
              const Preset& newPreset,
              const RenderContext& context,
              const libprojectM::Audio::FrameAudioData& audioData,
              double currentFrameTime);

private:
    // Any OpenGL-specific members if needed
};

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM
