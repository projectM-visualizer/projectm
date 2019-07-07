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
#include "DXContext.h"
#include "utility.h"
#include "shell_defines.h"
#include "resource.h"
#define COMPILE_MULTIMON_STUBS 1
#include <multimon.h>
#include <strsafe.h>

// note: added WS_EX_CONTROLPARENT and WS_TABSTOP for embedwnd so window frame will pass on KB commands to us, if it has focus & receives them.
//       however, it is still not working.  Maksim says he needs to use GetNextDlgTabItem() and then it will work.
//       aha- had to remove WS_EX_CONTROLPARENT and WS_OVERLAPPED.  Should now work with winamp 5.5 build 1620.
#define MY_EXT_WINDOW_STYLE (m_current_mode.m_skin ? 0/*WS_EX_CONTROLPARENT*/ : ((m_current_mode.screenmode==DESKTOP) ? (WS_EX_TOOLWINDOW) : 0)) // note: changed from TOOLWINDOW to APPWINDOW b/c we wanted the plugin to appear in the taskbar.
#define SKINNED_WS (WS_VISIBLE|WS_CHILDWINDOW/*|WS_OVERLAPPED*/|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP)
#define MY_WINDOW_STYLE (m_current_mode.m_skin ? SKINNED_WS : ((m_current_mode.screenmode==FAKE_FULLSCREEN || m_current_mode.screenmode==DESKTOP) ? WS_POPUP : WS_OVERLAPPEDWINDOW))   // note: WS_POPUP (by itself) removes all borders, captions, etc.

#include "vis.h"
extern winampVisModule mod1;

DXContext::DXContext(HWND hWndWinamp,HINSTANCE hInstance,LPCWSTR szClassName,LPCSTR szWindowCaption,WNDPROC pProc,LONG_PTR uWindowLong, int minimize_winamp, wchar_t* szIniFile)
{
	m_classAtom = 0;
	m_szWindowCaption[0] = 0;
	m_hwnd = NULL;
	m_lpD3D = NULL;
	m_lpDevice = NULL;
	m_hmod_d3d9 = NULL;
	m_hmod_d3dx9 = NULL;
	m_zFormat = D3DFMT_UNKNOWN;
	for (int i=0; i<MAX_DXC_ADAPTERS; i++)
		m_orig_windowed_mode_format[i] = D3DFMT_UNKNOWN;
	m_ordinal_adapter = D3DADAPTER_DEFAULT;
	m_ignore_wm_destroy = 0;
	m_hwnd_winamp = hWndWinamp;
	m_minimize_winamp = minimize_winamp;
	m_winamp_minimized = 0;
	m_truly_exiting = 0;
	m_bpp = 0;
	m_frame_delay = 0;
	StringCbCopyW(m_szIniFile, sizeof(m_szIniFile), szIniFile);
	memset(&myWindowState,0,sizeof(myWindowState));
	m_szDriver[0] = 0;
	m_szDesc[0] = 0;

	WNDCLASSW wc = {0};

	// clear the error register

	m_lastErr = S_OK;

	// clear the active flag

	m_ready=FALSE;

	// Set up and register window class

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // CS_DBLCLKS lets the window receive WM_LBUTTONDBLCLK, for toggling fullscreen mode...
	wc.lpfnWndProc = (WNDPROC) pProc;
	wc.cbWndExtra = sizeof(DWORD);
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLUGIN_ICON));//NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szClassName;
	m_classAtom = RegisterClassW(&wc);
	if (!m_classAtom)
	{
		wchar_t title[64];
		int y = GetLastError();
		m_lastErr = DXC_ERR_REGWIN;
		MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_UNABLE_TO_REGISTER_WINDOW_CLASS),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
		Internal_CleanUp();
		return;
	}

	StringCbCopy(m_szWindowCaption, sizeof(m_szWindowCaption), szWindowCaption);
	m_hInstance = hInstance;
	m_uWindowLong = uWindowLong;
}

DXContext::~DXContext()
{
	Internal_CleanUp();
}

void DXContext::Internal_CleanUp()
{
	// clear active flag
	m_ready=FALSE;

	// release 3D interfaces
	SafeRelease(m_lpDevice);
	SafeRelease(m_lpD3D);

	// destroy the window
	if (m_truly_exiting)
	{
		// somebody else will destroy the window for us!
		m_hwnd = NULL;
		if (m_hmod_d3d9)
		{
			FreeLibrary(m_hmod_d3d9);
			m_hmod_d3d9 = NULL;
		}

		if (m_hmod_d3dx9)
		{
			m_hmod_d3dx9 = NULL;
		}
	}

	if (myWindowState.me)
	{
		DestroyWindow(myWindowState.me);
		myWindowState.me = NULL;
		m_hwnd = NULL;
	}
	else if (m_hwnd)
	{
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;
	}

	// unregister window class.  note: only works if window is already destroyed!
	if (m_classAtom)
	{
		UnregisterClass(MAKEINTATOM(m_classAtom), m_hInstance);
		m_classAtom = 0;
	}

	RestoreWinamp();
}

void DXContext::GetSnappedClientSize()
{
	// Call this whenever you set m_REAL_client_width/height while in windowed mode,
	// to compute an appropriate (oversized) internal canvas size.  At the end of each
	// frame, for display, the canvas will be centered & cropped.
	m_client_width  = m_REAL_client_width;
	m_client_height = m_REAL_client_height;
#if (SNAP_WINDOWED_MODE_BLOCKSIZE >= 1)
	if (m_current_mode.screenmode == WINDOWED)
	{
		// oversize it - then we'll just crop - so onscreen text has no stretching  :)
		m_client_width  = max(1, (m_REAL_client_width  + 31)/32)*32;
		m_client_height = max(1, (m_REAL_client_height + 31)/32)*32;
	}
#endif
}

BOOL DXContext::TestFormat(int ordinal_adapter, D3DFORMAT fmt)
{
	if (D3D_OK==m_lpD3D->CheckDeviceType(ordinal_adapter,D3DDEVTYPE_HAL,fmt,fmt,FALSE))
		return TRUE;
	return FALSE;
}

BOOL DXContext::TestDepth(int ordinal_adapter, D3DFORMAT fmt)
{
	if (D3D_OK!=m_lpD3D->CheckDeviceFormat(ordinal_adapter,D3DDEVTYPE_HAL,m_current_mode.display_mode.Format,
	                                       D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,fmt))
		return FALSE;
	if (D3D_OK!=m_lpD3D->CheckDepthStencilMatch(ordinal_adapter,D3DDEVTYPE_HAL,
	    m_current_mode.display_mode.Format,m_current_mode.display_mode.Format,fmt))
		return FALSE;
	return TRUE;
}

int DXContext::CheckAndCorrectFullscreenDispMode(int ordinal_adapter, D3DDISPLAYMODE *pdm)
{
	// given the user's choice of fullscreen display mode,
	// go through all the display modes available to the currently-selected adapter
	// and find the best match.

	// returns 1 if it altered pdm to the best match,
	// or 0 if it was able to find a perfect match.

	// if it returns 1, you might want to notify the user.


#define MAX_DISPLAY_MODES 4096
	D3DDISPLAYMODE list[MAX_DISPLAY_MODES];
	int nCount = min(m_lpD3D->GetAdapterModeCount(ordinal_adapter, D3DFMT_A8R8G8B8), MAX_DISPLAY_MODES);
	int nValid = 0;
	for (int i=0; i<nCount; i++)
		if (m_lpD3D->EnumAdapterModes(ordinal_adapter, D3DFMT_A8R8G8B8, i, &list[nValid]) == D3D_OK)
			nValid++;

	// do many passes through the set until we find a match,
	// each time relaxing more constraints.
	// outline of the passes:

	int bpp_desired = 0;
	switch (pdm->Format)
	{
	case D3DFMT_R8G8B8  : bpp_desired = 32; break;
	case D3DFMT_A8R8G8B8: bpp_desired = 32; break;
	case D3DFMT_X8R8G8B8: bpp_desired = 32; break;
	case D3DFMT_R5G6B5  : bpp_desired = 16; break;
	case D3DFMT_X1R5G5B5: bpp_desired = 16; break;
	case D3DFMT_A1R5G5B5: bpp_desired = 16; break;
	case D3DFMT_A4R4G4B4: bpp_desired = 16; break;
	case D3DFMT_R3G3B2  : bpp_desired =  8; break;
	case D3DFMT_A8R3G3B2: bpp_desired = 16; break;
	case D3DFMT_X4R4G4B4: bpp_desired = 16; break;
	}

	// rep   MATCH:
	//  0. w,h,r,f
	//  1. w,h,-,f
	//  2. w,h,r,-         pass:
	//  3. w,h,-,-           -on pass 0, for 'f', match exact format
	//  4. 8,6,r,f           -on pass 1, for 'f', just match # of bits per pixel
	//  5. 8,6,-,f              (more relaxed match)
	//  6. 8,6,r,-
	//  7. 8,6,-,-
	//  8. -,-,r,f
	//  9. -,-,-,f
	// 10. -,-,r,-
	// 11. -,-,-,-
	int found = 0;
	for (int rep=0; rep<12 && !found; rep++)
	{
		for (int pass=0; pass<2 && !found; pass++)
		{
			for (i=0; i<nValid && !found; i++)
			{
				bool bMatch = true;

				int bpp_this_mode = 0;
				switch (list[i].Format)
				{
				case D3DFMT_R8G8B8  : bpp_this_mode = 32; break;
				case D3DFMT_A8R8G8B8: bpp_this_mode = 32; break;
				case D3DFMT_X8R8G8B8: bpp_this_mode = 32; break;
				case D3DFMT_R5G6B5  : bpp_this_mode = 16; break;
				case D3DFMT_X1R5G5B5: bpp_this_mode = 16; break;
				case D3DFMT_A1R5G5B5: bpp_this_mode = 16; break;
				case D3DFMT_A4R4G4B4: bpp_this_mode = 16; break;
				case D3DFMT_R3G3B2  : bpp_this_mode =  8; break;
				case D3DFMT_A8R3G3B2: bpp_this_mode = 16; break;
				case D3DFMT_X4R4G4B4: bpp_this_mode = 16; break;
				}

				if (rep < 4)
				{
					if (pdm->Width != list[i].Width)
						bMatch = false;
					if (pdm->Height != list[i].Height)
						bMatch = false;
				}
				else if (rep < 8)
				{
					if (DEFAULT_FULLSCREEN_WIDTH != list[i].Width)
						bMatch = false;
					if (DEFAULT_FULLSCREEN_HEIGHT != list[i].Height)
						bMatch = false;
				}

				if (((rep/2)%2)==0)
				{
					if (pass==0 && pdm->Format != list[i].Format)
						bMatch = false;
					else if (pass==1 && bpp_desired != bpp_this_mode)
						bMatch = false;
				}

				if (((rep%2)==0) && pdm->RefreshRate != list[i].RefreshRate)
				{
					bMatch = false;
				}

				if (bMatch)
				{
					memcpy(pdm, &list[i], sizeof(D3DDISPLAYMODE));
					found = 1;
					if (rep != 0 || pass != 0)
					{
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

BOOL CALLBACK MyMonitorEnumProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
)
{
	RECT* p = (RECT*)dwData;
	if (hMonitor)
	{
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (GetMonitorInfo(hMonitor, &mi))
		{
			p->top    = min(p->top   , mi.rcMonitor.top);
			p->left   = min(p->left  , mi.rcMonitor.left);
			p->right  = max(p->right , mi.rcMonitor.right);
			p->bottom = max(p->bottom, mi.rcMonitor.bottom);
		}
	}

	return TRUE;
}

int DXContext::GetWindowedModeAutoSize(int iteration)
{
	// note: requires 'm_monitor_rect' has been set!

	// generically determine size of window, for windowed mode:
	int x = m_monitor_rect.right-m_monitor_rect.left;
	int y = m_monitor_rect.bottom-m_monitor_rect.top;

	// if running in horz/vert-span multi-display mode, base the window size on
	// an actual display size, not the giant double-sized monitor.  Also, position
	// the window on the same monitor that Winamp is on.
	if (x >= y*2)
	{
		x /= 2;

		// move window to same display that Winamp is on:
		WINDOWPLACEMENT wp;
		wp.length = sizeof(wp);
		if (GetWindowPlacement(m_hwnd_winamp, &wp))
		{
			int winamp_center_x = (wp.rcNormalPosition.right + wp.rcNormalPosition.left)/2;
			if (winamp_center_x > x)
			{
				m_monitor_rect.left += x;
				m_monitor_rect.right += x;
			}
		}
	}
	else if (y > x*4/3)
	{
		y /= 2;

		// move window to same display that Winamp is on:
		WINDOWPLACEMENT wp;
		wp.length = sizeof(wp);
		if (GetWindowPlacement(m_hwnd_winamp, &wp))
		{
			int winamp_center_y = (wp.rcNormalPosition.top + wp.rcNormalPosition.bottom)/2;
			if (winamp_center_y > y)
			{
				m_monitor_rect.top += y;
				m_monitor_rect.bottom += y;
			}
		}
	}

	int size = min(x, y);
	size = (int)(size*DEFAULT_WINDOW_SIZE);
	size = (size/64 - iteration)*64;
	if (size < 64)
		size = 64;

	return size;
}

void DXContext::WriteSafeWindowPos()
{
	if (m_current_mode.screenmode == WINDOWED)
	{
		WritePrivateProfileIntW(64,     L"nMainWndTop",    m_szIniFile, L"settings");
		WritePrivateProfileIntW(64,     L"nMainWndLeft",   m_szIniFile, L"settings");
		WritePrivateProfileIntW(64+256, L"nMainWndRight",  m_szIniFile, L"settings");
		WritePrivateProfileIntW(64+256, L"nMainWndBottom", m_szIniFile, L"settings");
		WritePrivateProfileIntW(64,     L"avs_wx",m_szIniFile,L"settings");
		WritePrivateProfileIntW(64,     L"avs_wy",m_szIniFile,L"settings");
		WritePrivateProfileIntW(256, L"avs_ww",m_szIniFile,L"settings");
		WritePrivateProfileIntW(256, L"avs_wh",m_szIniFile,L"settings");
	}
}

// {0000000A-000C-0010-FF7B-01014263450C}
const GUID avs_guid =
  { 10, 12, 16, { 255, 123, 1, 1, 66, 99, 69, 12 } };

BOOL DXContext::Internal_Init(DXCONTEXT_PARAMS *pParams, BOOL bFirstInit)
{
	memcpy(&m_current_mode, pParams, sizeof(DXCONTEXT_PARAMS));
	memset(&myWindowState,0,sizeof(myWindowState));

	// various checks
	if (m_current_mode.screenmode != WINDOWED)
		m_current_mode.m_skin = 0;

	// 1. destroy old window
	if (m_hwnd)
	{
		m_ignore_wm_destroy = 1;
		DestroyWindow(m_hwnd);
		m_ignore_wm_destroy = 0;
		m_hwnd = NULL;
	}

	// 2. CHECK TO MAKE SURE DIRECTX/DDRAW IS INSTALLED
	if (bFirstInit)
	{
		// Test for DirectX 9 + start it
		// note: if you don't call LoadLibrary here, and you're on a system
		//       where DX9 is missing, Direct3DCreate8() might crash; so call it.
		int d3d9_already_loaded = (GetModuleHandle("d3d9.dll") != NULL) ? 1 : 0;
		if (!d3d9_already_loaded)
			m_hmod_d3d9 = LoadLibrary("d3d9.dll");

		if ((!d3d9_already_loaded && !m_hmod_d3d9) ||
		    !(m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION))
		   )
		{
			MissingDirectX(NULL);
			m_lastErr = DXC_ERR_CREATE3D;
			return FALSE;
		}

		if (!m_hmod_d3dx9)
			m_hmod_d3dx9 = FindD3DX9(m_hwnd_winamp);

		if ((!m_hmod_d3dx9))
		{
			MissingDirectX(NULL);
			m_lastErr = DXC_ERR_CREATE3D;
			return FALSE;
		}
	}

	// 3. get the smallest single rectangle that encloses ALL the monitors on the desktop:
	SetRect(&m_all_monitors_rect, 0, 0, 0, 0);
	EnumDisplayMonitors(NULL, NULL, MyMonitorEnumProc, (LPARAM)&m_all_monitors_rect);

	// 4. some DirectX- / DDraw-specific stuff.  Also determine hPluginMonitor.
	HMONITOR hPluginMonitor = NULL;
	{
		D3DADAPTER_IDENTIFIER9 temp;

		// find the ordinal # of the adapter whose GUID matches what the user picked from the config panel,
		// and whose DeviceName matches as well.
		// if no match found, use D3DADAPTER_DEFAULT.
		m_ordinal_adapter = D3DADAPTER_DEFAULT;
		int nAdapters = m_lpD3D->GetAdapterCount();
		{
			for (int i=0; i<nAdapters; i++)
			{
				if ((m_lpD3D->GetAdapterIdentifier(i, /*D3DENUM_NO_WHQL_LEVEL*/ 0, &temp) == D3D_OK) &&
				    (memcmp(&temp.DeviceIdentifier, &m_current_mode.adapter_guid, sizeof(GUID))==0) &&
				    !strcmp(temp.DeviceName, m_current_mode.adapter_devicename)
				   )
				{
					m_ordinal_adapter = i;
					break;
				}
			}
		}

		if (m_lpD3D->GetAdapterIdentifier(m_ordinal_adapter, /*D3DENUM_NO_WHQL_LEVEL*/ 0, &temp) == D3D_OK)
		{
			StringCbCopy(m_szDriver, sizeof(m_szDriver), temp.Driver);
			StringCbCopy(m_szDesc, sizeof(m_szDesc), temp.Description);
		}

		int caps_ok = 0;
		int caps_tries = 0;
		int changed_fs_disp_mode;

		// try to get the device caps for the adapter selected from the config panel.
		// if GetDeviceCaps() fails, it's probably because the adapter has been
		// removed from the system (or disabled), so we try again with other adapter(s).
		do
		{
			changed_fs_disp_mode = 0;

			SetRect(&m_monitor_rect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

			// get bounding rect of the monitor attached to the adapter (to assist w/window positioning)
			// note: in vert/horz span setups (psuedo-multimon),
			//       this will be 2048x768 or 1024x1536 or something like that.
			hPluginMonitor = m_lpD3D->GetAdapterMonitor(m_ordinal_adapter);
			/*if (hPluginMonitor)
			{
			    MONITORINFO mi;
			    mi.cbSize = sizeof(mi);
			    if (GetMonitorInfo(hPluginMonitor, &mi))
			    {
			        memcpy(&m_monitor_rect, &mi.rcMonitor, sizeof(RECT));
			        memcpy(&m_monitor_work_rect, &mi.rcWork, sizeof(RECT));
			    }
			}*/

			if (bFirstInit)
			{
				for (int i=0; i<min(nAdapters, MAX_DXC_ADAPTERS); i++)
				{
					// if this is the first call to Init, get the display mode's original color format,
					// before we go changing it:
					D3DDISPLAYMODE d3ddm;
					if (FAILED(m_lpD3D->GetAdapterDisplayMode(i, &d3ddm)))
					{
						d3ddm.Format = D3DFMT_UNKNOWN;
					}
					m_orig_windowed_mode_format[i] = d3ddm.Format;
				}
			}

			// figure out pixel (color) format for back buffer: (m_current_mode.display_mode.Format)
			if (m_current_mode.screenmode!=FULLSCREEN && m_ordinal_adapter < MAX_DXC_ADAPTERS)
				m_current_mode.display_mode.Format = m_orig_windowed_mode_format[m_ordinal_adapter];
			// else
			// for fullscreen, use what they gave us

			if (m_current_mode.display_mode.Format == D3DFMT_UNKNOWN ||
			    !TestFormat(m_ordinal_adapter, m_current_mode.display_mode.Format))
			{
				// if they try to run the plugin without ever running the config panel
				// first (& pressing OK), then the fullscreen pixelformat hasn't been
				// chosen... so we try all the possilibities until one works:
				if (TestFormat(m_ordinal_adapter,D3DFMT_A8R8G8B8)) m_current_mode.display_mode.Format = D3DFMT_A8R8G8B8;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_X8R8G8B8)) m_current_mode.display_mode.Format = D3DFMT_X8R8G8B8;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_R8G8B8)) m_current_mode.display_mode.Format = D3DFMT_R8G8B8  ;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_R5G6B5)) m_current_mode.display_mode.Format = D3DFMT_R5G6B5  ;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_X1R5G5B5)) m_current_mode.display_mode.Format = D3DFMT_X1R5G5B5;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_A1R5G5B5)) m_current_mode.display_mode.Format = D3DFMT_A1R5G5B5;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_A4R4G4B4)) m_current_mode.display_mode.Format = D3DFMT_A4R4G4B4;
				else if (TestFormat(m_ordinal_adapter,D3DFMT_X4R4G4B4)) m_current_mode.display_mode.Format = D3DFMT_X4R4G4B4;
			}

			if (m_current_mode.display_mode.Format==D3DFMT_UNKNOWN)
			{
				wchar_t title[64];
				m_lastErr = DXC_ERR_FORMAT;
				MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_DIRECTX_INIT_FAILED),
						    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
						    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
				return FALSE;
			}

			if (m_current_mode.screenmode == FULLSCREEN)
				changed_fs_disp_mode = CheckAndCorrectFullscreenDispMode(m_ordinal_adapter, &m_current_mode.display_mode);

			// figure out pixel format of the z-buffer: (m_zFormat)
			m_zFormat = D3DFMT_UNKNOWN;
			/*
			if      (TestDepth(m_ordinal_adapter,D3DFMT_D32         )) m_zFormat=D3DFMT_D32;
			else if (TestDepth(m_ordinal_adapter,D3DFMT_D24S8       )) m_zFormat=D3DFMT_D24S8;
			else if (TestDepth(m_ordinal_adapter,D3DFMT_D24X4S4     )) m_zFormat=D3DFMT_D24X4S4;
			else if (TestDepth(m_ordinal_adapter,D3DFMT_D24X8       )) m_zFormat=D3DFMT_D24X8;
			else if (TestDepth(m_ordinal_adapter,D3DFMT_D16         )) m_zFormat=D3DFMT_D16;
			else if (TestDepth(m_ordinal_adapter,D3DFMT_D15S1       )) m_zFormat=D3DFMT_D15S1;
			else if (TestDepth(m_ordinal_adapter,D3DFMT_D16_LOCKABLE)) m_zFormat=D3DFMT_D16_LOCKABLE;
			*/

			// get device caps:
			memset(&m_caps, 0, sizeof(m_caps));
			if (FAILED(m_lpD3D->GetDeviceCaps(m_ordinal_adapter, D3DDEVTYPE_HAL, &m_caps)))
			{
				// that adapter was found in the system, but it might be disabled
				// (i.e. 'extend my Windows desktop onto this monitor') is unchecked)
				// so, try other adapters (try all sequentially).

				if (caps_tries < nAdapters)
				{
					// try again, this time using the default adapter:
					m_ordinal_adapter = caps_tries;
					caps_tries++;
				}
				else
				{
					wchar_t title[64];
					m_lastErr = DXC_ERR_CAPSFAIL;
					MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_DXC_ERR_CAPSFAIL),
							    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
							    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
					return FALSE;
				}
			}
			else
			{
				caps_ok = 1;
			}
		}
		while (!caps_ok);

		if (changed_fs_disp_mode)
		{
			wchar_t title[64];
			MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_FS_DISPLAY_MODE_SELECTED_IS_INVALID),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_WARNING, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
		}

		switch (m_current_mode.display_mode.Format)
		{
		case D3DFMT_R8G8B8  : m_bpp = 32; break;
		case D3DFMT_A8R8G8B8: m_bpp = 32; break;
		case D3DFMT_X8R8G8B8: m_bpp = 32; break;
		case D3DFMT_R5G6B5  : m_bpp = 16; break;
		case D3DFMT_X1R5G5B5: m_bpp = 16; break;
		case D3DFMT_A1R5G5B5: m_bpp = 16; break;
		case D3DFMT_A8R3G3B2: m_bpp = 16; break;
		case D3DFMT_A4R4G4B4: m_bpp = 16; break;
		case D3DFMT_X4R4G4B4: m_bpp = 16; break;
		case D3DFMT_R3G3B2  : m_bpp =  8; break; // misleading?  implies a palette...
		}
	}

	// 5. set m_monitor_rect and m_monitor_work_rect.
	if (hPluginMonitor)
	{
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (GetMonitorInfo(hPluginMonitor, &mi))
		{
			m_monitor_rect = mi.rcMonitor;
			m_monitor_rect_orig = mi.rcMonitor;
			m_monitor_work_rect = mi.rcWork;
			m_monitor_work_rect_orig = mi.rcWork;
		}
	}

	// 6. embedded window stuff [where the plugin window is integrated w/winamp]
	if (m_current_mode.m_skin)
	{
		// set up the window's position on screen
		// note that we'd prefer to set the CLIENT size we want, but we can't, so we'll just do
		// this here, and later, adjust the client rect size to what's left...
		int size = GetWindowedModeAutoSize(0);  // note: requires 'm_monitor_rect' has been set!
		myWindowState.r.left   = GetPrivateProfileIntW(L"settings",L"avs_wx",64,m_szIniFile);
		myWindowState.r.top    = GetPrivateProfileIntW(L"settings",L"avs_wy",64,m_szIniFile);
		myWindowState.r.right  = myWindowState.r.left + GetPrivateProfileIntW(L"settings",L"avs_ww",size+24,m_szIniFile);
		myWindowState.r.bottom = myWindowState.r.top  + GetPrivateProfileIntW(L"settings",L"avs_wh",size+40,m_szIniFile);

		// only works on winamp 2.90+!
		int success = 0;
		if (GetWinampVersion(mod1.hwndParent) >= 0x2900)
		{
			SET_EMBED_GUID((&myWindowState), avs_guid);
			myWindowState.flags |= EMBED_FLAGS_NOTRANSPARENCY;
			HWND (*e)(embedWindowState *v);
			*(void**)&e = (void *)SendMessage(mod1.hwndParent,WM_WA_IPC,(LPARAM)0,IPC_GET_EMBEDIF);
			if (e)
			{
				m_current_mode.parent_window = e(&myWindowState);
				if (m_current_mode.parent_window)
				{
					SetWindowText(m_current_mode.parent_window, m_szWindowCaption);
					success = 1;
				}
			}
		}

		if (!success)
			m_current_mode.m_skin = 0;
	}

	// remember the client rect that was originally desired...
	RECT windowed_mode_desired_client_rect;
	windowed_mode_desired_client_rect.top    = GetPrivateProfileIntW(L"settings",L"nMainWndTop",-1,m_szIniFile);
	windowed_mode_desired_client_rect.left   = GetPrivateProfileIntW(L"settings",L"nMainWndLeft",-1,m_szIniFile);
	windowed_mode_desired_client_rect.right  = GetPrivateProfileIntW(L"settings",L"nMainWndRight",-1,m_szIniFile);
	windowed_mode_desired_client_rect.bottom = GetPrivateProfileIntW(L"settings",L"nMainWndBottom",-1,m_szIniFile);

	// ...and in case windowed mode init fails severely,
	// set it up to try next time for a simple 256x256 window.
	WriteSafeWindowPos();

	// 7. create the window, if not already created
	if (!m_hwnd)
	{
		m_hwnd = CreateWindowEx(
		           MY_EXT_WINDOW_STYLE, // extended style
		           MAKEINTATOM(m_classAtom), // class
		           m_szWindowCaption, // caption
		           MY_WINDOW_STYLE, // style
		           0, // left
		           0, // top
		           256,  // temporary width
		           256,  // temporary height
		           m_current_mode.parent_window,  // parent window
		           NULL, // menu
		           m_hInstance, // instance
		           (LPVOID)m_uWindowLong
		         ); // parms

		if (!m_hwnd)
		{
			wchar_t title[64];
			m_lastErr = DXC_ERR_CREATEWIN;
			MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_CREATEWINDOW_FAILED),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
			return FALSE;
		}

		SendMessage(m_hwnd_winamp, WM_WA_IPC, (WPARAM)m_hwnd, IPC_SETVISWND);

		if (m_current_mode.m_skin)
		{
			if (GetWinampVersion(mod1.hwndParent) < 0x5051)
				ShowWindow(m_current_mode.parent_window,SW_SHOWNA); // showing the parent wnd will make it size the child, too
			else
				SendMessage(m_current_mode.parent_window, WM_USER+102, 0, 0); // benski> major hack alert. winamp's embedwnd will call ShowWindow in response.  SendMessage moves us over to the main thread (we're currently sitting on the viz thread)
		}		
	}

	// 8. minimize winamp before creating devices & such, so there aren't
	//    any confusing window-focus issues
	MinimizeWinamp(hPluginMonitor);

	// 9. loop to try and create the window.
	//      if in windowed mode and not enough vidmem, it will try again w/smaller window
	//      (repeatedly, until window client size would be < 64)
	int iteration = 0;
	int device_ok = 0;
	do
	{
		// set the window position
		if (m_current_mode.screenmode==DESKTOP ||
		    m_current_mode.screenmode==FAKE_FULLSCREEN)
		{
			int x = m_monitor_rect.right - m_monitor_rect.left;
			int y = m_monitor_rect.bottom - m_monitor_rect.top;

			if (x >= y*2)
			{
				// (pseudo-multimon modes like 2048x768)
				int mid = (m_monitor_rect.left + m_monitor_rect.right)/2;
				if (m_current_mode.m_dualhead_horz==1) // show on left side
					m_monitor_rect.right = mid;
				else if (m_current_mode.m_dualhead_horz==2) // show on right side
					m_monitor_rect.left = mid;
			}
			else if (y > x*4/3)
			{
				// (pseudo-multimon modes like 1024x1536)
				int mid = (m_monitor_rect.top + m_monitor_rect.bottom)/2;
				if (m_current_mode.m_dualhead_vert==1) // show on top half
					m_monitor_rect.bottom = mid;
				else if (m_current_mode.m_dualhead_vert==2) // show on bottom half
					m_monitor_rect.top = mid;
			}

			// recompute width & height (into x,y):
			x = m_monitor_rect.right - m_monitor_rect.left;
			y = m_monitor_rect.bottom - m_monitor_rect.top;

			m_client_width  = x;
			m_client_height = y;
			m_window_width  = x;
			m_window_height = y;

			if (m_current_mode.screenmode == DESKTOP)
			{
				// note: we initially hide the window, and then
				// only display it once the desktop is all nice & ready.
				// see CPluginShell::DrawAndDisplay().

				RECT r = m_monitor_rect;

				// if possible, shrink the desktop window so it doesn't cover the taskbar.
				HWND hTaskbar = FindWindow("Shell_TrayWnd", "");
				if (hTaskbar)
				{
					RECT taskbar;
					GetWindowRect(hTaskbar, &taskbar);
					int tbw = taskbar.right - taskbar.left;
					int tbh = taskbar.bottom-taskbar.top;

					if (taskbar.bottom == m_monitor_rect.bottom &&
					    taskbar.left == m_monitor_rect.left &&
					    taskbar.right == m_monitor_rect.right)
					{
						r.bottom -= tbh;
					}
					else if (taskbar.top == m_monitor_rect.top &&
					         taskbar.left == m_monitor_rect.left &&
					         taskbar.right == m_monitor_rect.right)
					{
						r.top += tbh;
					}
					else if (taskbar.left == m_monitor_rect.left &&
					         taskbar.top == m_monitor_rect.top &&
					         taskbar.bottom == m_monitor_rect.bottom)
					{
						r.left += tbw;
					}
					else if (taskbar.right == m_monitor_rect.right &&
					         taskbar.top == m_monitor_rect.top &&
					         taskbar.bottom == m_monitor_rect.bottom)
					{
						r.right -= tbw;
					}

					m_client_width  = r.right - r.left;
					m_client_height = r.bottom - r.top;
					m_REAL_client_width = m_client_width;
					m_REAL_client_height = m_client_height;
					m_window_width  = m_client_width;
					m_window_height = m_client_height;

					//...ok, but text is squished - some w/h is not right...

				}

				SetWindowPos(m_hwnd,HWND_BOTTOM,r.left,r.top,r.right-r.left,r.bottom-r.top,SWP_HIDEWINDOW);
			}
			else // FAKE_FULLSCREEN
			{
				if (memcmp(&m_all_monitors_rect, &m_monitor_rect, sizeof(RECT))==0)
				{
					// there's only one display, and it's entirely covered
					// by the plugin -> PUT THE PLUGIN ABOVE THE TASKBAR
					// -> normally, if the user clicked another window,
					//      it would pop the taskbar to the top; but we don't
					//      have to worry about that here, since we're taking
					//      up the whole screen.
					// -> don't worry about making the text, etc. avoid
					//      the taskbar in this case (see DrawAndDisplay())
					// -> DO worry about hiding the mouse cursor in this case
					//      (see WM_SETCURSOR handler)

					m_fake_fs_covers_all = 1;
					//SetWindowPos(m_hwnd,HWND_TOPMOST,m_monitor_rect.left,m_monitor_rect.top,m_window_width,m_window_height,SWP_SHOWWINDOW);
				}
				else
				{
					// there is space to work outside of the plugin window.
					// -> here we pretty much have to let the taskbar stay on
					//   top, because it really likes to be there; i.e.,
					//   if you click any other window, it automatically
					//   pops up again.
					// -> therefore, TRY TO KEEP THE WINDOW ON BOTTOM
					//      (below the taskbar). (see PushWindowToBack)
					// -> don't worry about hiding the mouse cursor in this case
					//      (see WM_SETCURSOR handler)
					// -> DO worry about making the text, etc. avoid
					//      the taskbar in this case (see DrawAndDisplay())

					// (note that if taskbar is in the way, they can move it,
					//   since there are other monitors available)

					m_fake_fs_covers_all = 0;
					//SetWindowPos(m_hwnd,HWND_TOP,m_monitor_rect.left,m_monitor_rect.top,m_window_width,m_window_height,SWP_SHOWWINDOW);
				}

				SetWindowPos(m_hwnd,HWND_TOPMOST,m_monitor_rect.left,m_monitor_rect.top,m_window_width,m_window_height,SWP_SHOWWINDOW);
			}
		}
		else if (m_current_mode.screenmode == FULLSCREEN)
		{
			int x = m_current_mode.display_mode.Width ;
			int y = m_current_mode.display_mode.Height;
			int cx = m_monitor_rect.right - m_monitor_rect.left;
			int cy = m_monitor_rect.bottom - m_monitor_rect.top;

			// test #1
			if (x >= y*2 || y > x*4/3)     // tackle problem of vert/horz spans
			{
				wchar_t title[64];
				int ret = MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_TRYING_TO_ENTER_FS_MODE_WITH_MULTIPLE_DISPLAYS),
									  WASABI_API_LNGSTRINGW_BUF(IDS_TIP, title, 64),
									  MB_OKCANCEL|MB_SETFOREGROUND|MB_TOPMOST);
				if (ret==IDCANCEL)
				{
					m_lastErr = DXC_ERR_USER_CANCELED;
					return FALSE;
				}
			}

			// test #2
			if ((cx >= cy*2 && x < y*2) || (cy > cx*4/3 && y <= x*4/3))
			{
				wchar_t title[64];
				int ret = MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_TRYING_TO_ENTER_FS_MODE_WITH_MULTIPLE_DISPLAYS_2),
									  WASABI_API_LNGSTRINGW_BUF(IDS_TIP, title, 64),
									  MB_OKCANCEL|MB_SETFOREGROUND|MB_TOPMOST);
				if (ret==IDCANCEL)
				{
					m_lastErr = DXC_ERR_USER_CANCELED;
					return FALSE;
				}
			}

			m_client_width  = x;
			m_client_height = y;
			m_window_width  = x;
			m_window_height = y;
			SetWindowPos(m_hwnd,HWND_TOPMOST,m_monitor_rect.left,m_monitor_rect.top,m_window_width,m_window_height,SWP_SHOWWINDOW);
		}
		else // WINDOWED
		{
			RECT margin;
			if (m_current_mode.m_skin)
			{
				RECT r1, r2;
				GetWindowRect(m_current_mode.parent_window, &r1);
				GetWindowRect(m_hwnd , &r2);
				margin.left  = r2.left - r1.left;
				margin.right = r1.right - r2.right;
				margin.top   = r2.top - r1.top;
				margin.bottom= r1.bottom - r2.bottom;
			}
			else
			{
				RECT r1;
				SetRect(&r1, 0, 0, 256, 256);
				AdjustWindowRect(&r1, MY_WINDOW_STYLE, 0);
				margin.left  = 0 - r1.left;
				margin.right = r1.right - 256;
				margin.top   = 0 - r1.top;
				margin.bottom= r1.bottom - 256;
			}

			int autosize = 1;

			RECT r = windowed_mode_desired_client_rect;
			if (iteration==0 && r.top != -1 && r.left != -1 && r.bottom != -1 && r.right != -1)
			{
				// use prev. window coordinates:
				m_REAL_client_width  = r.right - r.left;
				m_REAL_client_height = r.bottom - r.top;
				GetSnappedClientSize();
				if (m_current_mode.m_skin) // check this here in case they got a non-aligned size by resizing when "integrated with winamp" was unchecked, then checked it & ran the plugin...
				{
					// STRANGE ALIGNMENTS FOR THE WINDOW FRAME: (required by winamp 2):
					// the window frame's width must be divisible by 25, and height by 29.
					if (GetWinampVersion(mod1.hwndParent) < 0x4000) // ... winamp 5 doesn't have this prob.  (test vs. 0x4000 because winamp5 betas have version tags like 0x4987)
					{
						m_REAL_client_width  = ((m_client_width + margin.left + margin.right)/25)*25 - margin.left - margin.right;
						m_REAL_client_height = ((m_client_height + margin.top + margin.bottom)/29)*29 - margin.top - margin.bottom;
						GetSnappedClientSize();
					}
				}

				// transform screen-space CLIENT rect into screen-space WINDOW rect
				r.top    = windowed_mode_desired_client_rect.top    - margin.top;
				r.left   = windowed_mode_desired_client_rect.left   - margin.left;
				r.right  = r.left + margin.left + m_REAL_client_width  + margin.right;
				r.bottom = r.top  + margin.top  + m_REAL_client_height + margin.bottom;

				// make sure the window is entirely visible on the selected monitor;
				//   otherwise, autosize/place it.
				// (note that this test is only appled 1) at startup, and 2) after a resize/max/restore.
				//  this test is not applied when merely moving the window.)
				if (r.top    >= m_monitor_work_rect.top &&
				    r.left   >= m_monitor_work_rect.left &&
				    r.right  <= m_monitor_work_rect.right &&
				    r.bottom <= m_monitor_work_rect.bottom)
				{
					if (m_current_mode.m_skin)
					{
						m_window_width  = m_REAL_client_width ; // m_window_width/height are for OUR borderless window, not the embedwnd parent frame.
						m_window_height = m_REAL_client_height;
						SetWindowPos(m_current_mode.parent_window,HWND_NOTOPMOST, r.left, r.top, r.right-r.left, r.bottom-r.top, /*SWP_SHOWWINDOW|*//*SWP_ASYNCWINDOWPOS*/0);
						SetWindowPos(m_hwnd ,HWND_NOTOPMOST, windowed_mode_desired_client_rect.left,
						             windowed_mode_desired_client_rect.top,
						             m_REAL_client_width,
						             m_REAL_client_height,
						             SWP_SHOWWINDOW);
					}
					else
					{
						m_window_width  = r.right - r.left;
						m_window_height = r.bottom - r.top;
						SetWindowPos(m_hwnd,HWND_NOTOPMOST,r.left,r.top,m_window_width,m_window_height,SWP_SHOWWINDOW);
					}

					autosize = 0;
				}
			}

			if (autosize)
			{
				int size = GetWindowedModeAutoSize(iteration); // note: requires 'm_monitor_rect' has been set!

				m_REAL_client_width  = size;
				m_REAL_client_height = size;
				GetSnappedClientSize();

				if (m_current_mode.m_skin)
				{
					// STRANGE ALIGNMENTS FOR THE WINDOW FRAME: (required by winamp 2):
					// the window frame's width must be divisible by 25, and height by 29.
					if (GetWinampVersion(mod1.hwndParent) < 0x4000) // ... winamp 5 doesn't have this prob.  (test vs. 0x4000 because winamp5 betas have version tags like 0x4987)
					{
						m_REAL_client_width  = ((m_client_width + margin.left + margin.right)/25)*25 - margin.left - margin.right;
						m_REAL_client_height = ((m_client_height + margin.top + margin.bottom)/29)*29 - margin.top - margin.bottom;
						GetSnappedClientSize();
					}

					m_window_width  = m_client_width ; // m_window_width/height are for OUR [borderless] window, not the parent window (which is the embedwnd frame).
					m_window_height = m_client_height;
					SetWindowPos(m_current_mode.parent_window,HWND_NOTOPMOST, m_monitor_work_rect.left+32, m_monitor_work_rect.top+32, m_client_width + margin.left + margin.right, m_client_height + margin.top + margin.bottom, /*SWP_SHOWWINDOW|*//*SWP_ASYNCWINDOWPOS*/0);
					SetWindowPos(m_hwnd ,HWND_NOTOPMOST, m_monitor_work_rect.left+32 + margin.left, m_monitor_work_rect.top+32 + margin.top, m_client_width, m_client_height, SWP_SHOWWINDOW);
				}
				else
				{
					SetRect(&r, 0, 0, size, size);
					AdjustWindowRect(&r, MY_WINDOW_STYLE, 0);

					m_window_width  = r.right - r.left;
					m_window_height = r.bottom - r.top;

					SetWindowPos(m_hwnd,HWND_NOTOPMOST, m_monitor_work_rect.left+32, m_monitor_work_rect.top+32, m_window_width, m_window_height, SWP_SHOWWINDOW);
				}
			}
		}

		m_frame_delay = 1;      // set this to 2 if you use triple buffering!

		{
			m_current_mode.display_mode.Width  = m_client_width;
			m_current_mode.display_mode.Height = m_client_height;

			// set up m_d3dpp (presentation parameters):
			ZeroMemory(&m_d3dpp,sizeof(m_d3dpp));
			m_d3dpp.Windowed         = (m_current_mode.screenmode==FULLSCREEN) ? 0 : 1;
			m_d3dpp.BackBufferFormat = m_current_mode.display_mode.Format;
			m_d3dpp.BackBufferWidth  = m_client_width;
			m_d3dpp.BackBufferHeight = m_client_height;
			m_d3dpp.BackBufferCount  = m_current_mode.nbackbuf;
			if (m_current_mode.screenmode==FULLSCREEN)
				m_d3dpp.SwapEffect   = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP;
			else    // windowed or fake FS
				m_d3dpp.SwapEffect   = (m_current_mode.allow_page_tearing) ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_COPY;//D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP;
			// note: multisampling is only allowed if swapeffect is DISCARD!
			m_d3dpp.MultiSampleType  = (m_d3dpp.SwapEffect==D3DSWAPEFFECT_DISCARD) ? m_current_mode.multisamp : D3DMULTISAMPLE_NONE;
			//m_d3dpp.hDeviceWindow  = m_hwnd;
			if (m_current_mode.screenmode==FULLSCREEN)
			{
				m_d3dpp.FullScreen_RefreshRateInHz = m_current_mode.display_mode.RefreshRate;//D3DPRESENT_RATE_DEFAULT;
				m_d3dpp.PresentationInterval       = m_current_mode.allow_page_tearing ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_ONE;//D3DPRESENT_INTERVAL_IMMEDIATE;//D3DPRESENT_INTERVAL_ONE;
			}
			if (m_zFormat != D3DFMT_UNKNOWN)
			{
				m_d3dpp.EnableAutoDepthStencil=TRUE;
				m_d3dpp.AutoDepthStencilFormat=m_zFormat;
			}

			// finally, create the device:
			HRESULT hRes;
			if (FAILED(hRes = m_lpD3D->CreateDevice(
			                    m_ordinal_adapter,
			                    D3DDEVTYPE_HAL,
			                    m_hwnd,
			                    (m_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_MIXED_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			                    &m_d3dpp,
			                    &m_lpDevice)))
			{
				int code = LOWORD(hRes);

				wchar_t str[1024];
				if (code==2156) //D3DERR_NOTAVAILABLE
				{
					m_lastErr = DXC_ERR_CREATEDEV_NOT_AVAIL;

					wchar_t str[2048];
					WASABI_API_LNGSTRINGW_BUF(IDS_UNABLE_TO_CREATE_DIRECTX_DEVICE, str, 2048);

					if (m_current_mode.screenmode == FULLSCREEN)
						StringCbCatW(str, sizeof(str), WASABI_API_LNGSTRINGW(IDS_OLDER_DISPLAY_ADAPTER_CATENATION));
					else
						StringCbCatW(str, sizeof(str), WASABI_API_LNGSTRINGW(IDS_OLDER_DISPLAY_ADAPTER_CATENATION_2));

					MessageBoxW(m_hwnd,str,
							   WASABI_API_LNGSTRINGW(IDS_MILKDROP_ERROR),
							   MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
					return FALSE;
				}
				else if (m_current_mode.screenmode==WINDOWED && m_client_width>64)
				{
					// DO NOTHING; try again w/smaller window
				}
				else if (m_current_mode.screenmode != WINDOWED || m_client_width <= 64)
				{
					// usually, code==2154 here, which is D3DERR_OUTOFVIDEOMEMORY
					m_lastErr = DXC_ERR_CREATEDEV_PROBABLY_OUTOFVIDEOMEMORY;
					StringCbPrintfW(str, sizeof(str), WASABI_API_LNGSTRINGW(IDS_DIRECTX_INIT_FAILED_X), LOWORD(hRes));

					// NOTE: *A 'SUGGESTION' SCREEN SHOULD APPEAR NEXT, PROVIDED BY THE CALLER*
					MessageBoxW(m_hwnd, str,
							    WASABI_API_LNGSTRINGW(IDS_MILKDROP_ERROR),
							    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
					return FALSE;
				}
			}
			else
			{
				device_ok = 1;
			}
		}

		iteration++;
	}
	while (!device_ok);

	// set initial viewport
	SetViewport();

	// for desktop mode, push window to back again:
	if (m_current_mode.screenmode==DESKTOP)
		SetWindowPos(m_hwnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	if (m_current_mode.m_skin)
	{
		if (GetWinampVersion(mod1.hwndParent) < 0x5051) 
			SetFocus(m_current_mode.parent_window);
		else
			PostMessage(m_current_mode.parent_window, WM_USER+103, 0, 0); 
		
		//SetActiveWindow(m_current_mode.parent_window);
		//SetForegroundWindow(m_current_mode.parent_window);
	}

	/*if (m_current_mode.screenmode == WINDOWED)
		SaveWindow();*/

	// return success
	m_ready = TRUE;
	// benski> a little hack to get the window size correct. it seems to work
	if (m_current_mode.screenmode==WINDOWED)
		PostMessage(m_hwnd, WM_USER+555, 0, 0);
	return TRUE;
}

BOOL DXContext::StartOrRestartDevice(DXCONTEXT_PARAMS *pParams)
{
	// call this to [re]initialize the DirectX environment with new parameters.
	// examples: startup; toggle windowed/fullscreen mode; change fullscreen resolution;
	//   and so on.
	// be sure to clean up all your DirectX stuff first (textures, vertex buffers,
	//   D3DX allocations, etc.) and reallocate it afterwards!

	// note: for windowed mode, 'pParams->disp_mode' (w/h/r/f) is ignored.

	// destroy old window
	if (myWindowState.me)
	{
		m_ignore_wm_destroy = 1;
		if (m_current_mode.screenmode == WINDOWED)
			SaveWindow();
		DestroyWindow(myWindowState.me);
		myWindowState.me = NULL;
		m_ignore_wm_destroy = 0;
		m_hwnd=0;
	}
	else if (m_hwnd)
	{
		SendMessage(m_hwnd_winamp, WM_WA_IPC, NULL, IPC_SETVISWND);
		m_ignore_wm_destroy = 1;
		DestroyWindow(m_hwnd);
		m_ignore_wm_destroy = 0;
		m_hwnd = NULL;
	}

	if (!m_ready)
	{
		// first-time init: create a fresh new device
		return Internal_Init(pParams, TRUE);
	}
	else
	{
		// re-init: preserve the DX9 object (m_lpD3D),
		// but destroy and re-create the DX9 device (m_lpDevice).
		m_ready = FALSE;

		SafeRelease(m_lpDevice);
		// but leave the D3D object!

		RestoreWinamp();
		return Internal_Init(pParams, FALSE);
	}
}

BOOL DXContext::OnUserResizeWindow(RECT *new_window_rect, RECT *new_client_rect)
{
	// call this function on WM_EXITSIZEMOVE when running windowed.
	// don't bother calling this when fullscreen.
	// be sure to clean up all your DirectX stuff first (textures, vertex buffers,
	//   D3DX allocations, etc.) and reallocate it afterwards!

	if (!m_ready || (m_current_mode.screenmode != WINDOWED))
		return FALSE;

	if ((m_client_width  == new_client_rect->right - new_client_rect->left) &&
	    (m_client_height == new_client_rect->bottom - new_client_rect->top) &&
	    (m_window_width  == new_window_rect->right - new_window_rect->left) &&
	    (m_window_height == new_window_rect->bottom - new_window_rect->top))
	{
		return TRUE;
	}

	m_ready = FALSE;

	m_window_width  = new_window_rect->right  - new_window_rect->left;
	m_window_height = new_window_rect->bottom - new_window_rect->top;
	m_REAL_client_width  = new_client_rect->right  - new_client_rect->left;
	m_REAL_client_height = new_client_rect->bottom - new_client_rect->top;
	GetSnappedClientSize();  //sets m_client_width/height, but with snapping, if in windowed mode.

	m_d3dpp.BackBufferWidth  = m_client_width;
	m_d3dpp.BackBufferHeight = m_client_height;
	if (m_lpDevice->Reset(&m_d3dpp) != D3D_OK)
	{
		WriteSafeWindowPos();

		wchar_t title[64];
		MessageBoxW(m_hwnd, WASABI_API_LNGSTRINGW(IDS_WINDOW_RESIZE_FAILED),
				    WASABI_API_LNGSTRINGW_BUF(IDS_OUT_OF_VIDEO_MEMORY, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);

		m_lastErr = DXC_ERR_RESIZEFAILED;
		return FALSE;
	}

	SetViewport();
	m_ready = TRUE;
	return TRUE;
}

void DXContext::SetViewport()
{
	D3DVIEWPORT9 v;
	v.X = 0;
	v.Y = 0;
	v.Width = m_client_width;
	v.Height = m_client_height;
	v.MinZ = 0.0f;
	v.MaxZ = 1.0f;
	m_lpDevice->SetViewport(&v);
}

void DXContext::MinimizeWinamp(HMONITOR hPluginMonitor)
{
	// minimize Winamp window

	HMONITOR hWinampMon = MonitorFromWindow(m_hwnd_winamp, MONITOR_DEFAULTTONEAREST);
	HMONITOR hPluginMon = hPluginMonitor;//MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);//m_lpD3D->GetAdapterMonitor(ordinal_adapter);

	if ((m_current_mode.screenmode == FULLSCREEN || m_current_mode.screenmode == FAKE_FULLSCREEN) &&
	    (m_minimize_winamp) &&
	    (hWinampMon && hPluginMon && hPluginMon==hWinampMon) &&
	    (!m_winamp_minimized)
	   )
	{
		// nitpicky check: if we're in fake fullscreen mode
		// and are only going to display on half the screen,
		// don't minimize Winamp.
		if (m_current_mode.screenmode == FAKE_FULLSCREEN)
		{
			int x = m_monitor_rect.right - m_monitor_rect.left;
			int y = m_monitor_rect.bottom - m_monitor_rect.top;
			if ((x >= y*2 && m_current_mode.m_dualhead_horz != 0) ||
			    (y > x*4/3 && m_current_mode.m_dualhead_vert != 0))
			{
				return;
			}
		}

		ShowWindow(m_hwnd_winamp, SW_MINIMIZE);
		// also restore the focus to the plugin window, since this will steal it:
		SetFocus(m_hwnd);
		SetActiveWindow(m_hwnd);
		SetForegroundWindow(m_hwnd);
		m_winamp_minimized = 1;
	}
}

void DXContext::RestoreWinamp()
{
	if (m_winamp_minimized)
	{
		ShowWindow(m_hwnd_winamp, SW_RESTORE);
		m_winamp_minimized = 0;
	}
}

void DXContext::UpdateMonitorWorkRect()
{
	// get active monitor's bounding rectangle (to assist w/window positioning)
	// note: in vert/horz span setups (psuedo-multimon),
	//       this will be 2048x768 or 1024x1536 or something like that.

	// calling this each frame allows you to detect when the taskbar
	// moves around on the screen (from edge to edge), and rearrange
	// the visual elements accordingly, so nothing is obscured.

	HMONITOR hMon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);//m_lpD3D->GetAdapterMonitor(m_ordinal_adapter);
	if (hMon)
	{
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		if (GetMonitorInfo(hMon, &mi))
		{
			m_monitor_work_rect = mi.rcWork;
			m_monitor_work_rect_orig = mi.rcWork;

			// if the monitor rect we're using is the same as the
			// whole area of the monitor, there's no need to update it...
			//if (memcmp(&mi.rcMonitor, &m_monitor_rect, sizeof(RECT))==0)
			//    return;

			// otherwise, we're doing a half-screen special case
			// and are running in some pseudo-multimon res like
			// 2048x768 or 1024x1536, but only using half of it
			// (i.e. fake fullscreen or desktop mode)

			// therefore... we need to update the work-area rectangle
			// to reflect which half of the screen it's on.

			if (m_monitor_rect.left == mi.rcMonitor.left)
				m_monitor_work_rect.left = mi.rcWork.left;
			else
				m_monitor_work_rect.left = m_monitor_rect.left + (mi.rcWork.left - mi.rcMonitor.left);

			if (m_monitor_rect.top == mi.rcMonitor.top)
				m_monitor_work_rect.top = mi.rcWork.top;
			else
				m_monitor_work_rect.top = m_monitor_rect.top + (mi.rcWork.top - mi.rcMonitor.top);

			if (m_monitor_rect.right == mi.rcMonitor.right)
				m_monitor_work_rect.right = mi.rcWork.right;
			else
				m_monitor_work_rect.right = m_monitor_rect.right;

			if (m_monitor_rect.bottom == mi.rcMonitor.bottom)
				m_monitor_work_rect.bottom = mi.rcWork.bottom;
			else
				m_monitor_work_rect.bottom = m_monitor_rect.bottom;
		}
	}
}

void DXContext::SaveWindow()
{
	if (m_current_mode.screenmode == WINDOWED)
	{
		RECT c;
		GetClientRect(m_hwnd, &c);

		// convert client rect from client coords to screen coords:
		// (window rect is already in screen coords...)
		POINT p;
		p.x = c.left;
		p.y = c.top;
		if (ClientToScreen(m_hwnd, &p))
		{
			c.left += p.x;
			c.right += p.x;
			c.top += p.y;
			c.bottom += p.y;
		}

		// save bounds for window CLIENT area, but in screen coords
		WritePrivateProfileIntW(c.top,   L"nMainWndTop",    m_szIniFile, L"settings");
		WritePrivateProfileIntW(c.left,  L"nMainWndLeft",   m_szIniFile, L"settings");
		WritePrivateProfileIntW(c.right, L"nMainWndRight",  m_szIniFile, L"settings");
		WritePrivateProfileIntW(c.bottom,L"nMainWndBottom", m_szIniFile, L"settings");

		// also save bounds for embedwnd
		if (m_current_mode.m_skin && myWindowState.me)
		{
			WritePrivateProfileIntW(myWindowState.r.left,L"avs_wx",m_szIniFile,L"settings");
			WritePrivateProfileIntW(myWindowState.r.top ,L"avs_wy",m_szIniFile,L"settings");
			WritePrivateProfileIntW(myWindowState.r.right-myWindowState.r.left,L"avs_ww",m_szIniFile,L"settings");
			WritePrivateProfileIntW(myWindowState.r.bottom-myWindowState.r.top,L"avs_wh",m_szIniFile,L"settings");
		}
		else if (!m_current_mode.m_skin && m_hwnd)
		{
			RECT r;
			GetWindowRect(m_hwnd, &r);
			WritePrivateProfileIntW(r.left,L"avs_wx",m_szIniFile,L"settings");
			WritePrivateProfileIntW(r.top ,L"avs_wy",m_szIniFile,L"settings");
			WritePrivateProfileIntW(r.right-r.left,L"avs_ww",m_szIniFile,L"settings");
			WritePrivateProfileIntW(r.bottom-r.top,L"avs_wh",m_szIniFile,L"settings");
		}
	}
}