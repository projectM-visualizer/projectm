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
    float const outerBorderSize = static_cast<float>(*presetPerFrameContext.ob_size) * .5f;
    float const innerBorderSize = static_cast<float>(*presetPerFrameContext.ib_size) * .5f;
    float const texelOffset = 1.0f - outerBorderSize;

    std::array<std::array<float, 4>, 10> const points = {{
        // Outer
        {0.0f, 0.0f, outerBorderSize, 0.0f},
        {0.0f, 1.0f, outerBorderSize, texelOffset},
        {1.0f, 1.0f, texelOffset, texelOffset},
        {1.0f, 0.0f, texelOffset, outerBorderSize},
        {outerBorderSize, 0.0f, outerBorderSize, outerBorderSize},

        // Inner
        {outerBorderSize, outerBorderSize, outerBorderSize + innerBorderSize, outerBorderSize},
        {outerBorderSize, texelOffset, outerBorderSize + innerBorderSize, texelOffset - innerBorderSize},
        {texelOffset, texelOffset, texelOffset - innerBorderSize, texelOffset - innerBorderSize},
        {texelOffset, outerBorderSize, texelOffset - innerBorderSize, outerBorderSize + innerBorderSize},
        {outerBorderSize + innerBorderSize, outerBorderSize, outerBorderSize + innerBorderSize, outerBorderSize + innerBorderSize},
    }};

    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, points.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

    glVertexAttrib4f(1,
                     static_cast<float>(*presetPerFrameContext.ob_r),
                     static_cast<float>(*presetPerFrameContext.ob_g),
                     static_cast<float>(*presetPerFrameContext.ob_b),
                     static_cast<float>(*presetPerFrameContext.ob_a));

    // No additive drawing for borders
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_vaoID);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

    glVertexAttrib4f(1,
                     static_cast<float>(*presetPerFrameContext.ib_r),
                     static_cast<float>(*presetPerFrameContext.ib_g),
                     static_cast<float>(*presetPerFrameContext.ib_b),
                     static_cast<float>(*presetPerFrameContext.ib_a));

    // 1st pass for inner
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 10);

    // 2nd pass for inner
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 10);

    glBindVertexArray(0);
}
