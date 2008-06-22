#ifndef Pipeline_HPP
#define Pipeline_HPP

#include <vector>
#include "PerPixelMesh.hpp"
#include "Renderable.hpp"
#include "Filters.hpp"
#include "BeatDetect.hpp"

class Pipeline
{
public:

	 int fps;

	 float time;
	 int   frame;
	 float progress;

	 bool textureWrap;
	 float screenDecay;

	 float videoEchoAlpha;
	 float videoEchoZoom;
	 float videoEchoOrientation;

	 std::vector<RenderItem*> drawables;
	 std::vector<RenderItem*> compositeDrawables;

	 Pipeline();
	 virtual void Render(BeatDetect &music);
	 virtual Point PerPixel(Point p, PerPixelContext context);
};

#endif
