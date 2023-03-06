#include "Filters.hpp"

#include "projectM-opengl.h"

#include "Renderer/ShaderEngine.hpp"

Filters::Filters(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void Filters::InitVertexAttrib()
{
    std::array<std::array<float, 2>, 4> points = {{{-0.5, -0.5},
                                                   {-0.5, 0.5},
                                                   {0.5, 0.5},
                                                   {0.5, -0.5}}};

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDisableVertexAttribArray(1);
}

void Filters::Brighten()
{
    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glBindVertexArray(m_vaoID);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBlendFunc(GL_ZERO, GL_DST_COLOR);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);

    Shader::Unbind();
}

void Filters::Darken()
{
    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glBlendFunc(GL_ZERO, GL_DST_COLOR);

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader::Unbind();
}

void Filters::Invert()
{
    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader::Unbind();
}

void Filters::Solarize()
{
    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_COLOR);

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBlendFunc(GL_DST_COLOR, GL_ONE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader::Unbind();
}
