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

void Pipeline::setStaticPerPixel(int _gx, int _gy)
{
	staticPerPixel = true;
    this->gx = _gx;
    this->gy = _gy;

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

PixelPoint Pipeline::PerPixel(PixelPoint p, const PerPixelContext context)
{return p;}
