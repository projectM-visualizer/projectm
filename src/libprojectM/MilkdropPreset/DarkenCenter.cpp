#include "DarkenCenter.hpp"

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shader = m_presetState.untexturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    m_mesh.Draw();

    glDisable(GL_BLEND);
    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();
}

} // namespace MilkdropPreset
} // namespace libprojectM
