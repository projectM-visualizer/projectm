#include "audioCapture.hpp"
#include "pmSDL.hpp"


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
    want.samples = projectm_pcm_get_max_samples();
    want.callback = projectMSDL::audioInputCallbackF32;
    want.userdata = this;

    // index -1 means "system deafult", which is used if we pass deviceName == NULL
    const char *deviceName = selectedAudioDevice == -1 ? NULL : SDL_GetAudioDeviceName(selectedAudioDevice, true);
    audioDeviceID = SDL_OpenAudioDevice(deviceName, true, &want, &have, 0);

    if (audioDeviceID == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        return 0;
    }

    // read characteristics of opened capture device
    if(deviceName == NULL)
        deviceName = "<System default capture device>";
    SDL_Log("Opened audio capture device index=%i devId=%i: %s", selectedAudioDevice, audioDeviceID, deviceName);
    std::string deviceToast = deviceName; // Example: Microphone rear
    deviceToast += " selected";
    projectm_set_toast_message(_projectM, deviceToast.c_str());
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
        projectm_pcm_add_float_1ch_data(app->_projectM, reinterpret_cast<float*>(stream), len/sizeof(float));
    else if (app->audioChannelsCount == 2)
        projectm_pcm_add_float_2ch_data(app->_projectM, reinterpret_cast<float*>(stream), len/sizeof(float));
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
    projectm_pcm_add_16bit_2ch_512(app->_projectM, pcm16);
}

int projectMSDL::toggleAudioInput() {
    // trigger a toggle with CMD-I or CTRL-I
    if (wasapi) { // we are currently on WASAPI, so we are going to revert to a microphone/line-in input.
        if (this->openAudioInput())
            this->beginAudioCapture();
        CurAudioDevice = -1;        // start from system default device
        selectedAudioDevice = CurAudioDevice;
        this->wasapi = false; // Track wasapi as off so projectMSDL will stop listening to WASAPI loopback in pmSDL_main.
    }
    else {
        this->endAudioCapture(); // end current audio capture.
        CurAudioDevice++; // iterate device index
        if (CurAudioDevice >= (int)NumAudioDevices) { // We reached outside the boundaries of available audio devices.
            CurAudioDevice = -1; // Return to the default audio device.
#ifdef WASAPI_LOOPBACK
            // If we are at the boundary and WASAPI is enabled then let's load WASAPI instead.
            projectm_set_toast_message(_projectM, "Loopback audio selected");
            SDL_Log("Loopback audio selected");
            this->fakeAudio = false; // disable fakeAudio in case it was enabled.
            this->wasapi = true; // Track wasapi as on so projectMSDL will listen to it.
#else
            if (NumAudioDevices == 0) // If WASAPI_LOOPBACK was not enabled and there is only the default audio device, it's pointless to toggle anything.
            {
                SDL_Log("Only the default audio capture device is available. There is nothing to toggle at this time.");
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

#ifdef DEBUG
    for (unsigned int i = 0; i < NumAudioDevices; i++) {
        SDL_Log("Found audio capture device %d: %s", i, SDL_GetAudioDeviceName(i, true));
    }
#endif

    // We start with the system default capture device (index -1).
    // Note: this might work even if NumAudioDevices == 0 (example: if only a
    // monitor device exists, and SDL_HINT_AUDIO_INCLUDE_MONITORS is not set).
    // So we always try it, and revert to fakeAudio if the default fails _and_ NumAudioDevices == 0.
    CurAudioDevice = -1;
    selectedAudioDevice = -1;
    if(!initAudioInput() && NumAudioDevices == 0) {
        // the default device doesn't work, and there's no other device to try
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        projectm_set_toast_message(_projectM, "No audio capture devices found: using simulated audio");
        fakeAudio = true;
        return 0;
    }

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
