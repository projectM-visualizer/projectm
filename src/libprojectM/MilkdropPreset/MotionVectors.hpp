#pragma once

#include "LineGeometry.hpp"
#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <Renderer/Mesh.hpp>
#include <Renderer/VertexArray.hpp>
#include <Renderer/VertexBuffer.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Draws a flexible motion vector field.
 *
 * Uses the same drawing logic as Milkdrop, but instead of reverse-propagating the motion data
 * on the CPU, projectM does this within the Motion Vector vertex shader. The Warp effect draws the
 * final U/V coordinates to a float texture, which is then used in the next frame to calculate the
 * vector length at the location of the line origin.
 */
class MotionVectors
{
public:
    MotionVectors() = delete;

    explicit MotionVectors(PresetState& presetState);

    /**
     * Renders the motion vectors.
     * @param presetPerFrameContext The per-frame context variables.
     * @param motionTexture The u/v "motion" texture written by the warp shader.
     */
    void Draw(const PerFrameContext& presetPerFrameContext, std::shared_ptr<Renderer::Texture> motionTexture);

private:
    std::shared_ptr<Renderer::Shader> GetShader();

    /**
     * @brief Gets (or compiles) the scaled quad-line program for motion vectors.
     * @return The program, or nullptr if the driver rejects it (the vectors are drawn as GL lines then).
     */
    std::shared_ptr<Renderer::Shader> GetQuadShader();

    /**
     * @brief Draws one scaled quad per start point in m_vectorStarts, in one instanced call.
     */
    void DrawQuads(const PerFrameContext& presetPerFrameContext, float lineScale, float minimumLength);

    PresetState& m_presetState; //!< The global preset state.

    Renderer::Mesh m_motionVectorMesh; //!< The Motion Vector geometry.

    Renderer::VertexArray m_quadArray;                                                       //!< Vertex array for the scaled quad vectors.
    Renderer::VertexBuffer<Renderer::Point> m_vectorStarts{Renderer::VertexBufferUsage::StreamDraw}; //!< Start points of the scaled quad vectors.
    bool m_quadArrayInitialized{false};                                                      //!< m_quadArray's attributes are set up.
    std::weak_ptr<Renderer::Shader> m_motionVectorQuadShader;                                //!< The scaled quad-line motion vector shader.
    bool m_quadShaderFailed{false};                                                          //!< The quad-line shader did not compile; draw GL lines.

    std::weak_ptr<Renderer::Shader> m_motionVectorShader;                                                           //!< The motion vector shader, calculates the trace positions in the GPU.
    std::shared_ptr<Renderer::Sampler> m_sampler{std::make_shared<Renderer::Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)}; //!< The texture sampler.
};

} // namespace MilkdropPreset
} // namespace libprojectM
