#pragma once

#include "PresetState.hpp"

#include "Renderer/RenderItem.hpp"

/**
 * @brief Darkens the screen center a bit on each frame.
 */
class DarkenCenter : public RenderItem
{
public:
    DarkenCenter() = delete;

    explicit DarkenCenter(PresetState& presetState);

    void InitVertexAttrib();

    /**
     * Applies the darkening area.
     */
    void Draw();

private:
    PresetState& m_presetState; //!< The global preset state.
    float m_aspectY{}; //!< Previous Y aspect ration.
};
