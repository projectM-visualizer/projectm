/*
 * Pipeline.cpp
 *
 *  Created on: Jun 17, 2008
 *      Author: pete
 */
#include "Pipeline.hpp"
#include "wipemalloc.h"

Pipeline::Pipeline() : staticPerPixel(false),gx(0),gy(0),blur1n(1), blur2n(1), blur3n(1),
blur1x(1), blur2x(1), blur3x(1),
blur1ed(1){}

float **alloc_mesh(size_t gx, size_t gy);
float **free_mesh(float **mesh);

void Pipeline::setStaticPerPixel(int gx, int gy)
{
	staticPerPixel = true;
	this->gx = gx;
	this->gy = gy;

	this->x_mesh = alloc_mesh(gx, gy);
	this->y_mesh = alloc_mesh(gx, gy);
}

Pipeline::~Pipeline()
{
	if (staticPerPixel)
	{
		free_mesh(x_mesh);
		free_mesh(y_mesh);
	}
}

//void Pipeline::Render(const BeatDetect &music, const PipelineContext &context){}
PixelPoint Pipeline::PerPixel(PixelPoint p, const PerPixelContext context)
{return p;}
