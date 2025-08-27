#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "Renderer/RenderItem.hpp"

namespace libprojectM {
namespace MilkdropPreset {



class Border : public Renderer::RenderItem
{
public:
    Border() = delete;

    explicit Border(PresetState& presetState);

    void InitVertexAttrib() override;


    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    PresetState& m_presetState;
};

}
}
