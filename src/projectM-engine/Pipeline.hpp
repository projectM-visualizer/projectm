#ifndef Pipeline_HPP
#define Pipeline_HPP

#include <vector>
#include "Transformation.hpp"
#include "Renderable.hpp"
#include "Filters.hpp"

class Pipeline
{
public:

	 int fps;

	 int mesh_x;
	 int mesh_y;

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

	 std::vector<PerPixelTransform*> perPixelTransforms;
	 std::vector<RenderItem*> drawables;
	 std::vector<RenderItem*> compositeDrawables;

	 Pipeline(int mesh_x, int mesh_y);
	 virtual void Render();
};

#endif
