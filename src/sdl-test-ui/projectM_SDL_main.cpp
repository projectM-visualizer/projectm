

#include "pmSDL.hpp"

static int mainLoop(void* userData)
{
    projectMSDL** appRef = (projectMSDL**) userData;
    auto app = *appRef;

#if UNLOCK_FPS
    auto start = startUnlockedFPSCounter();
#endif

    
    int fps = app->fps();
    if (fps <= 0)
        fps = 60;
    const Uint32 frame_delay = 1000 / fps;
    Uint32 last_time = SDL_GetTicks();

    
    while (!app->done)
    {
        
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

int main(int argc, char* argv[])
{
    projectMSDL* app = setupSDLApp();

    int status = mainLoop(&app);


    SDL_GL_DeleteContext(app->_openGlContext);
#if !FAKE_AUDIO
    if (!app->wasapi)
        app->endAudioCapture();
#endif
    delete app;

    return status;
}
