#include "DarkenCenter.hpp"

#include <Renderer/BlendMode.hpp>

namespace libprojectM {
namespace MilkdropPreset {

DarkenCenter::DarkenCenter(PresetState& presetState)
    : m_presetState(presetState)
{
    Init();
}

void DarkenCenter::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), nullptr);                                            // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), reinterpret_cast<void*>(offsetof(ColoredPoint, r))); // colors
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
