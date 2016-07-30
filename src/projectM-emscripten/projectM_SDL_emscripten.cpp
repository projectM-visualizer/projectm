/**
 * Emscripten port of projectm-SDLvis
 *  Mischa Spiegelmock, 2014
 *
 */

#include <math.h>
#include <projectM.hpp>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include "SDL.h"
#elif EMSCRIPTEN
#include <emscripten.h>
#include <GL/gl.h>
#include <SDL.h>
#endif

const float FPS = 60;

typedef struct {
    projectM *pm;
    SDL_Window *win;
    SDL_Renderer *rend;
    bool done;
    projectM::Settings settings;
    SDL_AudioDeviceID audioInputDevice;
} projectMApp;

int selectAudioInput(projectMApp *app) {
    int i, count = SDL_GetNumAudioDevices(0);  // param=isCapture (not yet functional)
    
    if (! count) {
        fprintf(stderr, "No audio input capture devices detected\n");
        return 0;
    }
    
    printf("count: %d\n", count);
    for (i = 0; i < count; ++i) {
        printf("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
    }
    
    return 1;
}

void renderFrame(projectMApp *app) {
    int i;
    short pcm_data[2][512];
    SDL_Event evt;
    
    SDL_PollEvent(&evt);
    switch (evt.type) {
        case SDL_KEYDOWN:
            // ...
            break;
        case SDL_QUIT:
            app->done = true;
            break;
    }


//        projectMEvent evt;
//        projectMKeycode key;
//        projectMModifier mod;
//
//        /** Process SDL events */
//        SDL_Event event;
//        while ( SDL_PollEvent( &event ) ) {
//            /** Translate into projectM codes and process */
//            evt = sdl2pmEvent( event );
//            key = sdl2pmKeycode( event.key.keysym.sym );
//            mod = sdl2pmModifier( (SDLMod)event.key.keysym.mod );
//            if ( evt == PROJECTM_KEYDOWN ) {
//                pm->key_handler( evt, key, mod );
//              }
//          }

    /** Produce some fake PCM data to stuff into projectM */
    for ( i = 0 ; i < 512 ; i++ ) {
        if ( i % 2 == 0 ) {
            pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
            pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
          } else {
            pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
            pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
          }
        if ( i % 2 == 1 ) {
            pcm_data[0][i] = -pcm_data[0][i];
            pcm_data[1][i] = -pcm_data[1][i];
          }
    }

    /** Add the waveform data */
    app->pm->pcm()->addPCM16(pcm_data);

    glClearColor( 0.0, 0.5, 0.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    app->pm->renderFrame();
    glFlush();

    #if SDL_MAJOR_VERSION==2
        SDL_RenderPresent(app->rend);
    #elif SDL_MAJOR_VERSION==1
        SDL_GL_SwapBuffers();
    #endif
}

int main( int argc, char *argv[] ) {
    projectMApp app;
    app.done = 0;

    int width = 784,
        height = 784;


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    // get an audio input device
    if (! selectAudioInput(&app)) {
        fprintf(stderr, "Failed to open audio input device\n");
        return 1;
    }

    // SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    // SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    // SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    // SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    // SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    #if SDL_MAJOR_VERSION==2
        app.win = SDL_CreateWindow("SDL Fun Party Time", 50, 50, width, height, 0);
        app.rend = SDL_CreateRenderer(app.win, 0, SDL_RENDERER_ACCELERATED);
        if (! app.rend) {
            fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
            return PROJECTM_ERROR;
        }
        SDL_SetWindowTitle(app.win, "SDL Fun Party Time");
        printf("SDL init version 2\n");
    #elif SDL_MAJOR_VERSION==1
        screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_DOUBLEBUF);
        printf("SDL init version 1\n");
        if (! screen) {
            fprintf(stderr, "Failed to create renderer with SDL_SetVideoMode(): %s\n", SDL_GetError());
            return PROJECTM_ERROR;
        }
    #endif
    
    #ifdef PANTS
    if ( fsaa ) {
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
        printf( "SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value );
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value );
        printf( "SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa, value );
    }
    #endif

    app.settings.meshX = 1;
    app.settings.meshY = 1;
    app.settings.fps   = FPS;
    app.settings.textureSize = 2048;  // idk?
    app.settings.windowWidth = width;
    app.settings.windowHeight = height;
    app.settings.smoothPresetDuration = 3; // seconds
    app.settings.presetDuration = 5; // seconds
    app.settings.beatSensitivity = 0.8;
    app.settings.aspectCorrection = 1;
    app.settings.easterEgg = 0; // ???
    app.settings.shuffleEnabled = 1;
    app.settings.softCutRatingsEnabled = 1; // ???
#ifdef EMSCRIPTEN
    app.settings.presetURL = "/build/presets";
#else
    app.settings.presetURL = "presets_tryptonaut";
    app.settings.menuFontURL = "fonts/Vera.ttf";
    app.settings.titleFontURL = "fonts/Vera.ttf";
#endif
    
    // init projectM
    app.pm = new projectM(app.settings);
    printf("init projectM\n");
    app.pm->selectRandom(true);
    printf("select random\n");
    app.pm->projectM_resetGL(width, height);
    printf("resetGL\n");

    // mainloop. non-emscripten version here for comparison/testing
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(renderFrame, 0, 0);
#else
    // standard main loop
    const Uint32 frame_delay = 1000/FPS;
    Uint32 last_time = SDL_GetTicks();
    while (! app.done) {
        renderFrame(&app);
        Uint32 elapsed = SDL_GetTicks() - last_time;
        if (elapsed < frame_delay)
            SDL_Delay(frame_delay - elapsed);
        last_time = SDL_GetTicks();
    }
#endif

    return PROJECTM_SUCCESS;
}
