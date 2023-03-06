#pragma once

#include "PresetState.hpp"

#include <Renderer/RenderItem.hpp>

class Filters : public RenderItem
{
public:
    Filters() = delete;
    explicit Filters(PresetState& presetState);

    void InitVertexAttrib();

    /**
     * @brief Brightens the image
     */
    void Brighten();

    void Darken();

    void Invert();

    void Solarize();

private:
    PresetState& m_presetState; //!< The global preset state.
};
