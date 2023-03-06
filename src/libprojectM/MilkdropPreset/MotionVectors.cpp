#include "MotionVectors.hpp"

#include <glm/gtc/type_ptr.hpp>

MotionVectors::MotionVectors(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void MotionVectors::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDisableVertexAttribArray(1);
}

void MotionVectors::Draw(const PerFrameContext& presetPerFrameContext)
{
    int countX = static_cast<int>(*presetPerFrameContext.mv_x);
    int countY = static_cast<int>(*presetPerFrameContext.mv_y);

    if (countX <= 0 || countY <= 0)
    {
        return;
    }

    float divertX = static_cast<float>(*presetPerFrameContext.mv_x) - static_cast<float>(countX);
    float divertY = static_cast<float>(*presetPerFrameContext.mv_y) - static_cast<float>(countY);

    if (countX > 64)
    {
        countX = 64;
        divertX = 0.0f;
    }
    if (countY > 48)
    {
        countY = 48;
        divertY = 0.0f;
    }

    auto divertX2 = static_cast<float>(*presetPerFrameContext.mv_dx);
    auto divertY2 = static_cast<float>(*presetPerFrameContext.mv_dy);

    auto lengthMultiplier = static_cast<float>(*presetPerFrameContext.mv_l);

    // Clamp X/Y diversions to 0..1
    if (divertX < 0.0f)
    {
        divertX = 0.0f;
    }
    if (divertX > 1.0f)
    {
        divertX = 1.0f;
    }
    if (divertY < 0.0f)
    {
        divertY = 0.0f;
    }
    if (divertY > 1.0f)
    {
        divertY = 1.0f;
    }

    float const inverseWidth = 1.0f / static_cast<float>(m_presetState.viewportWidth);
    float const minimalLength = 1.0f * inverseWidth;

    struct Point {
        float x{};
        float y{};
    };

    std::vector<Point> lineVertices(static_cast<std::size_t>(countX + 1) * 2); // countX + 1 lines for each grid row, 2 vertices each.

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glVertexAttrib4f(1,
                     static_cast<float>(*presetPerFrameContext.mv_r),
                     static_cast<float>(*presetPerFrameContext.mv_g),
                     static_cast<float>(*presetPerFrameContext.mv_b),
                     static_cast<float>(*presetPerFrameContext.mv_a));

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);


    for (int y = 0; y < countY; y++)
    {
        float const posY = (static_cast<float>(y) + 0.25f) / (static_cast<float>(countY) + divertY + 0.25f - 1.0f) - divertY2;

        if (posY > 0.0001f && posY < 0.9999f)
        {
            int vertex = 0;
            for (int x = 0; x < countX; x++)
            {
                float const posX = (static_cast<float>(x) + 0.25f) / (static_cast<float>(countX) + divertX + 0.25f - 1.0f) + divertX2;

                if (posX > 0.0001f && posX < 0.9999f)
                {
                    float posX2{};
                    float posY2{};

                    // Uses the warp mesh texture transformation to get the motion direction of this point.
                    ReversePropagatePoint(posX, posY, posX2, posY2);

                    // Enforce minimum trail length
                    {
                        float distX = posX2 - posX;
                        float distY = posY2 - posY;

                        distX *= lengthMultiplier;
                        distY *= lengthMultiplier;

                        float length = sqrtf(distX * distX + distY * distY);
                        if (length > minimalLength)
                        {
                        }
                        else if (length > 0.00000001f)
                        {
                            length = minimalLength / length;
                            distX *= length;
                            distY *= length;
                        }
                        else
                        {
                            distX = minimalLength;
                            distY = minimalLength;
                        }

                        posX2 = posX + distX;
                        posY2 = posY + distY;
                    }

                    // Assign line vertices
                    lineVertices.at(vertex).x = posX * 2.0f - 1.0f;
                    lineVertices.at(vertex).y = posY * 2.0f - 1.0f;
                    lineVertices.at(vertex + 1).x = posX2 * 2.0f - 1.0f;
                    lineVertices.at(vertex + 1).y = posY2 * 2.0f - 1.0f;

                    vertex += 2;
                }
            }

            // Draw a row of lines.
            glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * lineVertices.size(), lineVertices.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size()));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader::Unbind();

    glDisable(GL_BLEND);
}

void MotionVectors::ReversePropagatePoint(float posX1, float posY1, float& posX2, float& posY2)
{
}
