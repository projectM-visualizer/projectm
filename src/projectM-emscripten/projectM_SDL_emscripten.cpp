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

const char *PROJECTEM_CONFIG_PATH = "build/config.inp";

static projectM *globalPM = NULL;
static SDL_Surface *screen;
static SDL_Window *win;
static SDL_Renderer *rend;

bool done = false;

void renderFrame() {
    int i;
    short pcm_data[2][512];
    SDL_Event evt;
    
    SDL_PollEvent(&evt);
    switch (evt.type) {
        case SDL_KEYDOWN:
            // ...
            break;
        case SDL_QUIT:
            done = true;
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
    globalPM->pcm()->addPCM16(pcm_data);

    glClearColor( 0.0, 0.5, 0.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    globalPM->renderFrame();
    glFlush();

    #if SDL_MAJOR_VERSION==2
        SDL_RenderPresent(rend);
    #elif SDL_MAJOR_VERSION==1
        SDL_Flip(screen);
    #endif
}


int main( int argc, char *argv[] ) {
    int width = 784,
        height = 784;


    SDL_Init(SDL_INIT_VIDEO);

    //  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    //  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    //  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );

    //SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 8 );
    //  SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    // SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    // SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    #if SDL_MAJOR_VERSION==2
        win = SDL_CreateWindow("SDL Fun Party Time", 50, 50, width, height, 0);
        rend = SDL_CreateRenderer(win, 0, SDL_RENDERER_ACCELERATED);
        if (! rend) {
            fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
            return PROJECTM_ERROR;
        }
        SDL_SetWindowTitle(win, "SDL Fun Party Time");
        printf("SDL init version 2\n");
    #elif SDL_MAJOR_VERSION==1
        screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF);
        printf("SDL init version 1\n");
    #endif
    
    #ifdef PANTS
    if ( fsaa ) {
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
        printf( "SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value );
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value );
        printf( "SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa, value );
    }
    #endif

    // init projectM
    globalPM = new projectM(PROJECTEM_CONFIG_PATH);
    globalPM->selectRandom(true);
    globalPM->projectM_resetGL(width, height);

    // mainloop. non-emscripten version here for comparison/testing
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(renderFrame, 30, 0);
#else
    // standard main loop
    const Uint32 frame_delay = 1000/60;
    Uint32 last_time = SDL_GetTicks();
    while (! done) {
        renderFrame();
        Uint32 elapsed = SDL_GetTicks() - last_time;
        if (elapsed < frame_delay)
            SDL_Delay(frame_delay - elapsed);
        last_time = SDL_GetTicks();
    }
#endif

    return PROJECTM_SUCCESS;
}
