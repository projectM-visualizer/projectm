/*
 * VideoEcho.hpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#ifndef VIDEOECHO_HPP_
#define VIDEOECHO_HPP_

#include "projectM-opengl.h"
#include "RenderItem.hpp"
#include "Common.hpp"

enum Orientation
{
	Normal=0, FlipX, FlipY, FlipXY
};

class VideoEcho: public RenderItem
{
public:
	VideoEcho();
	virtual ~VideoEcho();

	float a;
	float zoom;
	Orientation orientation;

    void InitVertexAttrib();
	void Draw(RenderContext &context);
};

#endif /* VIDEOECHO_HPP_ */
