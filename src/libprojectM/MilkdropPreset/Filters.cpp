#include "Filters.hpp"

namespace libprojectM {
namespace MilkdropPreset {

Filters::Filters(const PresetState& presetState)
    : m_presetState(presetState)
    , m_filterMesh(Renderer::VertexBufferUsage::StaticDraw)
{
    m_filterMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::TriangleStrip);
    m_filterMesh.SetVertexCount(4);
}

void Filters::Draw()
{
    UpdateMesh();

    if (m_viewportWidth == 0 || m_viewportHeight == 0)
    {
        return;
    }

    glEnable(GL_BLEND);

    auto shader = m_presetState.untexturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    if (m_presetState.brighten)
    {
        Brighten();
    }
    if (m_presetState.darken)
    {
        Darken();
    }
    if (m_presetState.solarize)
    {
        Solarize();
    }
    if (m_presetState.invert)
    {
        Invert();
    }

    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();

    glDisable(GL_BLEND);
}


void Filters::Brighten()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    m_filterMesh.Draw();
    glBlendFunc(GL_ZERO, GL_DST_COLOR);
    m_filterMesh.Draw();
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    m_filterMesh.Draw();
}

void Filters::Darken()
{
    glBlendFunc(GL_ZERO, GL_DST_COLOR);
    m_filterMesh.Draw();
}

void Filters::Solarize()
{
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_COLOR);
    m_filterMesh.Draw();
    glBlendFunc(GL_DST_COLOR, GL_ONE);
    m_filterMesh.Draw();
}

void Filters::Invert()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    m_filterMesh.Draw();
}

void Filters::UpdateMesh()
{
    if (m_viewportWidth == m_presetState.renderContext.viewportSizeX &&
        m_viewportHeight == m_presetState.renderContext.viewportSizeY)
    {
        return;
    }

    m_viewportWidth = m_presetState.renderContext.viewportSizeX;
    m_viewportHeight = m_presetState.renderContext.viewportSizeY;

    float const fOnePlusInvWidth = 1.0f + 1.0f / static_cast<float>(m_viewportWidth);
    float const fOnePlusInvHeight = 1.0f + 1.0f / static_cast<float>(m_viewportHeight);
    m_filterMesh.Vertices().Set({{-fOnePlusInvWidth, fOnePlusInvHeight},
                                 {fOnePlusInvWidth, fOnePlusInvHeight},
                                 {-fOnePlusInvWidth, -fOnePlusInvHeight},
                                 {fOnePlusInvWidth, -fOnePlusInvHeight}});

    m_filterMesh.Update();
}

} // namespace MilkdropPreset
} // namespace libprojectM
