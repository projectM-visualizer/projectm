/**
 * Emscripten port of projectm-SDLvis
 *  Mischa Spiegelmock, 2014
 *
 */

#include <math.h>
#include <projectM.h>
#include <sdltoprojectM.h>

#include <emscripten.h>
#include <GL/gl.h>
#include <SDL.h>

#include <string>

#include <dirent.h>

const float FPS = 60;

struct projectMApp
{
    projectm_handle pm{ nullptr };
    SDL_Window* win{ nullptr };
    SDL_Renderer* rend{ nullptr };
    bool done{ false };
    projectm_settings settings;
    SDL_AudioDeviceID audioInputDevice{ 0 };
    int audioInputDevicesCount{ 0 };
    int audioChannelsCount{ 0 };
    bool isFullscreen{ false };
};

projectMApp app;

void audioInputCallbackF32(void* userdata, unsigned char* stream, int len)
{
    if (app.audioChannelsCount == 1)
    {
        projectm_pcm_add_float_1ch_data(app.pm, reinterpret_cast<float*>(stream), len / sizeof(float));
    }
    else if (app.audioChannelsCount == 2)
    {
        projectm_pcm_add_float_2ch_data(app.pm, reinterpret_cast<float*>(stream), len / sizeof(float));
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Multichannel audio not supported");
    }
}

bool selectAudioInput(projectMApp* application)
{
    app.audioInputDevicesCount = SDL_GetNumAudioDevices(1);

    if (!app.audioInputDevicesCount)
    {
        fprintf(stderr, "No audio input capture devices detected. Faking audio using random data.\n");
        return false;
    }

    printf("count: %d\n", app.audioInputDevicesCount);
    for (int i = 0; i < app.audioInputDevicesCount; ++i)
    {
        printf("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 1));
    }

    SDL_AudioSpec want, have;

    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = projectm_pcm_get_max_samples();
    want.callback = &audioInputCallbackF32;

    // Start with first device
    app.audioInputDevice = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, true), true, &want, &have, 0);
    app.audioChannelsCount = have.channels;

    if (have.channels == 1 || have.channels == 2)
    {
        SDL_PauseAudioDevice(app.audioInputDevice, false);
        printf("Audio device specs: Channels=%d, Samplerate=%d, Format=%d\n", have.channels, have.freq, have.format);
    }
    return true;
}

void keyHandler(const SDL_Event& sdl_evt)
{
    projectMEvent evt;
    projectMKeycode key;
    projectMModifier mod;
    auto sdl_mod = static_cast<SDL_Keymod>(sdl_evt.key.keysym.mod);
    SDL_Keycode sdl_keycode = sdl_evt.key.keysym.sym;
    bool keyMod = false;

    // Left or Right Gui or Left Ctrl
    if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL)
    {
        keyMod = true;
    }

    // handle keyboard input (for our app first, then projectM)
    switch (sdl_keycode)
    {
        case SDLK_BACKSPACE:
            projectm_delete_search_text(app.pm);
            break;
        case SDLK_RETURN:
            if (!projectm_is_text_input_active(app.pm, false))
            {
                SDL_StartTextInput();
            }
            break;
        case SDLK_ESCAPE:
            if (projectm_is_text_input_active(app.pm, false))
            {
                SDL_StopTextInput();
            }
            break;
        case SDLK_f:
            if (keyMod)
            {
                app.isFullscreen = !app.isFullscreen;
                SDL_SetWindowFullscreen(app.win, app.isFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
                return; // handled
            }
            break;

        case SDLK_SPACE:
            if (!projectm_is_text_input_active(app.pm, true))
            {
                projectm_lock_preset(app.pm, !projectm_is_preset_locked(app.pm));
            }
            break;

    }

    // translate into projectM codes and perform default projectM handler
    evt = sdl2pmEvent(&sdl_evt);
    mod = sdl2pmModifier(sdl_mod);
    key = sdl2pmKeycode(sdl_keycode, sdl_mod);
    projectm_key_handler(app.pm, evt, key, mod);
}

void renderFrame()
{
    SDL_Event evt;

    SDL_PollEvent(&evt);
    switch (evt.type)
    {
        case SDL_KEYDOWN:
            keyHandler(evt);
            break;
        case SDL_TEXTINPUT:
            if (projectm_is_text_input_active(app.pm, true))
            {
                projectm_set_search_text(app.pm, evt.text.text);
                projectm_populate_preset_menu(app.pm);
            }
            break;
        case SDL_QUIT:
            app.done = true;
            break;
    }

    if (app.audioInputDevicesCount == 0)
    {
        short pcm_data[2][512];

        /** Produce some fake PCM data to stuff into projectM */
        for (int i = 0; i < 512; i++)
        {
            if (i % 2 == 0)
            {
                pcm_data[0][i] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
                pcm_data[1][i] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
            }
            else
            {
                pcm_data[0][i] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
                pcm_data[1][i] = (float) (rand() / ((float) RAND_MAX) * (pow(2, 14)));
            }
            if (i % 2 == 1)
            {
                pcm_data[0][i] = -pcm_data[0][i];
                pcm_data[1][i] = -pcm_data[1][i];
            }
        }

        /** Add the waveform data */
        projectm_pcm_add_16bit_2ch_512(app.pm, pcm_data);
    }


    glClearColor(0.0, 0.5, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectm_render_frame(app.pm);
    glFlush();

    SDL_RenderPresent(app.rend);
}

int main(int argc, char* argv[])
{
    int width = 1024;
    int height = 1024;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    app.win = SDL_CreateWindow("projectM", 50, 50, width, height, 0);
    app.rend = SDL_CreateRenderer(app.win, 0, SDL_RENDERER_ACCELERATED);
    if (!app.rend)
    {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    app.settings.mesh_x = 48;
    app.settings.mesh_y = 32;
    app.settings.fps = FPS;
    app.settings.texture_size = 1024;
    app.settings.window_width = width;
    app.settings.window_height = height;
    app.settings.soft_cut_duration = 3; // seconds
    app.settings.preset_duration = 5; // seconds
    app.settings.beat_sensitivity = 0.8;
    app.settings.aspect_correction = 1;
    app.settings.easter_egg = 0; // ???
    app.settings.shuffle_enabled = 1;
    app.settings.soft_cut_ratings_enabled = 1; // ???
    app.settings.preset_url = projectm_alloc_string(8);
    strncpy(app.settings.preset_url, "presets", 8);

    // init projectM
    app.pm = projectm_create_settings(&(app.settings), PROJECTM_FLAG_NONE);
    printf("init projectM\n");
    projectm_select_random_preset(app.pm, true);
    printf("select random\n");
    projectm_set_window_size(app.pm, width, height);
    printf("resetGL\n");

    // get an audio input device
    if (!selectAudioInput(&app))
    {
        fprintf(stderr, "Failed to open audio input device\n");
        return 1;
    }

    // Allocate a new a stream given the current directory name
    DIR* m_dir;
    if ((m_dir = opendir("/")) == nullptr)
    {
        printf("error opening /\n");
    }
    else
    {
        struct dirent* dir_entry;
        while ((dir_entry = readdir(m_dir)) != nullptr)
        {
            printf("%s\n", dir_entry->d_name);
        }
    }

    auto playlistSize = projectm_get_playlist_size(app.pm);
    for (unsigned int i = 0; i < playlistSize; i++)
    {
        auto presetName = projectm_get_preset_name(app.pm, i);
        printf("%u\t%s\n", i, presetName);
        projectm_free_string(presetName);
    }

    emscripten_set_main_loop(renderFrame, 0, 0);

    return 0;
}
