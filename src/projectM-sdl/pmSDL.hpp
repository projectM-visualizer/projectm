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

#include "projectM-opengl.h"
#include <projectM.hpp>
#include <sdltoprojectM.h>
#include <iostream>
#include <sys/stat.h>


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
        #ifdef WIN32
            #define DATADIR_PATH std::string(SDL_GetBasePath()) + "../../../src\\projectM-sdl";
        #else
            #define DATADIR_PATH "/usr/local/share/projectM"
        #endif
    #endif /** WIN32 */
//    #warning "DATADIR_PATH is not defined - falling back to /usr/local/share/projectM"
#endif

class projectMSDL : public projectM {
public:


    bool done;
    projectMSDL(Settings settings, int flags);
    projectMSDL(std::string config_file, int flags);
    void init(SDL_Window *window, SDL_GLContext *glCtx, const bool renderToTexture = false);
    int openAudioInput();
    int toggleAudioInput();
    int initAudioInput();
    void beginAudioCapture();
    void endAudioCapture();
	void stretchMonitors();
	void nextMonitor();
    void toggleFullScreen();
    void resize(unsigned int width, unsigned int height);
    void renderFrame();
    void pollEvent();
    void maximize();
    std::string getActivePresetName();
    void addFakePCM();
    
    //Holds text input information
	struct inputText
	{
		bool isOn = false;
		bool isRendering = false;
		std::string text = "";
		void reset()
		{
			isOn = false;
			//isRendering = false;
			text = "";
		}
	} input;

    virtual void presetSwitchedEvent(bool isHardCut, size_t index) const;

private:
    SDL_Window *win;
    SDL_GLContext *glCtx;
    bool isFullScreen;
    SDL_AudioDeviceID audioInputDevice;
    unsigned int width, height;
    bool renderToTexture;
    GLuint programID = 0;
    GLuint m_vbo = 0;
    GLuint m_vao = 0;
    GLuint textureID = 0;

    // audio input device characteristics
    unsigned int NumAudioDevices;
    unsigned int CurAudioDevice;
    unsigned short audioChannelsCount;
    unsigned short audioSampleRate;
    unsigned short audioSampleCount;
    SDL_AudioFormat audioFormat;
    SDL_AudioDeviceID audioDeviceID;
    SDL_AudioDeviceID selectedAudioDevice;

    static void audioInputCallbackF32(void *userdata, unsigned char *stream, int len);
    static void audioInputCallbackS16(void *userdata, unsigned char *stream, int len);

    void keyHandler(SDL_Event *);
    SDL_AudioDeviceID selectAudioInput(int _count);
    void renderTexture();
};


#endif /* pmSDL_hpp */
