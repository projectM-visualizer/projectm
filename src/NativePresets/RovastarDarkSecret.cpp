/*
 * RovastarFractopiaFrantic.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#include "Pipeline.hpp"
#include "MilkdropCompatability.hpp"
#include "Transformation.hpp"
#include "MilkdropWaveform.hpp"
#include "Filters.hpp"
#include "NativePreset.hpp"

class RovastarDarkSecret : public Pipeline
{
public:

	Shape shape1, shape2, shape3, shape4;
	Border border;
	Invert invert;

	RovastarDarkSecret() : Pipeline()
	{
		screenDecay = 0.9;
		textureWrap = true;

		drawables.push_back(&border);
		drawables.push_back(&shape1);
		drawables.push_back(&shape2);
		drawables.push_back(&shape3);
		drawables.push_back(&shape4);

		compositeDrawables.push_back(&invert);

		border.outer_size = 0.015;
		border.inner_size = 0.010;
		border.outer_a = 1.0;
		border.inner_a = 1.0;

		shape1.sides = 4;
		shape1.additive = true;
		shape1.radius=1.621747;
		shape1.textured = true;
		shape1.tex_zoom=0.424973;
		shape1.a = 1;
		shape1.border_a = 0.0;
		shape1.r = 1;
		shape1.g = 0;
		shape1.b = 0;
		shape1.a2 = 0;

		shape2.sides = 4;
		shape2.additive = true;
		shape2.radius=1.621747;
		shape2.textured = true;
		shape2.tex_zoom = 0.424973;
		shape2.a = 1;
		shape2.border_a = 0.0;
		shape2.r = 1;
		shape2.g = 0;
		shape2.b = 1;
		shape2.a2 = 0;


		shape3.sides = 4;
		shape3.additive = true;
		shape3.radius=1.621747;
		shape3.textured = true;
		shape3.tex_zoom = 0.424973;
		shape3.a = 1;
		shape3.border_a = 0.0;
		shape3.r = 0;
		shape3.g = 1;
		shape3.b = 1;
		shape3.a2 = 0;

		shape4.sides = 4;
		shape4.additive = false;
		shape4.radius=1.621747;
		shape4.textured = true;
		shape4.tex_zoom = 0.424973;
		shape4.a = 1;
		shape4.border_a = 0.0;
		shape4.r = 1;
		shape4.g = 1;
		shape4.b = 1;
		shape4.a2 = 0;


		q8 = 0;
		q7 = 0;
		oldq7 = 0;
	}

	float q8, oldq8, q7, oldq7, q6;
	float dx, dy;

	virtual void Render(const BeatDetect &music, const PipelineContext &context)
	{

		float wave_r = 0.5+ 0.2*(music.bass-1);
		float wave_g = 0.5+ 0.2*(music.mid-1.2);
		float wave_b = 0.5+ 0.2*(music.treb-.5);

		border.outer_r = 1-wave_r;
		border.outer_g = 1-wave_g;
		border.outer_b = 1-wave_b;

		border.inner_r = 0.75 + 0.25*sin(context.time*0.4123);
		border.inner_g = 0.25 + 0.25*cos(context.time*0.87);
		border.inner_b = 0.5+0.5*sin(1.23*context.time);

		float val = 1.2*music.bass+0.4*music.bass_att+0.1*music.treb+0.1*music.treb_att+0.1*music.mid+0.1*music.mid_att;

		q8 = oldq8 +0.003*(((pow(val,6)/context.fps) + (pow(val,5)/context.fps) + (pow(val,4)/context.fps) + (pow(val,3)/context.fps) + (pow(val,2)/context.fps) +(val)/context.fps));
		oldq8 = q8;
		q7 =oldq7+ 0.001*(pow(val,7)/context.fps);
		oldq7 = q7;

		dy = 0.5 + 0.01*(sin(0.786*q7));
		dx = 0.1*sin(1.143*q8);
		q6 = 15+0.1*(((pow(val,6)/context.fps) + (pow(val,5)/context.fps) + (pow(val,4)/context.fps) + (pow(val,3)/context.fps) + (pow(val,2)/context.fps) +(val)/context.fps));

		shape1.x = 0.5 + 0.1*sin(q7*0.986);
		shape1.y = 0.5 + 0.1*sin(q7*0.846);
		shape1.tex_ang = 3.1515 + 3.1415*sin(q7*0.4521) +0.05*sin(context.time);

		shape2.x = 0.5 + 0.1*sin(q7*0.986);
		shape2.y = 0.5 + 0.1*sin(q7*0.846);
		shape2.tex_ang = 3.1515 + 3.1415*sin(q7*0.4521) +0.1*sin(context.time);

		shape3.x = 0.5 + 0.1*sin(q7*0.986);
		shape3.y = 0.5 + 0.1*sin(q7*0.846);
		shape3.tex_ang = 3.1515 + 3.1415*sin(q7*0.4521) +0.15*sin(context.time);

		shape4.x = 0.5 + 0.1*sin(q7*0.986);
		shape4.y = 0.5 + 0.1*sin(q7*0.846);
		shape4.tex_ang = 3.1515 + 3.1415*sin(q7*0.4521) +0.2*sin(context.time);;

	}

	virtual PixelPoint PerPixel(PixelPoint p, const PerPixelContext context)
	{
		float pdy=dy+0.008*cos((p.x-0.5 - 0.1*sin(q7))*(q6));
		Transforms::Zoom(p,context,13.290894,1);
		Transforms::Transform(p,context,dx,pdy);
		return p;
	}
};


typedef NativePreset<RovastarDarkSecret> RovastarDarkSecretPreset;


extern "C" RovastarDarkSecretPreset * create(const char * url) {
	return new RovastarDarkSecretPreset(std::string(url));
}

extern "C" void destroy(RovastarDarkSecretPreset * preset) {
    delete preset;
}
