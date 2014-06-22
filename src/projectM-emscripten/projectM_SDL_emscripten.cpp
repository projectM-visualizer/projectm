/**
 * Emscripten port of projectm-SDLvis
 *  Mischa Spiegelmock, 2014
 *
 */

#include <math.h>
#include <projectM.hpp>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <SDL2/SDL.h>
#elif EMSCRIPTEN
#include <emscripten.h>
#include <GL/gl.h>
#include <SDL.h>
#endif

//#include "emscripten_sdltoprojectM.h"

projectM *globalPM = NULL;

int dumpFrame = 0;
int frameNumber = 0;

extern void addPCM16(short [2][512]);

void renderFrame() {

    int i;
    short pcm_data[2][512];

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
    globalPM->pcm()->addPCM16(pcm_data);

    glClearColor( 0.0, 0.5, 0.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    globalPM->renderFrame();

    glFlush();
}


int main( int argc, char *argv[] ) {
    int width = 784,
        height = 784;
    SDL_Surface *screen;
    SDL_Window *win;
    SDL_Renderer *rend;

    SDL_Init(SDL_INIT_VIDEO);

    //  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    //  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    //  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );

    //SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 8 );
    //  SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    //  w = 512; h = 512; bpp = 16;
    #if SDL_MAJOR_VERSION==2
    if (! SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_BORDERLESS, &win, &rend)) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return PROJECTM_ERROR;
    }
    #elif SDL_MAJOR_VERSION==1
        screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_HWSURFACE) ;
    #endif

    if (screen == NULL) {
        return PROJECTM_ERROR;
    }

    #ifdef PANTS
    if ( fsaa ) {
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
        printf( "SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value );
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value );
        printf( "SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa, value );
    }
    #endif

    globalPM = (projectM *)malloc( sizeof( projectM ) );

    //    globalPM->renderTarget->texsize = 1024;

    //    globalPM->fontURL = (char *)malloc( sizeof( char ) * 512 );
    //    strcpy( globalPM->fontURL, "/etc/projectM/fonts" );
    //    
    //    globalPM->presetURL = (char *)malloc( sizeof( char ) * 512 );
    //    strcpy( globalPM->presetURL, "/etc/projectM/presets" );

    globalPM->selectRandom(true);

    globalPM->projectM_resetGL( width, height );

    // mainloop. non-emscripten version here for comparison/testing
    #ifdef EMSCRIPTEN
    emscripten_set_main_loop(renderFrame, 30, 0);
    #else
    Uint32 frame_start, frame_time;
    const Uint32 frame_length = 1000/60;
    while (1) {
        frame_start = SDL_GetTicks();
        renderFrame();
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_length > frame_time) {
            SDL_Delay(frame_length - frame_time);
        }
    }
    #endif

    return PROJECTM_SUCCESS;
}
