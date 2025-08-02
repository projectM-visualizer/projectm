#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <projectM-opengl.h>

#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Renders a video "echo" (ghost image) effect and gamma adjustments.
 */
class VideoEcho : public libprojectM::Renderer::Backend::OpenGL::OpenGLRenderItem
{
public:
    VideoEcho() = delete;
    explicit VideoEcho(const PresetState& presetState);

    void InitVertexAttrib() override;

    void Draw();

private:
    void DrawVideoEcho();

    void DrawGammaAdjustment();

    const PresetState& m_presetState; //!< The global preset state.

    std::array<std::array<float, 3>, 4> m_shade; // !< Random, changing color values for the four corners
    std::array<TexturedPoint, 4> m_vertices; //!< The video echo/gamma adj mesh
    Renderer::Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_LINEAR};
};

} // namespace MilkdropPreset
} // namespace libprojectM
