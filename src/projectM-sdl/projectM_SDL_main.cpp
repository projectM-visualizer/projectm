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
* main.cpp
* Authors: Created by Mischa Spiegelmock on 6/3/15.
*
* 
*	RobertPancoast77@gmail.com :
* experimental Stereoscopic SBS driver functionality
* WASAPI looback implementation
* 
*
*/

#include "pmSDL.hpp"

#if OGL_DEBUG
void DebugLog(GLenum source,
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
std::string getConfigFilePath(std::string datadir_path)
{
	struct stat sb;
	const char *path = datadir_path.c_str();

	// first check if settings file is in DATA_DIR
	std::string configFilePath = path;
	std::string configFullPath = configFilePath;
	configFullPath += "config.inp";

	if (stat(configFullPath.c_str(), &sb) == 0)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Config: %s was found!\n", configFullPath.c_str());
		return configFilePath;
	}

	// now check settings directories (windows / other)
	configFilePath = "";
#ifdef WIN32
	configFilePath += getenv("APPDATA");
	configFilePath += "\\projectM\\";
#else
	configFilePath += getenv("home");
	configFilePath += "/.projectM/";
#endif

	configFullPath = configFilePath + "config.inp";
	if (stat(configFullPath.c_str(), &sb) == 0)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Config: %s was found!\n", configFullPath.c_str());
		return configFilePath;
	}

	// finally try current working directory.
	configFilePath = "";
#ifdef WIN32
	configFilePath += ".\\";
#else
	configFilePath += "./";
#endif

	configFullPath = configFilePath + "config.inp";
	if (stat(configFullPath.c_str(), &sb) == 0)
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Config: %s was found!\n", configFullPath.c_str());
		return configFilePath;
	}
	
	return "";
}


// ref https://blogs.msdn.microsoft.com/matthew_van_eerde/2008/12/16/sample-wasapi-loopback-capture-record-what-you-hear/
#ifdef WASAPI_LOOPBACK

HRESULT get_default_device(IMMDevice **ppMMDevice) {
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pMMDeviceEnumerator;

	// activate a device enumerator
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pMMDeviceEnumerator
	);
	if (FAILED(hr)) {
		ERR(L"CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x", hr);
		return hr;
	}

	// get the default render endpoint
	hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, ppMMDevice);
	if (FAILED(hr)) {
		ERR(L"IMMDeviceEnumerator::GetDefaultAudioEndpoint failed: hr = 0x%08x", hr);
		return hr;
	}

	return S_OK;
}

#endif /** WASAPI_LOOPBACK */

int main(int argc, char *argv[]) {
#ifndef WIN32
srand((int)(time(NULL)));
#endif

#ifdef WASAPI_LOOPBACK
	HRESULT hr;

    hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        ERR(L"CoInitialize failed: hr = 0x%08x", hr);
    }


	IMMDevice *pMMDevice(NULL);
    // open default device if not specified
    if (NULL == pMMDevice) {
        hr = get_default_device(&pMMDevice);
        if (FAILED(hr)) {
            return hr;
        }
    }

	bool bInt16 = false;
    UINT32 foo = 0;
	PUINT32 pnFrames = &foo;

	// activate an IAudioClient
	IAudioClient *pAudioClient;
	hr = pMMDevice->Activate(
		__uuidof(IAudioClient),
		CLSCTX_ALL, NULL,
		(void**)&pAudioClient
	);
	if (FAILED(hr)) {
		ERR(L"IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x", hr);
		return hr;
	}

	// get the default device periodicity
	REFERENCE_TIME hnsDefaultDevicePeriod;
	hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
	if (FAILED(hr)) {
		ERR(L"IAudioClient::GetDevicePeriod failed: hr = 0x%08x", hr);
		return hr;
	}

	// get the default device format
	WAVEFORMATEX *pwfx;
	hr = pAudioClient->GetMixFormat(&pwfx);
	if (FAILED(hr)) {
		ERR(L"IAudioClient::GetMixFormat failed: hr = 0x%08x", hr);
		return hr;
	}

	if (bInt16) {
		// coerce int-16 wave format
		// can do this in-place since we're not changing the size of the format
		// also, the engine will auto-convert from float to int for us
		switch (pwfx->wFormatTag) {
		case WAVE_FORMAT_IEEE_FLOAT:
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			pwfx->wBitsPerSample = 16;
			pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
			pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
			break;

		case WAVE_FORMAT_EXTENSIBLE:
		{
			// naked scope for case-local variable
			PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
			if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
				pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
				pEx->Samples.wValidBitsPerSample = 16;
				pwfx->wBitsPerSample = 16;
				pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
				pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
			}
			else {
				ERR(L"%s", L"Don't know how to coerce mix format to int-16");
				return E_UNEXPECTED;
			}
		}
		break;

		default:
			ERR(L"Don't know how to coerce WAVEFORMATEX with wFormatTag = 0x%08x to int-16", pwfx->wFormatTag);
			return E_UNEXPECTED;
		}
	}
        
	UINT32 nBlockAlign = pwfx->nBlockAlign;
	*pnFrames = 0;

	// call IAudioClient::Initialize
	// note that AUDCLNT_STREAMFLAGS_LOOPBACK and AUDCLNT_STREAMFLAGS_EVENTCALLBACK
	// do not work together...
	// the "data ready" event never gets set
	// so we're going to do a timer-driven loop
	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_LOOPBACK,
		0, 0, pwfx, 0
	);
	if (FAILED(hr)) {
		ERR(L"pAudioClient->Initialize error");
		return hr;
	}

	// activate an IAudioCaptureClient
	IAudioCaptureClient *pAudioCaptureClient;
	hr = pAudioClient->GetService(
		__uuidof(IAudioCaptureClient),
		(void**)&pAudioCaptureClient
	);
	if (FAILED(hr)) {
		ERR(L"pAudioClient->GetService error");
		return hr;
	}

	// call IAudioClient::Start
	hr = pAudioClient->Start();
	if (FAILED(hr)) {
		ERR(L"pAudioClient->Start error");
		return hr;
	}

	bool bDone = false;
	bool bFirstPacket = true;
    UINT32 nPasses = 0;

#endif /** WASAPI_LOOPBACK */

#if UNLOCK_FPS
    setenv("vblank_mode", "0", 1);
#endif
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

#ifdef USE_GLES
    // use GLES 2.0 (this may need adjusting)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

#else
	// Disabling compatibility profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#endif

    
    SDL_Window *win = SDL_CreateWindow("projectM", 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    SDL_GL_GetDrawableSize(win,&width,&height);
    
#if STEREOSCOPIC_SBS

	// enable stereo
	if (SDL_GL_SetAttribute(SDL_GL_STEREO, 1) == 0) 
	{
		SDL_Log("SDL_GL_STEREO: true");
	}

	// requires fullscreen mode
	SDL_ShowCursor(false);
	SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

#endif


	SDL_GLContext glCtx = SDL_GL_CreateContext(win);


    SDL_Log("GL_VERSION: %s", glGetString(GL_VERSION));
    SDL_Log("GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    SDL_Log("GL_VENDOR: %s", glGetString(GL_VENDOR));

    SDL_SetWindowTitle(win, "projectM Visualizer");
    
    SDL_GL_MakeCurrent(win, glCtx);  // associate GL context with main window
    int avsync = SDL_GL_SetSwapInterval(-1); // try to enable adaptive vsync
    if (avsync == -1) { // adaptive vsync not supported
        SDL_GL_SetSwapInterval(1); // enable updates synchronized with vertical retrace
    }

    
    projectMSDL *app;
    
    std::string base_path = DATADIR_PATH;

    // load configuration file folder (it might be different than DATADIR_PATH)
    std::string configFilePath = getConfigFilePath(base_path);

    if (configFilePath.empty()) base_path = SDL_GetBasePath(); // if no configfile was found, use SDL base path.
	if (configFilePath != base_path) base_path = configFilePath; // if config file was found in a different directory than DATADIR_PATH, change base path to it.

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Using data directory: %s\n", base_path.c_str());

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
    projectM::Settings settings;
    settings.windowWidth = width;
    settings.windowHeight = height;
    settings.meshX = 128;
    settings.meshY = settings.meshX * heightWidthRatio;
	settings.fps = maxRefreshRate;
    settings.smoothPresetDuration = 3; // seconds
    settings.presetDuration = 22; // seconds
    settings.beatSensitivity = 0.8;
    settings.aspectCorrection = 1;
    settings.shuffleEnabled = 1;
    settings.softCutRatingsEnabled = 1; // ???
    // get path to our app, use CWD or resource dir for presets/fonts/etc
    settings.presetURL = base_path + "presets";
//        settings.presetURL = base_path + "presets/presets_shader_test";
    settings.menuFontURL = base_path + "fonts/Vera.ttf";
    settings.titleFontURL = base_path + "fonts/Vera.ttf";
        
	// init with settings & config (if config was found)
	projectM::ConfigPreset configPreset;
	if (!configFilePath.empty()) {
		configPreset.config_file = configFilePath + "config.inp";
	}
	else {
		configPreset.config_file = "";
	}
	configPreset.settings = settings;
	
    app = new projectMSDL(configPreset, 0);
    app->init(win, &glCtx);

#if STEREOSCOPIC_SBS
	app->toggleFullScreen();
#endif

#if OGL_DEBUG && !USE_GLES
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugLog, NULL);
#endif

#if !FAKE_AUDIO && !WASAPI_LOOPBACK
    // get an audio input device
    if (app->openAudioInput())
	    app->beginAudioCapture();
#endif

#if TEST_ALL_PRESETS
    uint buildErrors = 0;
    for(unsigned int i = 0; i < app->getPlaylistSize(); i++) {
        std::cout << i << "\t" << app->getPresetName(i) << std::endl;
        app->selectPreset(i);
        if (app->getErrorLoadingCurrentPreset()) {
            buildErrors++;
        }
    }

    if (app->getPlaylistSize()) {
        fprintf(stdout, "Preset loading errors: %d/%d [%d%%]\n", buildErrors, app->getPlaylistSize(), (buildErrors*100) / app->getPlaylistSize());
    }

    delete app;

    return PROJECTM_SUCCESS;
#endif

#if UNLOCK_FPS
    int32_t frame_count = 0;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    int64_t start = ( ((int64_t)now.tv_sec * 1000L) + (now.tv_nsec / 1000000L) );
#endif

    // standard main loop
    int fps = app->settings().fps;
    printf("fps: %d\n", fps);
    if (fps <= 0)
        fps = 60;
    const Uint32 frame_delay = 1000/fps;
    Uint32 last_time = SDL_GetTicks();
    while (! app->done) {
        app->renderFrame();
#if FAKE_AUDIO
        app->addFakePCM();
#endif
#ifdef WASAPI_LOOPBACK
        // drain data while it is available
        nPasses++;
		UINT32 nNextPacketSize;
		for (
			hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
			SUCCEEDED(hr) && nNextPacketSize > 0;
			hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize)
			) {
			// get the captured data
			BYTE *pData;
			UINT32 nNumFramesToRead;
			DWORD dwFlags;

			hr = pAudioCaptureClient->GetBuffer(
				&pData,
				&nNumFramesToRead,
				&dwFlags,
				NULL,
				NULL
			);
			if (FAILED(hr)) {
				return hr;
			}

			LONG lBytesToWrite = nNumFramesToRead * nBlockAlign;

			/** Add the waveform data */
			app->pcm()->addPCMfloat((float *)pData, nNumFramesToRead);

			*pnFrames += nNumFramesToRead;

			hr = pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
			if (FAILED(hr)) {
				return hr;
			}

			bFirstPacket = false;
		}

		if (FAILED(hr)) {
			return hr;
		}

#endif /** WASAPI_LOOPBACK */

#if UNLOCK_FPS
        frame_count++;
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        if (( ((int64_t)now.tv_sec * 1000L) + (now.tv_nsec / 1000000L) ) - start > 5000) {
            SDL_GL_DeleteContext(glCtx);
            delete(app);
            fprintf(stdout, "Frames[%d]\n", frame_count);
            exit(0);
        }
#else
        app->pollEvent();
        Uint32 elapsed = SDL_GetTicks() - last_time;
        if (elapsed < frame_delay)
            SDL_Delay(frame_delay - elapsed);
        last_time = SDL_GetTicks();
#endif
    }
    
    SDL_GL_DeleteContext(glCtx);
#if !FAKE_AUDIO && !WASAPI_LOOPBACK
    app->endAudioCapture();
#endif
    delete app;

    return PROJECTM_SUCCESS;
}


