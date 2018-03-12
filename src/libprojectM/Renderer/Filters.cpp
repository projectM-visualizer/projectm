/*
 * Filters.cpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#include "Common.hpp"
#include "projectM-opengl.h"
#include "Filters.hpp"

void Brighten::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,points);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_ZERO, GL_DST_COLOR);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void Darken::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2,GL_FLOAT,0,points);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBlendFunc(GL_ZERO, GL_DST_COLOR);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisableClientState(GL_VERTEX_ARRAY);
}

void Invert::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2,GL_FLOAT,0,points);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisableClientState(GL_VERTEX_ARRAY);
}

void Solarize::Draw(RenderContext &context)
{
	float points[4][2] = {{-0.5, -0.5},
					      {-0.5,  0.5},
					      { 0.5,  0.5},
					      { 0.5,  -0.5}};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,points);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_DST_COLOR);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_DST_COLOR, GL_ONE);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisableClientState(GL_VERTEX_ARRAY);
}
