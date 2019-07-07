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
#include <shellapi.h>

//----------------------------------------------------------------------

#define VMS_DESKTOP_DLLNAME (SUBDIR L"data\\vms_desktop.dll")

//----------------------------------------------------------------------

typedef struct _SIMPLEVERTEX 
{
    float x, y;      // screen position    
    float z;         // Z-buffer depth    
    DWORD Diffuse;   // diffuse color. also acts as filler; aligns struct to 16 bytes (good for random access/indexed prims)
} SIMPLEVERTEX, *LPSIMPLEVERTEX; 

typedef struct _HELPVERTEX 
{
    float x, y;      // screen position    
    float z;         // Z-buffer depth    
    DWORD Diffuse;   // diffuse color. also acts as filler; aligns struct to 16 bytes (good for random access/indexed prims)
    float tu, tv;    // texture coordinates for texture #0
} HELPVERTEX, *LPHELPVERTEX; 

#define SIMPLE_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define HELP_VERTEX_FORMAT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//----------------------------------------------------------------------

// resides in vms_desktop.dll/lib:
int setHook(HWND hlv,HWND w,int version);
void removeHook();

//----------------------------------------------------------------------

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
bool IsVistaOrLater()
{
    // adapted from "Getting the System Version" on MSDN - http://msdn2.microsoft.com/en-us/library/ms724429.aspx
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    PGNSI pGNSI;
    BOOL bOsVersionInfoEx;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))
    {
        // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
        pGNSI = (PGNSI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo" );
        if(NULL != pGNSI)
            pGNSI(&si);
        else 
            GetSystemInfo(&si);

        if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
        {
            if ( osvi.dwMajorVersion >= 6 )
                return true;
        }
    }
    return false;
}

int CPluginShell::InitDesktopMode()
{
    if (m_screenmode != DESKTOP)
        return false;

    // check for Vista - if Vista, don't try to draw desktop icons.
    // [ vms_desktop.dll's message posts to the desktop listview window cause explorer to crash...
    //   whether it sends WM_NULL or WM_USER+516/517. ]
    if (m_desktop_show_icons && IsVistaOrLater())
        m_desktop_show_icons = false;

    if (!m_desktop_show_icons)
        return true;

    // note: we have to explicitly make sure the DLL is present,
    // since we're delay-loading it; otherwise, calling setHook, etc. will crash it.
    wchar_t szVmsDesktopDll[MAX_PATH];
    swprintf(szVmsDesktopDll, L"%s%s", GetPluginsDirPath(), VMS_DESKTOP_DLLNAME);
    if (!GetModuleHandleW(szVmsDesktopDll))
    {
        if (!LoadLibraryW(szVmsDesktopDll))
        {
            wchar_t buf[2048];
            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_COULD_NOT_FIND_FILE_FOR_DESKTOP_MODE_X), szVmsDesktopDll);
            MessageBoxW(GetPluginWindow(),buf,WASABI_API_LNGSTRINGW(IDS_MILKDROP_ERROR_FILE_MISSING), MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            //return false;
            m_desktop_icons_disabled = 1;
        }
        else
        {
            m_vms_desktop_loaded = 1;
        }
    }

    InitializeCriticalSection(&m_desktop_cs);

    m_desktop_icon_state = 0;
    m_desktop_icon_count = 0;
    m_desktop_icon_update_frame = 0;
    m_desktop_icon_size  = GetDesktopIconSize();

    // GDI font for desktop mode:
	LOGFONT lf = {0};
	wchar_t title[64];
    if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0))
    {
        if (!(m_font_desktop = CreateFontIndirect(&lf)))
        {
            MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_CREATING_GDI_DESKTOP_FONT),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            return false;
        }
    }
    else
    {
        if (!(m_font_desktop = CreateFont(14, 0, 0, 0, 0, 0, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Monotype Sans Serif")))
        {
            MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_CREATING_GDI_DESKTOP_FONT),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            return false;
        }
    }

    // Create D3DX font for drawing icon labels on desktop:
    if (pCreateFontW(m_lpDX->m_lpDevice, 14, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Monotype Sans Serif", &m_d3dx_desktop_font) != D3D_OK)
    {
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_CREATING_DESKTOP_FONT),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return false;
    }

    // create first texture for holding icon bitmaps:
    // (do it now, to ensure that at least 1 gets created, before
    //  the plugin does all of its DX9 allocations.)
    if (!CreateDesktopIconTexture(&m_desktop_icons_texture[0]))
    {
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_CREATING_TEXTURE_FOR_ICON_BITMAPS),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
    }
    
    if (!m_desktop_icons_disabled)
    {
        int ret = setHook(m_hWndDesktopListView, GetPluginWindow(), 1);
        if (ret == 1)
            m_desktop_hook_set = 1;
        else if (ret == -1)
            MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_OUTDATED_VMS_DESKTOP_DLL_NEED_TO_REINSTALL),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        else 
            MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_CREATING_HOOK_PROC_DESKTOP_ICONS_NOT_AVAILABLE),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
    }

    return true;
}

//----------------------------------------------------------------------

void CPluginShell::CleanUpDesktopMode()
{
    if (m_screenmode != DESKTOP)
        return;
    if (!m_desktop_show_icons)
        return;

    if (m_desktop_hook_set)
    {
        m_desktop_hook_set = 0;
        removeHook();
    }

    for (int i=0; i<MAX_ICON_TEXTURES; i++)
        SafeRelease(m_desktop_icons_texture[i]);
    SafeRelease(m_d3dx_desktop_font);

    if (m_desktop_wc_registered)
    {
        UnregisterClass(DESKTOP_MODE_KEYBOARD_INPUT_WINDOW_CLASSNAME, m_hInstance);
        m_desktop_wc_registered = 0;
    }

    if (m_font_desktop)
    {
        DeleteObject(m_font_desktop);
        m_font_desktop = 0;
    }

    m_icon_list.clear();

    DeleteCriticalSection(&m_desktop_cs);

    if (m_vms_desktop_loaded)
    {
        char szVmsDesktopDll[MAX_PATH];
        sprintf(szVmsDesktopDll, "%s%s", GetPluginsDirPath(), VMS_DESKTOP_DLLNAME);
        FreeLibrary(GetModuleHandle(szVmsDesktopDll));
        m_vms_desktop_loaded = 0;
    }
}

//----------------------------------------------------------------------

int CPluginShell::CreateDesktopIconTexture(IDirect3DTexture9** ppTex)
{
    // release old texture (shouldn't really be necessary)
    if (*ppTex)
    {
        (*ppTex)->Release();
        *ppTex = NULL;
    }

    // create new
    int ntries = (m_lpDX->m_d3dpp.BackBufferFormat == D3DFMT_R5G6B5) ? 3 : 1;
    for (int ntry=0; ntry<ntries; ntry++)
    {
        D3DFORMAT fmt = m_lpDX->m_d3dpp.BackBufferFormat;
        switch(m_lpDX->m_d3dpp.BackBufferFormat)
        {
        case D3DFMT_R8G8B8:
        case D3DFMT_X8R8G8B8:
            fmt = D3DFMT_A8R8G8B8;
            break;
        case D3DFMT_R5G6B5: // <- PROBLEM: NO ALPHA CHANNEL FOR ICONS
            if (ntry==0)
                switch(m_desktop_555_fix)
                {
                case 0: fmt = D3DFMT_R5G6B5;   break;
                case 1: fmt = D3DFMT_A1R5G5B5; break;
                case 2: fmt = D3DFMT_A8R8G8B8; break;
                }
            else if (ntry==1)
                switch(m_desktop_555_fix)
                {
                case 0: fmt = D3DFMT_A1R5G5B5; break;
                case 1: fmt = D3DFMT_A8R8G8B8; break;
                case 2: fmt = D3DFMT_A1R5G5B5; break;
                }
            else 
                switch(m_desktop_555_fix)
                {
                case 0: fmt = D3DFMT_A8R8G8B8; break;
                case 1: fmt = D3DFMT_R5G6B5;   break;
                case 2: fmt = D3DFMT_R5G6B5;   break;
                }
            break;
        case D3DFMT_X1R5G5B5:
            fmt = D3DFMT_A1R5G5B5;
            break;
        }

        if (m_lpDX->m_lpDevice->CreateTexture(ICON_TEXTURE_SIZE, ICON_TEXTURE_SIZE, 1, 0, fmt, D3DPOOL_MANAGED, ppTex, NULL) != D3D_OK)
            *ppTex = NULL;
        else 
            break;
    }

    return (*ppTex) ? 1 : 0;
}

//----------------------------------------------------------------------

void CPluginShell::DeselectDesktop()
{
    IconList::iterator p;
    for (p = m_icon_list.begin(); p != m_icon_list.end(); p++)
        p->selected = 0;
}

//----------------------------------------------------------------------

void CPluginShell::UpdateDesktopBitmaps()
{
    // update the D3DX textures that hold all the icons:
    
    int idx = 0;
    int texnum = 0;
    int show_msgs = 1;

    // if no icon texture could be created at startup, 
    // don't bother trying anything here, and don't give them
    // any extra error messages.
    if (!m_desktop_icons_texture[0])
        return;

    IconList::iterator p;
    for (p = m_icon_list.begin(); p != m_icon_list.end(); p++)
        p->icon_bitmap_idx = -1;
    
    do
    {
        idx = StuffIconBitmaps(idx, texnum++, &show_msgs);
    } 
    while (idx > 0 && texnum < MAX_ICON_TEXTURES);

	if (idx > 0)
	{
		wchar_t title[64];
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_WARNING, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
	}
}

//----------------------------------------------------------------------

int CPluginShell::StuffIconBitmaps(int iStartIconIdx, int iTexNum, int *show_msgs)
{
    // returns:
    //   0 if done (or error), or
    //   N if the texture is full & we need to start another one,
    //      where N is the new iStartIconIdx to use.

    if (m_screenmode != DESKTOP)
        return 0;

	wchar_t title[64];
    if (!m_desktop_icons_texture[iTexNum])
    {
        int ret = CreateDesktopIconTexture(&m_desktop_icons_texture[iTexNum]);
        if (!ret)
        {
            MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_TOO_MANY_UNIQUE_ICON_BITMAPS),
					    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_WARNING, title, 64),
					    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            return 0;
        }
    }

    D3DSURFACE_DESC sd;
    if (m_desktop_icons_texture[iTexNum]->GetLevelDesc(0, &sd) != D3D_OK)
    {
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_COULD_NOT_GET_LEVEL_DESCRIPTION),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return 0;
    }

    D3DLOCKED_RECT lr;
    if (m_desktop_icons_texture[iTexNum]->LockRect(0, &lr, NULL, 0) != D3D_OK)
    {
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_LOCKRECT_FAILED),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return 0;
    }
    if (lr.pBits == NULL)
    {
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_LR_PBITS_IS_NULL),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        m_desktop_icons_texture[iTexNum]->UnlockRect(0);
        return 0;
    }

    unsigned __int16* p16 = (unsigned __int16*)lr.pBits;
    unsigned __int32* p32 = (unsigned __int32*)lr.pBits;
    int WIDTH = sd.Width;

    int i;

    int start;
    int bpp;
    int rshift[3];      // 1. bits to first shift right r,g,b
    int mask[3];        // 2. mask for r, g, b
    int lshift[3];      // 3. bits to then shift left r,g,b

    switch(sd.Format)
    {
    case D3DFMT_R8G8B8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        start = 0xFF000000;
        bpp = 32;
        rshift[0] = 0;
        rshift[1] = 0;
        rshift[2] = 0;
        mask[0] = 0xFF;
        mask[1] = 0xFF;
        mask[2] = 0xFF;
        lshift[0] = 16;
        lshift[1] = 8;
        lshift[2] = 0;
        break;

    case D3DFMT_R5G6B5:
        start = 0x0000;
        bpp = 16;
        rshift[0] = 3;
        rshift[1] = 2;
        rshift[2] = 3;
        mask[0] = 0x1F;
        mask[1] = 0x3F;
        mask[2] = 0x1F;
        lshift[0] = 11;
        lshift[1] = 5;
        lshift[2] = 0;
        break;

    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
        start = 0x8000;
        bpp = 16;
        rshift[0] = 3;
        rshift[1] = 3;
        rshift[2] = 3;
        mask[0] = 0x1F;
        mask[1] = 0x1F;
        mask[2] = 0x1F;
        lshift[0] = 10;
        lshift[1] = 5;
        lshift[2] = 0;
        break;

    case D3DFMT_A4R4G4B4:
        start = 0xF000;
        bpp = 16;
        rshift[0] = 4;
        rshift[1] = 4;
        rshift[2] = 4;
        mask[0] = 0x0F;
        mask[1] = 0x0F;
        mask[2] = 0x0F;
        lshift[0] = 8;
        lshift[1] = 4;
        lshift[2] = 0;
        break;

    default:
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_UNKNOWN_PIXEL_FORMAT),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        m_desktop_icons_texture[iTexNum]->UnlockRect(0);
        return 0;
    }

    HDC hdc = GetDC(NULL);
    if (!hdc)
    {
        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_COULDNT_GET_HDC),
				    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
				    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        m_desktop_icons_texture[iTexNum]->UnlockRect(0);
        return 0;
    }

    #define MAX_ICON_SIZE 128
    unsigned char data[MAX_ICON_SIZE*MAX_ICON_SIZE*4];

    int nAcross = ICON_TEXTURE_SIZE/m_desktop_icon_size;
    int nDown   = ICON_TEXTURE_SIZE/m_desktop_icon_size;

    // for each icon, add its bitmap to the texture (if not already there),
    //   and set 'icon_bitmap_idx'.
    IconList::iterator p = m_icon_list.begin();
    for (i=0; i < iStartIconIdx; i++)
        p++;

    int bitmap_idx = 0;
    int list_idx   = iStartIconIdx;

    for ( ; p != m_icon_list.end() && bitmap_idx < nAcross*nDown; p++)
    {
        // note: 'p' points to the correct icon to start with, 
        //       but 'idx' starts at zero!
    
        // get the icon:
        SHFILEINFO sfi;
        int flags = SHGFI_ICON|SHGFI_PIDL|SHGFI_SHELLICONSIZE | ((m_desktop_icon_size > 32) ? SHGFI_LARGEICON : 0);
        if (SHGetFileInfo((LPCTSTR)p->pidl, 0, &sfi, sizeof(sfi), flags))
        {
            ICONINFO ii;
            if (GetIconInfo(sfi.hIcon, &ii))
            {
                int x0 = (bitmap_idx%nAcross)*m_desktop_icon_size;
                int y0 = (bitmap_idx/nAcross)*m_desktop_icon_size;
                int checksum[3] = { 0, 0, 0 };

                BITMAPINFO bmi;

                // pass 1: get the colors 

                bmi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth  = m_desktop_icon_size;
                bmi.bmiHeader.biHeight = m_desktop_icon_size;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                if (GetDIBits(
                  hdc,               // handle to DC
                  ii.hbmColor,       // handle to bitmap
                  0,                 // first scan line to set
                  m_desktop_icon_size,// number of scan lines to copy
                  data,              // array for bitmap bits
                  &bmi,              // bitmap data buffer
                  DIB_RGB_COLORS     // RGB or palette index
                ))
                {
                    int w = min(bmi.bmiHeader.biWidth , m_desktop_icon_size);
                    int h = min(bmi.bmiHeader.biHeight, m_desktop_icon_size);

                    for (int y=0; y<h; y++)
                    for (int x=0; x<w; x++)
                    {
                        int in_offset = ((h-1-y)*w + x)*4;
                        int r = data[in_offset+2];
                        int g = data[in_offset+1];
                        int b = data[in_offset+0];

                        checksum[0] += r;
                        checksum[1] += g;
                        checksum[2] += b;
        
                        int out_offset = (y0+y)*WIDTH + (x0+x);
                        if (bpp==16)
                            p16[out_offset] = start | 
                                (((r >> rshift[0]) & mask[0]) << lshift[0]) | 
                                (((g >> rshift[1]) & mask[1]) << lshift[1]) | 
                                (((b >> rshift[2]) & mask[2]) << lshift[2]);                
                        else
                            p32[out_offset] = start | 
                                (((r >> rshift[0]) & mask[0]) << lshift[0]) | 
                                (((g >> rshift[1]) & mask[1]) << lshift[1]) | 
                                (((b >> rshift[2]) & mask[2]) << lshift[2]);                                            
                    }
                }
                else
                {
                    if (*show_msgs) 
                        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_CALL_TO_GETDIBITS_FAILED),
								    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
								    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
                    *show_msgs = 0;
                }

                // pass 2: get the alpha mask

                bmi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth  = m_desktop_icon_size;
                bmi.bmiHeader.biHeight = m_desktop_icon_size;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                if (GetDIBits(
                  hdc,               // handle to DC
                  ii.hbmMask,        // handle to bitmap
                  0,                 // first scan line to set
                  m_desktop_icon_size,// number of scan lines to copy
                  data,              // array for bitmap bits
                  &bmi,              // bitmap data buffer
                  DIB_RGB_COLORS     // RGB or palette index
                ))
                {
                    int w = min(bmi.bmiHeader.biWidth , m_desktop_icon_size);
                    int h = min(bmi.bmiHeader.biHeight, m_desktop_icon_size);

                    for (int y=0; y<h; y++)
                    for (int x=0; x<w; x++)
                    {
                        int in_offset = ((h-1-y)*w + x)*4;
                        int r = data[in_offset+2];
                        int g = data[in_offset+1];
                        int b = data[in_offset+0];

                        checksum[0] += r;
                        checksum[1] += g;
                        checksum[2] += b;

                        if (r || g || b)
                        {
                            int out_offset = (y0+y)*WIDTH + (x0+x);
                            if (bpp==16)
                                p16[out_offset] &= ~start;
                            else
                                p32[out_offset] &= ~start;
                        }
                    }
                }
                else
                {
                    if (*show_msgs) 
                        MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_CALL_2_TO_GETDIBITS_FAILED),
								    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
								    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
                    *show_msgs = 0;
                }

                // check for duplicate icon, and if found, reuse it
                int done = 0;
                IconList::iterator q;
                for (q = m_icon_list.begin(); q != m_icon_list.end() && q != p; q++)
                {
                    if (checksum[0] == q->checksum[0] &&
                        checksum[1] == q->checksum[1] &&
                        checksum[2] == q->checksum[2])
                    {
                        p->icon_bitmap_idx = q->icon_bitmap_idx;
                        done = 1;
                        break;
                    }
                }

                // otherwise, keep new icon
                if (!done)
                {
                    p->icon_bitmap_idx = nAcross*nDown*iTexNum + bitmap_idx;
                    p->checksum[0] = checksum[0];
                    p->checksum[1] = checksum[1];
                    p->checksum[2] = checksum[2];

                    bitmap_idx++;
                }

                DeleteObject(ii.hbmMask);
                DeleteObject(ii.hbmColor);
            }
            else
            {
                if (*show_msgs) 
                    MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_GETICONINFO_FAILED),
							    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
							    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
                *show_msgs = 0;
            }

            DestroyIcon(sfi.hIcon);
        }
        else
        {
            if (*show_msgs) 
                MessageBoxW(GetPluginWindow(), WASABI_API_LNGSTRINGW(IDS_ERROR_UPDATING_ICON_BITMAPS_SHGETFILEINFO_FAILED),
						    WASABI_API_LNGSTRINGW_BUF(IDS_MILKDROP_ERROR, title, 64),
						    MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
            *show_msgs = 0;
        }

        list_idx++;
    }

    ReleaseDC(NULL, hdc);
    m_desktop_icons_texture[iTexNum]->UnlockRect(0);

    if (bitmap_idx >= nAcross*nDown)
        return list_idx;
    else
        return 0; // all done
}

//----------------------------------------------------------------------

void CPluginShell::RenderDesktop()
{
    if (m_screenmode != DESKTOP)
        return;
    if (!m_desktop_show_icons)
        return;
    if (m_desktop_icons_disabled)
        return;

    IconList::iterator p;

    EnterCriticalSection(&m_desktop_cs);

	int iconcount = static_cast<unsigned long>(SendMessage(m_hWndDesktopListView, LVM_GETITEMCOUNT, 0, 0));
    if (iconcount == 0) 
    {
        LeaveCriticalSection(&m_desktop_cs);
        return;
    }

    // if the icons list is empty,
    // or if we check it for consistency and an update is recommended (GetDesktopIcons(1)==2),
    // update the icons list & the bitmaps:
    /*if (m_icon_list.size()==0 || GetDesktopIcons(1)==2)
    {
        m_icon_list.clear();
        GetDesktopIcons(0);

        UpdateDesktopBitmaps();
    }*/

    // check for invalid entries.  (if there is an error in getItemData(),
    // it will return the icon_t structure anyway, but with empty strings.)
    int invalid_entries = 0;
    if (m_desktop_icon_state >= 2)
    {
        for (p = m_icon_list.begin(); p != m_icon_list.end() && !invalid_entries; p++)
        {
            if (p->name[0]==0)
                invalid_entries = 1;
            //if (p->pidl[0].mkid.cb==0 && p->pidl[0].mkid.abID[0]==0)
            if (p->pidl[0]==0 && p->pidl[1]==0)
                invalid_entries = 1;
        }
    }

    if (
        (m_desktop_icon_state == 0)         || 
        (m_desktop_icon_state >= 2 && m_desktop_icon_count != iconcount)   || 
        (m_desktop_icon_state >= 2 && invalid_entries)
       )
    {
        // begin total refresh
        m_desktop_icon_state = 1;
        m_desktop_icon_count = iconcount;
        m_desktop_icon_update_frame = GetFrame();
        m_icon_list.clear();

        SendMessage(GetPluginWindow(), WM_USER, 0x80000000 | iconcount, 0);//getItemData(i);

        // try to get the desktop window's listview to respond to
        // the queries as quickly as possible:
        {
            LeaveCriticalSection(&m_desktop_cs);
        
            DWORD procid = NULL;
            DWORD threadid = GetWindowThreadProcessId(m_hWndDesktopListView, &procid);
            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_SET_INFORMATION, FALSE, procid);
            DWORD x = GetPriorityClass(hProcess);
            if (x) SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);
           
            for (int i=0; i<5; i++)
            {
                Sleep(10);

                MSG msg;
                while(PeekMessage(&msg,GetPluginWindow(),0,0,PM_REMOVE)) 
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            if (x) SetPriorityClass(hProcess, x);
            CloseHandle(hProcess);
            
            EnterCriticalSection(&m_desktop_cs);
        }
    }
    else if (m_desktop_icon_state == 1 && 
             m_icon_list.size() < (size_t)m_desktop_icon_count)
    {
        // waiting for the 'total refresh' to complete
        // ...
        if (GetFrame() > m_desktop_icon_update_frame+64)
        {
            m_desktop_icon_state = 0;
        }
    }
    else
    if (m_desktop_icon_state == 1 && 
        m_icon_list.size() == m_desktop_icon_count)
    {
        // done with total refresh
        m_desktop_icon_state = 2;
        m_desktop_icon_update_frame = GetFrame();
        UpdateDesktopBitmaps();
    }
    else if (m_desktop_icon_state == 2)
    {
        if (GetFrame() > m_desktop_icon_update_frame+4)
        {
            m_desktop_icon_state = 3; // will mean we're waiting on data to return.
            m_desktop_icon_update_frame = GetFrame();
            int start = 0;
            int len = iconcount;
            SendMessage(GetPluginWindow(), WM_USER, start | (len << 16), 0);//getItemData(i);
        }
    }
    else if (m_desktop_icon_state == 3)
    {
        if (GetFrame() > m_desktop_icon_update_frame+64)
        {
            // timeout; give up waiting for update message to come back,
            // and just request another.
            m_desktop_icon_state = 2;
            m_desktop_icon_update_frame = GetFrame();
        }
    }

    // get horz. spacing between icons (...determines width of labels)
    ICONMETRICS icm;
    icm.cbSize = sizeof(icm);
    if (!SystemParametersInfo(SPI_GETICONMETRICS, sizeof(icm), &icm, 0))
        icm.iHorzSpacing = 68;

    /*int font_height = 0;
    {
        RECT r;
        m_d3dx_desktop_font->DrawText(NULL, "M", -1, &r, DT_CALCRECT, 0xFFFFFFFF);
        font_height = r.bottom - r.top;
    }*/

    // display the desktop.

    m_lpDX->m_lpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_lpDX->m_lpDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);//D3DTOP_SELECTARG1 );
    m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_lpDX->m_lpDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    int nAcross = ICON_TEXTURE_SIZE/m_desktop_icon_size;
    int nDown   = ICON_TEXTURE_SIZE/m_desktop_icon_size;

    // The icon's x and y coordinates (as returned by the 
    // getIconData/WM_COPYDATA system) are (0,0) at the
    // upper-left corner of the rectangle that encompasses
    // all the monitors together (m_lpDX->m_all_monitors_rect).

    // Note that in 'm_all_monitors_rect', (0,0) represents
    // the upper-left corner of the PRIMARY DISPLAY - not necessarily
    // the one we're showing the fake desktop on.

    // What we have to do here is determine icon_dx and icon_dy, 
    // which are the transformation from the coordinate space used
    // by the desktop itself (as returned in WM_COPYDATA) 
    // and the coordinates used by Windows itself (where 0,0 is
    // the upper-left corner of the PRIMARY DISPLAY, and coordinates
    // in other displays can be negative if they are above/left of it).

    int upperleft_x = min(m_lpDX->m_all_monitors_rect.left, m_lpDX->m_monitor_rect.left);
    int upperleft_y = min(m_lpDX->m_all_monitors_rect.top , m_lpDX->m_monitor_rect.top );
    int icon_dx = m_lpDX->m_monitor_rect.left - upperleft_x; // subtract this amount
    int icon_dy = m_lpDX->m_monitor_rect.top  - upperleft_y; // subtract this amount

    if (!m_desktop_manual_icon_scoot)
    {
        icon_dx -= m_lpDX->m_monitor_rect.left - m_lpDX->m_monitor_work_rect.left;
        icon_dy -= m_lpDX->m_monitor_rect.top  - m_lpDX->m_monitor_work_rect.top ;
    }

    // pass 0: draw normal text & icons
    // pass 1: redraw icon currently being dragged, transparently
    int nPasses = m_desktop_dragging ? 2 : 1;
    for (int pass=0; pass<nPasses; pass++)
    {
        // first, draw [blue backgrounds &] text labels
        {
            m_lpDX->m_lpDevice->SetVertexShader( NULL );
            m_lpDX->m_lpDevice->SetFVF( SIMPLE_VERTEX_FORMAT );
            m_lpDX->m_lpDevice->SetTexture(0, NULL);
            SIMPLEVERTEX verts[4];

            // pass2==0: draw text labels
            // pass2==1: draw text overtop
            // (separated for speed, so ID3DXFont can get the HDC just once for all the DrawText calls)
            for (int pass2=0; pass2<2; pass2++)
            {
                //if (pass2==1)
                    //m_d3dx_desktop_font->Begin();

                for (p = m_icon_list.begin(); p != m_icon_list.end(); p++)
                {
                    if (pass==0 || (p->selected && m_desktop_dragging))
                    {
                        int max_width = icm.iHorzSpacing-5+m_desktop_icon_size-32;//icm.iHorzSpacing-5;

                        int dx = 0;
                        int dy = 4;

                        if (pass>0)
                        {
                            dx += m_desktop_drag_curpos.x - m_desktop_drag_startpos.x;
                            dy += m_desktop_drag_curpos.y - m_desktop_drag_startpos.y;
                        }

                        SetRect(&p->label_rect, 
                            p->x + m_desktop_icon_size/2 - icon_dx - max_width/2 + dx, 
                            p->y + m_desktop_icon_size   - icon_dy               + dy, 
                            p->x + m_desktop_icon_size/2 - icon_dx + max_width/2 + dx, 
                            p->y + m_desktop_icon_size   - icon_dy + 0           + dy // will be extended by DT_CALCRECT step!
                        );

                        // calculate rect for the text label
                        DWORD style = DT_CENTER|DT_WORDBREAK|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS;
                        // these aren't supported by D3DX9:
                        style &= ~(DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_NOPREFIX);
                        m_d3dx_desktop_font->DrawText(NULL, p->name, -1, &p->label_rect, style|DT_CALCRECT, 0xFFFFFFFF);

                        // D3DX doesn't handle text so well if it goes off
                        // the left edge of the screen, so don't show text labels
                        // that are mostly off (on the left side):
                        if ((p->label_rect.left + p->label_rect.right)/2 > 0)
                        {
                            // also, if label would go off left edge of screen, 
                            // push it to the right:
                            if (p->label_rect.left < 0 && p->label_rect.right > 0)
                            {
                                p->label_rect.right -= p->label_rect.left;
                                p->label_rect.left = 0;
                            }

                            //if (p->selected)  // ...draw blue background around text label
                            if (pass2==0)
                            {
                                if (m_desktop_textlabel_boxes || p->selected)
                                {
                                    #define EXTEND_LEFT   3
                                    #define EXTEND_RIGHT  2
                                    #define EXTEND_TOP    0
                                    #define EXTEND_BOTTOM 2
                                    for (int i=0; i<4; i++)
                                    {
                                        verts[i].x = (i%2==0) ? (float)(-m_lpDX->m_client_width/2 + p->label_rect.left - EXTEND_LEFT) : (float)(-m_lpDX->m_client_width/2 + p->label_rect.right  + EXTEND_RIGHT); // was -2/+3
                                        verts[i].y = (i/2==0) ? (float)(m_lpDX->m_client_height/2 - p->label_rect.top  + EXTEND_TOP ) : (float)(m_lpDX->m_client_height/2 - p->label_rect.bottom - EXTEND_BOTTOM); // was +1/-1
                                        verts[i].z = 0;
                                        verts[i].Diffuse = (p->selected) ? m_desktop_sel_color : m_desktop_bk_color;//0xFF000000;
                                        if (pass>0)
                                            verts[i].Diffuse &= 0x7FFFFFFF;
                                    }
                                    m_lpDX->m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, sizeof(SIMPLEVERTEX));
                                }
                            }
                            else
                            {
                                DWORD text_color = (p->selected) ? m_desktop_sel_text_color : m_desktop_text_color;
                                if (pass==1)
                                    text_color &= 0x7FFFFFFF;
                                // these aren't supported by D3DX9:
                                style &= ~(DT_WORD_ELLIPSIS | DT_END_ELLIPSIS | DT_NOPREFIX);
                                m_d3dx_desktop_font->DrawText(NULL, p->name, -1, &p->label_rect, style, text_color);
                            }
                        }
                    }
                }

                //if (pass2==1)
                //    m_d3dx_desktop_font->End();
            }
        }

        m_lpDX->m_lpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_lpDX->m_lpDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        m_lpDX->m_lpDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        
        // second, draw icon bitmaps (overtop)
        if (m_desktop_icon_state >= 2)
        {
            int dx = 0;
            int dy = 0;   
            int iTexNum = 0;
    
            while (m_desktop_icons_texture[iTexNum] && iTexNum < MAX_ICON_TEXTURES)
            {
                HELPVERTEX verts[4];
                m_lpDX->m_lpDevice->SetVertexShader( NULL );
                m_lpDX->m_lpDevice->SetFVF( HELP_VERTEX_FORMAT );
                m_lpDX->m_lpDevice->SetTexture(0, m_desktop_icons_texture[iTexNum]);

                for (p = m_icon_list.begin(); p != m_icon_list.end(); p++)
                {
                    int icon_tex_idx = (p->icon_bitmap_idx==-1) ? 0 : (p->icon_bitmap_idx / (nAcross*nDown));
                    int icon_bitmap_idx = (p->icon_bitmap_idx==-1) ? 0 : (p->icon_bitmap_idx % (nAcross*nDown));
                    if (
                        (icon_tex_idx == iTexNum) &&
                        (pass==0 || (p->selected && m_desktop_dragging))
                       )
                    {
                        SetRect(&p->icon_rect, 
                            p->x - icon_dx + dx, 
                            p->y - icon_dy + dy, 
                            p->x - icon_dx + dx + m_desktop_icon_size, 
                            p->y - icon_dy + dy + m_desktop_icon_size);

                        int lookup_x = 0;
                        int lookup_y = 0;

                        if (icon_bitmap_idx >= 0)  // if -1, means icon didn't fit in the texture
                        {
                            lookup_x = icon_bitmap_idx % nAcross;
                            lookup_y = icon_bitmap_idx / nAcross;
                        }

                        for (int i=0; i<4; i++)
                        {
                            verts[i].x = (i%2==0) ? (float)(-m_lpDX->m_client_width/2 + p->icon_rect.left) : (float)(-m_lpDX->m_client_width/2 + p->icon_rect.right );
                            verts[i].y = (i/2==0) ? (float)(m_lpDX->m_client_height/2 - p->icon_rect.top ) : (float)(m_lpDX->m_client_height/2 - p->icon_rect.bottom);
                            verts[i].z = 0;
                            verts[i].tu = ((lookup_x + i%2)*m_desktop_icon_size + 0.5f)/(float)ICON_TEXTURE_SIZE;
                            verts[i].tv = ((lookup_y + i/2)*m_desktop_icon_size + 0.5f)/(float)ICON_TEXTURE_SIZE;
                            verts[i].Diffuse = (p->selected) ? m_desktop_sel_color : 0xFFFFFFFF;
                            if (pass>0)
                            {
                                verts[i].x += m_desktop_drag_curpos.x - m_desktop_drag_startpos.x;
                                verts[i].y -= m_desktop_drag_curpos.y - m_desktop_drag_startpos.y;
                                verts[i].Diffuse &= 0x7FFFFFFF;
                            }
                        }

                        m_lpDX->m_lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, sizeof(HELPVERTEX));
                    }
                }

                iTexNum++;
            }
        }
    }

    // finally, draw selection box (if user is dragging one)
    if (m_desktop_box)
    {
        m_lpDX->m_lpDevice->SetVertexShader( NULL );
        m_lpDX->m_lpDevice->SetFVF( SIMPLE_VERTEX_FORMAT );
        m_lpDX->m_lpDevice->SetTexture(0, NULL);
        SIMPLEVERTEX verts[5];        
        for (int i=0; i<4; i++)
        {
            verts[i].x = (i==1||i==2) ? (float)(-m_lpDX->m_client_width/2 + m_desktop_drag_startpos.x) : (float)(-m_lpDX->m_client_width/2 + m_desktop_drag_curpos.x);
            verts[i].y = (i==0||i==1) ? (float)(m_lpDX->m_client_height/2 - m_desktop_drag_startpos.y) : (float)(m_lpDX->m_client_height/2 - m_desktop_drag_curpos.y);
            verts[i].z = 0;
            verts[i].Diffuse = 0x80FFFFFF;
        }
        verts[4] = verts[0];
        m_lpDX->m_lpDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, verts, sizeof(SIMPLEVERTEX));
    }

    m_lpDX->m_lpDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_lpDX->m_lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_lpDX->m_lpDevice->SetTexture(0, NULL);

    LeaveCriticalSection(&m_desktop_cs);
}

//----------------------------------------------------------------------

/* 
NOTES LOG

-problem: you can't click on the 1st monitor's desktop & do stuff
    -> fixed if you get rid of m_desktop_focuswnd and
        don't block WM_MOUSEACTIVATE.
    -> but doing this causes a flash when you click on 
        the real desktop (in multimon setup) of any windows
        that are overtop of the fake desktop, since the
        fake desktop rises up in the Z order (activates)
        and then, next frame, gets pushed back again.
    -> so how do we avoid the flash?
        by [conditionally] stopping any z-order changes;
        intercept & change WM_WINDOWPOSCHANGING messages
        so there's no z-change, unless *we* specifically
        requested it (in PushWindowToBack()).
    -> new problem: right-click context menu won't go away
        until you click something.  
    -> was fixed by making the plugin window a child 
        of m_hWndDesktopListView.
*/