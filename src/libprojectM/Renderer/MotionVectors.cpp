#include "MotionVectors.hpp"

#include <glm/gtc/type_ptr.hpp>

MotionVectors::MotionVectors()
    : RenderItem()
{
    RenderItem::Init();
}

void MotionVectors::InitVertexAttrib() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void MotionVectors::Draw(RenderContext &context)
{
    float  intervalx=1.0/x_num;
    float  intervaly=1.0/y_num;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (x_num + y_num < 600)
    {
        int size = x_num * y_num ;

        floatPair *points = new float[size][2];

        for (int x=0;x<(int)x_num;x++)
        {
            for(int y=0;y<(int)y_num;y++)
            {
                float lx, ly;
                lx = x_offset+x*intervalx;
                ly = y_offset+y*intervaly;

                points[(x * (int)y_num) + y][0] = lx;
                points[(x * (int)y_num) + y][1] = ly;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair) * size, NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair) * size, points, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete[] points;

        glUseProgram(context.programID_v2f_c4f);

        glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

#ifndef GL_TRANSITION
        if (length <= 0.0) {
            glPointSize(1.0);
        } else {
            glPointSize(length);
        }
#endif

        glUniform1f(context.uniform_v2f_c4f_vertex_point_size, length);
        glVertexAttrib4f(1, r, g, b, a * masterAlpha);

        glBindVertexArray(m_vaoID);

        glDrawArrays(GL_POINTS,0,size);

        glBindVertexArray(0);
    }
}
