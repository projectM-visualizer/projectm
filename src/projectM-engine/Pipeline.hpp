#ifndef Pipeline_HPP
#define Pipeline_HPP

#include <vector>
#include "PerPixelMesh.hpp"
#include "Renderable.hpp"
#include "Filters.hpp"

class Pipeline
{
public:

	 int fps;

	 float time;
	 float bass;
	 float mid;
	 float treb;
	 float bass_att;
	 float mid_att;
	 float treb_att;
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
	 virtual void Render();
	 virtual Point PerPixel(Point p, PerPixelContext context);
};

#endif
