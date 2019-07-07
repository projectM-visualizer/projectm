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
#include "pluginshell.h"
#include "resource.h"
#include "utility.h"
#include "defines.h"
#include "shell_defines.h"
#include "vis.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <multimon.h>
#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>

#define PREFERRED_FORMAT D3DFMT_X8R8G8B8
#define MAX_PROPERTY_PAGES 8
#define MAX_DISPLAY_ADAPTERS 16
#define MAX_MAX_FPS 120
#define MAX_DISPLAY_MODES 1024

extern winampVisModule mod1;

IDirect3D9* g_lpDX;
HMODULE     g_hmod_d3d9;
HMODULE     g_hmod_d3dx9;
D3DADAPTER_IDENTIFIER9 g_disp_adapter_w[MAX_DISPLAY_ADAPTERS];   // NOTE: indices into this list might not equal the ordinal adapter indices!
D3DADAPTER_IDENTIFIER9 g_disp_adapter_fs[MAX_DISPLAY_ADAPTERS];  // NOTE: indices into this list might not equal the ordinal adapter indices!
D3DADAPTER_IDENTIFIER9 g_disp_adapter_dm[MAX_DISPLAY_ADAPTERS];  // NOTE: indices into this list might not equal the ordinal adapter indices!
D3DDISPLAYMODE         g_disp_mode[MAX_DISPLAY_MODES];
HWND        g_config_hwnd;
HWND        g_subwnd;
int         g_num_disp_modes;
int         g_nTab;
int         g_ignore_clicks;
int         g_zero_display_modes_warning_given;
int         g_proppage_id[MAX_PROPERTY_PAGES];

void GetCurrentDisplayMode(D3DDISPLAYMODE *pMode)
{
    if (!pMode)
        return;

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
        return;

    pMode->Width  = dm.dmPelsWidth;
    pMode->Height = dm.dmPelsHeight;
    pMode->Format = (dm.dmBitsPerPel==16) ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
    pMode->RefreshRate = dm.dmDisplayFrequency;
}

bool CPluginShell::InitConfig(HWND hDialogWnd)
{
    // ******* do initialization of global variables HERE *******
    // ******* do initialization of global variables HERE *******
    // ******* do initialization of global variables HERE *******

    g_lpDX = NULL;
    g_hmod_d3d9 = NULL;
	g_hmod_d3dx9 = NULL;
    g_num_disp_modes = 0;
    g_config_hwnd = hDialogWnd;
    g_ignore_clicks = 1;
    g_subwnd = NULL;
    g_nTab   = 0;
    g_zero_display_modes_warning_given = 0;

    g_proppage_id[0] = IDD_PROPPAGE_1;
    g_proppage_id[1] = IDD_PROPPAGE_2;
    g_proppage_id[2] = IDD_PROPPAGE_3;
    g_proppage_id[3] = IDD_PROPPAGE_4;
    g_proppage_id[4] = IDD_PROPPAGE_5;
    g_proppage_id[5] = IDD_PROPPAGE_6;
    g_proppage_id[6] = IDD_PROPPAGE_7;
    g_proppage_id[7] = IDD_PROPPAGE_8;

    // ******* do initialization of global variables HERE *******
    // ******* do initialization of global variables HERE *******
    // ******* do initialization of global variables HERE *******

    return true;
}

void CPluginShell::EndConfig()
{
    SafeRelease(g_lpDX);

    if (g_subwnd)
    {
		DestroyWindow(g_subwnd); 
		g_subwnd = NULL;
    }

    if (g_hmod_d3d9)
    {
        FreeLibrary(g_hmod_d3d9);
        g_hmod_d3d9 = NULL;
    }
		if (g_hmod_d3dx9)
    {
        g_hmod_d3dx9 = NULL;
    }
}

static bool AddButton(int pos, HWND tabctrl, LPWSTR szButtonText) 
{ 
    if (szButtonText && szButtonText[0] && szButtonText[0] != L' ')
    { 
        TCITEMW tie = {0};
        tie.mask = TCIF_TEXT | TCIF_IMAGE;
        tie.iImage = -1;
        tie.pszText = szButtonText;

        if (SendMessageW(tabctrl, TCM_INSERTITEMW, pos, (LPARAM)&tie) == -1)
            return false;
    }
    return true;
}

void CPluginShell::UpdateAdapters(int screenmode)
{
    int i;

    if (!g_lpDX) return;
    
    int nDispAdapters = 0;
    
    HWND ctrl;
    GUID* pGUID = NULL;
    char deviceName[256];
    switch(screenmode)
    {
    case FULLSCREEN: 
        ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_FS);
        pGUID = &m_adapter_guid_fullscreen;
        StringCbCopy(deviceName, sizeof(deviceName), m_adapter_devicename_fullscreen);
        break;
    case WINDOWED: 
        ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_W);
        pGUID = &m_adapter_guid_windowed;
        StringCbCopy(deviceName, sizeof(deviceName), m_adapter_devicename_windowed);
        break;
    /*case FAKE_FULLSCREEN: 
        ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_FFS);
        pGUID = &m_adapter_guid_fake_fullscreen;
        strcpy(deviceName, m_adapter_devicename_fake_fullscreen);
        break;*/
    case DESKTOP: 
        ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_DMS);
        pGUID = &m_adapter_guid_desktop;
        StringCbCopy(deviceName, sizeof(deviceName), m_adapter_devicename_desktop);
        break;
    }

    // clear the combo box
    SendMessage( ctrl, CB_RESETCONTENT, 0, 0);

    // repopulate the combo box with a list of adapters
    {
        char szDesc[1024];
        
        D3DADAPTER_IDENTIFIER9* global_adapter_list;
        switch(screenmode)
        {
        case FULLSCREEN: 
            global_adapter_list = g_disp_adapter_fs; 
            break;
        case WINDOWED: 
            global_adapter_list = g_disp_adapter_w; 
            break;
        /*case FAKE_FULLSCREEN: 
            global_adapter_list = g_disp_adapter_w;     // [sic]
            break;*/
        case DESKTOP: 
            global_adapter_list = g_disp_adapter_dm;
            break;
        }

        int nAdapters = g_lpDX->GetAdapterCount();

        // re-populate it:

        wchar_t szDebugFile[MAX_PATH];
        StringCbCopyW(szDebugFile, sizeof(szDebugFile), m_szConfigIniFile);
        wchar_t* p = wcsrchr(szDebugFile, L'\\');
        if (p)
        {
            lstrcpyW(p+1, ADAPTERSFILE);
            FILE* f = _wfopen(szDebugFile, L"w");
            if (f)
            {
                DWORD winamp_version = SendMessage(GetWinampWindow(),WM_WA_IPC,0,0);
                fprintf(f, "Winamp version = 0x%04X\n", winamp_version);
                fprintf(f, "Plugin long name = \"%s\", version=%d, subversion=%d\n", LONGNAME, INT_VERSION, INT_SUBVERSION);
                fprintf(f, "Enumeration of Display Adapters:\n");
                //fprintf(f, "...this is a temporary debug file created by MilkDrop 2.\n");
                //fprintf(f, "...don't worry - the final release of the plug-in will NOT generate this file.\n");
                for (i=0; i<nAdapters && nDispAdapters<MAX_DISPLAY_ADAPTERS; i++)
                {
                    if (g_lpDX->GetAdapterIdentifier(i, /*D3DENUM_NO_WHQL_LEVEL*/ 0, &global_adapter_list[nDispAdapters]) == D3D_OK)
                    {
                        // Now get the caps, and filter out any graphics cards that can't
                        // do, say, gouraud shading:

                        int adapter_ok = 1;

                        /*
                        D3DCAPS9 caps;
                        if (g_lpDX->GetDeviceCaps(i, D3DDEVTYPE_HAL, &caps)==D3D_OK)
                        {
                            // check the caps here, make sure the device is up to par.  example:
                            if (caps.ShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB)
                              adapter_ok = 0;
                        }
                        */

                        if (f) {
                            fprintf(f, "%d. Driver=%s\n", nDispAdapters+1, global_adapter_list[nDispAdapters].Driver);
                            fprintf(f, "    Description=%s\n",      global_adapter_list[nDispAdapters].Description);
                            fprintf(f, "    DeviceName=%s\n",       global_adapter_list[nDispAdapters].DeviceName);
                            fprintf(f, "    DriverVersion=0x%08X (%d)\n",global_adapter_list[nDispAdapters].DriverVersion);
                            fprintf(f, "    VendorId=%d\n",         global_adapter_list[nDispAdapters].VendorId);
                            fprintf(f, "    DeviceId=%d\n",         global_adapter_list[nDispAdapters].DeviceId);
                            fprintf(f, "    SubSysId=0x%08X\n",         global_adapter_list[nDispAdapters].SubSysId);
                            fprintf(f, "    Revision=%d\n",         global_adapter_list[nDispAdapters].Revision);
                            //fprintf(f, "    DeviceIdentifier=0x%08X\n", global_adapter_list[nDispAdapters].DeviceIdentifier);
                             char szGuidText[512];
                             GuidToText(&global_adapter_list[nDispAdapters].DeviceIdentifier, szGuidText, sizeof(szGuidText));
                            fprintf(f, "    WHQLLevel=%d\n",        global_adapter_list[nDispAdapters].WHQLLevel);
                            fprintf(f, "    GUID=%s\n", szGuidText);                   
                        }

                        if (adapter_ok)
                        {
                            sprintf(szDesc, "%d. %s   [%s]", nDispAdapters+1, global_adapter_list[nDispAdapters].Description, global_adapter_list[nDispAdapters].Driver);
                            SendMessage( ctrl, CB_ADDSTRING, nDispAdapters, (LPARAM)szDesc);
                            nDispAdapters++;
                        }
                    }
                }    
                fclose(f);
            }
        }

        // set selection(s):
        // find i where global_adapter_list[i].DeviceIdentifier is the same as last time,
        // and select it.
        int found = 0;
        for (i=0; i<nDispAdapters; i++)
        {
            if (!found && 
                memcmp(&global_adapter_list[i].DeviceIdentifier, pGUID, sizeof(GUID))==0 &&
                !strcmp(global_adapter_list[i].DeviceName, deviceName)
                )
            {
                SendMessage( ctrl, CB_SETCURSEL, i, 0);
                found = 1;
            }
        }
        if (!found)
            SendMessage( ctrl, CB_SETCURSEL, 0, 0);
    }

    if (screenmode == FULLSCREEN)
        UpdateFSAdapterDispModes();
    else
        UpdateDispModeMultiSampling(screenmode);
}

void CPluginShell::UpdateFSAdapterDispModes()   // (fullscreen only)
{
    wchar_t szfmt[256], str[256];
    int i;
    HWND hwnd_listbox = GetDlgItem( g_subwnd, IDC_DISP_MODE );

    int nVideoModesTotal = 0;
    if (!g_lpDX) return;
    int nAdapterOrdinal = GetCurrentlySelectedAdapter(FULLSCREEN);
    nVideoModesTotal = g_lpDX->GetAdapterModeCount(nAdapterOrdinal, PREFERRED_FORMAT);

    if (nVideoModesTotal <= 0 && !g_zero_display_modes_warning_given)
    {
        g_zero_display_modes_warning_given = 1;
		wchar_t title[64];
        MessageBoxW(g_config_hwnd, WASABI_API_LNGSTRINGW(IDS_GRAPHICS_SUBSYSTEM_IS_TEMPORARILY_UNSTABLE),
				   WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_WARNING, title, 64),
				   MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
    }

    // clear the combo box
    SendMessage( hwnd_listbox, CB_RESETCONTENT, 0, 0);

    int nAdded = 0;

    // re-populate it:
    for (i=0; i<nVideoModesTotal; i++)
    {
        if (nAdded >= MAX_DISPLAY_MODES)
            break;

        // get info for display mode into g_disp_mode[nAdded]
        if (g_lpDX->EnumAdapterModes(nAdapterOrdinal, PREFERRED_FORMAT, i, &g_disp_mode[nAdded]) != D3D_OK)
            continue;

        // add to combo box
        int bpp = 0;
        switch(g_disp_mode[nAdded].Format)
        {
        default:
        case D3DFMT_UNKNOWN      : WASABI_API_LNGSTRINGW_BUF(IDS_UNKNOWN, szfmt, 256);   bpp=0;  break;
        case D3DFMT_R8G8B8       : lstrcpynW(szfmt, L"RGB-888", 256);  bpp=32; break;
        case D3DFMT_A8R8G8B8     : lstrcpynW(szfmt, L"ARGB-8888", 256); bpp=32; break;
        case D3DFMT_X8R8G8B8     : lstrcpynW(szfmt, L"XRGB-8888", 256); bpp=32; break;
        case D3DFMT_R5G6B5       : lstrcpynW(szfmt, L"RGB-565", 256);   bpp=16; break;
        case D3DFMT_X1R5G5B5     : lstrcpynW(szfmt, L"XRGB-1555", 256);  bpp=16; break;
        case D3DFMT_A1R5G5B5     : lstrcpynW(szfmt, L"ARGB-1555", 256);  bpp=16; break;
        case D3DFMT_A4R4G4B4     : lstrcpynW(szfmt, L"ARGB-4444", 256);  bpp=16; break;
        case D3DFMT_X4R4G4B4     : lstrcpynW(szfmt, L"XRGB-4444", 256);  bpp=16; break;
        }
        swprintf(str, L" %s,  %4d x %4d,  %3d %s ",
				 szfmt, g_disp_mode[nAdded].Width,
				 g_disp_mode[nAdded].Height,
				 g_disp_mode[nAdded].RefreshRate,
				 WASABI_API_LNGSTRINGW(IDS_HZ));

        /*
        #ifdef DONT_ALLOW_8BIT_FULLSCREEN
            if (bpp==8) continue;
        #endif
        #ifdef DONT_ALLOW_16_24_32_BIT_FULLSCREEN
            if (bpp==16 || bpp==24 || bpp==32) continue;
        #endif
        */

        int nPos = SendMessageW( hwnd_listbox, CB_ADDSTRING, 0, (LPARAM)str);

        // keep a record of the original index, because the combo box SORTS the data:
        SendMessage( hwnd_listbox, CB_SETITEMDATA, nPos, i);

        nAdded++;
    }

    g_num_disp_modes = nAdded;

    // now set selection, based on best match to prev. video mode.  
    int found = 0;
    
    // Fallback order:
    // 0. exact match (4 params)   w,h,r,f
    // 1. ignore refresh rate, but still heed color format
    // 2. heed only w,h, and if desired_mode.Format is UNKNOWN, 
    //        try for a 16-bpp color format and 60 Hz
    // 3. heed only w,h, and if desired_mode.Format is UNKNOWN, 
    //        try for a 16-bpp color format at any Hz
    // 4. heed only w,h.

    D3DDISPLAYMODE desired_mode = m_disp_mode_fs;

    assert(desired_mode.Format != D3DFMT_UNKNOWN);
    // this should never happen anymore, now that we set smarter default FS display mode
    // (to match the current display mode) in PluginShell.cpp's PluginPreInitialize().
    /*if (desired_mode.Format==D3DFMT_UNKNOWN)
    {
        GetCurrentDisplayMode(&desired_mode);

        // first-time config: try to find a video mode that matches our ideal.
		// do many passes until we find one, each time relaxing more constraints.
		// outline of the passes:

		// PASS  MATCH:
        // 0. w,h,r,16bpp
        // 1. w,h,-,16bpp
        // 2. w,h,r,-
        // 3. w,h,-,-
        // 4. -,-,-,-
        for (int rep=0; rep<5; rep++)
        {
            for (i=0; i<g_num_disp_modes && !found; i++)
            {
                // we have to remap here, because the combo box SORTED the data:
                int id = SendMessage( hwnd_listbox, CB_GETITEMDATA, i, 0);

                bool bDesiredBitDepth = (
                                (g_disp_mode[id].Format == D3DFMT_R8G8B8  ) ||
                                (g_disp_mode[id].Format == D3DFMT_A8R8G8B8) ||     
                                (g_disp_mode[id].Format == D3DFMT_X8R8G8B8) 
                                        ); 

                bool bMatch = true;

                if (rep<=3 && desired_mode.Width!=g_disp_mode[id].Width) 
                    bMatch = false;
                if (rep<=3 && desired_mode.Height!=g_disp_mode[id].Height) 
                    bMatch = false;
                if ((rep==0 || rep==2) && desired_mode.RefreshRate!=g_disp_mode[id].RefreshRate) 
                    bMatch = false;
                if (rep<=1 && !bDesiredBitDepth)
                    bMatch = false;

                if (bMatch)
                {
                    SendMessage( hwnd_listbox, CB_SETCURSEL, i, 0);
                    found = 1;
                }
            }
        }

        // if no match could be found, select #0.
        if (!found)
            SendMessage( hwnd_listbox, CB_SETCURSEL, 0, 0);
    }
    else
    */
    {
        // find best match to prev. selection.
		// do many passes until we find one, each time relaxing more constraints.
		// outline of the passes:

        int bpp_desired = 0;
        switch(desired_mode.Format)
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
        for (int rep=0; rep<12 && !found; rep++)
        {
            for (int pass=0; pass<2 && !found; pass++)
            {
                for (i=0; i<g_num_disp_modes && !found; i++)
                {
                    // we have to remap here, because the combo box SORTED the data:
                    int id = SendMessage( hwnd_listbox, CB_GETITEMDATA, i, 0);

                    int bpp_this_mode = 0;
                    switch(g_disp_mode[id].Format)
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

                    bool bMatch = true;

                    if (rep < 4)
                    {
                        if (desired_mode.Width != g_disp_mode[id].Width) 
                            bMatch = false;
                        if (desired_mode.Height != g_disp_mode[id].Height) 
                            bMatch = false;
                    }
                    else if (rep < 8)
                    {
                        if (DEFAULT_FULLSCREEN_WIDTH != g_disp_mode[id].Width) 
                            bMatch = false;
                        if (DEFAULT_FULLSCREEN_HEIGHT != g_disp_mode[id].Height) 
                            bMatch = false;
                    }
        
                    if (((rep/2)%2)==0)
                    {
                        if (pass==0 && desired_mode.Format != g_disp_mode[id].Format)
                            bMatch = false;
                        else if (pass==1 && bpp_desired != bpp_this_mode)
                            bMatch = false;
                    }

                    if (((rep%2)==0) && desired_mode.RefreshRate!=g_disp_mode[id].RefreshRate) 
                    {
                        bMatch = false;
                    }

                    if (bMatch)
                    {
                        SendMessage( hwnd_listbox, CB_SETCURSEL, i, 0);
                        found = 1;
                    }
                }
            }
        }

        // if no match could be found, select #0.
        if (!found)
            SendMessage( hwnd_listbox, CB_SETCURSEL, 0, 0);
    }

    UpdateDispModeMultiSampling(0);
}

void CPluginShell::UpdateDispModeMultiSampling(int screenmode)
{
    int nSampleTypes = 0;

    HWND hwnd_listbox;
    switch(screenmode)
    {
    case FULLSCREEN:      hwnd_listbox = GetDlgItem(g_subwnd, IDC_FSMS);  break;
    case WINDOWED:        hwnd_listbox = GetDlgItem(g_subwnd, IDC_WMS);   break;
    case DESKTOP:         hwnd_listbox = GetDlgItem(g_subwnd, IDC_DMSMS);  break;
    }

    wchar_t str[256];
    int i;

    if (!g_lpDX)
        return;

    if ((screenmode == WINDOWED && !m_allow_page_tearing_w) ||
        (screenmode == FULLSCREEN && m_fake_fullscreen_mode && !m_allow_page_tearing_fs) ||
        (screenmode == DESKTOP && !m_allow_page_tearing_dm))
    {
        // page tearing not allowed -> disable multisampling!
        SendMessage( hwnd_listbox, CB_RESETCONTENT, 0, 0);
        SendMessageW( hwnd_listbox, CB_ADDSTRING, 0, (LPARAM)WASABI_API_LNGSTRINGW(IDS_DISABLED_PAGE_TEARING));
        SendMessage( hwnd_listbox, CB_SETITEMDATA, 0, 0 );
        SendMessage( hwnd_listbox, CB_SETCURSEL, 0, 0);
        EnableWindow( hwnd_listbox, 0 );
    }
    else
    {
        EnableWindow( hwnd_listbox, 1 );

        // figure out which [fullscreen/windowed] adapter is currently selected:
        int nAdapterOrdinal = GetCurrentlySelectedAdapter(screenmode);

        // figure out current format:
        D3DFORMAT format = D3DFMT_UNKNOWN;
        if ((screenmode == WINDOWED) || 
            (screenmode == FULLSCREEN && m_fake_fullscreen_mode) || 
            (screenmode == DESKTOP))
        {
            // ** get it from the current display mode 
            //    of the currently-selected [windowed/fake fullscreen] mode adapter **
            D3DDISPLAYMODE dispmode;
            if (g_lpDX->GetAdapterDisplayMode(nAdapterOrdinal, &dispmode) == D3D_OK)
                format = dispmode.Format;
        }
        else
        {
            // **get it from the currently-selected fullscreen display mode**
            int n = SendMessage( GetDlgItem( g_subwnd, IDC_DISP_MODE ), CB_GETCURSEL, 0, 0);
            if (n != CB_ERR) 
            {
                // since the combobox contents were sorted, we need to look up the original
                // index into g_disp_mode[]:
                n = SendMessage( GetDlgItem( g_subwnd, IDC_DISP_MODE ), CB_GETITEMDATA, n, 0);
                if (n != CB_ERR)
                    format = g_disp_mode[n].Format;
            }
        }

        D3DMULTISAMPLE_TYPE check[16] = {
            D3DMULTISAMPLE_NONE,
            D3DMULTISAMPLE_2_SAMPLES ,
            D3DMULTISAMPLE_3_SAMPLES ,
            D3DMULTISAMPLE_4_SAMPLES ,
            D3DMULTISAMPLE_5_SAMPLES ,
            D3DMULTISAMPLE_6_SAMPLES ,
            D3DMULTISAMPLE_7_SAMPLES ,
            D3DMULTISAMPLE_8_SAMPLES ,
            D3DMULTISAMPLE_9_SAMPLES ,
            D3DMULTISAMPLE_10_SAMPLES,
            D3DMULTISAMPLE_11_SAMPLES,
            D3DMULTISAMPLE_12_SAMPLES,
            D3DMULTISAMPLE_13_SAMPLES,
            D3DMULTISAMPLE_14_SAMPLES,
            D3DMULTISAMPLE_15_SAMPLES,
            D3DMULTISAMPLE_16_SAMPLES,
        };

        // clear the combo box
        SendMessage( hwnd_listbox, CB_RESETCONTENT, 0, 0);

        // re-populate it:
        for (i=0; i<16; i++)
        {
            if (i==0 || SUCCEEDED(g_lpDX->CheckDeviceMultiSampleType(nAdapterOrdinal, D3DDEVTYPE_HAL,
                                                           format, 
                                                           (screenmode==FULLSCREEN) ? 0 : 1,//bWindowed,
                                                           check[i], NULL)))
            {
                // add to listbox
                if (i==0)
                    WASABI_API_LNGSTRINGW_BUF(IDS_NONE, str, 256);
                else
                    StringCbPrintfW(str, sizeof(str), L"%2dX", i+1);

                SendMessage( hwnd_listbox, CB_ADDSTRING, nSampleTypes, (LPARAM)str);

                // set the item data to the D3DMULTISAMPLE_TYPE value:
                SendMessage( hwnd_listbox, CB_SETITEMDATA, nSampleTypes, check[i] );

                nSampleTypes++;
            }
        }

        // set prev. selection
        D3DMULTISAMPLE_TYPE prev_seln;
        switch(screenmode)
        {
        case FULLSCREEN:      prev_seln = m_multisample_fullscreen;      break;
        case WINDOWED:        prev_seln = m_multisample_windowed;        break;
        //case FAKE_FULLSCREEN: prev_seln = m_multisample_fake_fullscreen; break;
        case DESKTOP:         prev_seln = m_multisample_desktop;         break;
        }

        for (i=0; i<nSampleTypes; i++)
        {
            int id = SendMessage( hwnd_listbox, CB_GETITEMDATA, i, 0);
            if (id==prev_seln)
            {
                SendMessage( hwnd_listbox, CB_SETCURSEL, i, 0);
                return;
            }
        }

        SendMessage( hwnd_listbox, CB_SETCURSEL, 0, 0);
    }
}

void CPluginShell::UpdateMaxFps(int screenmode)
{
    // initialize sleep combo boxes
    HWND ctrl;
    switch(screenmode)
    {
    case FULLSCREEN:      ctrl = GetDlgItem(g_subwnd, IDC_FS_MAXFPS ); break;
    case WINDOWED:        ctrl = GetDlgItem(g_subwnd, IDC_W_MAXFPS  ); break;
    //case FAKE_FULLSCREEN: ctrl = GetDlgItem(g_subwnd, IDC_FFS_MAXFPS); break;
    case DESKTOP:         ctrl = GetDlgItem(g_subwnd, IDC_DMS_MAXFPS); break;
    }

    SendMessage( ctrl, CB_RESETCONTENT, 0, 0);
    for (int j=0; j<=MAX_MAX_FPS; j++)
    {
        wchar_t buf[256];
        if (j==0)
			WASABI_API_LNGSTRINGW_BUF(IDS_UNLIMITED, buf, 256);
        else 
			swprintf(buf, WASABI_API_LNGSTRINGW(IDS_X_FRAME_SEC), (j<MAX_MAX_FPS)?(MAX_MAX_FPS+1-j):(MAX_MAX_FPS+1-j));

        SendMessageW( ctrl, CB_ADDSTRING, j, (LPARAM)buf);
    }

    // set prev. selection
    int max_fps;
    switch(screenmode)
    {
    case FULLSCREEN:      max_fps = m_max_fps_fs; break;
    case WINDOWED:        max_fps = m_max_fps_w;  break;
    //case FAKE_FULLSCREEN: max_fps = m_max_fps_fake_fs; break;
    case DESKTOP:         max_fps = m_max_fps_dm; break;
    }
    if (max_fps == 0)
        SendMessage(ctrl, CB_SETCURSEL, 0,  0);
    else
        SendMessage(ctrl, CB_SETCURSEL, MAX_MAX_FPS-max_fps+1,  0);
}

int CPluginShell::GetCurrentlySelectedAdapter(int screenmode)
{
    // returns the ordinal adapter #.
    HWND ctrl;
    switch(screenmode)
    {
    case FULLSCREEN:      ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_FS ); break;
    case WINDOWED:        ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_W  ); break;
    //case FAKE_FULLSCREEN: ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_FFS); break;
    case DESKTOP:         ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_DMS); break;
    }

    int n = SendMessage(ctrl, CB_GETCURSEL, 0, 0);
    //if (n != CB_ERR) 
    //    n = SendMessage(ctrl, CB_GETITEMDATA, n, 0);
    if (n != CB_ERR) 
        return n;
    else
        return D3DADAPTER_DEFAULT;
}

void CPluginShell::SaveDisplayMode()
{
    //if (m_fake_fullscreen_mode)
    //    return;

    // read fullscreen display mode
    int n = SendMessage( GetDlgItem( g_subwnd, IDC_DISP_MODE ), CB_GETCURSEL, 0, 0);
    if (n != CB_ERR) 
    {
        // since the combobox contents were sorted, we need to look up the original
        // index into g_disp_mode[]:
        n = SendMessage( GetDlgItem( g_subwnd, IDC_DISP_MODE ), CB_GETITEMDATA, n, 0);
        if (n != CB_ERR)
            m_disp_mode_fs = g_disp_mode[n];
    }
}

void CPluginShell::SaveMultiSamp(int screenmode)
{
    HWND ctrl;
    switch(screenmode)
    {
    case FULLSCREEN:      ctrl = GetDlgItem(g_subwnd, IDC_FSMS);  break;
    case WINDOWED:        ctrl = GetDlgItem(g_subwnd, IDC_WMS);   break;
    //case FAKE_FULLSCREEN: ctrl = GetDlgItem(g_subwnd, IDC_FFSMS); break;
    case DESKTOP:         ctrl = GetDlgItem(g_subwnd, IDC_DMSMS); break;
    }

    // if page tearing is disabled, then multisampling must be disabled, 
    // so ignore multisample selection
    if (g_subwnd && g_nTab==0)
    {
        if (screenmode == WINDOWED && !m_allow_page_tearing_w)
            return;
        if (screenmode == DESKTOP && !m_allow_page_tearing_dm)
            return;
        if (screenmode == FULLSCREEN && m_fake_fullscreen_mode && !m_allow_page_tearing_fs)
            return;
    }

    // read windowed & fullscreen multisampling settings:
    int n = SendMessage(ctrl , CB_GETCURSEL, 0, 0);
    if (n != CB_ERR) 
    {
        n = SendMessage( ctrl, CB_GETITEMDATA, n, 0);
        if (n != CB_ERR)
        {
            switch(screenmode)
            {
            case FULLSCREEN:      m_multisample_fullscreen      = (D3DMULTISAMPLE_TYPE)n; break;
            case WINDOWED:        m_multisample_windowed        = (D3DMULTISAMPLE_TYPE)n; break;
            //case FAKE_FULLSCREEN: m_multisample_fake_fullscreen = (D3DMULTISAMPLE_TYPE)n; break;
            case DESKTOP:         m_multisample_desktop         = (D3DMULTISAMPLE_TYPE)n; break;
            }
        }
    }
}

void CPluginShell::SaveMaxFps(int screenmode)
{
    HWND ctrl;
    switch(screenmode)
    {
    case FULLSCREEN:      ctrl = GetDlgItem(g_subwnd, IDC_FS_MAXFPS);  break;
    case WINDOWED:        ctrl = GetDlgItem(g_subwnd, IDC_W_MAXFPS);   break;
    //case FAKE_FULLSCREEN: ctrl = GetDlgItem(g_subwnd, IDC_FFS_MAXFPS); break;
    case DESKTOP:         ctrl = GetDlgItem(g_subwnd, IDC_DMS_MAXFPS); break;
    }

    // read max fps settings
    int n = SendMessage(ctrl, CB_GETCURSEL, 0, 0);
    if (n != CB_ERR) 
    {
        if (n > 0)
            n = MAX_MAX_FPS+1 - n;

        switch(screenmode)
        {
        case FULLSCREEN:      m_max_fps_fs = n; break;
        case WINDOWED:        m_max_fps_w  = n; break;
        //case FAKE_FULLSCREEN: m_max_fps_fake_fs = n; break;
        case DESKTOP:         m_max_fps_dm = n; break;
        }
    }
}

void CPluginShell::SaveAdapter(int screenmode)
{
    HWND ctrl;
    switch(screenmode)
    {
    case FULLSCREEN:      ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_FS);  break;
    case WINDOWED:        ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_W);   break;
    //case FAKE_FULLSCREEN: ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_FFS); break;
    case DESKTOP:         ctrl = GetDlgItem(g_subwnd, IDC_ADAPTER_DMS); break;
    }

    // save windowed/fullscreen adapter
    int n = SendMessage( ctrl, CB_GETCURSEL, 0, 0);
    if (n != CB_ERR) 
    {
        switch(screenmode)
        {
        case FULLSCREEN:      
            m_adapter_guid_fullscreen      = g_disp_adapter_fs[n].DeviceIdentifier; 
            StringCbCopy(m_adapter_devicename_fullscreen, sizeof(m_adapter_devicename_fullscreen), g_disp_adapter_fs[n].DeviceName);
            //strcpy(m_adapter_desc_fullscreen, g_disp_adapter_fs[n].Description);
            break;
        case WINDOWED:        
            m_adapter_guid_windowed        = g_disp_adapter_w[n].DeviceIdentifier;  
            StringCbCopy(m_adapter_devicename_windowed, sizeof(m_adapter_devicename_windowed), g_disp_adapter_fs[n].DeviceName);
            //strcpy(m_adapter_desc_windowed, g_disp_adapter_fs[n].Description);
            break;
        //case FAKE_FULLSCREEN: 
            //m_adapter_guid_fake_fullscreen = g_disp_adapter_w[n].DeviceIdentifier;  
            //strcpy(m_adapter_desc_fake_fullscreen, g_disp_adapter_fs[n].Description);
            //break; // [sic]
        case DESKTOP:         
            m_adapter_guid_desktop         = g_disp_adapter_dm[n].DeviceIdentifier;  
            StringCbCopy(m_adapter_devicename_desktop, sizeof(m_adapter_devicename_desktop), g_disp_adapter_fs[n].DeviceName);
            //strcpy(m_adapter_desc_desktop, g_disp_adapter_fs[n].Description);
            break; 
        }
    }
}

/*
BOOL CALLBACK GenericTabCtrlProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    return 0;
}
*/

// OnTabChanged - processes the TCN_SELCHANGE notification. 
void CPluginShell::OnTabChanged(int nNewTab) 
{ 
    if (g_subwnd)
    {
        DestroyWindow(g_subwnd);
        g_subwnd = NULL;
    }

    g_nTab = nNewTab;

    if (g_nTab >= 0 && g_nTab < MAX_PROPERTY_PAGES)
    {
		HWND h = WASABI_API_CREATEDIALOGPARAMW(g_proppage_id[g_nTab], g_config_hwnd, this->TabCtrlProc, (LPARAM)this);
		// NOTE: CreateDialogParam will call TabCtrlProc with WM_INITDIALOG,
        // which is where 'g_subwnd' will get set.

        // do this here to ensure that the current prefs page is correctly themed
        if(!SendMessage(this->m_hWndWinamp,WM_WA_IPC,IPC_ISWINTHEMEPRESENT,IPC_USE_UXTHEME_FUNC))
        {
            SendMessage(this->m_hWndWinamp,WM_WA_IPC,(WPARAM)h,IPC_USE_UXTHEME_FUNC);
        }
    }
}

INT_PTR CALLBACK CPluginShell::TabCtrlProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    if (msg==WM_INITDIALOG && lParam > 0 && GetWindowLongPtr(hwnd,GWLP_USERDATA)==0)
        SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

    CPluginShell* p = (CPluginShell*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    if (p && g_nTab >= 0 && g_nTab < MAX_PROPERTY_PAGES)
    {
        if (msg==WM_INITDIALOG)
            g_subwnd = hwnd;

        if (g_nTab==0)
            p->PluginShellConfigTab1Proc(hwnd, msg, wParam, lParam);
        p->MyConfigTabProc(g_nTab+1, hwnd, msg, wParam, lParam);
        
        if (msg==WM_INITDIALOG)
        {
            // once it has been initialized, reposition the subdialog:
            RECT r;
		    GetWindowRect(GetDlgItem(g_config_hwnd,IDC_RECT),&r);
		    ScreenToClient(g_config_hwnd,(LPPOINT)&r);
		    SetWindowPos(g_subwnd,0,r.left,r.top,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	        ShowWindow(g_subwnd,SW_SHOWNA);
        }
    }

	const int controls[] = 
	{
		IDC_BRIGHT_SLIDER,
		IDC_HARDCUT_LOUDNESS,
	};
	if (FALSE != WASABI_API_APP->DirectMouseWheel_ProcessDialogMessage(hwnd, msg, wParam, lParam, controls, ARRAYSIZE(controls)))
	{
		return TRUE;
	}

    return FALSE;
}

BOOL CPluginShell::PluginShellConfigTab1Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    #ifdef _DEBUG
        OutputDebugMessage("   Tab1Proc: ", hwnd, msg, wParam, lParam);
    #endif

    switch (msg)
    {
    case WM_INITDIALOG:
        {
            // pre-checks
            if (m_start_fullscreen && m_start_desktop)
                m_start_desktop = 0;
            if (!mod1.hwndParent || SendMessage(mod1.hwndParent,WM_WA_IPC,0,0) < 0x2900)
            {
                m_skin = 0;
                EnableWindow(GetDlgItem(hwnd,IDC_CB_SKIN), 0);
                char buf[256];
                buf[0] = 0;
                GetWindowText(GetDlgItem(hwnd,IDC_CB_SKIN), buf, 255);
                StringCbCat(buf, sizeof(buf), " 2.90+");
                SetWindowText(GetDlgItem(hwnd,IDC_CB_SKIN), buf);
            }

            // set checkboxes
            CheckDlgButton(hwnd, IDC_CB_FS,  m_start_fullscreen);
            CheckDlgButton(hwnd, IDC_CB_DMS,  m_start_desktop);
            CheckDlgButton(hwnd, IDC_CB_FAKE, m_fake_fullscreen_mode);
            CheckDlgButton(hwnd, IDC_CB_PRESS_F1_MSG, m_show_press_f1_msg);
            CheckDlgButton(hwnd, IDC_CB_WPT, m_allow_page_tearing_w);
            CheckDlgButton(hwnd, IDC_CB_FSPT, m_allow_page_tearing_fs);
            CheckDlgButton(hwnd, IDC_CB_DMSPT, m_allow_page_tearing_dm);
            CheckDlgButton(hwnd, IDC_CB_MIN, m_minimize_winamp);
            CheckDlgButton(hwnd, IDC_CB_SAVE_CPU, m_save_cpu);
            CheckDlgButton(hwnd, IDC_CB_SKIN, m_skin);
			CheckDlgButton(hwnd, IDC_CB_FIXSLOWTEXT, m_fix_slow_text);
            CheckDlgButton(hwnd, IDC_CB_VJMODE, m_vj_mode);

            // Enumerate available adapters.
            UpdateAdapters(0);    // fullscreen
              //-calls UpdateFSAdapterDispModes()
              //-which then calls UpdateDispModeMultiSampling(0).
            UpdateAdapters(1);    // windowed
              //-skips UpdateFSAdapterDispModes() (not necessary for windowed mode)
              //-then calls UpdateDispModeMultiSampling(1).
            UpdateAdapters(3);    // desktop
              //-skips UpdateFSAdapterDispModes() (not necessary for fake fullscreen mode)
              //-then calls UpdateDispModeMultiSampling(2).
            UpdateMaxFps(0);
            UpdateMaxFps(1);
            UpdateMaxFps(3); // desktop

            // disable a few things if fake fullscreen mode enabled:
            EnableWindow(GetDlgItem(hwnd, IDC_DISP_MODE), !m_fake_fullscreen_mode);
            //EnableWindow(GetDlgItem(hwnd, IDC_FSMS), !m_fake_fullscreen_mode);
        }        
        break;

    case WM_DESTROY:
        {
            // read checkboxes
            m_start_fullscreen     = DlgItemIsChecked(hwnd, IDC_CB_FS   );
            m_start_desktop        = DlgItemIsChecked(hwnd, IDC_CB_DMS  );
            m_fake_fullscreen_mode = DlgItemIsChecked(hwnd, IDC_CB_FAKE );
            m_show_press_f1_msg    = DlgItemIsChecked(hwnd, IDC_CB_PRESS_F1_MSG);
            m_allow_page_tearing_w = DlgItemIsChecked(hwnd, IDC_CB_WPT  );
            m_allow_page_tearing_fs= DlgItemIsChecked(hwnd, IDC_CB_FSPT  );
            m_allow_page_tearing_dm= DlgItemIsChecked(hwnd, IDC_CB_DMSPT);
            m_minimize_winamp      = DlgItemIsChecked(hwnd, IDC_CB_MIN  );
            m_save_cpu             = DlgItemIsChecked(hwnd, IDC_CB_SAVE_CPU );
            m_fix_slow_text        = DlgItemIsChecked(hwnd, IDC_CB_FIXSLOWTEXT);
            m_vj_mode              = DlgItemIsChecked(hwnd, IDC_CB_VJMODE);
            
            if (mod1.hwndParent && SendMessage(mod1.hwndParent,WM_WA_IPC,0,0) >= 0x2900)
                m_skin             = DlgItemIsChecked(hwnd, IDC_CB_SKIN );

            // read all 3 adapters
            SaveAdapter(0);
            SaveAdapter(1);
            SaveAdapter(3);

            // read fullscreen display mode
            SaveDisplayMode();

            // read all 3 multisampling settings:
            SaveMultiSamp(0);
            SaveMultiSamp(1);
            SaveMultiSamp(3);

            // read all 3 max fps settings
            SaveMaxFps(0);
            SaveMaxFps(1);
            SaveMaxFps(3);
        }
        break;

    case WM_COMMAND:
        if (!g_ignore_clicks)
        {
            int id = LOWORD(wParam);
            g_ignore_clicks = 1;
            switch(id)
            {
			case ID_FONTS:
                WASABI_API_DIALOGBOXPARAMW(IDD_FONTDIALOG, hwnd, FontDialogProc, (LPARAM)this);
                break;

			case ID_DM_MORE:
                WASABI_API_DIALOGBOXPARAMW(IDD_DESKTOPMODE, hwnd, DesktopOptionsDialogProc, (LPARAM)this);
                break;

			case ID_DUALHEAD:
                WASABI_API_DIALOGBOXPARAMW(IDD_DUALHEAD, hwnd, DualheadDialogProc, (LPARAM)this);
                break;

            case IDC_ADAPTER_FS:
                SaveDisplayMode();
                SaveMultiSamp(FULLSCREEN);
                SaveAdapter(0);
                UpdateFSAdapterDispModes();
                break;

            case IDC_ADAPTER_W:
                SaveMultiSamp(WINDOWED);
                UpdateDispModeMultiSampling(WINDOWED);
                break;

                /*
            case IDC_ADAPTER_FFS:
                SaveMultiSamp(FAKE_FULLSCREEN);
                UpdateDispModeMultiSampling(FAKE_FULLSCREEN);
                break;
                */

            case IDC_ADAPTER_DMS:
                SaveMultiSamp(DESKTOP);
                UpdateDispModeMultiSampling(DESKTOP);
                break;
                
            case IDC_DISP_MODE:
                SaveMultiSamp(FULLSCREEN);
                UpdateDispModeMultiSampling(FULLSCREEN);
                break;

            case IDC_CB_WPT:
                SaveMultiSamp(WINDOWED);
                m_allow_page_tearing_w = DlgItemIsChecked(hwnd, IDC_CB_WPT);
                UpdateDispModeMultiSampling(WINDOWED);
                break;

            case IDC_CB_FSPT:
                SaveMultiSamp(FULLSCREEN);
                m_allow_page_tearing_fs = DlgItemIsChecked(hwnd, IDC_CB_FSPT);
                UpdateDispModeMultiSampling(FULLSCREEN);
                break;

            case IDC_CB_DMSPT:
                SaveMultiSamp(DESKTOP);
                m_allow_page_tearing_dm = DlgItemIsChecked(hwnd, IDC_CB_DMSPT);
                UpdateDispModeMultiSampling(DESKTOP);
                break;

            case IDC_CB_FS:
                m_start_fullscreen     = DlgItemIsChecked(hwnd, IDC_CB_FS   );
                if (m_start_fullscreen && m_start_desktop)
                {
                    m_start_desktop = 0;
                    CheckDlgButton(hwnd, IDC_CB_DMS,  m_start_desktop);
                }
                break;
            
            case IDC_CB_DMS:
                m_start_desktop        = DlgItemIsChecked(hwnd, IDC_CB_DMS  );
                if (m_start_fullscreen && m_start_desktop)
                {
                    m_start_fullscreen = 0;
                    CheckDlgButton(hwnd, IDC_CB_FS,  m_start_fullscreen);
                }
                break;

            case IDC_CB_FAKE:
                SaveMultiSamp(FULLSCREEN);
                m_fake_fullscreen_mode = DlgItemIsChecked(hwnd, IDC_CB_FAKE );
                EnableWindow(GetDlgItem(hwnd, IDC_DISP_MODE), !m_fake_fullscreen_mode);
                CheckDlgButton(hwnd, IDC_CB_FSPT, m_fake_fullscreen_mode ? m_allow_page_tearing_fs : 0);
                EnableWindow(GetDlgItem(hwnd, IDC_CB_FSPT), m_fake_fullscreen_mode ? 1 : 0);
                UpdateDispModeMultiSampling(FULLSCREEN);
                break;

                /*
            case IDC_CB_FFSPT:
                SaveMultiSamp(FAKE_FULLSCREEN);
                m_allow_page_tearing_fake_fs = DlgItemIsChecked(hwnd, IDC_CB_FFSPT);
                UpdateDispModeMultiSampling(FAKE_FULLSCREEN);
                break;
                */
            }
            g_ignore_clicks = 0;
        }
        break;  // case WM_COMMAND

    case WM_HELP:
        if (lParam)
        {
            HELPINFO *ph = (HELPINFO*)lParam;
            wchar_t title[1024];
            wchar_t buf[2048];
            wchar_t ctrl_name[1024];
            GetWindowTextW(GetDlgItem(hwnd, ph->iCtrlId), ctrl_name, sizeof(ctrl_name)/sizeof(*ctrl_name));
            RemoveSingleAmpersands(ctrl_name);
            buf[0] = 0;
            
            switch(ph->iCtrlId)
            {
            case ID_FONTS:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
                WASABI_API_LNGSTRINGW_BUF(IDS_FONTS_HELP, buf, 2048);
                break;

            case ID_DUALHEAD:
				StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
                WASABI_API_LNGSTRINGW_BUF(IDS_DUAL_HEAD_HELP, buf, 2048);
                break;

            case IDC_W_MULTISAMPLING_CAPTION:
            case IDC_FS_MULTISAMPLING_CAPTION:
            case IDC_DMS_MULTISAMPLING_CAPTION:
            //case IDC_FFS_MULTISAMPLING_CAPTION:
            case IDC_WMS:
            case IDC_FSMS:
            case IDC_DMSMS:
            //case IDC_FFSMS:
                WASABI_API_LNGSTRINGW_BUF(IDS_MULTI_SAMPLING, title, 1024);
                WASABI_API_LNGSTRINGW_BUF(IDS_MULTI_SAMPLING_HELP, buf, 2048);
                break;

            case IDC_W_MAXFPS:
            case IDC_FS_MAXFPS:
            case IDC_DMS_MAXFPS:
            //case IDC_FFS_MAXFPS:
            case IDC_W_MAXFPS_CAPTION:
            case IDC_FS_MAXFPS_CAPTION:
            case IDC_DMS_MAXFPS_CAPTION:
            //case IDC_FFS_MAXFPS_CAPTION:
                WASABI_API_LNGSTRINGW_BUF(IDS_MAX_FRAMERATE, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_MAX_FRAMERATE_HELP, buf, 2048);
                break;

            case IDC_CB_FAKE:
                WASABI_API_LNGSTRINGW_BUF(IDS_FAKE_FULLSCREEN, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_FAKE_FULLSCREEN_HELP, buf, 2048);
                break;

            case IDC_ADAPTER_FS:
            case IDC_FS_ADAPTER_CAPTION:
                WASABI_API_LNGSTRINGW_BUF(IDS_FULLSCREEN_ADAPTER, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_FULLSCREEN_ADAPTER_HELP, buf, 2048);
                break;

            case IDC_ADAPTER_W:
            case IDC_W_ADAPTER_CAPTION:
				WASABI_API_LNGSTRINGW_BUF(IDS_WINDOWED_ADPATER, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_WINDOWED_ADPATER_HELP, buf, 2048);
                break;

            case IDC_ADAPTER_DMS:
            case IDC_DMS_ADAPTER_CAPTION:
				WASABI_API_LNGSTRINGW_BUF(IDS_DESKTOP_ADAPTER, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_DESKTOP_ADAPTER_HELP, buf, 2048);
                break;

            case IDC_DISP_MODE:
            case IDC_DISP_MODE_CAPTION:
                WASABI_API_LNGSTRINGW_BUF(IDS_FS_DISPLAY_MODE, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_FS_DISPLAY_MODE_HELP, buf, 2048);
                break;

            case IDC_CB_WPT:
            case IDC_CB_FSPT:
            case IDC_CB_DMSPT:
            //case IDC_CB_FFSPT:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_CHECKBOX), ctrl_name);
				WASABI_API_LNGSTRINGW_BUF(IDS_HELP_ON_X_CHECKBOX_HELP, buf, 2048);
                break;

            case IDC_CB_FS:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_CHECKBOX), ctrl_name);
                WASABI_API_LNGSTRINGW_BUF(IDS_FORCE_INTO_FS_MODE_HELP, buf, 2048);
                break;

            case IDC_CB_DMS:
				StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_CHECKBOX), ctrl_name);
                WASABI_API_LNGSTRINGW_BUF(IDS_FORCE_INTO_DESKTOP_MODE_HELP, buf, 2048);
                break;

            case IDC_CB_PRESS_F1_MSG:
                WASABI_API_LNGSTRINGW_BUF(IDS_HELP_ON_F1, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_HELP_ON_F1_HELP, buf, 2048);
                break;

            case IDC_CB_SKIN:
				StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_CHECKBOX), ctrl_name);
				WASABI_API_LNGSTRINGW_BUF(IDS_CB_SKIN_HELP, buf, 2048);
                break;

            case IDC_CB_SAVE_CPU:
                WASABI_API_LNGSTRINGW_BUF(IDS_SAVE_CPU_CHECKBOX, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_SAVE_CPU_CHECKBOX_HELP, buf, 2048);
                break;

            case IDC_CB_MIN:
                WASABI_API_LNGSTRINGW_BUF(IDS_HELP_MINIMIZE_WINAMP, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_HELP_MINIMIZE_WINAMP_HELP, buf, 2048);
                break;

            case IDC_CB_FIXSLOWTEXT:
                WASABI_API_LNGSTRINGW_BUF(IDS_TRY_TO_FIX_SLOW_TEXT, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_TRY_TO_FIX_SLOW_TEXT_HELP, buf, 2048);
                break;

            case IDC_CB_VJMODE:
                WASABI_API_LNGSTRINGW_BUF(IDS_VJ_MODE, title, 1024);
				WASABI_API_LNGSTRINGW_BUF(IDS_VJ_MODE_HELP, buf, 2048);
                break;

            case IDC_DMS_LABEL:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X), ctrl_name);
				WASABI_API_LNGSTRINGW_BUF(IDS_DMS_LABEL_HELP, buf, 2048);
                break;

            case IDC_FS_LABEL:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X), ctrl_name);
				WASABI_API_LNGSTRINGW_BUF(IDS_FS_LABEL_HELP, buf, 2048);
                break;
                
            case IDC_W_LABEL:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X), ctrl_name);
				WASABI_API_LNGSTRINGW_BUF(IDS_W_LABEL_HELP, buf, 2048);
                break;

            case ID_DM_MORE:
                StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X), ctrl_name);
				WASABI_API_LNGSTRINGW_BUF(IDS_DM_MORE_HELP, buf, 2048);
                break;
            }

            if (buf[0])
                MessageBoxW(hwnd, buf, title, MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);

        }
        break;  // case WM_HELP
    }

    return 0;
}

BOOL CALLBACK CPluginShell::ConfigDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    if (msg==WM_INITDIALOG && lParam > 0 && GetWindowLongPtr(hwnd,GWLP_USERDATA)==0)
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

    CPluginShell* p = (CPluginShell*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    if (p)
        return p->PluginShellConfigDialogProc(hwnd, msg, wParam, lParam);
    else
        return FALSE;
}

BOOL CPluginShell::PluginShellConfigDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    #ifdef _DEBUG
        OutputDebugMessage("CfgDlgProc:  ", hwnd, msg, wParam, lParam);
    #endif

    switch (msg)
    {
    case WM_DESTROY:
        EndConfig();
        return 0;

    case WM_INITDIALOG:
        { 
            // Initialize all config panel global variables:
            if (!InitConfig(hwnd))
            {
				wchar_t title[64];
                MessageBoxW(hwnd, WASABI_API_LNGSTRINGW(IDS_INITCONFIG_FAILED),
						    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
						    MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
                EndConfig();
                int id=LOWORD(wParam);
                EndDialog(hwnd,id);
                return false;
            }

            // set window caption
            SetWindowText( hwnd, WINDOWCAPTION );

            // Test for DirectX 9 + start it
            // note: if you don't call LoadLibrary here, and you're on a system
            //       where DX9 is missing, Direct3DCreate9() might crash; so call it.
            int d3d9_already_loaded = (GetModuleHandle("d3d9.dll") != NULL) ? 1 : 0;
            if (!d3d9_already_loaded)
                g_hmod_d3d9 = LoadLibrary("d3d9.dll");

            if ( (!d3d9_already_loaded && !g_hmod_d3d9) ||
                 !(g_lpDX = Direct3DCreate9(D3D_SDK_VERSION))
               )
            {
                MissingDirectX(hwnd);

                EndConfig();
                int id=LOWORD(wParam);
                EndDialog(hwnd,id);
                return false;
            }

				    if (!g_hmod_d3dx9)
					    g_hmod_d3dx9 = FindD3DX9(GetWinampWindow());

						if ((!g_hmod_d3dx9))
						{
	 		        MissingDirectX(hwnd);
              EndConfig();
              int id=LOWORD(wParam);
              EndDialog(hwnd,id);
              return false;
						}
                
            // enable the 'view website' button only if plugin author has #defined a URL (in defines.h):
            #ifndef PLUGIN_WEB_URL
                ShowWindow(GetDlgItem(hwnd, ID_WEB), SW_HIDE);
            #else
                if (wcslen(PLUGIN_WEB_URL)==0)
                    ShowWindow(GetDlgItem(hwnd, ID_WEB), SW_HIDE);
            #endif

            // enable the 'view docs' button only if plugin author has #defined a filename (in defines.h):
            #ifndef DOCFILE
                ShowWindow(GetDlgItem(hwnd, ID_DOCS), SW_HIDE);
            #else
                if (wcslen(DOCFILE)==0)
                    ShowWindow(GetDlgItem(hwnd, ID_DOCS), SW_HIDE);
            #endif

            // set contents of IDC_SZ_ABOUT
			wchar_t about[256];
			StringCchPrintfW(about, 256, WASABI_API_LNGSTRINGW(IDS_ABOUT_STRING), LONGNAMEW, AUTHOR_NAME, COPYRIGHT);
            SetDlgItemTextW(hwnd, IDC_SZ_ABOUT, about);

            // initialize tab control:
            {
				HWND tabWnd = GetDlgItem(hwnd,IDC_TABS);
                // Add Tabs:
                if (!AddButton(0, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_1)) ||
                    !AddButton(1, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_2)) ||
                    !AddButton(2, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_3)) ||
                    !AddButton(3, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_4)) ||
                    !AddButton(4, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_5)) ||
                    !AddButton(5, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_6)) ||
                    !AddButton(6, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_7)) ||
                    !AddButton(7, tabWnd, WASABI_API_LNGSTRINGW(IDS_CONFIG_PANEL_BUTTON_8)))
                {
					wchar_t title[64];
                    MessageBoxW(hwnd, WASABI_API_LNGSTRINGW(IDS_UNABLE_TO_LOAD_TABS),
							    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
							    MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
                    EndConfig();
                    int id=LOWORD(wParam);
                    EndDialog(hwnd,id);
                    return false;
                }

                // Simulate selection of the first tab. 
				int last_tab = GetPrivateProfileIntW(L"settings",L"last_tab",0,m_szConfigIniFile);
				TabCtrl_SetCurSel(tabWnd, last_tab);
                OnTabChanged(last_tab);
            }

            g_ignore_clicks = 0;

            SetFocus(hwnd);
        }
        return 0;

    case WM_NOTIFY: 
        if (!g_ignore_clicks)
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch(pnmh->code)
            {
            case TCN_SELCHANGE: 
                OnTabChanged(TabCtrl_GetCurSel(GetDlgItem(hwnd,IDC_TABS)));
                break;
            }
        }
        break;

    case WM_COMMAND:
        if (!g_ignore_clicks)
        {
            int id = LOWORD(wParam);
            switch(id)
            {
            case IDOK:
                // kill current tab window, so that its settings get read
				WritePrivateProfileIntW(TabCtrl_GetCurSel(GetDlgItem(hwnd,IDC_TABS)),L"last_tab",m_szConfigIniFile,L"settings");
                OnTabChanged(-1);

                // then save new config
                WriteConfig();

                EndDialog(hwnd,id);
                return 0;
            
            case IDCANCEL:
				WritePrivateProfileIntW(TabCtrl_GetCurSel(GetDlgItem(hwnd,IDC_TABS)),L"last_tab",m_szConfigIniFile,L"settings");
                EndDialog(hwnd,id);
                return 0;

            case ID_DOCS:
                {
                    wchar_t szPath[512], szFile[512];
                    lstrcpyW(szPath, m_szPluginsDirPath);
                    lstrcpyW(szFile, szPath);
                    lstrcatW(szFile, DOCFILE);
					
					intptr_t ret = myOpenURL(0,szFile);
                    if (ret <= 32)
                    {
                        wchar_t buf[1024];
                        switch(ret)
                        {
                        case SE_ERR_FNF:
                        case SE_ERR_PNF:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_DOCUMENTATION_FILE_NOT_FOUND), szFile);
                            break;
                        case SE_ERR_ACCESSDENIED:
                        case SE_ERR_SHARE:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_DOCUMENTATION_FILE_DENIED), szFile);
                            break;
                        case SE_ERR_NOASSOC:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_DOCUMENTATION_FILE_FAILED_DUE_TO_NO_ASSOC), szFile);
                            break;
                        default:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_DOCUMENTATION_FILE_FAILED_CODE_X), szFile, ret);
                            break;
                        }
                        MessageBoxW(hwnd, buf, WASABI_API_LNGSTRINGW(IDS_ERROR_OPENING_DOCUMENTATION), MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
                    }
                }
                break;

            case ID_WEB:
                {
                    intptr_t ret = myOpenURL(NULL, PLUGIN_WEB_URL);
                    if (ret <= 32)
                    {
                        wchar_t buf[1024];
                        switch(ret)
                        {
                        case SE_ERR_FNF:
                        case SE_ERR_PNF:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_URL_COULD_NOT_OPEN), PLUGIN_WEB_URL);
                            break;
                        case SE_ERR_ACCESSDENIED:
                        case SE_ERR_SHARE:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_URL_WAS_DENIED), PLUGIN_WEB_URL);
                            break;
                        case SE_ERR_NOASSOC:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_URL_FAILED_DUE_TO_NO_ASSOC), PLUGIN_WEB_URL);
                            break;
                        default:
                            StringCbPrintfW(buf, sizeof(buf), WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_URL_FAILED_CODE_X), PLUGIN_WEB_URL, ret);
                            break;
                        }
                        MessageBoxW(hwnd, buf, WASABI_API_LNGSTRINGW(IDS_ERROR_OPENING_URL), MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
                    }
                }
                break;

            case ID_DEFAULTS:
				wchar_t title[64];
                if (IDYES == MessageBoxW(hwnd, WASABI_API_LNGSTRINGW(IDS_RESTORE_ALL_DEFAULTS),
										 WASABI_API_LNGSTRINGW_BUF(IDS_RESTORE_ALL_DEFAULTS_TITLE, title, 64),
										 MB_YESNO|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL))
                {
                    DeleteFileW(m_szConfigIniFile);
                    Sleep(100);
                    EndDialog(hwnd,id);
                }
                break;

            default:
                return 0;
            }
        }
        break;        // case WM_COMMAND

    case WM_HELP:
        if (lParam)
				{
					HELPINFO *ph = (HELPINFO*)lParam;
					wchar_t title[1024];
					wchar_t buf[2048];
					wchar_t ctrl_name[1024];
					GetWindowTextW(GetDlgItem(hwnd, ph->iCtrlId), ctrl_name, sizeof(ctrl_name)/sizeof(*ctrl_name));
					RemoveSingleAmpersands(ctrl_name);
					buf[0] = 0;
					switch(ph->iCtrlId)
					{
					case IDOK:
						StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
						WASABI_API_LNGSTRINGW_BUF(IDS_OK_HELP, buf, 2048);
						break;

					case IDCANCEL:
						StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
						WASABI_API_LNGSTRINGW_BUF(IDS_CANCEL_HELP, buf, 2048);
						break;

					case ID_DEFAULTS:
						StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
						WASABI_API_LNGSTRINGW_BUF(IDS_RESTORE_DEFAULTS_HELP, buf, 2048);
						break;

					case ID_DOCS:
						StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
						WASABI_API_LNGSTRINGW_BUF(IDS_DOCUMENTATION_BUTTON_HELP, buf, 2048);
						break;

					case ID_WEB:
						StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_BUTTON), ctrl_name);
						WASABI_API_LNGSTRINGW_BUF(IDS_VIEW_ONLINE_DOCS_HELP, buf, 2048);
						break;

					default:
						return 0;
					}

					if (buf[0])
						MessageBoxW(hwnd, buf, title, MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
				}
        break;  // case WM_HELP
    } 

    return 0;
}