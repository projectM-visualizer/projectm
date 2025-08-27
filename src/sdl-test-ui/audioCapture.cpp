#include "audioCapture.hpp"
#include "pmSDL.hpp"


int projectMSDL::initAudioInput()
{

    SDL_AudioSpec want, have;



    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = want.freq / 60;
    want.callback = projectMSDL::audioInputCallbackF32;
    want.userdata = this;


    const char* deviceName = _selectedAudioDevice == -1 ? NULL : SDL_GetAudioDeviceName(_selectedAudioDevice, true);
    _audioDeviceId = SDL_OpenAudioDevice(deviceName, true, &want, &have, 0);

    if (_audioDeviceId == 0)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        return 0;
    }


    if (deviceName == NULL)
        deviceName = "<System default capture device>";
    SDL_Log("Opened audio capture device index=%i devId=%i: %s", _selectedAudioDevice, _audioDeviceId, deviceName);
    std::string deviceToast = deviceName;
    deviceToast += " selected";
#ifdef DEBUG
    SDL_Log("Samples: %i, frequency: %i, channels: %i, format: %i", have.samples, have.freq, have.channels, have.format);
#endif
    _audioChannelsCount = have.channels;

    return 1;
}

void projectMSDL::audioInputCallbackF32(void* userdata, unsigned char* stream, int len)
{
    projectMSDL* app = (projectMSDL*) userdata;




    if (app->_audioChannelsCount == 1)
        projectm_pcm_add_float(app->_projectM, reinterpret_cast<float*>(stream), len / sizeof(float) / 2, PROJECTM_MONO);
    else if (app->_audioChannelsCount == 2)
        projectm_pcm_add_float(app->_projectM, reinterpret_cast<float*>(stream), len / sizeof(float) / 2, PROJECTM_STEREO);
    else
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Multichannel audio not supported");
        SDL_Quit();
    }
}

int projectMSDL::toggleAudioInput()
{

    if (wasapi)
    {
        if (this->openAudioInput())
            this->beginAudioCapture();
        _curAudioDevice = -1;
        _selectedAudioDevice = _curAudioDevice;
        this->wasapi = false;
    }
    else
    {
        this->endAudioCapture();
        _curAudioDevice++;
        if (_curAudioDevice >= (int) _numAudioDevices)
        {
            _curAudioDevice = -1;
#ifdef WASAPI_LOOPBACK

            SDL_Log("Loopback audio selected");
            this->fakeAudio = false;
            this->wasapi = true;
#else
            if (_numAudioDevices == 0)
            {
                SDL_Log("Only the default audio capture device is available. There is nothing to toggle at this time.");
                return 1;
            }

            _selectedAudioDevice = _curAudioDevice;
            initAudioInput();
            this->beginAudioCapture();
#endif
        }
        else
        {

            _selectedAudioDevice = _curAudioDevice;
            initAudioInput();
            this->beginAudioCapture();
        }
    }
    return 1;
}

int projectMSDL::openAudioInput()
{
    fakeAudio = false;

#ifdef DEBUG
    const char* driver_name = SDL_GetCurrentAudioDriver();
    SDL_Log("Using audio driver: %s\n", driver_name);
#endif


    _numAudioDevices = SDL_GetNumAudioDevices(true);

#ifdef DEBUG
    for (unsigned int i = 0; i < _numAudioDevices; i++)
    {
        SDL_Log("Found audio capture device %d: %s", i, SDL_GetAudioDeviceName(i, true));
    }
#endif





    _curAudioDevice = -1;
    _selectedAudioDevice = -1;
    if (!initAudioInput() && _numAudioDevices == 0)
    {

        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        fakeAudio = true;
        return 0;
    }

    return 1;
}

void projectMSDL::beginAudioCapture()
{

    SDL_PauseAudioDevice(_audioDeviceId, false);
}

void projectMSDL::endAudioCapture()
{
    SDL_PauseAudioDevice(_audioDeviceId, true);
    SDL_CloseAudioDevice(_audioDeviceId);
}
