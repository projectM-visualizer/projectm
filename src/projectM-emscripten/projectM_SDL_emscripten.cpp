/**
 * Emscripten port of projectm-SDLvis
 *  Mischa Spiegelmock, 2014
 *
 */

#include <math.h>
#include <projectM.h>

#include <emscripten.h>
#include <GL/gl.h>
#include <SDL.h>

#include <string>

#include <dirent.h>

const float FPS = 60;

typedef struct {
    projectm_handle pm;
    SDL_Window *win;
    SDL_Renderer *rend;
    bool done;
    projectm_settings settings;
    SDL_AudioDeviceID audioInputDevice;
} projectMApp;

projectMApp app;

int selectAudioInput(projectMApp *application) {
    int i, count = SDL_GetNumAudioDevices(1);

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
            app.done = true;
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
    projectm_pcm_add_16bit_2ch_512(app.pm, pcm_data);

    glClearColor( 0.0, 0.5, 0.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectm_render_frame(app.pm);
    glFlush();

    SDL_RenderPresent(app.rend);
}

int main( int argc, char *argv[] ) {
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

    app.win = SDL_CreateWindow("SDL Fun Party Time", 50, 50, width, height, 0);
    app.rend = SDL_CreateRenderer(app.win, 0, SDL_RENDERER_ACCELERATED);
    if (! app.rend) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetWindowTitle(app.win, "SDL Fun Party Time");
    printf("SDL init version 2\n");


    #ifdef PANTS
    if ( fsaa ) {
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
        printf( "SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value );
        SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value );
        printf( "SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa, value );
    }
    #endif
    app.settings.mesh_x = 48;
    app.settings.mesh_y = 32;
    app.settings.fps   = FPS;
    app.settings.texture_size = 2048;  // idk?
    app.settings.window_width = width;
    app.settings.window_height = height;
    app.settings.soft_cut_duration = 3; // seconds
    app.settings.preset_duration = 5; // seconds
    app.settings.beat_sensitivity = 0.8;
    app.settings.aspect_correction = 1;
    app.settings.easter_egg = 0; // ???
    app.settings.shuffle_enabled = 1;
    app.settings.soft_cut_ratings_enabled = 1; // ???
    app.settings.preset_url = projectm_alloc_string(8);
    strncpy(app.settings.preset_url, "presets", 8);

    // init projectM
    app.pm = projectm_create_settings(&(app.settings), PROJECTM_FLAG_NONE);
    printf("init projectM\n");
    projectm_select_random_preset(app.pm, true);
    printf("select random\n");
    projectm_set_window_size(app.pm, width, height);
    printf("resetGL\n");

    // Allocate a new a stream given the current directory name
    DIR * m_dir;
    if ((m_dir = opendir("/")) == NULL)
    {
        printf("error opening /\n");
    } else {

		struct dirent * dir_entry;
		while ((dir_entry = readdir(m_dir)) != NULL)
		{
			printf("%s\n", dir_entry->d_name);
		}
	}

    auto playlistSize = projectm_get_playlist_size(app.pm);
    for(unsigned int i = 0; i < playlistSize; i++) {
        auto presetName = projectm_get_preset_name(app.pm, i);
        printf("%u\t%s\n", i, presetName);
        projectm_free_string(presetName);
    }

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

    return 0;
}
