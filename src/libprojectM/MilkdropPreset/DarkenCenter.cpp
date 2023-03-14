#include "DarkenCenter.hpp"

DarkenCenter::DarkenCenter(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
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
    glBindVertexArray(m_vaoID);

    if (m_presetState.renderContext.aspectY != m_aspectY)
    {
        m_aspectY = m_presetState.renderContext.aspectY;

        // Update mesh with new aspect ratio if needed
        float const halfSize = 0.05f;
        std::array<ColoredPoint, 6> vertices = {{{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f / 32.0f},
                                                 {0.0f - halfSize * m_aspectY, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                                                 {0.0f, 0.0f - halfSize, 0.0f, 0.0f, 0.0f, 0.0f},
                                                 {0.0f + halfSize * m_aspectY, 0.0, 0.0f, 0.0f, 0.0f, 0.0f},
                                                 {0.0f, 0.0f + halfSize, 0.0f, 0.0f, 0.0f, 0.0f},
                                                 {0.0f - halfSize * m_aspectY, 0.0, 0.0f, 0.0f, 0.0f, 0.0f}}};

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ColoredPoint) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    glDisable(GL_BLEND);
    glBindVertexArray(0);
    Shader::Unbind();
}
