#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Draws a flexible motion vector field.
 */
class MotionVectors : public libprojectM::Renderer::Backend::OpenGL::OpenGLRenderItem
{
public:
    MotionVectors() = delete;

    explicit MotionVectors(PresetState& presetState);

    void InitVertexAttrib() override;

    void Draw(const PerFrameContext& presetPerFrameContext, std::shared_ptr<Renderer::Texture> motionTexture);

private:
    std::shared_ptr<Renderer::Shader> GetShader();

    PresetState& m_presetState; //!< The global preset state.

    Renderer::Mesh m_motionVectorMesh; //!< The Motion Vector geometry.

    std::weak_ptr<Renderer::Shader> m_motionVectorShader;                                                           //!< The motion vector shader, calculates the trace positions in the GPU.
    std::shared_ptr<Renderer::Sampler> m_sampler{std::make_shared<Renderer::Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)}; //!< The texture sampler.
};

} // namespace MilkdropPreset
} // namespace libprojectM
