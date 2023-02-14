#pragma once

#include "projectM-opengl.h"
#include "Renderer/RenderItem.hpp"

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
