/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
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

//FIXME: these don't have to be global
projectM *globalPM = NULL;
int wvw, wvh, fvw, fvh;
bool fullscreen;

#ifdef LINUX
#include <SDL.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#endif

void renderLoop();

int main(int argc, char **argv)
{

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
	
	globalPM = new projectM(config_filename);
	
	// if started fullscreen, give PM new viewport dimensions
	if (fullscreen)
		globalPM->projectM_resetGL(fvw, fvh);
	
	renderLoop();
	
	// not reached
	
	return 1;
}

void renderLoop()
{
	while (1)
	{
		projectMEvent evt;
		projectMKeycode key;
		projectMModifier mod;
	#if 1
		/** Process SDL events */
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			/** Translate into projectM codes and process */
			evt = sdl2pmEvent(event);
			key = sdl2pmKeycode(event.key.keysym.sym);
			mod = sdl2pmModifier(event.key.keysym.mod);
			
			switch (evt)
			{
				case PROJECTM_KEYDOWN:
					switch (key)
					{
						case PROJECTM_K_ESCAPE:
							delete(globalPM);
							exit(0);
							break;
						case PROJECTM_K_f:
						{
							fullscreen = !fullscreen;
							if (fullscreen)
							{
								resize_display(fvw, fvh, fullscreen);
								globalPM->projectM_resetGL(fvw, fvh);
							} else
							{
								resize_display(wvw, wvh, fullscreen);
								globalPM->projectM_resetGL(wvw, wvh);
							}
							break;
						}
						case PROJECTM_K_q:
							exit(1);
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
		#endif
		globalPM->renderFrame();
		SDL_GL_SwapBuffers();
	}
	
}

