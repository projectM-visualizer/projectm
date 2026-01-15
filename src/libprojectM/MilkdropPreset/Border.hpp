#pragma once

#include "PresetState.hpp"

#include <Renderer/Mesh.hpp>

namespace libprojectM {
namespace MilkdropPreset {


/**
 * @brief Renders a border around the screen.
 */
class Border
{
public:
    Border() = delete;

    explicit Border(PresetState& presetState);

    /**
     * Draws the border.
     * @param presetPerFrameContext The per-frame context variables.
     */
    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    PresetState& m_presetState; //!< The global preset state.
    Renderer::Mesh m_borderMesh; //!< The border geometry.
};

} // namespace MilkdropPreset
} // namespace libprojectM
