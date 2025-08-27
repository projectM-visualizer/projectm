#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <Renderer/RenderItem.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {


class MotionVectors : public Renderer::RenderItem
{
public:
    MotionVectors() = delete;

    explicit MotionVectors(PresetState& presetState);

    void InitVertexAttrib();


    void Draw(const PerFrameContext& presetPerFrameContext, std::shared_ptr<Renderer::Texture> motionTexture);

private:
    struct MotionVectorVertex {
        float x{};
        float y{};
        int32_t index{};
    };

    std::shared_ptr<Renderer::Shader> GetShader();

    PresetState& m_presetState;

    std::weak_ptr<Renderer::Shader> m_motionVectorShader;
    std::shared_ptr<Renderer::Sampler> m_sampler{std::make_shared<Renderer::Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)};

    int m_lastVertexCount{};
};

}
}
