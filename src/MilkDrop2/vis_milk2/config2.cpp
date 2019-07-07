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
#include <strsafe.h>

int g_nFontSize[] = { 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28, 30, 32,
                      36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112, 120, 128 };

int CALLBACK EnumFontsProc(
  CONST LOGFONT *lplf,     // logical-font data
  CONST TEXTMETRIC *lptm,  // physical-font data
  DWORD dwType,            // font type
  LPARAM lpData            // application-defined data
)
{
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT1), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT2), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT3), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT4), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT5), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT6), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT7), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT8), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	SendMessage( GetDlgItem( (HWND)lpData, IDC_FONT9), CB_ADDSTRING, 0, (LPARAM)(lplf->lfFaceName));
	return 1;
}

void SaveFont2(td_fontinfo *fi, DWORD ctrl1, DWORD ctrl2, DWORD bold_id, DWORD ital_id, DWORD aa_id, HWND hwnd)
{
	HWND fontbox  = GetDlgItem( hwnd, ctrl1 );
	HWND sizebox  = GetDlgItem( hwnd, ctrl2 );

	// font face
	int t = SendMessage( fontbox, CB_GETCURSEL, 0, 0);
	SendMessageW( fontbox, CB_GETLBTEXT, t, (LPARAM)fi->szFace);

	// font size
	t = SendMessage( sizebox, CB_GETCURSEL, 0, 0);
    if (t != CB_ERR)
    {
        int nMax = sizeof(g_nFontSize)/sizeof(int);
        fi->nSize =g_nFontSize[nMax-1 - t]; 
    }

	// font options
	fi->bBold = DlgItemIsChecked(hwnd, bold_id);
	fi->bItalic = DlgItemIsChecked(hwnd, ital_id);
	fi->bAntiAliased = DlgItemIsChecked(hwnd, aa_id);
}

void InitFont2(td_fontinfo *fi, DWORD ctrl1, DWORD ctrl2, DWORD bold_id, DWORD ital_id, DWORD aa_id, HWND hwnd, DWORD ctrl4, wchar_t* szFontName)
{
    HWND namebox  = ctrl4 ? GetDlgItem( hwnd, ctrl4 ) : 0;
	HWND fontbox  = GetDlgItem( hwnd, ctrl1 );
	HWND sizebox  = GetDlgItem( hwnd, ctrl2 );
    ShowWindow(fontbox, SW_NORMAL);
    ShowWindow(sizebox, SW_NORMAL);
    ShowWindow(GetDlgItem(hwnd,bold_id), SW_NORMAL);
    ShowWindow(GetDlgItem(hwnd,ital_id), SW_NORMAL);
    ShowWindow(GetDlgItem(hwnd,aa_id), SW_NORMAL);
    if (namebox && szFontName && szFontName[0])
    {
        ShowWindow(namebox, SW_NORMAL);
        wchar_t buf[256];
        StringCbPrintfW(buf, sizeof(buf), L"%s:", szFontName);
        SetWindowTextW(GetDlgItem(hwnd,ctrl4), buf);
    }

	// set selection
	int nPos = SendMessageW( fontbox, CB_FINDSTRINGEXACT, -1, (LPARAM)fi->szFace);
	if (nPos == CB_ERR)
        nPos = 0;
    SendMessage( fontbox, CB_SETCURSEL, nPos, 0);

	//---------font size box-------------------
    int nSel = 0;
    int nMax = sizeof(g_nFontSize)/sizeof(int);
    for (int i=0; i<nMax; i++)
    {
        wchar_t buf[256];
        int s = g_nFontSize[nMax-1 - i];
        StringCbPrintfW(buf, sizeof(buf), L" %2d ", s);
		SendMessageW(sizebox, CB_ADDSTRING, i, (LPARAM)buf);
        if (s == fi->nSize)
            nSel = i;
    }
	SendMessage(sizebox, CB_SETCURSEL, nSel, 0); 

	//---------font options box-------------------
    CheckDlgButton(hwnd, bold_id, fi->bBold);
    CheckDlgButton(hwnd, ital_id, fi->bItalic);
    CheckDlgButton(hwnd, aa_id, fi->bAntiAliased);
}

void SCOOT_CONTROL(HWND hwnd, int ctrl_id, int dx, int dy) 
{
    RECT r; 
    GetWindowRect(GetDlgItem(hwnd,ctrl_id), &r); 
    ScreenToClient(hwnd, (LPPOINT)&r); 
    SetWindowPos (GetDlgItem(hwnd,ctrl_id), NULL, r.left + dx, r.top + dy, 0, 0, SWP_NOSIZE|SWP_NOZORDER); 
}

BOOL CALLBACK CPluginShell::FontDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    if (msg==WM_INITDIALOG && lParam > 0 && GetWindowLongPtr(hwnd,GWLP_USERDATA)==0)
        SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

    CPluginShell* p = (CPluginShell*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    if (p)
        return p->PluginShellFontDialogProc(hwnd, msg, wParam, lParam);
    else
        return FALSE;
}

BOOL CPluginShell::PluginShellFontDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    #ifdef _DEBUG
        OutputDebugMessage("FontDlgProc: ", hwnd, msg, wParam, lParam);
    #endif

    switch (msg)
    {
    case WM_DESTROY:
        return 0;

    case WM_INITDIALOG:
        { 
            // Initialize all font dialog global variables here:
            // ...

			HDC hdc = GetDC(hwnd);
			if (hdc)
			{
				EnumFonts(hdc, NULL, &EnumFontsProc, (LPARAM)hwnd);
				ReleaseDC(hwnd, hdc);
			}

            #define InitFont(n, m) InitFont2(&m_fontinfo[n-1], IDC_FONT##n, IDC_FONTSIZE##n, IDC_FONTBOLD##n, IDC_FONTITAL##n, IDC_FONTAA##n, hwnd, IDC_FONT_NAME_##n, m)
            InitFont(1, 0);
            InitFont(2, 0);
            InitFont(3, 0);
            InitFont(4, 0);
            #if (NUM_EXTRA_FONTS >= 1) 
                InitFont(5, WASABI_API_LNGSTRINGW(IDS_EXTRA_FONT_1_NAME)); 
            #endif
            #if (NUM_EXTRA_FONTS >= 2) 
                InitFont(6, WASABI_API_LNGSTRINGW(IDS_EXTRA_FONT_2_NAME)); 
            #endif
            #if (NUM_EXTRA_FONTS >= 3) 
                InitFont(7, EXTRA_FONT_3_NAME); 
            #endif
            #if (NUM_EXTRA_FONTS >= 4) 
                InitFont(5, EXTRA_FONT_4_NAME); 
            #endif
            #if (NUM_EXTRA_FONTS >= 5) 
                InitFont(9, EXTRA_FONT_5_NAME); 
            #endif

            // Finally, if not all extra fonts are in use, shrink the window size, and
            // move up any controls that were at the bottom:
            RECT r;
            GetWindowRect(hwnd, &r);
            int scoot_factor = 128*(MAX_EXTRA_FONTS-NUM_EXTRA_FONTS)/MAX_EXTRA_FONTS;
            if (scoot_factor>0)
            {
                SetWindowPos(hwnd, NULL, 0, 0, r.right-r.left, r.bottom-r.top - scoot_factor, SWP_NOMOVE|SWP_NOZORDER);
                SCOOT_CONTROL(hwnd, IDC_FONT_TEXT, 0, -scoot_factor);
                SCOOT_CONTROL(hwnd, IDOK, 0, -scoot_factor);
                SCOOT_CONTROL(hwnd, IDCANCEL, 0, -scoot_factor);
            }
        }
        break;

    case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			switch(id)
			{
			case IDOK:

                #define SaveFont(n) SaveFont2(&m_fontinfo[n-1], IDC_FONT##n, IDC_FONTSIZE##n, IDC_FONTBOLD##n, IDC_FONTITAL##n, IDC_FONTAA##n, hwnd)
                SaveFont(1);
                SaveFont(2);
                SaveFont(3);
                SaveFont(4);
                #if (NUM_EXTRA_FONTS >= 1) 
                    SaveFont(5); 
                #endif
                #if (NUM_EXTRA_FONTS >= 2) 
                    SaveFont(6); 
                #endif
                #if (NUM_EXTRA_FONTS >= 3) 
                    SaveFont(7); 
                #endif
                #if (NUM_EXTRA_FONTS >= 4) 
                    SaveFont(5); 
                #endif
                #if (NUM_EXTRA_FONTS >= 5) 
                    SaveFont(9); 
                #endif

				EndDialog(hwnd,id);
				break;

            case IDCANCEL:
				EndDialog(hwnd,id);
				break;
            }
        }
        break;

    } 
    
    return 0;
}

void EnableStuff(HWND hwnd, int bEnable)
{
    EnableWindow(GetDlgItem(hwnd, IDC_CB_BOX), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDC_CB_MANUAL_SCOOT), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDC_DM_ALPHA_FIX_CAPTION), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDC_DM_ALPHA_FIX), bEnable);
}

BOOL CALLBACK CPluginShell::DesktopOptionsDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    if (msg==WM_INITDIALOG && lParam > 0 && GetWindowLongPtr(hwnd,GWLP_USERDATA)==0)
        SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

    CPluginShell* p = (CPluginShell*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    if (p)
        return p->PluginShellDesktopOptionsDialogProc(hwnd, msg, wParam, lParam);
    else
        return FALSE;
}

BOOL CPluginShell::PluginShellDesktopOptionsDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    #ifdef _DEBUG
        OutputDebugMessage("DmDlgProc: ", hwnd, msg, wParam, lParam);
    #endif

    switch (msg)
    {
    case WM_DESTROY:
        return 0;

    case WM_INITDIALOG:
        { 
            CheckDlgButton(hwnd, IDC_CB_SHOW_ICONS, m_desktop_show_icons );
            CheckDlgButton(hwnd, IDC_CB_BOX, m_desktop_textlabel_boxes );
            CheckDlgButton(hwnd, IDC_CB_MANUAL_SCOOT, m_desktop_manual_icon_scoot);

            HWND ctrl = GetDlgItem(hwnd, IDC_DM_ALPHA_FIX);
            SendMessageW( ctrl, CB_ADDSTRING, 0, (LPARAM)WASABI_API_LNGSTRINGW(IDS_5_6_5_TEXTURE));
            SendMessageW( ctrl, CB_ADDSTRING, 1, (LPARAM)WASABI_API_LNGSTRINGW(IDS_5_5_5_TEXTURE));
            SendMessageW( ctrl, CB_ADDSTRING, 2, (LPARAM)WASABI_API_LNGSTRINGW(IDS_8_8_8_TEXTURE));
            SendMessageW( ctrl, CB_SETCURSEL, m_desktop_555_fix, 0 );

            EnableStuff(hwnd, m_desktop_show_icons);
        }
        break;

    case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			switch(id)
			{
            case IDC_CB_SHOW_ICONS:
                m_desktop_show_icons = DlgItemIsChecked(hwnd, IDC_CB_SHOW_ICONS);
                EnableStuff(hwnd, m_desktop_show_icons);
                break;

			case IDOK:
                m_desktop_show_icons = DlgItemIsChecked(hwnd, IDC_CB_SHOW_ICONS);
                m_desktop_textlabel_boxes = DlgItemIsChecked(hwnd, IDC_CB_BOX);
                m_desktop_manual_icon_scoot = DlgItemIsChecked(hwnd, IDC_CB_MANUAL_SCOOT);
                m_desktop_555_fix = SendMessage( GetDlgItem(hwnd, IDC_DM_ALPHA_FIX), CB_GETCURSEL, 0, 0 );

				EndDialog(hwnd,id);
				break;

            case IDCANCEL:
				EndDialog(hwnd,id);
				break;
            }
        }
        break;

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
						case IDC_DM_ALPHA_FIX:
						case IDC_DM_ALPHA_FIX_CAPTION:
							WASABI_API_LNGSTRINGW_BUF(IDS_NO_ALPHA_FALLBACK, title, 1024);
							WASABI_API_LNGSTRINGW_BUF(IDS_NO_ALPHA_FALLBACK_HELP, buf, 2048);
							break;

						case IDC_CB_SHOW_ICONS:
							StringCbPrintfW(title, sizeof(title), WASABI_API_LNGSTRINGW(IDS_HELP_ON_X_CHECKBOX), ctrl_name);
							WASABI_API_LNGSTRINGW_BUF(IDS_CB_SHOW_ICONS_HELP, buf, 2048);
							break;

						case IDC_CB_BOX:
							WASABI_API_LNGSTRINGW_BUF(IDS_CB_BOX, title, 1024);
							WASABI_API_LNGSTRINGW_BUF(IDS_CB_BOX_HELP, buf, 2048);
							break;

						case IDC_CB_MANUAL_SCOOT:
							WASABI_API_LNGSTRINGW_BUF(IDS_CB_MANUAL_SCOOT, title, 1024);
							WASABI_API_LNGSTRINGW_BUF(IDS_CB_MANUAL_SCOOT_HELP, buf, 2048);
							break;
						}

						if (buf[0])
							MessageBoxW(hwnd, buf, title, MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
				}
        break;
    } 
    return 0;
}

BOOL CALLBACK CPluginShell::DualheadDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    if (msg==WM_INITDIALOG && lParam > 0 && GetWindowLongPtr(hwnd,GWLP_USERDATA)==0)
        SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

    CPluginShell* p = (CPluginShell*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    if (p)
        return p->PluginShellDualheadDialogProc(hwnd, msg, wParam, lParam);
    else
        return FALSE;
}

BOOL CPluginShell::PluginShellDualheadDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    #ifdef _DEBUG
        OutputDebugMessage("DHDlgProc: ", hwnd, msg, wParam, lParam);
    #endif

    switch (msg)
    {
    case WM_DESTROY:
        return 0;

    case WM_INITDIALOG:
        { 
            HWND ctrl = GetDlgItem(hwnd, IDC_H_PICK);
            SendMessageW( ctrl, CB_ADDSTRING, 0, (LPARAM)WASABI_API_LNGSTRINGW(IDS_SPAN_BOTH_SCREENS));
            SendMessageW( ctrl, CB_ADDSTRING, 1, (LPARAM)WASABI_API_LNGSTRINGW(IDS_USE_LEFT_SCREEN_ONLY));
            SendMessageW( ctrl, CB_ADDSTRING, 2, (LPARAM)WASABI_API_LNGSTRINGW(IDS_USE_RIGHT_SCREEN_ONLY));
            SendMessage( ctrl, CB_SETCURSEL, m_dualhead_horz, 0 );

            ctrl = GetDlgItem(hwnd, IDC_V_PICK);
            SendMessageW( ctrl, CB_ADDSTRING, 0, (LPARAM)WASABI_API_LNGSTRINGW(IDS_SPAN_BOTH_SCREENS));
            SendMessageW( ctrl, CB_ADDSTRING, 1, (LPARAM)WASABI_API_LNGSTRINGW(IDS_USE_TOP_SCREEN_ONLY));
            SendMessageW( ctrl, CB_ADDSTRING, 2, (LPARAM)WASABI_API_LNGSTRINGW(IDS_USE_BOTTOM_SCREEN_ONLY));
            SendMessage( ctrl, CB_SETCURSEL, m_dualhead_vert, 0 );
        }
        break;

    case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			switch(id)
			{
			case IDOK:
                m_dualhead_horz = SendMessage( GetDlgItem(hwnd, IDC_H_PICK), CB_GETCURSEL, 0, 0 );
                m_dualhead_vert = SendMessage( GetDlgItem(hwnd, IDC_V_PICK), CB_GETCURSEL, 0, 0 );

				EndDialog(hwnd,id);
				break;

            case IDCANCEL:
				EndDialog(hwnd,id);
				break;
            }
        }
        break;
    } 
    return 0;
}