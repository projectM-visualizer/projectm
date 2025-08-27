#pragma once

#include "PresetState.hpp"

#include <Renderer/RenderItem.hpp>

namespace libprojectM {
namespace MilkdropPreset {


class DarkenCenter : public Renderer::RenderItem
{
public:
    DarkenCenter() = delete;

    explicit DarkenCenter(PresetState& presetState);

    void InitVertexAttrib();


    void Draw();

private:
    PresetState& m_presetState;
    float m_aspectY{};
};

}
}
