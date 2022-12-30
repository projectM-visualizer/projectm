/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2021 projectM Team
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
 * projectM-sdl
 * This is an implementation of projectM using libSDL2
 *
 * main.cpp
 * Authors: Created by Mischa Spiegelmock on 6/3/15.
 *
 *
 *	RobertPancoast77@gmail.com :
 * experimental Stereoscopic SBS driver functionality
 * WASAPI looback implementation
 *
 *
 */

#include "pmSDL.hpp"

static int mainLoop(void *userData) {
    projectMSDL **appRef = (projectMSDL **)userData;
    auto app = *appRef;
    
#if UNLOCK_FPS
    auto start = startUnlockedFPSCounter();
#endif
    
    // frame rate limiter
    int fps = app->fps();
    if (fps <= 0)
        fps = 60;
    const Uint32 frame_delay = 1000/fps;
    Uint32 last_time = SDL_GetTicks();
    
    // loop
    while (! app->done) {
        // render
        app->renderFrame();
        
        if (app->fakeAudio)
            app->addFakePCM();
        processLoopbackFrame(app);
        
#if UNLOCK_FPS
        advanceUnlockedFPSCounterFrame(start);
#else
        app->pollEvent();
        Uint32 elapsed = SDL_GetTicks() - last_time;
        if (elapsed < frame_delay)
            SDL_Delay(frame_delay - elapsed);
        last_time = SDL_GetTicks();
#endif
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    projectMSDL *app = setupSDLApp();
    
    int status = mainLoop(&app);

    // cleanup
    SDL_GL_DeleteContext(app->_openGlContext);
#if !FAKE_AUDIO
    if (!app->wasapi) // not currently using WASAPI, so we need to endAudioCapture.
        app->endAudioCapture();
#endif
    delete app;
    
    return status;
}


