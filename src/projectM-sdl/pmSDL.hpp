/**
* projectM -- Milkdrop-esque visualisation SDK
* Copyright (C)2003-2019 projectM Team
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
* See 'LICENSE.txt' included within this release
*
* projectM-sdl
* This is an implementation of projectM using libSDL2
* 
* pmSDL.hpp 
* Authors: Created by Mischa Spiegelmock on 2017-09-18.
*
*/


#ifndef pmSDL_hpp
#define pmSDL_hpp


// Disable LOOPBACK and FAKE audio to enable microphone input
#ifdef WIN32
#define WASAPI_LOOPBACK     1
#endif /** WIN32 */
#define FAKE_AUDIO          0
// ----------------------------
#define TEST_ALL_PRESETS    0
#define STEREOSCOPIC_SBS    0

// projectM
#include <projectM-opengl.h>
#include <projectM.h>
#include <sdltoprojectM.h>

// projectM SDL
#include "setup.hpp"
#include "loopback.hpp"
#include "audioCapture.hpp"


#if defined _MSC_VER
#  include <direct.h>
#endif

#include <fstream>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef WASAPI_LOOPBACK
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>

#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)
#define ERR(format, ...) LOG(L"Error: " format, __VA_ARGS__)

#endif /** WASAPI_LOOPBACK */

#ifdef WIN32
#define SDL_MAIN_HANDLED
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif /** WIN32 */


// DATADIR_PATH should be set by the root Makefile if this is being
// built with autotools.
#ifndef DATADIR_PATH
    #ifdef DEBUG
        #define DATADIR_PATH "."
        #warning "DATADIR_PATH is not defined - falling back to ./"
    #else
        #define DATADIR_PATH "/usr/local/share/projectM"
#ifndef WIN32
        #warning "DATADIR_PATH is not defined - falling back to /usr/local/share/projectM"
#endif /** WIN32 */
    #endif
#endif

class projectMSDL {
    
public:
    projectMSDL(SDL_GLContext glCtx, projectm_settings* settings, int flags);
    projectMSDL(SDL_GLContext glCtx, const std::string& config_file, int flags);

    ~projectMSDL();

    void init(SDL_Window *window, const bool renderToTexture = false);
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
    void setHelpText(const std::string& theValue);
    void renderFrame();
    void pollEvent();
    bool keymod = false;
    std::string getActivePresetName();
    void addFakePCM();
    projectm_handle projectM();
    const projectm_settings* settings();

    bool done{ false };
    bool mouseDown{ false };
    bool wasapi{ false }; // Used to track if wasapi is currently active. This bool will allow us to run a WASAPI app and still toggle to microphone inputs.
    bool fakeAudio{ false }; // Used to track fake audio, so we can turn it off and on.
    bool stretch{ false }; // used for toggling stretch mode

    SDL_GLContext glCtx{ nullptr };

private:
    static void presetSwitchedEvent(bool isHardCut, unsigned int index, void* context);

    static void audioInputCallbackF32(void *userdata, unsigned char *stream, int len);
    static void audioInputCallbackS16(void *userdata, unsigned char *stream, int len);

    void scrollHandler(SDL_Event *);
    void keyHandler(SDL_Event *);

    projectm_handle _projectM{ nullptr };
    projectm_settings* _settings{ nullptr };

    SDL_Window *win{ nullptr };
    bool isFullScreen{ false };
    SDL_AudioDeviceID audioInputDevice{ 0 };
    size_t width{ 0 };
    size_t height{ 0 };
    GLuint programID{ 0 };
    GLuint m_vbo{ 0 };
    GLuint m_vao{ 0 };
    GLuint textureID{ 0 };

    // audio input device characteristics
    unsigned int NumAudioDevices{ 0 };
    int CurAudioDevice{ 0 };            // SDL's device indexes are 0-based, -1 means "system default"
    unsigned short audioChannelsCount{ 0 };
    unsigned short audioSampleRate{ 0 };
    unsigned short audioSampleCount{ 0 };
    SDL_AudioFormat audioFormat{ 0 };
    SDL_AudioDeviceID audioDeviceID{ 0 };
    int selectedAudioDevice{ 0 };
};


#endif /* pmSDL_hpp */
