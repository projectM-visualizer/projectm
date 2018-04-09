//
//  main.cpp
//  projectM-sdl
//
//  Created by Mischa Spiegelmock on 6/3/15.
//
//  This is an implementation of projectM using libSDL2

#include "pmSDL.hpp"

// return path to config file to use
std::string getConfigFilePath() {
    const char *path = DATADIR_PATH;
    struct stat sb;
    
    // check if datadir exists.
    // it should exist if this application was installed. if not, assume we're developing it and use currect directory
    if (stat(path, &sb) != 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Could not open datadir path %s.\n", DATADIR_PATH);
    }
    
    std::string configFilePath = path;
    configFilePath += "/config.inp";
    
    // check if config file exists
    if (stat(configFilePath.c_str(), &sb) != 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "No config file %s found. Using development settings.\n", configFilePath.c_str());
        return "";
    }
    
    return configFilePath;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    if (! SDL_VERSION_ATLEAST(2, 0, 5)) {
        SDL_Log("SDL version 2.0.5 or greater is required. You have %i.%i.%i", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        return 1;
    }

    // default window size to usable bounds (e.g. minus menubar and dock)
    SDL_Rect initialWindowBounds;
#if SDL_VERSION_ATLEAST(2, 0, 5)
    // new and better
    SDL_GetDisplayUsableBounds(0, &initialWindowBounds);
#else
    SDL_GetDisplayBounds(0, &initialWindowBounds);
#endif
    int width = initialWindowBounds.w;
    int height = initialWindowBounds.h;

    SDL_Window *win = SDL_CreateWindow("projectM", 0, 0, width, height, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *rend = SDL_CreateRenderer(win, 0, SDL_RENDERER_ACCELERATED);
    if (! rend) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_Quit();
    }
    SDL_SetWindowTitle(win, "projectM Visualizer");
    
    projectMSDL *app;
    
    // load configuration file
    std::string configFilePath = getConfigFilePath();
    
    if (! configFilePath.empty()) {
        // found config file, use it
        app = new projectMSDL(configFilePath, 0);
    } else {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Config file not found, using development settings\n");
        float heightWidthRatio = (float)height / (float)width;
        projectM::Settings settings;
        settings.windowWidth = width;
        settings.windowHeight = height;
        settings.meshX = 300;
        settings.meshY = settings.meshX * heightWidthRatio;
        settings.fps   = FPS;
        settings.smoothPresetDuration = 3; // seconds
        settings.presetDuration = 7; // seconds
        settings.beatSensitivity = 0.8;
        settings.aspectCorrection = 1;
        settings.shuffleEnabled = 1;
        settings.softCutRatingsEnabled = 1; // ???
        // get path to our app, use CWD for presets/fonts/etc
        std::string base_path = SDL_GetBasePath();
        settings.presetURL = base_path + "presets/presets_tryptonaut";
        settings.menuFontURL = base_path + "fonts/Vera.ttf";
        settings.titleFontURL = base_path + "fonts/Vera.ttf";
        // init with settings
        app = new projectMSDL(settings, 0);
    }
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
    
    delete app;

    return PROJECTM_SUCCESS;
}
