#pragma once

#include "PresetState.hpp"

#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Darkens the screen center a bit on each frame.
 */
class DarkenCenter : public libprojectM::Renderer::Backend::OpenGL::OpenGLRenderItem
{
public:
    DarkenCenter() = delete;

    explicit DarkenCenter(PresetState& presetState);

    void InitVertexAttrib() override;

    /**
     * Applies the darkening area.
     */
    void Draw();

private:
    PresetState& m_presetState; //!< The global preset state.
    Renderer::Mesh m_mesh; //!< The "diamond" mesh.
    float m_aspectY{}; //!< Previous Y aspect ratio.
};

} // namespace MilkdropPreset
} // namespace libprojectM
