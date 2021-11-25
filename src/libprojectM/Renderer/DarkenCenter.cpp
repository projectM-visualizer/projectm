#include "DarkenCenter.hpp"

#include <glm/gtc/type_ptr.hpp>

DarkenCenter::DarkenCenter()
    : RenderItem()
{
    RenderItem::Init();
}

void DarkenCenter::InitVertexAttrib()
{
    float points_colors[6][6] = {
        { 0.5,  0.5,  0, 0, 0, (3.0f / 32.0f) * masterAlpha },
        { 0.45, 0.5,  0, 0, 0, 0 },
        { 0.5,  0.45, 0, 0, 0, 0 },
        { 0.55, 0.5,  0, 0, 0, 0 },
        { 0.5,  0.55, 0, 0, 0, 0 },
        { 0.45, 0.5,  0, 0, 0, 0 }};

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) 0); // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 2)); // colors

    glBufferData(GL_ARRAY_BUFFER, sizeof(points_colors), points_colors, GL_STATIC_DRAW);
}


void DarkenCenter::Draw(RenderContext& context)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glBindVertexArray(m_vaoID);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    glBindVertexArray(0);
}
