//
//  pmSDL.cpp
//  SDLprojectM
//
//  Created by Mischa Spiegelmock on 2017-09-18.
//

#include "pmSDL.hpp"

void projectMSDL::audioInputCallbackF32(void *userdata, unsigned char *stream, int len) {
    projectMSDL *app = (projectMSDL *) userdata;
    //    printf("LEN: %i\n", len);
    // stream is (i think) samples*channels floats (native byte order) of len BYTES
    app->pcm()->addPCMfloat((float *)stream, len/sizeof(float));
}

void projectMSDL::audioInputCallbackS16(void *userdata, unsigned char *stream, int len) {
    //    printf("LEN: %i\n", len);
    projectMSDL *app = (projectMSDL *) userdata;
    short pcm16[2][512];

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < app->audioChannelsCount; j++) {
            pcm16[j][i] = stream[i+j];
        }
    }
    app->pcm()->addPCM16(pcm16);
}

SDL_AudioDeviceID projectMSDL::selectAudioInput(int count) {
    // ask the user which capture device to use
    // printf("Please select which audio input to use:\n");
    printf("Detected devices:\n");
    for (int i = 0; i < count; i++) {
        printf("  %i: 🎤%s\n", i, SDL_GetAudioDeviceName(i, true));
    }

    return 0;
}

int projectMSDL::openAudioInput() {
    // get audio driver name (static)
    const char* driver_name = SDL_GetCurrentAudioDriver();
    SDL_Log("Using audio driver: %s\n", driver_name);

    // get audio input device
    int i, count = SDL_GetNumAudioDevices(true);  // capture, please
    if (count == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        SDL_Quit();
    }
    for (i = 0; i < count; i++) {
        SDL_Log("Found audio capture device %d: %s", i, SDL_GetAudioDeviceName(i, true));
    }

    SDL_AudioDeviceID selectedAudioDevice = 0;  // device to open
    if (count > 1) {
        // need to choose which input device to use
        selectedAudioDevice = selectAudioInput(count);
        if (selectedAudioDevice > count) {
            SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio input device specified.");
            SDL_Quit();
        }
    }

    // params for audio input
    SDL_AudioSpec want, have;

    // requested format
    SDL_zero(want);
    want.freq = 48000;
    want.format = AUDIO_F32;  // float
    want.channels = 2;
    want.samples = 512;
    want.callback = projectMSDL::audioInputCallbackF32;
    want.userdata = this;

    audioDeviceID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(selectedAudioDevice, true), true, &want, &have, 0);

    if (audioDeviceID == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        SDL_Quit();
    }

    // read characteristics of opened capture device
    SDL_Log("Opened audio capture device %i: %s", audioDeviceID, SDL_GetAudioDeviceName(selectedAudioDevice, true));
    SDL_Log("Sample rate: %i, frequency: %i, channels: %i, format: %i", have.samples, have.freq, have.channels, have.format);
    audioChannelsCount = have.channels;
    audioSampleRate = have.freq;
    audioSampleCount = have.samples;
    audioFormat = have.format;
    audioInputDevice = audioDeviceID;
    return 1;
}

void projectMSDL::beginAudioCapture() {
    // allocate a buffer to store PCM data for feeding in
    unsigned int maxSamples = audioChannelsCount * audioSampleCount;
    pcmBuffer = (unsigned char *) malloc(maxSamples);
    SDL_PauseAudioDevice(audioDeviceID, false);
    pcm()->initPCM(2048);
}

void projectMSDL::endAudioCapture() {
    free(pcmBuffer);
    SDL_PauseAudioDevice(audioDeviceID, true);
}

void projectMSDL::maximize() {
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return;
    }

    SDL_SetWindowSize(win, dm.w, dm.h);
    resize(dm.w, dm.h);
}

void projectMSDL::toggleFullScreen() {
    maximize();
    if (isFullScreen) {
        SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        isFullScreen = false;
        SDL_ShowCursor(true);
    } else {
        SDL_ShowCursor(false);
        SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
        isFullScreen = true;
    }
}

void projectMSDL::keyHandler(SDL_Event *sdl_evt) {
    projectMEvent evt;
    projectMKeycode key;
    projectMModifier mod;
    SDL_Keymod sdl_mod = (SDL_Keymod) sdl_evt->key.keysym.mod;
    SDL_Keycode sdl_keycode = sdl_evt->key.keysym.sym;

    // handle keyboard input (for our app first, then projectM)
    switch (sdl_keycode) {
        case SDLK_f:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL) {
                // command-f: fullscreen
                toggleFullScreen();
                return; // handled
            }
            break;
    }

    // translate into projectM codes and perform default projectM handler
    evt = sdl2pmEvent(sdl_evt);
    key = sdl2pmKeycode(sdl_keycode);
    mod = sdl2pmModifier(sdl_mod);
    key_handler(evt, key, mod);
}

void projectMSDL::addFakePCM() {
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
    pcm()->addPCM16(pcm_data);
}

void projectMSDL::resize(unsigned int width_, unsigned int height_) {
    width = width_;
    height = height_;
    settings.windowWidth = width;
    settings.windowHeight = height;
    projectM_resetGL(width, height);
}

void projectMSDL::pollEvent() {
    SDL_Event evt;

    SDL_PollEvent(&evt);
    switch (evt.type) {
        case SDL_WINDOWEVENT:
            switch (evt.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    resize(evt.window.data1, evt.window.data2);
                    break;
            }
            break;
        case SDL_KEYDOWN:
            keyHandler(&evt);
            break;
        case SDL_QUIT:
            done = true;
            break;
    }
}

void projectMSDL::renderFrame() {
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectM::renderFrame();
    glFlush();

    SDL_RenderPresent(rend);
}

projectMSDL::projectMSDL(Settings settings, int flags) : projectM(settings, flags) {
    width = getWindowWidth();
    height = getWindowHeight();
    done = 0;
    isFullScreen = false;
}

projectMSDL::projectMSDL(std::string config_file, int flags) : projectM(config_file, flags) {
    width = getWindowWidth();
    height = getWindowHeight();
    done = 0;
    isFullScreen = false;
}

void projectMSDL::init(SDL_Window *window, SDL_Renderer *renderer) {
    win = window;
    rend = renderer;
    selectRandom(true);
    projectM_resetGL(width, height);
}
