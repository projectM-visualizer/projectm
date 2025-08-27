#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <projectM-opengl.h>

#include <Renderer/RenderItem.hpp>

namespace libprojectM {
namespace MilkdropPreset {


class VideoEcho : public Renderer::RenderItem
{
public:
    VideoEcho() = delete;
    explicit VideoEcho(const PresetState& presetState);

    void InitVertexAttrib() override;

    void Draw();

private:
    void DrawVideoEcho();

    void DrawGammaAdjustment();

    const PresetState& m_presetState;

    std::array<std::array<float, 3>, 4> m_shade;
    std::array<TexturedPoint, 4> m_vertices;
    Renderer::Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_LINEAR};
};

}
}
