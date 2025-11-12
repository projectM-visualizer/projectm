#include "DarkenCenter.hpp"

#include <Renderer/BlendMode.hpp>

namespace libprojectM {
namespace MilkdropPreset {

DarkenCenter::DarkenCenter(PresetState& presetState)
    : m_presetState(presetState)
    , m_mesh(Renderer::VertexBufferUsage::StaticDraw, true, false)
{
    m_mesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::TriangleFan);
    m_mesh.SetVertexCount(6);
    m_mesh.Colors().Set({{0.0f, 0.0f, 0.0f, 3.0f / 32.0f},
                         {0.0f, 0.0f, 0.0f, 0.0f},
                         {0.0f, 0.0f, 0.0f, 0.0f},
                         {0.0f, 0.0f, 0.0f, 0.0f},
                         {0.0f, 0.0f, 0.0f, 0.0f},
                         {0.0f, 0.0f, 0.0f, 0.0f}});
    m_mesh.Update();
}

void DarkenCenter::Draw()
{
    if (m_presetState.renderContext.aspectY != m_aspectY)
    {
        m_aspectY = m_presetState.renderContext.aspectY;

        // Update mesh with new aspect ratio if needed
        constexpr float halfSize = 0.05f;
        m_mesh.Vertices().Set({{0.0f, 0.0f},
                               {0.0f - halfSize * m_aspectY, 0.0f},
                               {0.0f, 0.0f - halfSize},
                               {0.0f + halfSize * m_aspectY, 0.0f},
                               {0.0f, 0.0f + halfSize},
                               {0.0f - halfSize * m_aspectY, 0.0f}});
        m_mesh.Update();
    }

    Renderer::BlendMode::Set(true, Renderer::BlendMode::Function::SourceAlpha, Renderer::BlendMode::Function::OneMinusSourceAlpha);

    auto shader = m_presetState.untexturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    shader->SetUniformFloat("vertex_point_size", 1.0f);

    m_mesh.Draw();

    Renderer::BlendMode::SetBlendActive(false);
    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();
}

} // namespace MilkdropPreset
} // namespace libprojectM
