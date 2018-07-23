//
//  pmSDL.hpp
//  SDLprojectM
//
//  Created by Mischa Spiegelmock on 2017-09-18.
//

#ifndef pmSDL_hpp
#define pmSDL_hpp

#include "projectM-opengl.h"
#include <projectM.hpp>
#include <sdltoprojectM.h>
#include <iostream>
#include <sys/stat.h>
#include <SDL2/SDL.h>

// DATADIR_PATH should be set by the root Makefile if this is being
// built with autotools.
#ifndef DATADIR_PATH
    #ifdef DEBUG
        #define DATADIR_PATH "."
        #warning "DATADIR_PATH is not defined - falling back to ./"
    #else
        #define DATADIR_PATH "/usr/local/share/projectM"
        #warning "DATADIR_PATH is not defined - falling back to /usr/local/share/projectM"
    #endif
#endif

const float FPS = 60;

class projectMSDL : public projectM {
public:
    bool done;

    projectMSDL(Settings settings, int flags);
    projectMSDL(std::string config_file, int flags);
    void init(SDL_Window *window, SDL_GLContext *glCtx, const bool renderToTexture = false);
    int openAudioInput();
    void beginAudioCapture();
    void endAudioCapture();
    void toggleFullScreen();
    void resize(unsigned int width, unsigned int height);
    void renderFrame();
    void pollEvent();
    void maximize();
    std::string getActivePresetName();
    void addFakePCM();

private:
    SDL_Window *win;
    SDL_GLContext *glCtx;
    bool isFullScreen;
    projectM::Settings settings;
    SDL_AudioDeviceID audioInputDevice;
    unsigned int width, height;
    bool renderToTexture;
    GLuint programID = 0;
    GLuint m_vbo = 0;
    GLuint m_vao = 0;
    GLuint textureID = 0;

    // audio input device characteristics
    unsigned short audioChannelsCount;
    unsigned short audioSampleRate;
    unsigned short audioSampleCount;
    SDL_AudioFormat audioFormat;
    SDL_AudioDeviceID audioDeviceID;

    static void audioInputCallbackF32(void *userdata, unsigned char *stream, int len);
    static void audioInputCallbackS16(void *userdata, unsigned char *stream, int len);

    void keyHandler(SDL_Event *);
    SDL_AudioDeviceID selectAudioInput(int count);
    void renderTexture();
};


#endif /* pmSDL_hpp */
