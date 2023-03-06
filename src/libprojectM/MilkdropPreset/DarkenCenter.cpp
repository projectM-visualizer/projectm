#include "DarkenCenter.hpp"

#include <glm/gtc/type_ptr.hpp>

DarkenCenter::DarkenCenter(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void DarkenCenter::InitVertexAttrib()
{
    std::array<std::array<float, 6>, 6> points_colors = {{{0.5, 0.5, 0, 0, 0, 3.0f / 32.0f},
                                                          {0.45, 0.5, 0, 0, 0, 0},
                                                          {0.5, 0.45, 0, 0, 0, 0},
                                                          {0.55, 0.5, 0, 0, 0, 0},
                                                          {0.5, 0.55, 0, 0, 0, 0},
                                                          {0.45, 0.5, 0, 0, 0, 0}}};

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, nullptr);                                    // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, reinterpret_cast<void*>(sizeof(float) * 2)); // colors

    glBufferData(GL_ARRAY_BUFFER, sizeof(points_colors), points_colors.data(), GL_STATIC_DRAW);
}


void DarkenCenter::Draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glBindVertexArray(m_vaoID);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    glBindVertexArray(0);

    Shader::Unbind();
}
