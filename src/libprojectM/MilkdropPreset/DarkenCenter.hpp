#pragma once

#include "PresetState.hpp"

#include <Renderer/Mesh.hpp>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Darkens the screen center a bit on each frame.
 */
class DarkenCenter
{
public:
    DarkenCenter() = delete;

    explicit DarkenCenter(PresetState& presetState);

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
