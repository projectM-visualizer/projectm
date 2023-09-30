#include "Filters.hpp"

#include "projectM-opengl.h"

Filters::Filters(const PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void Filters::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), reinterpret_cast<void*>(offsetof(Point, x)));
}

void Filters::Draw()
{
    UpdateMesh();

    if (m_viewportWidth == 0 || m_viewportHeight == 0)
    {
        return;
    }

    glEnable(GL_BLEND);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    glBindVertexArray(m_vaoID);
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

    glBindVertexArray(0);

    Shader::Unbind();

    glDisable(GL_BLEND);
}


void Filters::Brighten()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBlendFunc(GL_ZERO, GL_DST_COLOR);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Filters::Darken()
{
    glBlendFunc(GL_ZERO, GL_DST_COLOR);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Filters::Solarize()
{
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_COLOR);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBlendFunc(GL_DST_COLOR, GL_ONE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Filters::Invert()
{
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

    std::array<RenderItem::Point, 4> points;

    float const fOnePlusInvWidth = 1.0f + 1.0f / static_cast<float>(m_viewportWidth);
    float const fOnePlusInvHeight = 1.0f + 1.0f / static_cast<float>(m_viewportHeight);
    points[0].x = -fOnePlusInvWidth;
    points[1].x = fOnePlusInvWidth;
    points[2].x = -fOnePlusInvWidth;
    points[3].x = fOnePlusInvWidth;
    points[0].y = fOnePlusInvHeight;
    points[1].y = fOnePlusInvHeight;
    points[2].y = -fOnePlusInvHeight;
    points[3].y = -fOnePlusInvHeight;

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
