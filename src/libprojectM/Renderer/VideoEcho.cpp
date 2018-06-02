/*
 * VideoEcho.cpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#include "VideoEcho.hpp"

VideoEcho::VideoEcho(): a(0), zoom(1), orientation(Normal)
{
	// TODO Auto-generated constructor stub

}

VideoEcho::~VideoEcho()
{
	// TODO Auto-generated destructor stub
}

void VideoEcho::Draw(RenderContext &context)
{
		float tex[4][2] = {{0, 1},
				   {0, 0},
				   {1, 0},
				   {1, 1}};

		float points[4][2] = {{-0.5, -0.5},
				      {-0.5,  0.5},
				      { 0.5,  0.5},
				      { 0.5,  -0.5}};

#ifndef GL_TRANSITION
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2,GL_FLOAT,0,points);
		glTexCoordPointer(2,GL_FLOAT,0,tex);
#endif
    
		//Now Blend the Video Echo
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef GL_TRANSITION
		glMatrixMode(GL_TEXTURE);

		//draw video echo
		glColor4f(1.0, 1.0, 1.0, a * masterAlpha);
		glTranslatef(.5, .5, 0);
		glScalef(1.0/zoom, 1.0/zoom, 1);
		glTranslatef(-.5, -.5, 0);
    
		int flipx=1, flipy=1;
		switch (orientation)
		{
			case Normal: flipx=1;flipy=1;break;
			case FlipX: flipx=-1;flipy=1;break;
			case FlipY: flipx=1;flipy=-1;break;
			case FlipXY: flipx=-1;flipy=-1;break;
			default: flipx=1;flipy=1; break;
		}

		double pointsFlip[4][2] = {{-0.5*flipx, -0.5*flipy},
					  {-0.5*flipx,  0.5*flipy},
					  { 0.5*flipx,  0.5*flipy},
					  { 0.5*flipx, -0.5*flipy}};

		glVertexPointer(2,GL_FLOAT,0,pointsFlip);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

}
