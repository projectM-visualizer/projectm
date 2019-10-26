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
* pmSDL.cpp
* Authors: Created by Mischa Spiegelmock on 2017-09-18.
*
*
* experimental Stereoscopic SBS driver functionality by
*	RobertPancoast77@gmail.com
*/

#include "pmSDL.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Renderer/ShaderEngine.hpp"



void projectMSDL::audioInputCallbackF32(void *userdata, unsigned char *stream, int len) {
    projectMSDL *app = (projectMSDL *) userdata;
    //    printf("LEN: %i\n", len);
    // stream is (i think) samples*channels floats (native byte order) of len BYTES
    if (app->m_audioChannelsCount == 1)
        app->pcm()->addPCMfloat((float *)stream, len/sizeof(float));
    else if (app->m_audioChannelsCount == 2)
        app->pcm()->addPCMfloat_2ch((float *)stream, len/sizeof(float));
    else {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Multichannel audio not supported");
        SDL_Quit();
    }
}

void projectMSDL::audioInputCallbackS16(void *userdata, unsigned char *stream, int len) {
    //    printf("LEN: %i\n", len);
    projectMSDL *app = (projectMSDL *) userdata;
    short pcm16[2][512];

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < app->m_audioChannelsCount; j++) {
            pcm16[j][i] = stream[i+j];
        }
    }
    app->pcm()->addPCM16(pcm16);
}

// Open a specified audio capture device.
int projectMSDL::openAudioCaptureDevice(const SDLAudioDeviceInfo& deviceInfo) {
    // params for audio input
    SDL_AudioSpec want, have;

    // requested format
    // https://wiki.libsdl.org/SDL_AudioSpec#Remarks
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_F32;  // float
    want.channels = 2;
    want.samples = PCM::maxsamples;
    want.callback = projectMSDL::audioInputCallbackF32;
    want.userdata = this;

    SDL_AudioDeviceID audioDeviceID = SDL_OpenAudioDevice(deviceInfo.name.c_str(), true, &want, &have, 0);

    if (audioDeviceID == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio capture device: %s", SDL_GetError());
        SDL_Quit();
    }

    // read characteristics of opened capture device
    SDL_Log("Opened audio capture device index=%i devId=%i: %s", deviceInfo.index, audioDeviceID, deviceInfo.name.c_str());
    SDL_Log("Samples: %i, frequency: %i, channels: %i, format: %i", have.samples, have.freq, have.channels, have.format);
    m_audioChannelsCount = have.channels;
    m_audioSampleRate = have.freq;
    m_audioSampleCount = have.samples;
    m_audioFormat = have.format;
    m_audioInputDevice = audioDeviceID;
    return 1;
}

// Release the audio device currently in use.
void projectMSDL::releaseAudioCaptureDevice() {
    endAudioCapture();
    SDL_CloseAudioDevice(m_audioInputDevice);
}

// Enumerate audio capture devices and select a default device.
int projectMSDL::initAudioInput() {
    // get audio driver name (static)
    const char* driverName = SDL_GetCurrentAudioDriver();
    SDL_Log("Using audio driver: %s\n", driverName);

    // get audio input device
    uint deviceCount = SDL_GetNumAudioDevices(true);  // capture, please
    if (deviceCount == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No audio capture devices found");
        SDL_Quit();
    }

    printf("Detected devices:\n");
    for (uint i = 0; i < deviceCount; i++) {
        std::string deviceName = SDL_GetAudioDeviceName(i, true);

        // Store this capture device's info.
        m_audioCaptureDevicesInfo.push_back( SDLAudioDeviceInfo(deviceName, i) );

        printf("  %i: 🎤%s\n", i, deviceName.c_str());
    }

    // Default to device 0.
    return openAudioCaptureDevice(m_audioCaptureDevicesInfo[0]);
}

void projectMSDL::beginAudioCapture() {
    // allocate a buffer to store PCM data for feeding in
    SDL_PauseAudioDevice(m_audioInputDevice, false);
}

void projectMSDL::endAudioCapture() {
    SDL_PauseAudioDevice(m_audioInputDevice, true);
}

void projectMSDL::maximize() {
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return;
    }

    SDL_SetWindowSize(m_window, dm.w, dm.h);
    resize(dm.w, dm.h);
}

void projectMSDL::toggleFullScreen() {
    maximize();
    if (m_isFullScreen) {
        SDL_SetWindowFullscreen(m_window, 0);
        m_isFullScreen = false;
        SDL_ShowCursor(true);
    } else {
        SDL_ShowCursor(false);
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
        m_isFullScreen = true;
    }
}

void projectMSDL::setFullScreen(bool fullscreen) {
    if (!fullscreen) {
        SDL_SetWindowFullscreen(m_window, 0);
        SDL_ShowCursor(true);
    } else {
        SDL_ShowCursor(false);
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    }

    m_isFullScreen = fullscreen;
}

void projectMSDL::keyHandler(SDL_Event *sdl_evt) {
    projectMEvent evt;
    projectMKeycode key;
    projectMModifier mod;
    SDL_Keymod sdl_mod = (SDL_Keymod) sdl_evt->key.keysym.mod;
    SDL_Keycode sdl_keycode = sdl_evt->key.keysym.sym;

	// handle keyboard input (for our app first, then projectM)
    switch (sdl_keycode) {
        case SDLK_q:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL) {
                // cmd/ctrl-q = quit
                done = 1;
                return;
            }
            break;


        case SDLK_f:
            if (sdl_mod & KMOD_LGUI || sdl_mod & KMOD_RGUI || sdl_mod & KMOD_LCTRL) {
                // command-f: fullscreen
				// Stereo requires fullscreen
#if !STEREOSCOPIC_SBS
				toggleFullScreen();
#endif
                return; // handled
            }
            break;

        case SDLK_LEFT:
            // selectPrevious(true);
            break;
        case SDLK_RIGHT:
            // selectNext(true);
            break;
        case SDLK_UP:
            break;
        case SDLK_DOWN:
            break;

        case SDLK_F3:
            m_showMenu = !m_showMenu;   // Toggle config UI.
            break;


        case SDLK_SPACE:
            setPresetLock(
                !isPresetLocked()
            );
            break;
        case SDLK_F1:
        case SDLK_ESCAPE:

            // exit(1);
            // show help with other keys
            sdl_keycode = SDLK_F1;
            break;
        case SDLK_DELETE:
            /*
            try {
                if (selectedPresetIndex(index)) {
                    DeleteFile(
                        LPCSTR(
                            getPresetURL(index).c_str()
                        )
                    );
                }
            }
            catch (const std::exception & e) {
                printf("Delete failed");
            }
            */
            break;
        case SDLK_RETURN:
            /*
            try {
                if (selectedPresetIndex(index)) {
                    CopyFile(
                            LPCSTR(
                                    app->getPresetURL(index).c_str()
                            ),
                            LPCTSTR(L"C:\\"),
                            false
                    );
                }
            }
            catch (const std::exception & e) {
                printf("Delete failed");
            }
                    */
            break;
    }

    // translate into projectM codes and perform default projectM handler
    evt = sdl2pmEvent(sdl_evt);
    key = sdl2pmKeycode(sdl_keycode);
    mod = sdl2pmModifier(sdl_mod);
    key_handler(evt, key, mod);
}

void projectMSDL::addFakePCM() {
    int i;
    short pcm_data[2][512];
    /** Produce some fake PCM data to stuff into projectM */
    for ( i = 0 ; i < 512 ; i++ ) {
        if ( i % 2 == 0 ) {
            pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
            pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
        } else {
            pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
            pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,14) ) );
        }
        if ( i % 2 == 1 ) {
            pcm_data[0][i] = -pcm_data[0][i];
            pcm_data[1][i] = -pcm_data[1][i];
        }
    }

    /** Add the waveform data */
    pcm()->addPCM16(pcm_data);
}

void projectMSDL::resize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
    projectM_resetGL(m_width, m_height);
}

void projectMSDL::pollEvent() {
    SDL_Event evt;

    while (SDL_PollEvent(&evt))
    {
        // Process ImGui events.
        ImGui_ImplSDL2_ProcessEvent(&evt);
        
        switch (evt.type) {
            case SDL_WINDOWEVENT:
                switch (evt.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        resize(evt.window.data1, evt.window.data2);
                        break;
                }
                break;
            case SDL_KEYDOWN:
                keyHandler(&evt);
                break;
            case SDL_QUIT:
                done = true;
                break;
        }
    }
}

void projectMSDL::renderImGuiMenu() {

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();

    // Uncomment to show the big demo window (you can browse its code to learn more about Dear ImGui!).
    // ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Begin("Menu", &m_showMenu)) {

        // Preset playlist.
        ImGui::BeginChild("Preset Playlist", ImVec2(200, 0), true, ImGuiWindowFlags_AlwaysAutoResize);

        // Get the currently selected preset. 
        // Would be more efficient in presetSwitchedEvent but const prevents this.
        selectedPresetIndex(m_selectedPresetIndex);

        uint playlistSize = getPlaylistSize();
        
        for (uint i = 0; i < playlistSize; i++) {
            std::stringstream presetName;  
            presetName << "[" << i << "] " << getPresetName(i);              

            ImGui::Selectable(presetName.str().c_str(), m_selectedPresetIndex == i);

            bool itemHovered = ImGui::IsItemHovered();

            uint oldSelectedPreset = m_selectedPresetIndex;
            if (itemHovered && ImGui::IsMouseDoubleClicked(0))    // double click                                                                                                                                                                                                                   
                m_selectedPresetIndex = i;

            // Show full name of preset in a tooltip.
            if (itemHovered)
                ImGui::SetTooltip("%s", presetName.str().c_str());

            // Select new preset if requested.
            if (oldSelectedPreset != m_selectedPresetIndex)
                selectPreset(m_selectedPresetIndex, true);
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginGroup();
        {
        // Audio capture device combobox. 
#if !FAKE_AUDIO && !WASAPI_LOOPBACK
            ImGui::PushItemWidth(-1);
            ImGui::Text("Capture Device:"); ImGui::SameLine();

            static uint selectedItem = 0;
            const char* deviceName = m_audioCaptureDevicesInfo[selectedItem].name.c_str();

            if (ImGui::BeginCombo("##CaptureDeviceCombo", deviceName, 0)) {
                size_t devicesInfoSize = m_audioCaptureDevicesInfo.size();

                // For each audio capture device.
                for (size_t i = 0; i < devicesInfoSize; i++) {
                    bool isSelected = (selectedItem == i);
                    if (ImGui::Selectable(m_audioCaptureDevicesInfo[i].name.c_str(), isSelected)) {
                        if (i != selectedItem) {
                            releaseAudioCaptureDevice();
                            openAudioCaptureDevice(m_audioCaptureDevicesInfo[i]);
                        }
                        selectedItem = i;
                    }

                    // Set the initial focus when opening the combo (scrolling + for keyboard 
                    // navigation support in the upcoming navigation branch)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::Separator();
#endif       

            // Play mode radio buttons
            ImGui::Text("Play:"); ImGui::SameLine();
            if (ImGui::RadioButton("Shuffle", m_shufflePlay == true)) {
                m_shufflePlay = true;
                setShuffleEnabled(true); 
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Continuous", m_shufflePlay == false)) {
                m_shufflePlay = false;
                setShuffleEnabled(false); 
            }

            // Duration slider
            int oldDuration = m_presetDuration;
            ImGui::PushItemWidth(-1);
            ImGui::Text("Duration:"); ImGui::SameLine(); 
            ImGui::SliderInt("", &m_presetDuration, 1, 60);   
            if (oldDuration != m_presetDuration)
                changePresetDuration(m_presetDuration);
            ImGui::PopItemWidth();
            ImGui::Separator();

            // Fullscreen enable/disable checkbox.
            bool oldFullScreen = m_isFullScreen;
            ImGui::Checkbox("FullScreen", &m_isFullScreen);
            if (oldFullScreen != m_isFullScreen)
                setFullScreen(m_isFullScreen);
        }
        ImGui::EndGroup();
    
    }

    ImGui::End();

    ImGuiIO& io = ImGui::GetIO();
    
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void projectMSDL::renderFrame() {
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectM::renderFrame();

    if (m_renderToTexture)
        renderTexture();

    if (m_showMenu)
        renderImGuiMenu();

    SDL_GL_SwapWindow(m_window);
}

projectMSDL::projectMSDL(Settings settings, int flags) : projectM(settings, flags) {
    m_width = getWindowWidth();
    m_height = getWindowHeight();
    done = 0;
    m_isFullScreen = false;
}

projectMSDL::projectMSDL(std::string config_file, int flags) : projectM(config_file, flags) {
    m_width = getWindowWidth();
    m_height = getWindowHeight();
    done = 0;
    m_isFullScreen = false;
}

void projectMSDL::init(SDL_Window *window, SDL_GLContext *glCtx, const bool renderToTexture) {

    // Initial settings.
    m_presetDuration = settings().presetDuration;
    selectedPresetIndex(m_selectedPresetIndex);
    m_showMenu = false;
    m_shufflePlay = isShuffleEnabled();

    m_window = window;
    m_glCtx = glCtx;
    projectM_resetGL(m_width, m_height);

    const char* glslVersion = "#version 150";

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    m_imguiCtx = ImGui::CreateContext();

    // Setup ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, glCtx);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // are we rendering to a texture?
    m_renderToTexture = renderToTexture;
    if (renderToTexture) {
        m_programID = ShaderEngine::CompileShaderProgram(ShaderEngine::v2f_c4f_t2f_vert, ShaderEngine::v2f_c4f_t2f_frag, "v2f_c4f_t2f");
        m_textureID = projectM::initRenderToTexture();

        float points[16] = {
            -0.8, -0.8,
            0.0,    0.0,

            -0.8, 0.8,
            0.0,   1.0,

            0.8, -0.8,
            1.0,    0.0,

            0.8, 0.8,
            1.0,    1.0,
        };

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, points, GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions

        glDisableVertexAttribArray(1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}


std::string projectMSDL::getActivePresetName() {
    unsigned int index = 0;
    if (selectedPresetIndex(index)) {
        return getPresetName(index);
    }
    return std::string();
}


void projectMSDL::renderTexture() {
    static int frame = 0;
    frame++;

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, getWindowWidth(), getWindowHeight());

    glUseProgram(m_programID);

    glUniform1i(glGetUniformLocation(m_programID, "texture_sampler"), 0);

    glm::mat4 mat_proj = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 10.0f);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 mat_model = glm::mat4(1.0f);
    mat_model = glm::translate(mat_model, glm::vec3(cos(frame*0.023),
                                                    cos(frame*0.017),
                                                    -5+sin(frame*0.022)*2));
    mat_model = glm::rotate(mat_model, glm::radians((float) sin(frame*0.0043)*360),
                            glm::vec3(sin(frame*0.0017),
                                      sin(frame *0.0032),
                                      1));

    glm::mat4 mat_transf = glm::mat4(1.0f);
    mat_transf = mat_proj * mat_model;

    glUniformMatrix4fv(glGetUniformLocation(m_programID, "vertex_transformation"), 1, GL_FALSE, glm::value_ptr(mat_transf));

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
}

void projectMSDL::presetSwitchedEvent(bool isHardCut, size_t index) const {
    std::string presetName = getPresetName(index);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Displaying preset: %s\n", presetName.c_str());
}
