//
//  pmSDL.hpp
//  SDLprojectM
//
//  Created by Mischa Spiegelmock on 2017-09-18.
//  Copyright © 2017 MVS Technical Group Inc. All rights reserved.
//

#ifndef pmSDL_hpp
#define pmSDL_hpp



#ifdef __linux__
#ifdef USE_GLES1
#include <GLES/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#include <projectM.hpp>
#include <sdltoprojectM.h>
#include <iostream>

const float FPS = 60;

class projectMSDL : projectM {
public:
    bool done;

    projectMSDL(Settings settings, int flags = FLAG_NONE);
    void init(SDL_Window *window, SDL_Renderer *renderer);
    int openAudioInput();
    void beginAudioCapture();
    void endAudioCapture();
    void toggleFullScreen();
    void resize(unsigned int width, unsigned int height);
    void renderFrame();
    void pollEvent();
    void maximize();

private:
    SDL_Window *win;
    SDL_Renderer *rend;
    bool isFullScreen;
    projectM::Settings settings;
    SDL_AudioDeviceID audioInputDevice;
    unsigned int width, height;

    // audio input device characteristics
    unsigned short audioChannelsCount;
    unsigned short audioSampleRate;
    unsigned short audioSampleCount;
    SDL_AudioFormat audioFormat;
    SDL_AudioDeviceID audioDeviceID;
    unsigned char *pcmBuffer;  // pre-allocated buffer for audioInputCallback

    static void audioInputCallbackF32(void *userdata, unsigned char *stream, int len);
    static void audioInputCallbackS16(void *userdata, unsigned char *stream, int len);


    void addFakePCM();
    void keyHandler(SDL_Event *);
    SDL_AudioDeviceID selectAudioInput(int count);
};


#endif /* pmSDL_hpp */
