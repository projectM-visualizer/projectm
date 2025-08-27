

#pragma once

#include <SDL2/SDL.h>


#ifdef _WIN32
#define WASAPI_LOOPBACK 1
#endif
#define FAKE_AUDIO 0

#define TEST_ALL_PRESETS 0
#define STEREOSCOPIC_SBS 0


#include <projectM-4/playlist.h>
#include <projectM-4/projectM.h>


#include "audioCapture.hpp"
#include "loopback.hpp"
#include "opengl.h"
#include "setup.hpp"


#if defined _MSC_VER
#include <direct.h>
#endif

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <sys/stat.h>

#ifdef WASAPI_LOOPBACK
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <windows.h>

#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>

#include <mmsystem.h>
#include <stdio.h>


#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)
#define ERR(format, ...) LOG(L"Error: " format, __VA_ARGS__)

#endif

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif




#ifndef DATADIR_PATH
#ifdef DEBUG
#define DATADIR_PATH "."
#ifndef _WIN32
#warning "DATADIR_PATH is not defined - falling back to ./"
#else
#pragma warning "DATADIR_PATH is not defined - falling back to ./"
#endif
#else
#define DATADIR_PATH "/usr/local/share/projectM"
#ifndef _WIN32
#warning "DATADIR_PATH is not defined - falling back to /usr/local/share/projectM"
#endif
#endif
#endif

class projectMSDL
{

public:
    projectMSDL(SDL_GLContext glCtx, const std::string& presetPath);

    ~projectMSDL();

    void init(SDL_Window* window, const bool renderToTexture = false);
    int openAudioInput();
    int toggleAudioInput();
    int initAudioInput();
    void beginAudioCapture();
    void endAudioCapture();
    void stretchMonitors();
    void nextMonitor();
    void toggleFullScreen();
    void resize(unsigned int width, unsigned int height);
    void touch(float x, float y, int pressure, int touchtype = 0);
    void touchDrag(float x, float y, int pressure);
    void touchDestroy(float x, float y);
    void touchDestroyAll();
    void renderFrame();
    void pollEvent();
    bool keymod = false;
    std::string getActivePresetName();
    void addFakePCM();
    projectm_handle projectM();
    void setFps(size_t fps);
    size_t fps() const;

    bool done{false};
    bool mouseDown{false};
    bool wasapi{false};
    bool fakeAudio{false};
    bool stretch{false};

    SDL_GLContext _openGlContext{nullptr};

private:
    static void presetSwitchedEvent(bool isHardCut, uint32_t index, void* context);

    static void audioInputCallbackF32(void* userdata, unsigned char* stream, int len);

    void UpdateWindowTitle();

    void scrollHandler(SDL_Event*);
    void keyHandler(SDL_Event*);

    projectm_handle _projectM{nullptr};
    projectm_playlist_handle _playlist{nullptr};

    SDL_Window* _sdlWindow{nullptr};
    bool _isFullScreen{false};
    size_t _width{0};
    size_t _height{0};
    size_t _fps{60};

    bool _shuffle{true};


    unsigned int _numAudioDevices{0};
    int _curAudioDevice{0};
    unsigned short _audioChannelsCount{0};
    SDL_AudioDeviceID _audioDeviceId{0};
    int _selectedAudioDevice{0};

    std::string _presetName;
};
