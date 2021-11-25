#include "Border.hpp"

#include <glm/gtc/type_ptr.hpp>

Border::Border()
    : RenderItem()
{
    RenderItem::Init();
}

void Border::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glDisableVertexAttribArray(1);
}

void Border::Draw(RenderContext& context)
{
    //Draw Borders
    float of = outer_size * .5;
    float iff = inner_size * .5;
    float texof = 1.0 - of;

    float points[40] = {
        // Outer
        0, 0, of, 0,
        0, 1, of, texof,
        1, 1, texof, texof,
        1, 0, texof, of,
        of, 0, of, of,

        // Inner
        of, of, of + iff, of,
        of, texof, of + iff, texof - iff,
        texof, texof, texof - iff, texof - iff,
        texof, of, texof - iff, of + iff,
        of + iff, of, of + iff, of + iff,
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, points, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glVertexAttrib4f(1, outer_r, outer_g, outer_b, outer_a * masterAlpha);

    //no additive drawing for borders
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_vaoID);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

    glVertexAttrib4f(1, inner_r, inner_g, inner_b, inner_a * masterAlpha);

    // 1st pass for inner
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 10);

    // 2nd pass for inner
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 10);

    glBindVertexArray(0);
}
