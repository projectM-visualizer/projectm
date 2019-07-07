/*
  LICENSE
  -------
Copyright 2005-2013 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "api.h"
#include <windows.h>
#include "vis.h"
#include "plugin.h"
#include "defines.h"
#include "resource.h"
#include "utility.h"

CPlugin  g_plugin;
_locale_t g_use_C_locale = 0;
char keyMappings[8];
bool g_bFullyExited = true;

// wasabi based services for localisation support
api_service *WASABI_API_SVC = 0;
api_language *WASABI_API_LNG = 0;
api_application *WASABI_API_APP = 0;
api_syscb *WASABI_API_SYSCB = 0;
HINSTANCE WASABI_API_LNG_HINST = 0, WASABI_API_ORIG_HINST = 0;

void config(struct winampVisModule *this_mod); // configuration dialog
int init(struct winampVisModule *this_mod);       // initialization for module
int render1(struct winampVisModule *this_mod);  // rendering for module 1
void quit(struct winampVisModule *this_mod);   // deinitialization for module

// our only plugin module in this plugin:
winampVisModule mod1 =
{
    MODULEDESC,
    NULL,	// hwndParent
    NULL,	// hDllInstance
    0,		// sRate
    0,		// nCh
    0,		// latencyMS - tells winamp how much in advance you want the audio data, 
			//             in ms.
    10,		// delayMS - if winamp tells the plugin to render a frame and it takes
			//           less than this # of milliseconds, winamp will sleep (go idle)
            //           for the remainder.  In effect, this limits the framerate of
            //           the plugin.  A value of 10 would cause a fps limit of ~100.
            //           Derivation: (1000 ms/sec) / (10 ms/frame) = 100 fps.
    0,		// spectrumNch
    2,		// waveformNch
    { 0, },	// spectrumData
    { 0, },	// waveformData
    config,
    init,
    render1, 
    quit
};

// getmodule routine from the main header. Returns NULL if an invalid module was requested,
// otherwise returns either mod1, mod2 or mod3 depending on 'which'.
winampVisModule *getModule(int which)
{
    switch (which)
    {
        case 0: return &mod1;
        //case 1: return &mod2;
        //case 2: return &mod3;
        default: return NULL;
    }
}

// Module header, includes version, description, and address of the module retriever function
winampVisHeader hdr = { VIS_HDRVER, DLLDESC, getModule };

// use this to get our own HINSTANCE since overriding DllMain(..) causes instant crashes (should see why)
static HINSTANCE GetMyInstance()
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	if(VirtualQuery(GetMyInstance, &mbi, sizeof(mbi)))
		return (HINSTANCE)mbi.AllocationBase;
	return NULL;
}

// this is the only exported symbol. returns our main header.
// if you are compiling C++, the extern "C" { is necessary, so we just #ifdef it
#ifdef __cplusplus
extern "C" {
#endif
	__declspec( dllexport ) winampVisHeader *winampVisGetHeader(HWND hwndParent)
	{
		if(!WASABI_API_LNG_HINST)
		{
			// loader so that we can get the localisation service api for use
			WASABI_API_SVC = (api_service*)SendMessage(hwndParent, WM_WA_IPC, 0, IPC_GET_API_SERVICE);
			if (WASABI_API_SVC == (api_service*)1) WASABI_API_SVC = NULL;

			waServiceFactory *sf = WASABI_API_SVC->service_getServiceByGuid(languageApiGUID);
			if (sf) WASABI_API_LNG = reinterpret_cast<api_language*>(sf->getInterface());

			sf = WASABI_API_SVC->service_getServiceByGuid(applicationApiServiceGuid);
			if (sf) WASABI_API_APP = reinterpret_cast<api_application*>(sf->getInterface());

			sf = WASABI_API_SVC->service_getServiceByGuid(syscbApiServiceGuid);
			if (sf) WASABI_API_SYSCB = reinterpret_cast<api_syscb*>(sf->getInterface());

			// need to have this initialised before we try to do anything with localisation features
			WASABI_API_START_LANG(GetMyInstance(),VisMilkdropLangGUID);

			/* added for v2.25 as a quick work around to allow partial
			/* keyboard mappings (mainly coming from de-de requirements)
			** [yY][Y][yY][zZ]
			**  1   2   3   4
			**
			** 1 - does yes for the 3 different prompt types
			** 2 - does Ctrl+Y for stopping display of custom message of song title
			** 3 - something for preset editing (not 100% sure what)
			** 4 - used for the previous track sent to Winamp
			*/
			WASABI_API_LNGSTRING_BUF(IDS_KEY_MAPPINGS, keyMappings, 8);

			// as we're under a different thread we need to set the locale
			//WASABI_API_LNG->UseUserNumericLocale();
			g_use_C_locale = WASABI_API_LNG->Get_C_NumericLocale();
		}

		return &hdr;
	}
#ifdef __cplusplus
}
#endif
 
bool WaitUntilPluginFinished(HWND hWndWinamp)
{
    int slept = 0;
    while (!g_bFullyExited && slept < 1000)
    {
        Sleep(50);
        slept += 50;
    }

    if (!g_bFullyExited)
    {
		wchar_t title[64];
        MessageBoxW(hWndWinamp, WASABI_API_LNGSTRINGW(IDS_ERROR_THE_PLUGIN_IS_ALREADY_RUNNING),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return false;
    }

    return true;
}

HWND GetDialogBoxParent(HWND winamp)
{
	HWND parent = (HWND)SendMessage(winamp, WM_WA_IPC, 0, IPC_GETDIALOGBOXPARENT);
	if (!parent || parent == (HWND)1)
		return winamp;
	return parent;
}

// configuration. Passed this_mod, as a "this" parameter. Allows you to make one configuration
// function that shares code for all your modules (you don't HAVE to use it though, you can make
// config1(), config2(), etc...)
void config(struct winampVisModule *this_mod)
{
    if (!g_bFullyExited)
    {
      g_plugin.OnAltK();
      return;
    }

    g_bFullyExited = false;
    g_plugin.PluginPreInitialize(this_mod->hwndParent, this_mod->hDllInstance);
    WASABI_API_DIALOGBOXPARAMW(IDD_CONFIG, GetDialogBoxParent(this_mod->hwndParent), g_plugin.ConfigDialogProc, (LPARAM)&g_plugin);
    g_bFullyExited = true;
}

int (*warand)(void) = 0;

int fallback_rand_fn(void) {
  return rand();
}

// initialization. Registers our window class, creates our window, etc. Again, this one works for
// both modules, but you could make init1() and init2()...
// returns 0 on success, 1 on failure.
int init(struct winampVisModule *this_mod)
{
    DWORD version = GetWinampVersion(mod1.hwndParent);

	if (!warand)
    {
		warand = (int (*)(void))SendMessage(this_mod->hwndParent, WM_WA_IPC, 0, IPC_GET_RANDFUNC);
        if ((size_t)warand <= 1)
        {
            warand = fallback_rand_fn;
        }
    }
    
    if (!WaitUntilPluginFinished(this_mod->hwndParent))
    {
        return 1;        
    }

    if (GetWinampVersion(mod1.hwndParent) < 0x4000)
    {
        // query winamp for its playback state
        LRESULT ret = SendMessage(this_mod->hwndParent, WM_USER, 0, 104); 
        // ret=1: playing, ret=3: paused, other: stopped

        if (ret != 1)
        {
			wchar_t title[64];
            MessageBoxW(this_mod->hwndParent, WASABI_API_LNGSTRINGW(IDS_THIS_PLUGIN_NEEDS_MUSIC_TO_RUN),
					   WASABI_API_LNGSTRINGW_BUF(IDS_NO_MUSIC_PLAYING, title, 64),
					   MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL );
            return 1;  // failure
        }
    }

    g_bFullyExited = false;

    if (!g_plugin.PluginPreInitialize(this_mod->hwndParent, this_mod->hDllInstance))
    {
        g_plugin.PluginQuit();
        g_bFullyExited = true;
        return 1;
    }

    if (!g_plugin.PluginInitialize())
    {
        g_plugin.PluginQuit();
        g_bFullyExited = true;
        return 1;
    }

    return 0;    // success
}

// render function for oscilliscope. Returns 0 if successful, 1 if visualization should end.
int render1(struct winampVisModule *this_mod)
{
	if (g_plugin.PluginRender(this_mod->waveformData[0], this_mod->waveformData[1]))
		return 0;    // ok
	else
		return 1;    // failed
}

// cleanup (opposite of init()). Should destroy the window, unregister the window class, etc.
void quit(struct winampVisModule *this_mod)
{
	g_plugin.PluginQuit();
	g_bFullyExited = true;
}