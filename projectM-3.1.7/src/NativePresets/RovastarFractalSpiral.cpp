/*
 * RovastarFractalSpiral.hpp
 *
 *  Created on: Jun 22, 2008
 *      Author: pete
 */

#include <cmath>

#include "Pipeline.hpp"
#include "Transformation.hpp"
#include "MilkdropCompatability.hpp"
#include "NativePreset.hpp"

class RovastarFractalSpiral : public Pipeline
{
public:

	Border border;

	float dx, dy, cx, cy, time;

	RovastarFractalSpiral() : Pipeline()
	{
		drawables.push_back(&border);

		textureWrap = true;
		screenDecay = 1.0;


		border.outer_size = 0.01;
		border.outer_a = 1.0;
		border.inner_size = 0.02;
		border.inner_a = 1.0;
	}

	virtual void Render(const BeatDetect &music, const PipelineContext &context)
	{
		time = context.time;

		border.outer_r = 0.3 - 0.3*(0.5*sin(time*0.701)+ 0.3*cos(time*0.438));
		border.outer_g = 0.6- 0.4*sin(time*2.924);
		border.outer_b = 0.35 - 0.3*cos(time*0.816);
		cx = 0.5 - 0.1*sin(time*0.342);
		cy = 0.5 + 0.1*sin(time*0.433);

		border.inner_r = 0.5 + 0.5*sin(time*3.034);
		border.inner_g = 0.5 + 0.5*sin(time*2.547);
		border.inner_b = 0.5 + 0.5*sin(time*1.431);
		dx = -0.008*sin(time*0.23);
		dy = -0.008*sin(time*0.2);
	}

	virtual PixelPoint PerPixel(PixelPoint p, const PerPixelContext context)
	{
		float q1 = 8.05+(sin(p.x+0.137*time)-cos(p.y+0.213*time));
		int val1 = std::abs(p.x*3-0.4*sin(q1));
		int val2 = std::abs(p.y*3+0.4*sin(q1));
		val1 = val1%2;
		val2 = val2%2;

		float box=(1-context.rad)+ 0.5 * val1 + 0.5*val2;

		float zoom = if_milk(above(box,1),q1*.1,0.998531);
		float rot = if_milk(above(box,1),1*sin(0.385*time),0.02);

		Transforms::Zoom(p,context,zoom,1.01);
		Transforms::Rotate(p,context,rot,cx,cy);
		Transforms::Transform(p,context,dx,dy);

		return p;
	}

};


typedef NativePreset<RovastarFractalSpiral> RovastarFractalSpiralPreset;

extern "C" RovastarFractalSpiralPreset * create(const char * url) {
	return new RovastarFractalSpiralPreset(std::string(url));
}

extern "C" void destroy(RovastarFractalSpiralPreset * preset) {
    delete preset;
}
