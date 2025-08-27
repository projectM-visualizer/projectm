

#include "pmSDL.hpp"

#include <vector>

projectMSDL::projectMSDL(SDL_GLContext glCtx, const std::string& presetPath)
    : _openGlContext(glCtx)
    , _projectM(projectm_create())
    , _playlist(projectm_playlist_create(_projectM))
{
    projectm_get_window_size(_projectM, &_width, &_height);
    projectm_playlist_set_preset_switched_event_callback(_playlist, &projectMSDL::presetSwitchedEvent, static_cast<void*>(this));
    projectm_playlist_add_path(_playlist, presetPath.c_str(), true, false);
    projectm_playlist_set_shuffle(_playlist, _shuffle);
}

projectMSDL::~projectMSDL()
{
    projectm_playlist_destroy(_playlist);
    _playlist = nullptr;
    projectm_destroy(_projectM);
    _projectM = nullptr;
}


void projectMSDL::stretchMonitors()
{
    int displayCount = SDL_GetNumVideoDisplays();
    if (displayCount >= 2)
    {
        std::vector<SDL_Rect> displayBounds;
        for (int i = 0; i < displayCount; i++)
        {
            displayBounds.push_back(SDL_Rect());
            SDL_GetDisplayBounds(i, &displayBounds.back());
        }

        int mostXLeft = 0;
        int mostXRight = 0;
        int mostYUp = 0;
        int mostYDown = 0;

        for (int i = 0; i < displayCount; i++)
        {
            if (displayBounds[i].x < mostXLeft)
            {
                mostXLeft = displayBounds[i].x;
            }
            if ((displayBounds[i].x + displayBounds[i].w) > mostXRight)
            {
                mostXRight = displayBounds[i].x + displayBounds[i].w;
            }
        }
        for (int i = 0; i < displayCount; i++)
        {
            if (displayBounds[i].y < mostYUp)
            {
                mostYUp = displayBounds[i].y;
            }
            if ((displayBounds[i].y + displayBounds[i].h) > mostYDown)
            {
                mostYDown = displayBounds[i].y + displayBounds[i].h;
            }
        }

        int mostWide = abs(mostXLeft) + abs(mostXRight);
        int mostHigh = abs(mostYUp) + abs(mostYDown);

        SDL_SetWindowPosition(_sdlWindow, mostXLeft, mostYUp);
        SDL_SetWindowSize(_sdlWindow, mostWide, mostHigh);
    }
}


void projectMSDL::nextMonitor()
{
    int displayCount = SDL_GetNumVideoDisplays();
    int currentWindowIndex = SDL_GetWindowDisplayIndex(_sdlWindow);
    if (displayCount >= 2)
    {
        std::vector<SDL_Rect> displayBounds;
        int nextWindow = currentWindowIndex + 1;
        if (nextWindow >= displayCount)
        {
            nextWindow = 0;
        }

        for (int i = 0; i < displayCount; i++)
        {
            displayBounds.push_back(SDL_Rect());
            SDL_GetDisplayBounds(i, &displayBounds.back());
        }
        SDL_SetWindowPosition(_sdlWindow, displayBounds[nextWindow].x, displayBounds[nextWindow].y);
        SDL_SetWindowSize(_sdlWindow, displayBounds[nextWindow].w, displayBounds[nextWindow].h);
    }
}

void projectMSDL::toggleFullScreen()
{
    if (_isFullScreen)
    {
        SDL_SetWindowFullscreen(_sdlWindow, 0);
        _isFullScreen = false;
        SDL_ShowCursor(true);
    }
    else
    {
        SDL_ShowCursor(false);
        SDL_SetWindowFullscreen(_sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        _isFullScreen = true;
    }
}

void projectMSDL::scrollHandler(SDL_Event* sdl_evt)
{

    if (sdl_evt->wheel.y > 0)
    {
        projectm_playlist_play_previous(_playlist, true);
    }

    if (sdl_evt->wheel.y < 0)
    {
        projectm_playlist_play_next(_playlist, true);
    }
}

void projectMSDL::keyHandler(SDL_Event* sdl_evt)
{
    SDL_Keymod sdl_mod = (SDL_Keymod) sdl_evt->key.keysym.mod;
    SDL_Keycode sdl_keycode = sdl_evt->key.keysym.sym;


    if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
    {
        keymod = true;
    }


    switch (sdl_keycode)
    {
        case SDLK_a:
            projectm_set_aspect_correction(_projectM, !projectm_get_aspect_correction(_projectM));
            break;

        case SDLK_q:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
            {

                done = 1;
                return;
            }
            break;

        case SDLK_i:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
            {
                toggleAudioInput();
                return;
            }
            break;

        case SDLK_s:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
            {


#if !STEREOSCOPIC_SBS
                if (!this->stretch)
                {
                    stretchMonitors();
                    this->stretch = true;
                }
                else
                {
                    toggleFullScreen();
                    this->stretch = false;
                }
#endif
                return;
            }

        case SDLK_m:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
            {


#if !STEREOSCOPIC_SBS
                nextMonitor();
#endif
                this->stretch = false;
                return;
            }

        case SDLK_f:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
            {


#if !STEREOSCOPIC_SBS
                toggleFullScreen();
#endif
                this->stretch = false;
                return;
            }
            break;

        case SDLK_r:

            projectm_playlist_set_shuffle(_playlist, true);
            projectm_playlist_play_next(_playlist, true);
            projectm_playlist_set_shuffle(_playlist, _shuffle);
            break;

        case SDLK_y:
            _shuffle = !_shuffle;
            projectm_playlist_set_shuffle(_playlist, _shuffle);
            break;

        case SDLK_LEFT:
            projectm_playlist_play_previous(_playlist, true);
            break;

        case SDLK_RIGHT:
            projectm_playlist_play_next(_playlist, true);
            break;

        case SDLK_UP:
            projectm_set_beat_sensitivity(_projectM, projectm_get_beat_sensitivity(_projectM) + 0.01f);
            break;

        case SDLK_DOWN:
            projectm_set_beat_sensitivity(_projectM, projectm_get_beat_sensitivity(_projectM) - 0.01f);
            break;

        case SDLK_SPACE:
            projectm_set_preset_locked(_projectM, !projectm_get_preset_locked(_projectM));
            UpdateWindowTitle();
            break;
    }
}

void projectMSDL::addFakePCM()
{
    int i;
    int16_t pcm_data[2 * 512];

    for (i = 0; i < 512; i++)
    {
        if (i % 2 == 0)
        {
            pcm_data[2 * i] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
            pcm_data[2 * i + 1] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
        }
        else
        {
            pcm_data[2 * i] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
            pcm_data[2 * i + 1] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
        }
        if (i % 2 == 1)
        {
            pcm_data[2 * i] = -pcm_data[2 * i];
            pcm_data[2 * i + 1] = -pcm_data[2 * i + 1];
        }
    }


    projectm_pcm_add_int16(_projectM, pcm_data, 512, PROJECTM_STEREO);
}

void projectMSDL::resize(unsigned int width_, unsigned int height_)
{
    _width = width_;
    _height = height_;


    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) == 0)
    {
        SDL_ShowCursor(_isFullScreen ? SDL_DISABLE : SDL_ENABLE);
    }

    projectm_set_window_size(_projectM, _width, _height);
}

void projectMSDL::pollEvent()
{
    SDL_Event evt;

    int mousex = 0;
    float mousexscale = 0;
    int mousey = 0;
    float mouseyscale = 0;
    int mousepressure = 0;
    while (SDL_PollEvent(&evt))
    {
        switch (evt.type)
        {
            case SDL_WINDOWEVENT:
                int h, w;
                SDL_GL_GetDrawableSize(_sdlWindow, &w, &h);
                switch (evt.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        resize(w, h);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        resize(w, h);
                        break;
                }
                break;
            case SDL_MOUSEWHEEL:
                scrollHandler(&evt);

            case SDL_KEYDOWN:
                keyHandler(&evt);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (evt.button.button == SDL_BUTTON_LEFT)
                {

                    if (!mouseDown)
                    {

                        SDL_GetMouseState(&mousex, &mousey);

                        mousexscale = (mousex / (float) _width);
                        mouseyscale = ((_height - mousey) / (float) _height);

                        touch(mousexscale, mouseyscale, mousepressure);
                        mouseDown = true;
                    }
                }
                else if (evt.button.button == SDL_BUTTON_RIGHT)
                {
                    mouseDown = false;


                    if (keymod)
                    {
                        touchDestroyAll();
                        keymod = false;
                        break;
                    }


                    SDL_GetMouseState(&mousex, &mousey);


                    mousexscale = (mousex / (float) _width);
                    mouseyscale = ((_height - mousey) / (float) _height);


                    touchDestroy(mousexscale, mouseyscale);
                }
                break;

            case SDL_MOUSEBUTTONUP:
                mouseDown = false;
                break;

            case SDL_QUIT:
                done = true;
                break;
        }
    }


    if (mouseDown)
    {

        SDL_GetMouseState(&mousex, &mousey);

        mousexscale = (mousex / (float) _width);
        mouseyscale = ((_height - mousey) / (float) _height);

        touchDrag(mousexscale, mouseyscale, mousepressure);
    }
}


void projectMSDL::touch(float x, float y, int pressure, int touchtype)
{
#ifdef PROJECTM_TOUCH_ENABLED
    projectm_touch(_projectM, x, y, pressure, static_cast<projectm_touch_type>(touchtype));
#endif
}


void projectMSDL::touchDrag(float x, float y, int pressure)
{
    projectm_touch_drag(_projectM, x, y, pressure);
}


void projectMSDL::touchDestroy(float x, float y)
{
    projectm_touch_destroy(_projectM, x, y);
}


void projectMSDL::touchDestroyAll()
{
    projectm_touch_destroy_all(_projectM);
}

void projectMSDL::renderFrame()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectm_opengl_render_frame(_projectM);

    SDL_GL_SwapWindow(_sdlWindow);
}

void projectMSDL::init(SDL_Window* window, const bool _renderToTexture)
{
    _sdlWindow = window;
    projectm_set_window_size(_projectM, _width, _height);

#ifdef WASAPI_LOOPBACK
    wasapi = true;
#endif
}

std::string projectMSDL::getActivePresetName()
{
    unsigned int index = projectm_playlist_get_position(_playlist);
    if (index)
    {
        auto presetName = projectm_playlist_item(_playlist, index);
        std::string presetNameString(presetName);
        projectm_playlist_free_string(presetName);
        return presetNameString;
    }
    return {};
}

void projectMSDL::presetSwitchedEvent(bool isHardCut, unsigned int index, void* context)
{
    auto app = reinterpret_cast<projectMSDL*>(context);
    auto presetName = projectm_playlist_item(app->_playlist, index);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Displaying preset: %s\n", presetName);

    app->_presetName = presetName;
    projectm_playlist_free_string(presetName);

    app->UpdateWindowTitle();
}

projectm_handle projectMSDL::projectM()
{
    return _projectM;
}

void projectMSDL::setFps(size_t fps)
{
    _fps = fps;
}

size_t projectMSDL::fps() const
{
    return _fps;
}

void projectMSDL::UpdateWindowTitle()
{
    std::string title = "projectM ➫ " + _presetName;
    if (projectm_get_preset_locked(_projectM))
    {
        title.append(" [locked]");
    }
    SDL_SetWindowTitle(_sdlWindow, title.c_str());
}
