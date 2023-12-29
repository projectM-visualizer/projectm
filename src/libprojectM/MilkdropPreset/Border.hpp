#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "Renderer/RenderItem.hpp"

namespace libprojectM {
namespace MilkdropPreset {


/**
 * @brief Renders a border around the screen.
 */
class Border : public Renderer::RenderItem
{
public:
    Border() = delete;

    explicit Border(PresetState& presetState);

    void InitVertexAttrib() override;

    /**
     * Draws the border.
     * @param presetPerFrameContext The per-frame context variables.
     */
    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    PresetState& m_presetState; //!< The global preset state.
};

} // namespace MilkdropPreset
} // namespace libprojectM
