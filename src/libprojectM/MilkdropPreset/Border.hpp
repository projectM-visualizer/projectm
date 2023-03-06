#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "Renderer/RenderItem.hpp"

/**
 * @brief Renders a border around the screen.
 */
class Border : public RenderItem
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
