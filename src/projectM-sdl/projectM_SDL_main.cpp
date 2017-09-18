//
//  main.cpp
//  projectM-sdl
//
//  Created by Mischa Spiegelmock on 6/3/15.
//  Copyright (c) 2015 Mischa Spiegelmock. All rights reserved.
//
//  This is an implementation of projectM using libSDL2

#include <SDL2/SDL.h>

#include "pmSDL.hpp"

int main( int argc, char *argv[] ) {
    projectM::Settings settings;

    settings.windowWidth = 1024;
    settings.windowHeight = 768;
    settings.meshX = 1;
    settings.meshY = 1;
    settings.fps   = FPS;
    settings.textureSize = 2048;  // idk?
    settings.smoothPresetDuration = 3; // seconds
    settings.presetDuration = 7; // seconds
    settings.beatSensitivity = 0.8;
    settings.aspectCorrection = 1;
    settings.easterEgg = 0; // ???
    settings.shuffleEnabled = 1;
    settings.softCutRatingsEnabled = 1; // ???
    settings.presetURL = "presets/presets_tryptonaut";
    settings.menuFontURL = "fonts/Vera.ttf";
    settings.titleFontURL = "fonts/Vera.ttf";
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    auto win = SDL_CreateWindow("projectM", 0, 0, settings.windowWidth, settings.windowHeight, SDL_WINDOW_RESIZABLE);
    auto rend = SDL_CreateRenderer(win, 0, SDL_RENDERER_ACCELERATED);
    if (! rend) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_Quit();
    }
    SDL_SetWindowTitle(win, "projectM Visualizer");

    auto *app = new projectMSDL(settings, 0);
    
    app->init(win, rend);
    
    // get an audio input device
    app->openAudioInput();
    app->beginAudioCapture();
    
    // standard main loop
    const Uint32 frame_delay = 1000/FPS;
    Uint32 last_time = SDL_GetTicks();
    while (! app->done) {
        app->renderFrame();
        app->pollEvent();
        Uint32 elapsed = SDL_GetTicks() - last_time;
        if (elapsed < frame_delay)
            SDL_Delay(frame_delay - elapsed);
        last_time = SDL_GetTicks();
    }
    
    return PROJECTM_SUCCESS;
}
