#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <Renderer/RenderItem.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Draws a flexible motion vector field.
 *
 * This is broken right now, as it only renders a relatively static 1px point grid, with no apparent motion trails.
 * Milkdrop renders this as lines with trails.
 *
 * @todo Reverse-engineer the original Milkdrop code and reimplement it properly.
 *       https://github.com/projectM-visualizer/milkdrop2/blob/f05b0d811a87a17c4624170c26c93bac39b05bde/src/vis_milk2/milkdropfs.cpp#L1239
 */
class MotionVectors : public Renderer::RenderItem
{
public:
    MotionVectors() = delete;

    explicit MotionVectors(PresetState& presetState);

    void InitVertexAttrib();

    /**
     * Renders the motion vectors.
     * @param presetPerFrameContext The per-frame context variables.
     * @param motionTexture The u/v "motion" texture written by the warp shader.
     */
    void Draw(const PerFrameContext& presetPerFrameContext, std::shared_ptr<Renderer::Texture> motionTexture);

private:
    struct MotionVectorVertex
    {
        float x{};
        float y{};
        int32_t index{};
    };

    PresetState& m_presetState; //!< The global preset state.

    Renderer::Shader m_motionVectorShader; //!< The motion vector shader, calculates the trace positions in the GPU.
    std::shared_ptr<Renderer::Sampler> m_sampler{std::make_shared<Renderer::Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)}; //!< The texture sampler.

    int m_lastVertexCount{}; //!< Number of vertices drawn in the previous draw call.
};

} // namespace MilkdropPreset
} // namespace libprojectM
