#include "setup.hpp"

#include <projectM-4/logging.h>
#include "ConfigFile.h"

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
    #ifdef _WIN32
        mkdir(projectM_home.c_str());
    #else
        mkdir(projectM_home.c_str(), 0755);
    #endif
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
#ifdef USE_GLES
    // use GLES 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    // Disabling compatibility profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
#if OGL_DEBUG && !defined (USE_GLES)
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugGL, NULL);
#endif
}

namespace {
void logMessage(const char* message, projectm_log_level severity, void* userData)
{
    switch (severity)
    {
        case PROJECTM_LOG_LEVEL_FATAL:
        case PROJECTM_LOG_LEVEL_ERROR:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            break;
        case PROJECTM_LOG_LEVEL_WARN:
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            break;
        case PROJECTM_LOG_LEVEL_TRACE:
        case PROJECTM_LOG_LEVEL_DEBUG:
            // redirect debug logs to info for now
            //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            //break;
        case PROJECTM_LOG_LEVEL_NOTSET:
        case PROJECTM_LOG_LEVEL_INFO:
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message);
            break;
    }
}
}

// initialize SDL, openGL, config
projectMSDL *setupSDLApp() {
    projectMSDL *app;
    seedRand();

    projectm_set_log_callback(&logMessage, false, nullptr);
    projectm_set_log_level(PROJECTM_LOG_LEVEL_DEBUG, false);

    if (!initLoopback())
		{
			SDL_Log("Failed to initialize audio loopback device.");
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

    SDL_SetWindowTitle(win, "projectM");

    SDL_GL_MakeCurrent(win, glCtx);  // associate GL context with main window
    int avsync = SDL_GL_SetSwapInterval(-1); // try to enable adaptive vsync
    if (avsync == -1) { // adaptive vsync not supported
        SDL_GL_SetSwapInterval(1); // enable updates synchronized with vertical retrace
    }

#ifdef USE_GLES
    if (!gladLoadGLES2(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress))) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading GLAD for GLES2\n");
    }
#else
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress))) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error loading GLAD for GL Core Profile\n");
    }
#endif

    std::string base_path = DATADIR_PATH;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Using data directory: %s\n", base_path.c_str());

    // load configuration file
    std::string configFilePath = getConfigFilePath(base_path);
    std::string presetURL = base_path + "/presets";

    app = new projectMSDL(glCtx, presetURL);

    if (! configFilePath.empty())
    {
        // found config file, load it
        SDL_Log("Using config from %s", configFilePath.c_str());

        ConfigFile config(configFilePath);
        auto* projectMHandle = app->projectM();

        projectm_set_mesh_size(projectMHandle, config.read<uint32_t>("Mesh X", 32), config.read<uint32_t>("Mesh Y", 24));
        SDL_SetWindowSize(win, config.read<uint32_t>("Window Width", 1024), config.read<uint32_t>("Window Height", 768));
        projectm_set_soft_cut_duration(projectMHandle, config.read<double>("Smooth Preset Duration", config.read<int>("Smooth Transition Duration", 3)));
        projectm_set_preset_duration(projectMHandle, config.read<double>("Preset Duration", 30));
        projectm_set_easter_egg(projectMHandle, config.read<float>("Easter Egg Parameter", 0.0));
        projectm_set_hard_cut_enabled(projectMHandle,  config.read<bool>("Hard Cuts Enabled", false));
        projectm_set_hard_cut_duration(projectMHandle, config.read<double>("Hard Cut Duration", 60));
        projectm_set_hard_cut_sensitivity(projectMHandle, config.read<float>("Hard Cut Sensitivity", 1.0));
        projectm_set_beat_sensitivity(projectMHandle, config.read<float>("Beat Sensitivity", 1.0));
        projectm_set_aspect_correction(projectMHandle, config.read<bool>("Aspect Correction", true));
        projectm_set_fps(projectMHandle, config.read<int32_t>("FPS", 60));

        app->setFps(config.read<uint32_t>("FPS", 60));
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
