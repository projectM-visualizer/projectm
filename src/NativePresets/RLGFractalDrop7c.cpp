/*
 * RovastarFractalSpiral.hpp
 *
 *  Created on: Jun 22, 2008
 *      Author: pete
 */


#include "Pipeline.hpp"
#include "Transformation.hpp"
#include "MilkdropCompatability.hpp"
#include "VideoEcho.hpp"
#include "NativePreset.hpp"
#include "MilkdropWaveform.hpp"

class RLGFractalDrop7c : public Pipeline
{
public:

	Shape shape1,shape2,shape3,shape4;
	MilkdropWaveform wave;
	VideoEcho videoEcho;

	float movement, t1, t2, t3, t4;

	RLGFractalDrop7c() : Pipeline()
	{
		drawables.push_back(&shape1);
		drawables.push_back(&shape2);
		drawables.push_back(&shape3);
		drawables.push_back(&shape4);
		drawables.push_back(&wave);

		compositeDrawables.push_back(&videoEcho);

		textureWrap = true;
		screenDecay = 1.0;

		videoEcho.orientation = FlipX;
		videoEcho.zoom = 1.006752;
		videoEcho.a = 0.5;

		wave.mode = DoubleLine;
		wave.additive = true;
		wave.scale = 3.815202;
		wave.smoothing = 0.9;
		wave.modOpacityEnd = 1.1;
		wave.modOpacityStart = 0.0;
		wave.maximizeColors = true;
		wave.modulateAlphaByVolume = true;
		wave.r = 0.65;
		wave.g = 0.65;
		wave.b = 1;
		wave.a = 0.1;

		movement = 0;

		t1 = (rand()%100)*0.01;
		t2 = (rand()%100)*0.01;
		t3 = (rand()%100)*0.01;
		t4 = (rand()%100)*0.01;

		shape1.sides =3;
		shape1.x = 0.37;
		shape1.y = 0.5;
		shape1.radius = 6.811289;
		shape1.ang = 3.644249;
		shape1.a = 0.5;
		shape1.a2 = 0.5;
		shape1.border_r = 1;
		shape1.border_g = 1;
		shape1.border_b = 1;
		shape1.border_a = 1;

		shape2.sides =100;
		shape2.additive = true;
		shape2.textured = true;

		shape2.radius = 0.897961;
		shape2.ang = 3.644249;
		shape2.a = 0.5;
		shape2.a2 = 1;

		shape3.sides =100;
		shape3.additive = true;
		shape3.textured = true;
		shape3.radius = 0.513861;
		shape3.ang = 4.209736;
		shape3.a = 1;
		shape3.a2 = 1;

		shape4.sides =100;
		shape4.additive = true;
		shape4.textured = true;
		shape4.ang = 0;
		shape4.a = 1;
		shape4.r = 0.6;
		shape4.g = 0.8;
		shape4.b = 1;
		shape4.a2 = 1;
	}

	virtual void Render(const BeatDetect &music, const PipelineContext &context)
	{
		movement += 0.1*max(0,music.bass+music.bass_att-2) + 0.15*pow(music.bass,3) + 0.005;
		float time = context.time;

		shape1.ang = movement*(0.303 + 0.01*t1);
		shape1.r = min(1,max(0,0+ 0.1*sin(time*0.417 + 1)));
		shape1.g = min(1,max(0,0 + 0.1*sin(time*0.391 + 2)));
		shape1.b = min(1,max(0,0 + 0.1*sin(time*0.432 + 4)));
		shape1.r2 = min(1,max(0, 0.02*sin(time*0.657 + 3)));
		shape1.g2 = min(1,max(0, 0.02*sin(time*0.737 + 5)));
		shape1.b2 = min(1,max(0, 0.02*sin(time*0.884 + 6)));
		shape1.additive = ((0.5+0.15*(music.bass+music.bass_att)) > 1.0) ? true : false;


		shape2.x = 0.37 + 0.07*sin(movement*0.15+3);
		shape2.y = 0.5 + 0.03*sin(movement*0.19+1);
		shape2.tex_ang = movement*(0.01 + 0.0001*t2);
		shape2.r = min(1,max(0, 1 + 0.01*sin(time*0.0417 + 1)));
		shape2.g = min(1,max(0, 1 + 0.01*sin(time*0.391 + 2)));
		shape2.b = min(1,max(0, 1 + 0.01*sin(time*0.432 + 4)));
		shape2.r2 = min(1,max(0, 0.01*sin(time*0.457 + 3)));
		shape2.g2 = min(1,max(0, 0.01*sin(time*0.0437 + 5)));
		shape2.b2 = min(1,max(0, 0.01*sin(time*0.484 + 6)));

		shape3.x = 0.67 + 0.05*sin(movement*0.017);
		shape3.y = 0.43 + 0.09*sin(movement*0.013);
		shape3.tex_ang = movement*(0.02 + 0.0001*t3);
		shape3.radius = 0.222979 * (0.9 + 0.2*t4);
		shape3.r = min(1,max(0,1+ 0.01*sin(time*0.417 + 1)));
		shape3.g = min(1,max(0,1 + 0.01*sin(time*0.391 + 2)));
		shape3.b = min(1,max(0,1 + 0.01*sin(time*0.432 + 4)));
		shape3.r2 = min(1,max(0,0.01*sin(time*0.457 + 3)));
		shape3.g2 = min(1,max(0,0.01*sin(time*0.437 + 5)));
		shape3.b2 = min(1,max(0,0.01*sin(time*0.484 + 6)));

		shape4.x = 0.5 + 0.08*sin(movement*0.25);
		shape4.y = 0.5 + 0.1*sin(movement*0.5+2);
		shape4.ang = time;
	}

	virtual PixelPoint PerPixel(PixelPoint p, const PerPixelContext context)
	{
		Transforms::Zoom(p,context,1.029902,1.00);
		return p;
	}

};


typedef NativePreset<RLGFractalDrop7c> RLGFractalDrop7Preset;

extern "C" RLGFractalDrop7Preset * create(const char * url) {
	return new RLGFractalDrop7Preset(std::string(url));
}

extern "C" void destroy(RLGFractalDrop7Preset * preset) {
    delete preset;
}
