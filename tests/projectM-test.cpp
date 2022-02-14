/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2008 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include "video_init.h"
#include <projectM.hpp>
#include "sdltoprojectM.h"
#include "ConfigFile.h"
#include "getConfigFilename.h"
#include "projectM-opengl.h"
#include <stdlib.h>
#include <cassert>

projectM *globalPM= NULL;

// window stuff
int wvw, wvh, fvw, fvh;
bool fullscreen;

void renderLoop();

// texture test
bool doTextureTest = false;
void textureTest();

// memleak test
bool doMemleakTest = false;
int memLeakIterations = 100;

int main(int argc, char **argv) {

	// fix `fullscreen quit kills mouse` issue.
	atexit(SDL_Quit);

	std::string config_filename = getConfigFilename();
	ConfigFile config(config_filename);

	// window dimensions from configfile
	wvw = config.read<int>("Window Width", 512);
	wvh = config.read<int>("Window Height", 512);
	fullscreen = config.read("Fullscreen", true);

	init_display(wvw, wvh, &fvw, &fvh, fullscreen);

	SDL_WM_SetCaption(PROJECTM_TITLE, NULL);

	// memleak test
	while (doMemleakTest) {
		static int k = 0;
		std::cerr << "[iter " << k++ << "]" << std::endl;
		globalPM = new projectM(config_filename);
		assert(globalPM);
		delete (globalPM);
		if (k >= memLeakIterations)
			break;
	}

	globalPM = new projectM(config_filename);

	// if started fullscreen, give PM new viewport dimensions
	if (fullscreen)
		globalPM->projectM_resetGL(fvw, fvh);

	renderLoop();

	// not reached
	return 1;
}

float fakePCM[512];


void cleanup() {
  delete(globalPM);
  exit(0);
}


void renderLoop() {
	while (1) {
		projectMEvent evt;
		projectMKeycode key;
		projectMModifier mod;

		/** Process SDL events */
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			/** Translate into projectM codes and process */
			evt = sdl2pmEvent(event);
            key = sdl2pmKeycode(event.key.keysym.sym, event.key.keysym.mod);
			mod = sdl2pmModifier(event.key.keysym.mod);

			switch (evt) {
			case PROJECTM_KEYDOWN:
				switch (key) {
				case PROJECTM_K_ESCAPE:
					cleanup();
					break;
				case PROJECTM_K_f: {
					fullscreen = !fullscreen;
					if (fullscreen) {
						resize_display(fvw, fvh, fullscreen);
						globalPM->projectM_resetGL(fvw, fvh);
					} else {
						resize_display(wvw, wvh, fullscreen);
						globalPM->projectM_resetGL(wvw, wvh);
					}
					break;
				}
				case PROJECTM_K_q:
					cleanup();
					break;
				default:
					globalPM->key_handler(evt, key, mod);
				}
				break;

			case PROJECTM_VIDEORESIZE:
				wvw = event.resize.w;
				wvh = event.resize.h;
				resize_display(wvw, wvh, 0);
				globalPM->projectM_resetGL(wvw, wvh);
				break;

			default:
				// not for us, give it to projectM
				globalPM->key_handler(evt, key, mod);
				break;
			}
		}
 fakePCM[0]=0;
        for (int x = 1; x< 512;x++)
        {
                fakePCM[x] = fakePCM[x-1] + (rand()%200 - 100) *.002;
        }


        globalPM->pcm()->addPCMfloat(fakePCM, 512);





		globalPM->renderFrame();

		if (doTextureTest)
			textureTest();

		SDL_GL_SwapBuffers();
	}
}

void textureTest() {
	static int textureHandle = globalPM->initRenderToTexture();
	static int frame = 0;
	frame++;

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (fullscreen)
		glViewport(0, 0, fvw, fvh);
	else
		glViewport(0, 0, wvw, wvh);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 2, 10);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(cos(frame*0.023), cos(frame*0.017), -5+sin(frame*0.022)*2);
	glRotatef(sin(frame*0.0043)*360, sin(frame*0.0017)*360, sin(frame *0.0032)
			*360, 1);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glColor4d(1.0, 1.0, 1.0, 1.0);

	glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
	glVertex3d(-0.8, 0.8, 0);
	glTexCoord2d(0, 0);
	glVertex3d(-0.8, -0.8, 0);
	glTexCoord2d(1, 0);
	glVertex3d(0.8, -0.8, 0);
	glTexCoord2d(1, 1);
	glVertex3d(0.8, 0.8, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
}
