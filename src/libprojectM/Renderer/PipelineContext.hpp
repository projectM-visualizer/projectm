/*
 * PipelineContext.hpp
 *
 *  Created on: Jun 22, 2008
 *      Author: pete
 */

#ifndef PIPELINECONTEXT_HPP_
#define PIPELINECONTEXT_HPP_

class PipelineContext
{
public:
	int fps;
    float time;
    float presetStartTime;
	int   frame;
	float progress;

    int pixelsx;
    int pixelsy;

    float aspectx;
    float aspecty;

    PipelineContext();
	virtual ~PipelineContext();
};

#endif /* PIPELINECONTEXT_HPP_ */
