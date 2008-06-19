/*
 * RovastarFractopiaFrantic.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#include "Pipeline.hpp"
#include "math.h"
#include <stdlib.h>


class RovastarFranticFractopia : public Pipeline
{
public:

	PerPixelDelta delta;
	Zoom zoom;

	Border border;
	MotionVectors vectors;

	float q4, q5, q6;

	float movement;

	RovastarFranticFractopia(int mesh_x, int mesh_y) : Pipeline(mesh_x, mesh_y),
		delta(mesh_x,mesh_y,0,0),
		zoom(mesh_x, mesh_y, 0.98, 1.503744)
	{
		drawables.push_back(&vectors);
		drawables.push_back(&border);

		perPixelTransforms.push_back(&zoom);
		perPixelTransforms.push_back(&delta);

		screenDecay = 1.0;

		videoEchoOrientation = 0;
		videoEchoZoom = 1.0;
		videoEchoAlpha = 1.0;

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

	virtual void Render()
	{

		movement += 0.5*(((bass+bass_att + 0.075*pow((bass+0.6*bass_att+0.2*treb_att),3)))/(float)fps);
	    if (movement > 10000.0)
	    	movement = 0.0;

	    border.outer_r = q4+0.25*sin(movement*3.816);
	    border.outer_g = q4+0.25*sin(movement*0.744);
	    border.outer_b = q4+0.25*sin(movement*0.707);

		if(bass+bass_att > 3.0) textureWrap = 1;
		else textureWrap = 0;

		vectors.y_offset = 0.03*sin(movement*0.34);
		vectors.x_offset = 0.035*(sin(movement*0.217)+cos(movement*0.413)+sin(movement*0.311));

		float dx =0.01*sin(movement*5);
		float dy =0.0005*(bass+bass_att);

		for (int x = 0; x < mesh_x; x++)
			for (int y = 0; y < mesh_y; y++)
			{
				float xval=x/(float)(mesh_x-1);
				float yval=-((y/(float)(mesh_y-1))-1);

				float myy = yval-(0.250025);
				float myx = xval-0.5;
				delta.dx[x][y] = dx + 2*(2*myx*myy);
				delta.dy[x][y] = dy + 2*((myy*myy) - (myx*myx));
			}
	}

};
