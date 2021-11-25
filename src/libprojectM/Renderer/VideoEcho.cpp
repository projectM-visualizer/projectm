/*
 * VideoEcho.cpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#include "VideoEcho.hpp"
#include "ShaderEngine.hpp"
#include <glm/gtc/type_ptr.hpp>

VideoEcho::VideoEcho(): a(0), zoom(1), orientation(Normal)
{
    Init();
}

VideoEcho::~VideoEcho()
{
}

void VideoEcho::InitVertexAttrib() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);   // Positions

    glDisableVertexAttribArray(1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures
}

void VideoEcho::Draw(RenderContext &context)
{
		int flipx=1, flipy=1;
		switch (orientation)
		{
			case Normal: flipx=1;flipy=1;break;
			case FlipX: flipx=-1;flipy=1;break;
			case FlipY: flipx=1;flipy=-1;break;
			case FlipXY: flipx=-1;flipy=-1;break;
			default: flipx=1;flipy=1; break;
		}

    float buffer_data[8][2] = {
        {-0.5f*flipx, -0.5f*flipy},
        {0.0, 1.0},

        {-0.5f*flipx,  0.5f*flipy},
        {0.0, 0.0},

        { 0.5f*flipx,  0.5f*flipy},
        {1.0, 0.0},

        { 0.5f*flipx, -0.5f*flipy},
        {1.0, 1.0}
    };

    glm::mat4 mat_first_translation = glm::mat4(1.0);
    mat_first_translation[3][0] = -0.5;
    mat_first_translation[3][1] = -0.5;

    glm::mat4  mat_scale = glm::mat4(1.0);
    mat_scale[0][0] = 1.0/zoom;
    mat_scale[1][1] = 1.0/zoom;

    glm::mat4  mat_second_translation = glm::mat4(1.0);
    mat_second_translation[3][0] = 0.5;
    mat_second_translation[3][1] = 0.5;

    for (int i = 1; i < 8; i+=2) {
        glm::vec4 texture = glm::vec4(buffer_data[i][0], buffer_data[i][1], 0, 1);
        texture = mat_first_translation * texture;
        texture = mat_scale * texture;
        texture = mat_second_translation * texture;

        buffer_data[i][0] = texture[0];
        buffer_data[i][1] = texture[1];
    }


    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, buffer_data, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(context.programID_v2f_c4f_t2f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_t2f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));
    glUniform1i(context.uniform_v2f_c4f_t2f_frag_texture_sampler, 0);


    //Now Blend the Video Echo
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, a * masterAlpha);

    glBindVertexArray(m_vaoID);

    //draw video echo
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
