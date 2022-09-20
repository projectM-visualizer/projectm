#include "setup.hpp"

#include <SDL2/SDL_hints.h>

#include <chrono>
#include <cmath>

#if OGL_DEBUG
void debugGL(GLenum source,
             GLenum type,
             GLuint id,
             GLenum severity,
             GLsizei length,
             const GLchar* message,
             const void* userParam) {

    /*if (type != GL_DEBUG_TYPE_OTHER)*/
    {
        std::cerr << " -- \n" << "Type: " <<
        type << "; Source: " <<
        source <<"; ID: " << id << "; Severity: " <<
        severity << "\n" << message << "\n";
    }
}
#endif

// return path to config file to use
std::string getConfigFilePath(std::string datadir_path) {
    char* home = NULL;
    std::string projectM_home;
    std::string projectM_config = DATADIR_PATH;

    projectM_config = datadir_path;

#ifdef _MSC_VER
    home=getenv("USERPROFILE");
#else
    home=getenv("HOME");
#endif

    projectM_home = std::string(home);
    projectM_home += "/.projectM";

    // Create the ~/.projectM directory. If it already exists, mkdir will do nothing
#if defined _MSC_VER
    _mkdir(projectM_home.c_str());
#else
    mkdir(projectM_home.c_str(), 0755);
#endif

    projectM_home += "/config.inp";
    projectM_config += "/config.inp";

    std::ifstream f_home(projectM_home);
    std::ifstream f_config(projectM_config);
    std::cout << "f_home " << f_home.good() << "\n";

    if (f_config.good() && !f_home.good()) {
        std::ifstream f_src;
        std::ofstream f_dst;

        f_src.open(projectM_config, std::ios::in  | std::ios::binary);
        f_dst.open(projectM_home,   std::ios::out | std::ios::binary);
        f_dst << f_src.rdbuf();
        f_dst.close();
        f_src.close();
        return std::string(projectM_home);
    } else if (f_home.good()) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Read ~/.projectM/config.inp\n");
        return std::string(projectM_home);
    } else if (f_config.good()) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Cannot create ~/.projectM/config.inp, using %s\n", projectM_config.c_str());
        return std::string(projectM_config);
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Using implementation defaults, your system is really messed up, I'm surprised we even got this far\n");
        return "";
    }
}


void seedRand() {
#ifndef _WIN32
    srand((int)(time(NULL)));
#endif
}

void initGL() {
#if USE_GLES
    // use GLES 2.0 (this may need adjusting)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    // Disabling compatibility profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
}

void dumpOpenGLInfo() {
    SDL_Log("- GL_VERSION: %s", glGetString(GL_VERSION));
    SDL_Log("- GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    SDL_Log("- GL_VENDOR: %s", glGetString(GL_VENDOR));
}

void initStereoscopicView(SDL_Window *win) {
#if STEREOSCOPIC_SB
    // enable stereo
    if (SDL_GL_SetAttribute(SDL_GL_STEREO, 1) == 0)
    {
        SDL_Log("SDL_GL_STEREO: true");
    }

    // requires fullscreen mode
    SDL_ShowCursor(false);
    SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
#endif
}

void enableGLDebugOutput() {
#if OGL_DEBUG && !USE_GLES
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugGL, NULL);
#endif
}

// initialize SDL, openGL, config
projectMSDL *setupSDLApp() {
    projectMSDL *app;
    seedRand();
        
    if (!initLoopback())
		{
			SDL_Log("Failed to initialize audio loopback devide.");
			exit(1);
		}

#if UNLOCK_FPS
    setenv("vblank_mode", "0", 1);
#endif

#ifdef SDL_HINT_AUDIO_INCLUDE_MONITORS
    SDL_SetHint(SDL_HINT_AUDIO_INCLUDE_MONITORS, "1");
#endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    if (! SDL_VERSION_ATLEAST(2, 0, 5)) {
        SDL_Log("SDL version 2.0.5 or greater is required. You have %i.%i.%i", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
        exit(1);
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

    initGL();

    SDL_Window *win = SDL_CreateWindow("projectM", 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_GL_GetDrawableSize(win,&width,&height);

    initStereoscopicView(win);

    SDL_GLContext glCtx = SDL_GL_CreateContext(win);

#if defined(_WIN32)
	GLenum err = glewInit();
#endif /** _WIN32 */

    dumpOpenGLInfo();

    SDL_SetWindowTitle(win, "projectM");

    SDL_GL_MakeCurrent(win, glCtx);  // associate GL context with main window
    int avsync = SDL_GL_SetSwapInterval(-1); // try to enable adaptive vsync
    if (avsync == -1) { // adaptive vsync not supported
        SDL_GL_SetSwapInterval(1); // enable updates synchronized with vertical retrace
    }

    std::string base_path = DATADIR_PATH;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Using data directory: %s\n", base_path.c_str());

    // load configuration file
    std::string configFilePath = getConfigFilePath(base_path);

    if (! configFilePath.empty()) {
        // found config file, use it
        app = new projectMSDL(glCtx, configFilePath, 0);
        SDL_Log("Using config from %s", configFilePath.c_str());
    } else {
        // use some sane defaults if config file not found
        base_path = SDL_GetBasePath();
        SDL_Log("Config file not found, using built-in settings. Data directory=%s\n", base_path.c_str());

        // Get max refresh rate from attached displays to use as built-in max FPS.
        int i = 0;
        int maxRefreshRate = 0;
        SDL_DisplayMode current;
        for (i = 0; i < SDL_GetNumVideoDisplays(); ++i)
        {
            if (SDL_GetCurrentDisplayMode(i, &current) == 0)
            {
                if (current.refresh_rate > maxRefreshRate) maxRefreshRate = current.refresh_rate;
            }
        }
        if (maxRefreshRate <= 60) maxRefreshRate = 60;

        float heightWidthRatio = (float)height / (float)width;
        std::string presetURL = base_path + "presets";
        std::string menuFontURL = base_path + "presets";
        std::string titleFontURL = base_path + "presets";

        auto settings = projectm_alloc_settings();
        settings->window_width = width;
        settings->window_height = height;
        settings->mesh_x = 128;
        settings->mesh_y = settings->mesh_x * heightWidthRatio;
        settings->fps = maxRefreshRate;
        settings->soft_cut_duration = 3; // seconds
        settings->preset_duration = 22; // seconds
        settings->hard_cut_enabled = true;
        settings->hard_cut_duration = 60;
        settings->hard_cut_sensitivity = 1.0;
        settings->beat_sensitivity = 1.0;
        settings->aspect_correction = 1;
        settings->shuffle_enabled = 1;
        settings->soft_cut_ratings_enabled = 1; // ???
        // get path to our app, use CWD or resource dir for presets/fonts/etc
        settings->preset_path = projectm_alloc_string(presetURL.length() + 1);
        strncpy(settings->preset_path, presetURL.c_str(), presetURL.length());
        // init with settings
        app = new projectMSDL(glCtx, settings, 0);
    }

    // center window and full desktop screen
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
        width = dm.w;
        height = dm.h;
    } else {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
    }
    SDL_SetWindowPosition(win, initialWindowBounds.x, initialWindowBounds.y);
    SDL_SetWindowSize(win, width, height);
    app->resize(width, height);

    // Create a help menu specific to SDL
    std::string modKey = "CTRL";

#if __APPLE__
    modKey = "CMD";
#endif

    app->init(win);

#if STEREOSCOPIC_SBS
    app->toggleFullScreen();
#endif
#if FAKE_AUDIO
    app->fakeAudio  = true;
#endif

    enableGLDebugOutput();
    configureLoopback(app);

#if !FAKE_AUDIO && !WASAPI_LOOPBACK
    // get an audio input device
    if (app->openAudioInput())
        app->beginAudioCapture();
#endif

#if TEST_ALL_PRESETS
    testAllPresets(app);
    return 0;
#endif

    return app;
}

int64_t startUnlockedFPSCounter() {
	using namespace std::chrono;
	auto currentTime = steady_clock::now();
	auto currentTimeMs = time_point_cast<milliseconds>(currentTime);
	auto elapsedMs = currentTime.time_since_epoch();

	return elapsedMs.count();
}

void advanceUnlockedFPSCounterFrame(int64_t startFrame) {
    static int32_t frameCount = 0;

    frameCount++;
	auto currentElapsedMs = startUnlockedFPSCounter();
	if (currentElapsedMs - startFrame > 5000)
	{
        printf("Frames[%d]\n", frameCount);
        exit(0);
    }
}
