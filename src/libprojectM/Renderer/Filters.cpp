/*
 * Filters.cpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#include "Common.hpp"
#include "projectM-opengl.h"
#include "Filters.hpp"
#include "ShaderEngine.hpp"
#include <glm/gtc/type_ptr.hpp>

void Brighten::InitVertexAttrib() {
    float points[4][2] = {{-0.5, -0.5},
                      {-0.5,  0.5},
                      { 0.5,  0.5},
                      { 0.5,  -0.5}};

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void Brighten::Draw(RenderContext &context)
{
    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glBindVertexArray(m_vaoID);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_ZERO, GL_DST_COLOR);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);
}

void Darken::InitVertexAttrib() {
    float points[4][2] = {{-0.5, -0.5},
                      {-0.5,  0.5},
                      { 0.5,  0.5},
                      { 0.5,  -0.5}};

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void Darken::Draw(RenderContext &context)
{
    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

	glBlendFunc(GL_ZERO, GL_DST_COLOR);

    glBindVertexArray(m_vaoID);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Invert::InitVertexAttrib() {
    float points[4][2] = {{-0.5, -0.5},
                      {-0.5,  0.5},
                      { 0.5,  0.5},
                      { 0.5,  -0.5}};

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void Invert::Draw(RenderContext &context)
{

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

    glBindVertexArray(m_vaoID);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Solarize::InitVertexAttrib() {
    float points[4][2] = {{-0.5, -0.5},
                          {-0.5,  0.5},
                          { 0.5,  0.5},
                          { 0.5,  -0.5}};

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void Solarize::Draw(RenderContext &context)
{
    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

	glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_COLOR);

    glBindVertexArray(m_vaoID);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_DST_COLOR, GL_ONE);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
