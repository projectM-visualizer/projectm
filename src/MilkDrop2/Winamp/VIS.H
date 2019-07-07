#ifndef NULLSOFT_VISH
#define NULLSOFT_VISH
// Visualization plugin interface

typedef struct winampVisModule {
  char *description;      // description of module
  HWND hwndParent;        // parent window (filled in by calling app)
  HINSTANCE hDllInstance; // instance handle to this DLL (filled in by calling app)
  int sRate;		      // sample rate (filled in by calling app)
  int nCh;			      // number of channels (filled in...)
  int latencyMs;          // latency from call of RenderFrame to actual drawing
                          // (calling app looks at this value when getting data)
  int delayMs;            // delay between calls in ms

  // the data is filled in according to the respective Nch entry
  int spectrumNch;
  int waveformNch;
  unsigned char spectrumData[2][576];
  unsigned char waveformData[2][576];

  void (__cdecl *Config)(struct winampVisModule *this_mod);  // configuration dialog
  int (__cdecl *Init)(struct winampVisModule *this_mod);     // 0 on success, creates window, etc
  int (__cdecl *Render)(struct winampVisModule *this_mod);   // returns 0 if successful, 1 if vis should end
  void (__cdecl *Quit)(struct winampVisModule *this_mod);    // call when done

  void *userData;         // user data, optional
} winampVisModule;

typedef struct {
  int version;            // VID_HDRVER
  char *description;      // description of library
  winampVisModule* (__cdecl *getModule)(int);
} winampVisHeader;

// exported symbols
#ifdef USE_VIS_HDR_HWND
typedef winampVisHeader* (__cdecl *winampVisGetHeaderType)(HWND);

// version of current module (0x102 == 1.02)
#define VIS_HDRVER 0x102

#else
typedef winampVisHeader* (__cdecl *winampVisGetHeaderType)();

// version of current module (0x101 == 1.01)
#define VIS_HDRVER 0x101

#endif

// Version note:
//
// Updated to 1.02 for 5.36+
// Added passing of Winamp's main hwnd in the call to the exported winampVisGetHeader()
// which allows for primarily the use of localisation features with the bundled plugins.
// If you want to use the new version then either you can edit you version of vis.h or
// you can add USE_VIS_HRD_HWND to your project's defined list or before use of vis.h
//

// Miscellaneous notes:
// * Any window that remains in foreground should optimally pass keystrokes to the parent
//   (Winamp's) window, so that the user can still control it unless escape is pressed or
//   some option key specific to the visualization is pressed.
// * Storing configuration can be done any where though it's recommended to use the api
//   IPC_GETINIDIRECTORY as the basis of the path to save things to e.g. INIDIR\plugins\plugin.ini
// * ints are 32 bits and structure members are aligned on the default 8 byte boundaries.

// These are the return values to be used with the uninstall plugin export function:
// __declspec(dllexport) int __cdecl winampUninstallPlugin(HINSTANCE hDllInst, HWND hwndDlg, int param)
// which determines if Winamp can uninstall the plugin immediately or on winamp restart.
// If this is not exported then Winamp will assume an uninstall with reboot is the only way.
// Note: visualization plugins are always uninstalled without a reboot (unlike other plugin types).
//
#define VIS_PLUGIN_UNINSTALL_NOW 0x0
//
// Uninstall support was added from 5.0+ and uninstall now support from 5.5+ though note
// that it is down to you to ensure that if uninstall now is returned that it will not
// cause a crash i.e. don't use if you've been subclassing the main window.
//
// The HWND passed in the calling of winampUninstallPlugin(..) is the preference page HWND.
//

// For a vis plugin to be correctly detected by Winamp you need to ensure that
// the exported winampVisGetHeader(..) is exported as an undecorated function
// e.g.
// #ifdef __cplusplus
//   extern "C" {
// #endif
// __declspec(dllexport) winampVisHeader * __cdecl winampVisGetHeader(){ return &plugin; }
// #ifdef __cplusplus
//   }
// #endif
//

#endif