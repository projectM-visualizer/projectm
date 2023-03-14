#include "Border.hpp"

#include <glm/gtc/type_ptr.hpp>

Border::Border(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void Border::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDisableVertexAttribArray(1);
}

void Border::Draw(const PerFrameContext& presetPerFrameContext)
{
    // Draw Borders
    float const outerBorderSize = static_cast<float>(*presetPerFrameContext.ob_size);
    float const innerBorderSize = static_cast<float>(*presetPerFrameContext.ib_size);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    // No additive drawing for borders
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    std::array<Point, 4> vertices{};
    for (int border = 0; border < 2; border++)
    {
        float r = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_r) : static_cast<float>(*presetPerFrameContext.ib_r);
        float g = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_g) : static_cast<float>(*presetPerFrameContext.ib_g);
        float b = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_b) : static_cast<float>(*presetPerFrameContext.ib_b);
        float a = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_a) : static_cast<float>(*presetPerFrameContext.ib_a);

        if (a > 0.001f)
        {
            glVertexAttrib4f(1, r, g, b, a);

            float innerRadius = (border == 0) ? 1.0f - outerBorderSize : 1.0f - outerBorderSize - innerBorderSize;
            float outerRadius = (border == 0) ? 1.0f : 1.0f - outerBorderSize;

            vertices[0].x = innerRadius;
            vertices[1].x = outerRadius;
            vertices[2].x = outerRadius;
            vertices[3].x = innerRadius;
            vertices[0].y = innerRadius;
            vertices[1].y = outerRadius;
            vertices[2].y = -outerRadius;
            vertices[3].y = -innerRadius;

            for (int rot = 0; rot < 4; rot++)
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * 4, vertices.data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Rotate 90 degrees
                // Milkdrop code calculates cos(PI/2) and sin(PI/2), which is 0 and 1 respectively.
                // Our code here simplifies the expressions accordingly.
                for (int vertex = 0; vertex < 4; vertex++)
                {
                    float const x = vertices[vertex].x;
                    float const y = vertices[vertex].y;
                    vertices[vertex].x = -y; // x * cos(PI/2) - y * sin(PI/2) == x * 0 - y * 1
                    vertices[vertex].y = x;  // x * sin(PI/2) + y * cos(PI/2) == x * 1 + y * 0
                }
            }
        }
    }

    Shader::Unbind();

    glDisable(GL_BLEND);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
