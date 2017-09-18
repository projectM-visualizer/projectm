//
//  main.cpp
//  projectM-sdl
//
//  Created by Mischa Spiegelmock on 6/3/15.
//  Copyright (c) 2015 Mischa Spiegelmock. All rights reserved.
//
//  This is an implementation of projectM using libsdl2


#include <SDL2/SDL.h>

#ifdef __linux__
#ifdef USE_GLES1
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <SDL2/SDL.h>
#include <CoreAudio/CoreAudio.h>
#endif

#include <projectM.hpp>
#include <sdltoprojectM.h>
#include <iostream>
const float FPS = 60;

typedef struct {
    projectM *pm;
    SDL_Window *win;
    SDL_Renderer *rend;
    bool done;
    projectM::Settings settings;
    SDL_AudioDeviceID audioInputDevice;
    
    // audio input device characteristics
    unsigned short audioChannelsCount;
    unsigned short audioSampleRate;
    unsigned short audioSampleCount;
    SDL_AudioFormat audioFormat;
    SDL_AudioDeviceID audioDeviceID;
    unsigned char *pcmBuffer;  // pre-allocated buffer for audioInputCallback
} projectMApp;

void audioInputCallbackF32(void *userdata, unsigned char *stream, int len) {
//    printf("LEN: %i\n", len);
    projectMApp *app = (projectMApp *) userdata;
    // stream is (i think) samples*channels floats (native byte order) of len BYTES
    app->pm->pcm()->addPCMfloat((float *)stream, len/sizeof(float));
}

void audioInputCallbackS16(void *userdata, unsigned char *stream, int len) {
//    printf("LEN: %i\n", len);
    projectMApp *app = (projectMApp *) userdata;
    short pcm16[2][512];
    
    for (int i = 0; i < 512; i++) { //
        for (int j = 0; j < app->audioChannelsCount; j++) {
            pcm16[j][i] = stream[i+j];
        }
    }
    app->pm->pcm()->addPCM16(pcm16);
}


int openAudioInput(projectMApp *app) {
    // get audio input device
    int i, count = SDL_GetNumAudioDevices(true);  // capture, please
    if (count == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        SDL_Quit();
    }
    for (i = 0; i < count; i++) {
        SDL_Log("Found audio capture device %d: %s", i, SDL_GetAudioDeviceName(i, true));
    }
    
    // params for audio input
    SDL_AudioSpec want, have;
    
    // TODO: let user somehow select audio input device
    SDL_AudioDeviceID selectedAudioDevice = 0;  // hardcoded to use first device for now :/

    // requested format
    SDL_zero(want);
    want.freq = 48000;
    want.format = AUDIO_F32;  // float
    want.channels = 2;
    want.samples = 512;
    want.callback = audioInputCallbackF32;
    want.userdata = app;

    app->audioDeviceID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(selectedAudioDevice, true), true, &want, &have, 0);

    if (app->audioDeviceID == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        SDL_Quit();
    }

    // read characteristics of opened capture device
    SDL_Log("Opened audio capture device %i: %s", app->audioDeviceID, SDL_GetAudioDeviceName(selectedAudioDevice, true));
    SDL_Log("Sample rate: %i, frequency: %i, channels: %i, format: %i", have.samples, have.freq, have.channels, have.format);
    app->audioChannelsCount = have.channels;
    app->audioSampleRate = have.freq;
    app->audioSampleCount = have.samples;
    app->audioFormat = have.format;

    return 1;
}

void beginAudioCapture(projectMApp *app) {
    // allocate a buffer to store PCM data for feeding in
    unsigned int maxSamples = app->audioChannelsCount * app->audioSampleCount;
    app->pcmBuffer = (unsigned char *) malloc(maxSamples);
    SDL_PauseAudioDevice(app->audioDeviceID, false);
//    app->pm->pcm()->initPCM(maxSamples);
}

void endAudioCapture(projectMApp *app) {
    free(app->pcmBuffer);
    SDL_PauseAudioDevice(app->audioDeviceID, true);
}

void keyHandler(projectMApp *app, SDL_Event *sdl_evt) {
    projectMEvent evt;
    projectMKeycode key;
    projectMModifier mod;
    SDL_Keymod sdl_mod;

    /** Translate into projectM codes and process */
    evt = sdl2pmEvent(sdl_evt);
    key = sdl2pmKeycode(sdl_evt->key.keysym);
    sdl_mod = SDL_GetModState();
    mod = sdl2pmModifier(sdl_mod);
    if (evt == PROJECTM_KEYDOWN) {
        app->pm->key_handler( evt, key, mod );
    }
}

void addFakePCM(projectMApp *app) {
    int i;
    short pcm_data[2][512];
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
}

void renderFrame(projectMApp *app) {
    SDL_Event evt;
    
    SDL_PollEvent(&evt);
    switch (evt.type) {
        case SDL_KEYDOWN:
            keyHandler(app, &evt);
            break;
        case SDL_QUIT:
            app->done = true;
            break;
    }

    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    app->pm->renderFrame();
    glFlush();

    SDL_RenderPresent(app->rend);
}

int main( int argc, char *argv[] ) {
    projectMApp app;
    app.done = 0;

    int width = 784,
        height = 784;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    app.win = SDL_CreateWindow("projectM", 0, 0, width, height, 0);
    app.rend = SDL_CreateRenderer(app.win, 0, SDL_RENDERER_ACCELERATED);
    if (! app.rend) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return PROJECTM_ERROR;
    }
    SDL_SetWindowTitle(app.win, "projectM Visualizer");
    printf("SDL init version 2\n");

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
    app.settings.presetURL = "presets/presets_tryptonaut";
    app.settings.menuFontURL = "fonts/Vera.ttf";
    app.settings.titleFontURL = "fonts/Vera.ttf";
    
    // init projectM
    app.pm = new projectM(app.settings);
    app.pm->selectRandom(true);
    app.pm->projectM_resetGL(width, height);
    
    // get an audio input device
    openAudioInput(&app);
    beginAudioCapture(&app);

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

    return PROJECTM_SUCCESS;
}
