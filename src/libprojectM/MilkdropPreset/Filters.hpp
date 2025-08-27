#pragma once

#include "PresetState.hpp"

#include <Renderer/RenderItem.hpp>

namespace libprojectM {
namespace MilkdropPreset {


class Filters : public Renderer::RenderItem
{
public:
    Filters() = delete;
    explicit Filters(const PresetState& presetState);

    void InitVertexAttrib();


    void Draw();

private:
    void UpdateMesh();


    void Brighten();


    void Darken();


    void Solarize();


    void Invert();

    const PresetState& m_presetState;

    int m_viewportWidth{};
    int m_viewportHeight{};
};

}
}
