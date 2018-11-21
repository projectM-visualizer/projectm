/*
 * RovastarFractopiaFrantic.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#include <cmath>

#include "Pipeline.hpp"
#include "MilkdropCompatability.hpp"
#include "Transformation.hpp"
#include "MilkdropWaveform.hpp"
#include "NativePreset.hpp"

class RovastarDriftingChaos : public Pipeline
{
public:

	Shape shape1, shape2, shape3;
	MilkdropWaveform wave;

	RovastarDriftingChaos() : Pipeline()
	{
		screenDecay = 1.0;
		textureWrap = 0;

		drawables.push_back(&shape1);
		drawables.push_back(&shape2);
		drawables.push_back(&shape3);
		drawables.push_back(&wave);

		shape1.sides = 3;
		shape1.radius=0.550000;
		shape1.a = 0.1;
		shape1.a2 = 0.9;
		shape1.border_r = 1.0;
		shape1.border_g = 1.0;
		shape1.border_b = 1.0;
		shape1.border_a = 0.2;

		shape2.sides = 32;
		shape2.radius=0.40000;
		shape2.a = 1.0;
		shape2.a2 = 0.3;
		shape2.border_r = 1.0;
		shape2.border_g = 1.0;
		shape2.border_b = 1.0;
		shape2.border_a = 0.2;

		shape3.sides = 4;
		shape3.radius=0.40000;
		shape3.a = 0.6;
		shape3.a2 = 0.4;
		shape3.border_r = 1.0;
		shape3.border_g = 1.0;
		shape3.border_b = 1.0;
		shape3.border_a = 0.2;

	}

	float xamptarg, q8, oldq8, q7, xpos, ypos,xdir, xspeed, xamp, yamp, yamptarg,yspeed,ydir;
	float dx, dy, angle;

	virtual void Render(const BeatDetect &music, const PipelineContext &context)
	{

		float volume = 0.15*(music.bass+music.bass_att+music.treb+music.treb_att+music.mid+music.mid_att);

		xamptarg = if_milk(equal(context.frame%15,0),min(0.5*volume*music.bass_att,0.5),xamptarg);
		xamp = xamp + 0.5*(xamptarg-xamp);

		xdir = if_milk(above(std::abs(xpos),xamp),-sign(xpos),if_milk(below(std::abs(xspeed),0.1),2*above(xpos,0)-1,xdir));
		xspeed += xdir*xamp - xpos - xspeed*0.055*below(std::abs(xpos),xamp);
		xpos = xpos + 0.001*xspeed;
		dx = xpos*0.005;
		yamptarg = if_milk(equal(context.frame%15,0),min(0.3*volume*music.treb_att,0.5),yamptarg);
		yamp +=  0.5*(yamptarg-yamp);
		ydir = if_milk(above(std::abs(ypos),yamp),-sign(ypos),if_milk(below(std::abs(yspeed),0.1),2*above(ypos,0)-1,ydir));

		yspeed += ydir*yamp - ypos - yspeed*0.055*below(std::abs(ypos),yamp);
		ypos = ypos + 0.001*yspeed;

		dy = ypos*0.005;
		angle = 10*(dx-dy);

		q8 =oldq8+ 0.0003*(powf(1+1.2*music.bass+0.4*music.bass_att+0.1*music.treb+0.1*music.treb_att+0.1*music.mid+0.1*music.mid_att,6)/context.fps);
		oldq8 = q8;
		q7 = 0.003*(powf(1+1.2*music.bass+0.4*music.bass_att+0.1*music.treb+0.1*music.treb_att+0.1*music.mid+0.1*music.mid_att,6)/context.fps);

		shape1.ang = context.time*1.4;
				shape1.x = 0.5 + 0.08*cos(context.time*1.3) + 0.03*cos(context.time*0.7);
				shape1.y = 0.5 + 0.08*sin(context.time*1.4) + 0.03*sin(context.time*0.7);
				shape1.r = 0.5 + 0.5*sin(q8*0.613 + 1);
				shape1.g = 0.5 + 0.5*sin(q8*0.763 + 2);
				shape1.b = 0.5 + 0.5*sin(q8*0.771 + 5);
				shape1.r2 = 0.5 + 0.5*sin(q8*0.635 + 4);
				shape1.g2 = 0.5 + 0.5*sin(q8*0.616+ 1);
				shape1.b2 = 0.5 + 0.5*sin(q8*0.538 + 3);

				shape2.ang = context.time*1.7;
				shape2.x = 0.5 + 0.08*cos(context.time*1.1) + 0.03*cos(context.time*0.7);
				shape2.y = 0.5 + 0.08*sin(context.time*1.1) + 0.03*sin(context.time*0.7);
				shape2.r = 0.5 + 0.5*sin(q8*0.713 + 1);
				shape2.g = 0.5 + 0.5*sin(q8*0.563 + 2);
				shape2.b = 0.5 + 0.5*sin(q8*0.654 + 5);
				shape2.r2 = 0.5 + 0.5*sin(q8*0.885 + 4);
				shape2.g2 = 0.5 + 0.5*sin(q8*0.556+ 1);
				shape2.b2 = 0.5 + 0.5*sin(q8*0.638 + 3);

				shape3.ang = context.time*1.24;
				shape3.x = 0.5 - 0.08*cos(context.time*1.07) + 0.03*cos(context.time*0.7);
				shape3.y = 0.5 - 0.08*sin(context.time*1.33) + 0.03*sin(context.time*0.7);
				shape3.g = 0.5 + 0.5*sin(q8*0.713 + 1);
				shape3.b = 0.5 + 0.5*cos(q8*0.563 + 2);
				shape3.r = 0.5 + 0.5*sin(q8*0.654 + 5);
				shape3.r2 = 0.5 + 0.5*cos(q8*0.885 + 4);
				shape3.g2 = 0.5 + 0.5*cos(q8*0.556+ 1);
				shape3.b2 = 0.5 + 0.5*sin(q8*0.638 + 3);

	}

	virtual PixelPoint PerPixel(PixelPoint p, const PerPixelContext context)
	{
		Transforms::Zoom(p,context,1+0.05*context.rad,1);
		Transforms::Transform(p,context,dx,dy);
		Transforms::Rotate(p,context,angle,0.5,0.5);
		return p;

	}
};

typedef NativePreset<RovastarDriftingChaos> RovastarDriftingChaosPreset;

extern "C" RovastarDriftingChaosPreset * create(const char * url) {
	return new RovastarDriftingChaosPreset(std::string(url));
}

extern "C" void destroy(RovastarDriftingChaosPreset * preset) {
    delete preset;
}

