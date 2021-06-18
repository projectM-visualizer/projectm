/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

// video_init.c - SDL/Opengl Windowing Creation/Resizing Functions
//
// by Peter Sperl
//
// Opens an SDL Window and creates an OpenGL session
// also able to handle resizing and fullscreening of windows
// just call init_display again with differant variables

#include <projectM.hpp>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

extern SDL_Window *screen;
extern SDL_Renderer *renderer;

extern int texsize;
void setup_opengl(int w, int h);

void close_display()
{
	SDL_Quit();
}

void resize_display(int w, int h, int f)
{
	/* Information about the current video settings. */
	SDL_DisplayMode info;

	//  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	if (SDL_GetWindowDisplayMode(screen, &info) != 0)
	{
		/* This should probably never happen. */
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		//    projectM_vtable.disable_plugin (&projectM_vtable);
		return;
	}

	info.w = w;
	info.h = h;

	if (SDL_SetWindowDisplayMode(screen, &info))
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		return;
	}

	setup_opengl(w, h);
	SDL_ShowCursor(f ? SDL_DISABLE : SDL_ENABLE);
}

// init_display
//
// Sets screen to new width and height (w,h)
// Also switches between fullscreen and windowed
// with the boolean f (fullscreen)
void init_display(int w, int h, int *fvw, int *fvh, int fullscreen)
{

	int bpp = 0;
	/* Flags we will pass into SDL_SetVideoMode. */
	int flags = 0;
	/* First, initialize SDL's video subsystem. */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		/* Failed, exit. */
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		// projectM_vtable.disable_plugin (&projectM_vtable);
		return;
	}

	screen = SDL_CreateWindow(PROJECTM_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
														fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	if (screen == nullptr)
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		return;
	}

	renderer = SDL_CreateRenderer(screen, -1, 0);

	/* Information about the current video settings. */
	SDL_DisplayMode info;

	/* Let's get some video information. */
	if (SDL_GetWindowDisplayMode(screen, &info) != 0)
	{
		/* This should probably never happen. */
		fprintf(stderr, "Video mode query failed: %s\n", SDL_GetError());
		//    projectM_vtable.disable_plugin (&projectM_vtable);
		return;
	}

	printf("Screen Resolution: %d x %d\n", info.w, info.h);
	*fvw = info.w;
	*fvh = info.h;

	// SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	// SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	// SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );

	// SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 8 );
	//  SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 8 );
	//  SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_SetWindowFullscreen(screen, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0)
	{
		/*
		 * This could happen for a variety of reasons,
		 * including DISPLAY not being set, the specified
		 * resolution not being available, etc.
		 */
		fprintf(stderr, "Fullscreen mode set failed: %s\n", SDL_GetError());

		return;
	}

	info.w = w;
	info.h = h;

	if (SDL_SetWindowDisplayMode(screen, &info) != 0)
	{
		/*
		 * This could happen for a variety of reasons,
		 * including DISPLAY not being set, the specified
		 * resolution not being available, etc.
		 */
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());

		// projectM_vtable.disable_plugin (&projectM_vtable);
		return;
	}

	setup_opengl(w,h);
}

void setup_opengl(int w, int h)
{

	/* Our shading model--Gouraud (smooth). */
	glShadeModel(GL_SMOOTH);
	/* Culling. */
	//    glCullFace( GL_BACK );
	//    glFrontFace( GL_CCW );
	//    glEnable( GL_CULL_FACE );
	/* Set the clear color. */
	glClearColor(0, 0, 0, 0);
	/* Setup our viewport. */
	glViewport(0, 0, w, h);
	/*
	 * Change to the projection matrix and set
	 * our viewing volume.
	 */
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	//    gluOrtho2D(0.0, (GLfloat) width, 0.0, (GLfloat) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//    glFrustum(0.0, height, 0.0,width,10,40);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,texsize,texsize,0);
	// glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,texsize,texsize);
	glLineStipple(2, 0xAAAA);
}
