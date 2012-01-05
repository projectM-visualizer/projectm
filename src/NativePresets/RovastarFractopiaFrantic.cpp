/*
 * RovastarFractopiaFrantic.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#include "Pipeline.hpp"
#include "Transformation.hpp"
#include "NativePreset.hpp"

class RovastarFractopiaFrantic : public Pipeline
{
public:

	Border border;
	MotionVectors vectors;

	float dx, dy;
	float q4, q5, q6;

	float movement;

	RovastarFractopiaFrantic() : Pipeline()
	{
		drawables.push_back(&vectors);
		drawables.push_back(&border);

		screenDecay = 1.0;

		vectors.x_num = 64;
		vectors.y_num = 48;
		vectors.r = 0.0;
		vectors.g = 0.0;
		vectors.b = 0.0;
		vectors.a = 1.0;
		vectors.x_offset = -0.002;
		vectors.length = 2.0;

		border.outer_size = 0.05;
		border.outer_a = 1.0;

		border.inner_size = 0.04;
		border.inner_r = 0.0;
		border.inner_g = 0.0;
		border.inner_g = 0.0;
		border.inner_a = 1.0;

		q4 = 0.249+0.5*(rand()%100*0.01);
		q5 = 0.249+0.5*(rand()%100*0.01);
		q6 = 0.249+0.5*(rand()%100*0.01);

		movement = 0.0;
	}

	virtual void Render(const BeatDetect &music, const PipelineContext &context)
	{
		movement += 0.5*(((music.bass+music.bass_att + 0.075*pow((music.bass+0.6*music.bass_att+0.2*music.treb_att),3)))/(float)context.fps);
	    if (movement > 10000.0)
	    	movement = 0.0;

	    border.outer_r = q4+0.25*sin(movement*3.816);
	    border.outer_g = q4+0.25*sin(movement*0.744);
	    border.outer_b = q4+0.25*sin(movement*0.707);

		if(music.bass+music.bass_att > 3.0) textureWrap = 1;
		else textureWrap = 0;

		vectors.y_offset = 0.03*sin(movement*0.34);
		vectors.x_offset = 0.035*(sin(movement*0.217)+cos(movement*0.413)+sin(movement*0.311));

		dx =0.01*sin(movement*5);
		dy =0.0005*(music.bass+music.bass_att);

	}

	virtual PixelPoint PerPixel(PixelPoint p, const PerPixelContext context)
	{
		float myy = context.y-(0.250025);
		float myx = context.x-0.5;

		Transforms::Zoom(p,context,0.98,1.503744);

		p.x = p.x - (dx + 2*(2*myx*myy));
		p.y = p.y - (dy + 2*((myy*myy) - (myx*myx)));
		return p;
	}

};


typedef NativePreset<RovastarFractopiaFrantic> RovastarFractopiaFranticPreset;

extern "C" RovastarFractopiaFranticPreset * create(const char * url) {
	return new RovastarFractopiaFranticPreset(std::string(url));
}

extern "C" void destroy(RovastarFractopiaFranticPreset * preset) {
    delete preset;
}
