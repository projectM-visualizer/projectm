/*
 * Pipeline.cpp
 *
 *  Created on: Jun 17, 2008
 *      Author: pete
 */
#include "Pipeline.hpp"

Pipeline::Pipeline() {}
void Pipeline::Render(const BeatDetect &music, const PipelineContext &context){}
Point Pipeline::PerPixel(Point p, const PerPixelContext context)
{return p;}
