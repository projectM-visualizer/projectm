#include "audioCapture.hpp"

int projectMSDL::initAudioInput() {
    // params for audio input
    SDL_AudioSpec want, have;

    // requested format
    // https://wiki.libsdl.org/SDL_AudioSpec#Remarks
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32;  // float
    want.channels = 2;  // mono might be better?
    // lower might reduce latency
    want.samples = PCM::maxsamples;
    want.callback = projectMSDL::audioInputCallbackF32;
    want.userdata = this;

    audioDeviceID = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(selectedAudioDevice, true), true, &want, &have, 0);

    if (audioDeviceID == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        return 0;
    }

    // read characteristics of opened capture device
    SDL_Log("Opened audio capture device index=%i devId=%i: %s", selectedAudioDevice, audioDeviceID, SDL_GetAudioDeviceName(selectedAudioDevice, true));
    std::string deviceToast = SDL_GetAudioDeviceName(selectedAudioDevice, true); // Example: Microphone rear
    deviceToast += " selected";
    projectM::setToastMessage(deviceToast);
#ifdef DEBUG
    SDL_Log("Samples: %i, frequency: %i, channels: %i, format: %i", have.samples, have.freq, have.channels, have.format);
#endif
    audioChannelsCount = have.channels;
    audioSampleRate = have.freq;
    audioSampleCount = have.samples;
    audioFormat = have.format;
    audioInputDevice = audioDeviceID;

    return 1;
}

void projectMSDL::audioInputCallbackF32(void *userdata, unsigned char *stream, int len) {
    projectMSDL *app = (projectMSDL *) userdata;
//    printf("\nLEN: %i\n", len);
//    for (int i = 0; i < 64; i++)
//        printf("%X ", stream[i]);
    // stream is (i think) samples*channels floats (native byte order) of len BYTES
    if (app->audioChannelsCount == 1)
        app->pcm()->addPCMfloat((float *)stream, len/sizeof(float));
    else if (app->audioChannelsCount == 2)
        app->pcm()->addPCMfloat_2ch((float *)stream, len/sizeof(float));
    else {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Multichannel audio not supported");
        SDL_Quit();
    }
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

int projectMSDL::toggleAudioInput() {
    // trigger a toggle with CMD-I or CTRL-I
    if (wasapi) { // we are currently on WASAPI, so we are going to revert to a microphone/line-in input.
        if (this->openAudioInput())
            this->beginAudioCapture();
        CurAudioDevice = 0;
        selectedAudioDevice = CurAudioDevice;
        this->wasapi = false; // Track wasapi as off so projectMSDL will stop listening to WASAPI loopback in pmSDL_main.
    }
    else {
        this->endAudioCapture(); // end current audio capture.
        CurAudioDevice++; // iterate device index
        if (CurAudioDevice >= NumAudioDevices) { // We reached outside the boundaries of available audio devices.
            CurAudioDevice = 0; // Return to first audio device in the index.
#ifdef WASAPI_LOOPBACK
            // If we are at the boundary and WASAPI is enabled then let's load WASAPI instead.
            projectM::setToastMessage("Loopback audio selected");
            SDL_Log("Loopback audio selected");
            this->fakeAudio = false; // disable fakeAudio in case it was enabled.
            this->wasapi = true; // Track wasapi as on so projectMSDL will listen to it.
#else
            if (NumAudioDevices == 1) // If WASAPI_LOOPBACK was not enabled and there is only one audio device, it's pointless to toggle anything.
            {
                SDL_Log("There is only one audio capture device. There is nothing to toggle at this time.");
                return 1;
            }
            // If WASAPI_LOOPBACK is not enabled and we have multiple input devices, return to device index 0 and let's listen to that device.
            selectedAudioDevice = CurAudioDevice;
            initAudioInput();
            this->beginAudioCapture();
#endif
        }
        else {
            // This is a normal scenario where we move forward in the audio device index.
            selectedAudioDevice = CurAudioDevice;
            initAudioInput();
            this->beginAudioCapture();
        }
    }
    return 1;
}

int projectMSDL::openAudioInput() {
    fakeAudio = false; // if we are opening an audio input then there is no need for fake audio.
    // get audio driver name (static)
#ifdef DEBUG
    const char* driver_name = SDL_GetCurrentAudioDriver();
    SDL_Log("Using audio driver: %s\n", driver_name);
#endif

    // get audio input device
    NumAudioDevices = SDL_GetNumAudioDevices(true);  // capture, please

    CurAudioDevice = 0;
    if (NumAudioDevices == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        projectM::setToastMessage("No audio capture devices found: using simulated audio");
        fakeAudio = true;
        return 0;
    }
#ifdef DEBUG
    for (unsigned int i = 0; i < NumAudioDevices; i++) {
        SDL_Log("Found audio capture device %d: %s", i, SDL_GetAudioDeviceName(i, true));
    }
#endif

    // default selected Audio Device to 0.
    selectedAudioDevice = 0;
    initAudioInput();

    return 1;
}

void projectMSDL::beginAudioCapture() {
    // allocate a buffer to store PCM data for feeding in
    SDL_PauseAudioDevice(audioDeviceID, false);
}

void projectMSDL::endAudioCapture() {
    SDL_PauseAudioDevice(audioDeviceID, true);
    SDL_CloseAudioDevice(audioDeviceID);
}
