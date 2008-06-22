/*
 * Pipeline.cpp
 *
 *  Created on: Jun 17, 2008
 *      Author: pete
 */
#include "Pipeline.hpp"

Pipeline::Pipeline() {}
void Pipeline::Render(BeatDetect &music, PipelineContext &context){}
Point Pipeline::PerPixel(Point p, PerPixelContext context)
{return p;}
