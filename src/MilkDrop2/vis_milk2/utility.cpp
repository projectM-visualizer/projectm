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
#include "utility.h"
#include <math.h>
#include <locale.h>
#include <windows.h>
#ifdef _DEBUG
    #define D3D_DEBUG_INFO  // declare this before including d3d9.h
#endif
#include <d3d9.h>
#include "../Winamp/wa_ipc.h"
#include "resource.h"
#include <shellapi.h>

intptr_t myOpenURL(HWND hwnd, wchar_t *loc)
{
	if (loc)
	{
		bool override=false;
		WASABI_API_SYSCB->syscb_issueCallback(SysCallback::BROWSER, BrowserCallback::ONOPENURL, reinterpret_cast<intptr_t>(loc), reinterpret_cast<intptr_t>(&override));
		if (!override)
			return (intptr_t)ShellExecuteW(hwnd, L"open", loc, NULL, NULL, SW_SHOWNORMAL);
		else
			return 33;
	}
	return 33;
}

float PowCosineInterp(float x, float pow)
{
    // input (x) & output should be in range 0..1.
    // pow > 0: tends to push things toward 0 and 1
    // pow < 0: tends to push things toward 0.5.

    if (x<0) 
        return 0;
    if (x>1)
        return 1;

    int bneg = (pow < 0) ? 1 : 0;
    if (bneg)
        pow = -pow;

    if (pow>1000) pow=1000;

    int its = (int)pow;
    for (int i=0; i<its; i++)
    {
        if (bneg)
            x = InvCosineInterp(x);
        else
            x = CosineInterp(x);
    }
    float x2 = (bneg) ? InvCosineInterp(x) : CosineInterp(x);
    float dx = pow - its;
    return ((1-dx)*x + (dx)*x2);
}

float AdjustRateToFPS(float per_frame_decay_rate_at_fps1, float fps1, float actual_fps)
{
    // returns the equivalent per-frame decay rate at actual_fps

    // basically, do all your testing at fps1 and get a good decay rate;
    // then, in the real application, adjust that rate by the actual fps each time you use it.
    
    float per_second_decay_rate_at_fps1 = powf(per_frame_decay_rate_at_fps1, fps1);
    float per_frame_decay_rate_at_fps2 = powf(per_second_decay_rate_at_fps1, 1.0f/actual_fps);

    return per_frame_decay_rate_at_fps2;
}

float GetPrivateProfileFloatW(wchar_t *szSectionName, wchar_t *szKeyName, float fDefault, wchar_t *szIniFile)
{
    wchar_t string[64];
    wchar_t szDefault[64];
    float ret = fDefault;

    _swprintf_l(szDefault, L"%f", g_use_C_locale, fDefault);

    if (GetPrivateProfileStringW(szSectionName, szKeyName, szDefault, string, 64, szIniFile) > 0)
    {
        _swscanf_l(string, L"%f", g_use_C_locale, &ret);
    }
    return ret;
}

bool WritePrivateProfileFloatW(float f, wchar_t *szKeyName, wchar_t *szIniFile, wchar_t *szSectionName)
{
    wchar_t szValue[32];
    _swprintf_l(szValue, L"%f", g_use_C_locale, f);
    return (WritePrivateProfileStringW(szSectionName, szKeyName, szValue, szIniFile) != 0);
}

bool WritePrivateProfileIntW(int d, wchar_t *szKeyName, wchar_t *szIniFile, wchar_t *szSectionName)
{
    wchar_t szValue[32];
    swprintf(szValue, L"%d", d);
    return (WritePrivateProfileStringW(szSectionName, szKeyName, szValue, szIniFile) != 0);
}

void SetScrollLock(int bNewState, bool bPreventHandling)
{
	if(bPreventHandling) return;

    if (bNewState != (GetKeyState(VK_SCROLL) & 1))
    {
        // Simulate a key press
        keybd_event( VK_SCROLL,
                      0x45,
                      KEYEVENTF_EXTENDEDKEY | 0,
                      0 );

        // Simulate a key release
        keybd_event( VK_SCROLL,
                      0x45,
                      KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
                      0);
    }
}

void RemoveExtension(wchar_t *str)
{
    wchar_t *p = wcsrchr(str, L'.');
    if (p) *p = 0;
}

static void ShiftDown(wchar_t *str)
{
	while (*str)
	{
		str[0] = str[1];
		str++;
	}
}

void RemoveSingleAmpersands(wchar_t *str)
{
	while (*str)
	{
		if (str[0] == L'&')
		{
			if (str[1] == L'&') // two in a row: replace with single ampersand, move on
				str++;

			ShiftDown(str);
		}
		else
			str = CharNextW(str);
	}
}

void TextToGuid(char *str, GUID *pGUID)
{
    if (!str) return;
    if (!pGUID) return;

    DWORD d[11];
    
    sscanf(str, "%X %X %X %X %X %X %X %X %X %X %X", 
        &d[0], &d[1], &d[2], &d[3], &d[4], &d[5], &d[6], &d[7], &d[8], &d[9], &d[10]);

    pGUID->Data1 = (DWORD)d[0];
    pGUID->Data2 = (WORD)d[1];
    pGUID->Data3 = (WORD)d[2];
    pGUID->Data4[0] = (BYTE)d[3];
    pGUID->Data4[1] = (BYTE)d[4];
    pGUID->Data4[2] = (BYTE)d[5];
    pGUID->Data4[3] = (BYTE)d[6];
    pGUID->Data4[4] = (BYTE)d[7];
    pGUID->Data4[5] = (BYTE)d[8];
    pGUID->Data4[6] = (BYTE)d[9];
    pGUID->Data4[7] = (BYTE)d[10];
}

void GuidToText(GUID *pGUID, char *str, int nStrLen)
{
    // note: nStrLen should be set to sizeof(str).
    if (!str) return;
    if (!nStrLen) return;
    str[0] = 0;
    if (!pGUID) return;
    
    DWORD d[11];
    d[0]  = (DWORD)pGUID->Data1;
    d[1]  = (DWORD)pGUID->Data2;
    d[2]  = (DWORD)pGUID->Data3;
    d[3]  = (DWORD)pGUID->Data4[0];
    d[4]  = (DWORD)pGUID->Data4[1];
    d[5]  = (DWORD)pGUID->Data4[2];
    d[6]  = (DWORD)pGUID->Data4[3];
    d[7]  = (DWORD)pGUID->Data4[4];
    d[8]  = (DWORD)pGUID->Data4[5];
    d[9]  = (DWORD)pGUID->Data4[6];
    d[10] = (DWORD)pGUID->Data4[7];

    sprintf(str, "%08X %04X %04X %02X %02X %02X %02X %02X %02X %02X %02X", 
        d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10]);
}

/*
int GetPentiumTimeRaw(unsigned __int64 *cpu_timestamp)
{
    // returns 0 on failure, 1 on success 
    // warning: watch out for wraparound!
    
    // note: it's probably better to use QueryPerformanceFrequency 
    // and QueryPerformanceCounter()!

    // get high-precision time:
    __try
    {
        unsigned __int64 *dest = (unsigned __int64 *)cpu_timestamp;
        __asm 
        {
            _emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
            _emit 0x31       //  available on Pentium I and later.
            mov esi, dest
            mov [esi  ], eax    // lower 32 bits of tsc
            mov [esi+4], edx    // upper 32 bits of tsc
        }
        return 1;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }

    return 0;
}
        
double GetPentiumTimeAsDouble(unsigned __int64 frequency)
{
    // returns < 0 on failure; otherwise, returns current cpu time, in seconds.
    // warning: watch out for wraparound!

    // note: it's probably better to use QueryPerformanceFrequency 
    // and QueryPerformanceCounter()!

    if (frequency==0)
        return -1.0;

    // get high-precision time:
    __try
    {
        unsigned __int64 high_perf_time;
        unsigned __int64 *dest = &high_perf_time;
        __asm 
        {
            _emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
            _emit 0x31       //  available on Pentium I and later.
            mov esi, dest
            mov [esi  ], eax    // lower 32 bits of tsc
            mov [esi+4], edx    // upper 32 bits of tsc
        }
        __int64 time_s     = (__int64)(high_perf_time / frequency);  // unsigned->sign conversion should be safe here
        __int64 time_fract = (__int64)(high_perf_time % frequency);  // unsigned->sign conversion should be safe here
        // note: here, we wrap the timer more frequently (once per week) 
        // than it otherwise would (VERY RARELY - once every 585 years on
        // a 1 GHz), to alleviate floating-point precision errors that start 
        // to occur when you get to very high counter values.  
        double ret = (time_s % (60*60*24*7)) + (double)time_fract/(double)((__int64)frequency);
        return ret;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return -1.0;
    }

    return -1.0;
}
*/

#ifdef _DEBUG
    void OutputDebugMessage(char *szStartText, HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
    {
        // note: this function does NOT log WM_MOUSEMOVE, WM_NCHITTEST, or WM_SETCURSOR
        //        messages, since they are so frequent.
        // note: these identifiers were pulled from winuser.h

        //if (msg == WM_MOUSEMOVE || msg == WM_NCHITTEST || msg == WM_SETCURSOR)
        //    return;

        #ifdef _DEBUG
            char buf[64];
            int matched = 1;

            sprintf(buf, "WM_");
    
            switch(msg)
            {
            case 0x0001: lstrcat(buf, "CREATE"); break;                      
            case 0x0002: lstrcat(buf, "DESTROY"); break;
            case 0x0003: lstrcat(buf, "MOVE"); break;
            case 0x0005: lstrcat(buf, "SIZE"); break;
            case 0x0006: lstrcat(buf, "ACTIVATE"); break;
            case 0x0007: lstrcat(buf, "SETFOCUS"); break;
            case 0x0008: lstrcat(buf, "KILLFOCUS"); break;
            case 0x000A: lstrcat(buf, "ENABLE"); break;
            case 0x000B: lstrcat(buf, "SETREDRAW"); break;
            case 0x000C: lstrcat(buf, "SETTEXT"); break;
            case 0x000D: lstrcat(buf, "GETTEXT"); break;
            case 0x000E: lstrcat(buf, "GETTEXTLENGTH"); break;
            case 0x000F: lstrcat(buf, "PAINT"); break;
            case 0x0010: lstrcat(buf, "CLOSE"); break;
            case 0x0011: lstrcat(buf, "QUERYENDSESSION"); break;
            case 0x0012: lstrcat(buf, "QUIT"); break;
            case 0x0013: lstrcat(buf, "QUERYOPEN"); break;
            case 0x0014: lstrcat(buf, "ERASEBKGND"); break;
            case 0x0015: lstrcat(buf, "SYSCOLORCHANGE"); break;
            case 0x0016: lstrcat(buf, "ENDSESSION"); break;
            case 0x0018: lstrcat(buf, "SHOWWINDOW"); break;
            case 0x001A: lstrcat(buf, "WININICHANGE"); break;
            case 0x001B: lstrcat(buf, "DEVMODECHANGE"); break;
            case 0x001C: lstrcat(buf, "ACTIVATEAPP"); break;
            case 0x001D: lstrcat(buf, "FONTCHANGE"); break;
            case 0x001E: lstrcat(buf, "TIMECHANGE"); break;
            case 0x001F: lstrcat(buf, "CANCELMODE"); break;
            case 0x0020: lstrcat(buf, "SETCURSOR"); break;
            case 0x0021: lstrcat(buf, "MOUSEACTIVATE"); break;
            case 0x0022: lstrcat(buf, "CHILDACTIVATE"); break;
            case 0x0023: lstrcat(buf, "QUEUESYNC"); break;
            case 0x0024: lstrcat(buf, "GETMINMAXINFO"); break;
            case 0x0026: lstrcat(buf, "PAINTICON"); break;
            case 0x0027: lstrcat(buf, "ICONERASEBKGND"); break;
            case 0x0028: lstrcat(buf, "NEXTDLGCTL"); break;
            case 0x002A: lstrcat(buf, "SPOOLERSTATUS"); break;
            case 0x002B: lstrcat(buf, "DRAWITEM"); break;
            case 0x002C: lstrcat(buf, "MEASUREITEM"); break;
            case 0x002D: lstrcat(buf, "DELETEITEM"); break;
            case 0x002E: lstrcat(buf, "VKEYTOITEM"); break;
            case 0x002F: lstrcat(buf, "CHARTOITEM"); break;
            case 0x0030: lstrcat(buf, "SETFONT"); break;
            case 0x0031: lstrcat(buf, "GETFONT"); break;
            case 0x0032: lstrcat(buf, "SETHOTKEY"); break;
            case 0x0033: lstrcat(buf, "GETHOTKEY"); break;
            case 0x0037: lstrcat(buf, "QUERYDRAGICON"); break;
            case 0x0039: lstrcat(buf, "COMPAREITEM"); break;
            case 0x0041: lstrcat(buf, "COMPACTING"); break;
            case 0x0044: lstrcat(buf, "COMMNOTIFY"); break;
            case 0x0046: lstrcat(buf, "WINDOWPOSCHANGING"); break;
            case 0x0047: lstrcat(buf, "WINDOWPOSCHANGED"); break;
            case 0x0048: lstrcat(buf, "POWER"); break;
            case 0x004A: lstrcat(buf, "COPYDATA"); break;
            case 0x004B: lstrcat(buf, "CANCELJOURNAL"); break;

            #if(WINVER >= 0x0400)
            case 0x004E: lstrcat(buf, "NOTIFY"); break;
            case 0x0050: lstrcat(buf, "INPUTLANGCHANGEREQUEST"); break;
            case 0x0051: lstrcat(buf, "INPUTLANGCHANGE"); break;
            case 0x0052: lstrcat(buf, "TCARD"); break;
            case 0x0053: lstrcat(buf, "HELP"); break;
            case 0x0054: lstrcat(buf, "USERCHANGED"); break;
            case 0x0055: lstrcat(buf, "NOTIFYFORMAT"); break;
            case 0x007B: lstrcat(buf, "CONTEXTMENU"); break;
            case 0x007C: lstrcat(buf, "STYLECHANGING"); break;
            case 0x007D: lstrcat(buf, "STYLECHANGED"); break;
            case 0x007E: lstrcat(buf, "DISPLAYCHANGE"); break;
            case 0x007F: lstrcat(buf, "GETICON"); break;
            case 0x0080: lstrcat(buf, "SETICON"); break;
            #endif 

            case 0x0081: lstrcat(buf, "NCCREATE"); break;
            case 0x0082: lstrcat(buf, "NCDESTROY"); break;
            case 0x0083: lstrcat(buf, "NCCALCSIZE"); break;
            case 0x0084: lstrcat(buf, "NCHITTEST"); break;
            case 0x0085: lstrcat(buf, "NCPAINT"); break;
            case 0x0086: lstrcat(buf, "NCACTIVATE"); break;
            case 0x0087: lstrcat(buf, "GETDLGCODE"); break;
            case 0x0088: lstrcat(buf, "SYNCPAINT"); break;
            case 0x00A0: lstrcat(buf, "NCMOUSEMOVE"); break;
            case 0x00A1: lstrcat(buf, "NCLBUTTONDOWN"); break;
            case 0x00A2: lstrcat(buf, "NCLBUTTONUP"); break;
            case 0x00A3: lstrcat(buf, "NCLBUTTONDBLCLK"); break;
            case 0x00A4: lstrcat(buf, "NCRBUTTONDOWN"); break;
            case 0x00A5: lstrcat(buf, "NCRBUTTONUP"); break;
            case 0x00A6: lstrcat(buf, "NCRBUTTONDBLCLK"); break;
            case 0x00A7: lstrcat(buf, "NCMBUTTONDOWN"); break;
            case 0x00A8: lstrcat(buf, "NCMBUTTONUP"); break;
            case 0x00A9: lstrcat(buf, "NCMBUTTONDBLCLK"); break;
            case 0x0100: lstrcat(buf, "KEYDOWN"); break;
            case 0x0101: lstrcat(buf, "KEYUP"); break;
            case 0x0102: lstrcat(buf, "CHAR"); break;
            case 0x0103: lstrcat(buf, "DEADCHAR"); break;
            case 0x0104: lstrcat(buf, "SYSKEYDOWN"); break;
            case 0x0105: lstrcat(buf, "SYSKEYUP"); break;
            case 0x0106: lstrcat(buf, "SYSCHAR"); break;
            case 0x0107: lstrcat(buf, "SYSDEADCHAR"); break;
            case 0x0108: lstrcat(buf, "KEYLAST"); break;

            #if(WINVER >= 0x0400)
            case 0x010D: lstrcat(buf, "IME_STARTCOMPOSITION"); break;
            case 0x010E: lstrcat(buf, "IME_ENDCOMPOSITION"); break;
            case 0x010F: lstrcat(buf, "IME_COMPOSITION"); break;
            //case 0x010F: lstrcat(buf, "IME_KEYLAST"); break;
            #endif 

            case 0x0110: lstrcat(buf, "INITDIALOG"); break;
            case 0x0111: lstrcat(buf, "COMMAND"); break;
            case 0x0112: lstrcat(buf, "SYSCOMMAND"); break;
            case 0x0113: lstrcat(buf, "TIMER"); break;
            case 0x0114: lstrcat(buf, "HSCROLL"); break;
            case 0x0115: lstrcat(buf, "VSCROLL"); break;
            case 0x0116: lstrcat(buf, "INITMENU"); break;
            case 0x0117: lstrcat(buf, "INITMENUPOPUP"); break;
            case 0x011F: lstrcat(buf, "MENUSELECT"); break;
            case 0x0120: lstrcat(buf, "MENUCHAR"); break;
            case 0x0121: lstrcat(buf, "ENTERIDLE"); break;
            #if(WINVER >= 0x0500)
            case 0x0122: lstrcat(buf, "MENURBUTTONUP"); break;
            case 0x0123: lstrcat(buf, "MENUDRAG"); break;
            case 0x0124: lstrcat(buf, "MENUGETOBJECT"); break;
            case 0x0125: lstrcat(buf, "UNINITMENUPOPUP"); break;
            case 0x0126: lstrcat(buf, "MENUCOMMAND"); break;
            #endif 

            case 0x0132: lstrcat(buf, "CTLCOLORMSGBOX"); break;
            case 0x0133: lstrcat(buf, "CTLCOLOREDIT"); break;
            case 0x0134: lstrcat(buf, "CTLCOLORLISTBOX"); break;
            case 0x0135: lstrcat(buf, "CTLCOLORBTN"); break;
            case 0x0136: lstrcat(buf, "CTLCOLORDLG"); break;
            case 0x0137: lstrcat(buf, "CTLCOLORSCROLLBAR"); break;
            case 0x0138: lstrcat(buf, "CTLCOLORSTATIC"); break;

            //case 0x0200: lstrcat(buf, "MOUSEFIRST"); break;
            case 0x0200: lstrcat(buf, "MOUSEMOVE"); break;
            case 0x0201: lstrcat(buf, "LBUTTONDOWN"); break;
            case 0x0202: lstrcat(buf, "LBUTTONUP"); break;
            case 0x0203: lstrcat(buf, "LBUTTONDBLCLK"); break;
            case 0x0204: lstrcat(buf, "RBUTTONDOWN"); break;
            case 0x0205: lstrcat(buf, "RBUTTONUP"); break;
            case 0x0206: lstrcat(buf, "RBUTTONDBLCLK"); break;
            case 0x0207: lstrcat(buf, "MBUTTONDOWN"); break;
            case 0x0208: lstrcat(buf, "MBUTTONUP"); break;
            case 0x0209: lstrcat(buf, "MBUTTONDBLCLK"); break;

            #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
            case 0x020A: lstrcat(buf, "MOUSEWHEEL"); break;
            case 0x020E: lstrcat(buf, "MOUSELAST"); break;
            #else
            //case 0x0209: lstrcat(buf, "MOUSELAST"); break;
            #endif 

            case 0x0210: lstrcat(buf, "PARENTNOTIFY"); break;
            case 0x0211: lstrcat(buf, "ENTERMENULOOP"); break;
            case 0x0212: lstrcat(buf, "EXITMENULOOP"); break;

            #if(WINVER >= 0x0400)
            case 0x0213: lstrcat(buf, "NEXTMENU"); break;
            case 0x0214: lstrcat(buf, "SIZING"); break;
            case 0x0215: lstrcat(buf, "CAPTURECHANGED"); break;
            case 0x0216: lstrcat(buf, "MOVING"); break;
            case 0x0218: lstrcat(buf, "POWERBROADCAST"); break;
            case 0x0219: lstrcat(buf, "DEVICECHANGE"); break;
            #endif 

            /*
            case 0x0220: lstrcat(buf, "MDICREATE"); break;
            case 0x0221: lstrcat(buf, "MDIDESTROY"); break;
            case 0x0222: lstrcat(buf, "MDIACTIVATE"); break;
            case 0x0223: lstrcat(buf, "MDIRESTORE"); break;
            case 0x0224: lstrcat(buf, "MDINEXT"); break;
            case 0x0225: lstrcat(buf, "MDIMAXIMIZE"); break;
            case 0x0226: lstrcat(buf, "MDITILE"); break;
            case 0x0227: lstrcat(buf, "MDICASCADE"); break;
            case 0x0228: lstrcat(buf, "MDIICONARRANGE"); break;
            case 0x0229: lstrcat(buf, "MDIGETACTIVE"); break;
            */

            case 0x0230: lstrcat(buf, "MDISETMENU"); break;
            case 0x0231: lstrcat(buf, "ENTERSIZEMOVE"); break;
            case 0x0232: lstrcat(buf, "EXITSIZEMOVE"); break;
            case 0x0233: lstrcat(buf, "DROPFILES"); break;
            case 0x0234: lstrcat(buf, "MDIREFRESHMENU"); break;


            /*
            #if(WINVER >= 0x0400)
            case 0x0281: lstrcat(buf, "IME_SETCONTEXT"); break;
            case 0x0282: lstrcat(buf, "IME_NOTIFY"); break;
            case 0x0283: lstrcat(buf, "IME_CONTROL"); break;
            case 0x0284: lstrcat(buf, "IME_COMPOSITIONFULL"); break;
            case 0x0285: lstrcat(buf, "IME_SELECT"); break;
            case 0x0286: lstrcat(buf, "IME_CHAR"); break;
            #endif 
            #if(WINVER >= 0x0500)
            case 0x0288: lstrcat(buf, "IME_REQUEST"); break;
            #endif 
            #if(WINVER >= 0x0400)
            case 0x0290: lstrcat(buf, "IME_KEYDOWN"); break;
            case 0x0291: lstrcat(buf, "IME_KEYUP"); break;
            #endif 
            */

            #if(_WIN32_WINNT >= 0x0400)
            case 0x02A1: lstrcat(buf, "MOUSEHOVER"); break;
            case 0x02A3: lstrcat(buf, "MOUSELEAVE"); break;
            #endif 

            case 0x0300: lstrcat(buf, "CUT"); break;
            case 0x0301: lstrcat(buf, "COPY"); break;
            case 0x0302: lstrcat(buf, "PASTE"); break;
            case 0x0303: lstrcat(buf, "CLEAR"); break;
            case 0x0304: lstrcat(buf, "UNDO"); break;
            case 0x0305: lstrcat(buf, "RENDERFORMAT"); break;
            case 0x0306: lstrcat(buf, "RENDERALLFORMATS"); break;
            case 0x0307: lstrcat(buf, "DESTROYCLIPBOARD"); break;
            case 0x0308: lstrcat(buf, "DRAWCLIPBOARD"); break;
            case 0x0309: lstrcat(buf, "PAINTCLIPBOARD"); break;
            case 0x030A: lstrcat(buf, "VSCROLLCLIPBOARD"); break;
            case 0x030B: lstrcat(buf, "SIZECLIPBOARD"); break;
            case 0x030C: lstrcat(buf, "ASKCBFORMATNAME"); break;
            case 0x030D: lstrcat(buf, "CHANGECBCHAIN"); break;
            case 0x030E: lstrcat(buf, "HSCROLLCLIPBOARD"); break;
            case 0x030F: lstrcat(buf, "QUERYNEWPALETTE"); break;
            case 0x0310: lstrcat(buf, "PALETTEISCHANGING"); break;
            case 0x0311: lstrcat(buf, "PALETTECHANGED"); break;
            case 0x0312: lstrcat(buf, "HOTKEY"); break;

            #if(WINVER >= 0x0400)
            case 0x0317: lstrcat(buf, "PRINT"); break;
            case 0x0318: lstrcat(buf, "PRINTCLIENT"); break;

            case 0x0358: lstrcat(buf, "HANDHELDFIRST"); break;
            case 0x035F: lstrcat(buf, "HANDHELDLAST"); break;

            case 0x0360: lstrcat(buf, "AFXFIRST"); break;
            case 0x037F: lstrcat(buf, "AFXLAST"); break;
            #endif 

            case 0x0380: lstrcat(buf, "PENWINFIRST"); break;
            case 0x038F: lstrcat(buf, "PENWINLAST"); break;

            default: 
                sprintf(buf, "unknown"); 
                matched = 0;
                break;
            }

            int n = strlen(buf);
            int desired_len = 24;
            int spaces_to_append = desired_len-n;
            if (spaces_to_append>0)
            {
                for (int i=0; i<spaces_to_append; i++)
                    buf[n+i] = ' ';
                buf[desired_len] = 0;
            }      

            char buf2[256];
            if (matched)
                sprintf(buf2, "%shwnd=%08x, msg=%s, w=%08x, l=%08x\n", szStartText, hwnd, buf, wParam, lParam);
            else
                sprintf(buf2, "%shwnd=%08x, msg=unknown/0x%08x, w=%08x, l=%08x\n", szStartText, hwnd, msg, wParam, lParam);
            OutputDebugString(buf2);
        #endif
    }
#endif

void DownloadDirectX(HWND hwnd)
{
    wchar_t szUrl[] = L"http://www.microsoft.com/download/details.aspx?id=35";
    intptr_t ret = myOpenURL(NULL, szUrl);
    if (ret <= 32)
    {
        wchar_t buf[1024];
        switch(ret)
        {
        case SE_ERR_FNF:
        case SE_ERR_PNF:
            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_URL_COULD_NOT_OPEN), szUrl);
            break;
        case SE_ERR_ACCESSDENIED:
        case SE_ERR_SHARE:
            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_URL_WAS_DENIED), szUrl);
            break;
        case SE_ERR_NOASSOC:
            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_URL_FAILED_DUE_TO_NO_ASSOC), szUrl);
            break;
        default:
            swprintf(buf, WASABI_API_LNGSTRINGW(IDS_ACCESS_TO_URL_FAILED_CODE_X), szUrl, ret);
            break;
        }
        MessageBoxW(hwnd, buf, WASABI_API_LNGSTRINGW(IDS_ERROR_OPENING_URL),
					MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
    }
}

void MissingDirectX(HWND hwnd)
{
    // DIRECTX MISSING OR CORRUPT -> PROMPT TO GO TO WEB.
	wchar_t title[128];
    int ret = MessageBoxW(hwnd, 
        #ifndef D3D_SDK_VERSION
            --- error; you need to #include <d3d9.h> ---
        #endif
        #if (D3D_SDK_VERSION==120)
            // plugin was *built* using the DirectX 9.0 sdk, therefore, 
            // the dx9.0 runtime is missing or corrupt
            "Failed to initialize DirectX 9.0 or later.\n"
            "Milkdrop requires d3dx9_31.dll to be installed.\n"
            "\n"
            "Would you like to be taken to:\n"
			"http://www.microsoft.com/download/details.aspx?id=35,\n"
            "where you can update DirectX 9.0?\n"
            XXXXXXX
        #else
            // plugin was *built* using some other version of the DirectX9 sdk, such as 
            // 9.1b; therefore, we don't know exactly what version to tell them they need
            // to install; so we ask them to go get the *latest* version.
            WASABI_API_LNGSTRINGW(IDS_DIRECTX_MISSING_OR_CORRUPT_TEXT)
        #endif
        ,
		WASABI_API_LNGSTRINGW_BUF(IDS_DIRECTX_MISSING_OR_CORRUPT, title, 128),
		MB_YESNO|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL);
        
    if (ret==IDYES)
        DownloadDirectX(hwnd);
}

bool CheckForMMX()
{
    DWORD bMMX = 0;
    DWORD *pbMMX = &bMMX;
    __try {
        __asm {
            mov eax, 1
            cpuid
            mov edi, pbMMX
            mov dword ptr [edi], edx
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        bMMX = 0;
    }

    if (bMMX & 0x00800000)  // check bit 23
		return true;

	return false;
}

bool CheckForSSE()
{
#ifdef _WIN64
	return true; // All x64 processors support SSE
#else
    /*
    The SSE instruction set was introduced with the Pentium III and features:
        * Additional MMX instructions such as min/max
        * Prefetch and write-through instructions for optimizing data movement 
            from and to the L2/L3 caches and main memory
        * 8 New 128 bit XMM registers (xmm0..xmm7) and corresponding 32 bit floating point 
            (single precision) instructions
    */

	DWORD bSSE = 0;
	DWORD *pbSSE = &bSSE;
    __try {
	    __asm
	    {
		    mov eax, 1
		    cpuid
            mov edi, pbSSE
            mov dword ptr [edi], edx
	    }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        bSSE = 0;
    }

	if (bSSE & 0x02000000)  // check bit 25
		return true;

	return false;
#endif
}

void GetDesktopFolder(char *szDesktopFolder) // should be MAX_PATH len.
{
    // returns the path to the desktop folder, WITHOUT a trailing backslash.
    szDesktopFolder[0] = 0;
    ITEMIDLIST pidl;
    ZeroMemory(&pidl, sizeof(pidl));
    if (!SHGetPathFromIDList(&pidl, szDesktopFolder))
        szDesktopFolder[0] = 0;
}

void ExecutePidl(LPITEMIDLIST pidl, char *szPathAndFile, char *szWorkingDirectory, HWND hWnd)
{
    // This function was based on code by Jeff Prosise.

    // Note: for some reason, ShellExecuteEx fails when executing
    // *shortcuts* (.lnk files) from the desktop, using their PIDLs.  
    // So, if that fails, we try again w/the plain old text filename 
    // (szPathAndFile).

    char szVerb[] = "open";
    char szFilename2[MAX_PATH];

    sprintf(szFilename2, "%s.lnk", szPathAndFile);

    // -without the "no-verb" pass,
    //   certain icons still don't work (like shortcuts
    //   to IE, VTune...)
    // -without the "context menu" pass,
    //   certain others STILL don't work (Netscape...) 
    // -without the 'ntry' pass, shortcuts (to folders/files)
    //   don't work
    for (int verb_pass=0; verb_pass<2; verb_pass++)
    {
        for (int ntry=0; ntry<3; ntry++)
        {
            for (int context_pass=0; context_pass<2; context_pass++)
            {
                SHELLEXECUTEINFO sei = { sizeof(sei) };
                sei.hwnd = hWnd;
                sei.fMask = SEE_MASK_FLAG_NO_UI;
                if (context_pass==1)
                    sei.fMask |= SEE_MASK_INVOKEIDLIST;
                sei.lpVerb = (verb_pass) ? NULL : szVerb;
                sei.lpDirectory = szWorkingDirectory;
                sei.nShow = SW_SHOWNORMAL;
        
                if (ntry==0)
                {
                    // this case works for most non-shortcuts
                    sei.fMask |= SEE_MASK_IDLIST;
                    sei.lpIDList = pidl;
                }
                else if (ntry==1)
                {
                    // this case is required for *shortcuts to folders* to work
                    sei.lpFile = szPathAndFile;
                }
                else if (ntry==2)
                {
                    // this case is required for *shortcuts to files* to work
                    sei.lpFile = szFilename2;
                }

                if (ShellExecuteEx(&sei))
                    return;
            }
        }
    }
}

WNDPROC        g_pOldWndProc;
LPCONTEXTMENU2 g_pIContext2or3;

LRESULT CALLBACK HookWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
   //UINT uItem;
   //TCHAR szBuf[MAX_PATH];

   switch (msg) 
   { 
   case WM_DRAWITEM:
   case WM_MEASUREITEM:
      if(wp) break; // not menu related
   case WM_INITMENUPOPUP:
      g_pIContext2or3->HandleMenuMsg(msg, wp, lp);
      return (msg==WM_INITMENUPOPUP ? 0 : TRUE); // handled

   /*case WM_MENUSELECT:
      // if this is a shell item, get its descriptive text
      uItem = (UINT) LOWORD(wp);   
      if(0 == (MF_POPUP & HIWORD(wp)) && uItem >= 1 && uItem <= 0x7fff) 
      {
         g_pIContext2or3->GetCommandString(uItem-1, GCS_HELPTEXT,
            NULL, szBuf, sizeof(szBuf)/sizeof(szBuf[0]) );

         // set the status bar text
         ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->SetMessageText(szBuf);
         return 0;
      }
      break;*/

	default:
		break;
	}

	// for all untreated messages, call the original wndproc
	return ::CallWindowProc(g_pOldWndProc, hWnd, msg, wp, lp);
}

BOOL DoExplorerMenu (HWND hwnd, LPITEMIDLIST pidlMain, POINT point)
{
    LPMALLOC pMalloc;
    LPSHELLFOLDER psfFolder, psfNextFolder;
    LPITEMIDLIST pidlItem, pidlNextItem, *ppidl;
    LPCONTEXTMENU pContextMenu;
    CMINVOKECOMMANDINFO ici;
    UINT nCount, nCmd;
    BOOL bResult;
    HMENU hMenu;

    //
    // Get pointers to the shell's IMalloc interface and the desktop's
    // IShellFolder interface.
    //
    bResult = FALSE;

    if (!SUCCEEDED (SHGetMalloc (&pMalloc)))
		return bResult;

    if (!SUCCEEDED (SHGetDesktopFolder (&psfFolder))) {
        pMalloc->Release();
        return bResult;
    }

    if (nCount = GetItemCount (pidlMain)) // nCount must be > 0
    {
        //
        // Initialize psfFolder with a pointer to the IShellFolder
        // interface of the folder that contains the item whose context
        // menu we're after, and initialize pidlItem with a pointer to
        // the item's item ID. If nCount > 1, this requires us to walk
        // the list of item IDs stored in pidlMain and bind to each
        // subfolder referenced in the list.
        //
        pidlItem = pidlMain;

        while (--nCount) {
            //
            // Create a 1-item item ID list for the next item in pidlMain.
            //
            pidlNextItem = DuplicateItem (pMalloc, pidlItem);
            if (pidlNextItem == NULL) {
                psfFolder->Release();
                pMalloc->Release();
                return bResult;
            }

            //
            // Bind to the folder specified in the new item ID list.
            //
            if (!SUCCEEDED (psfFolder->BindToObject(pidlNextItem, NULL, IID_IShellFolder, (void**)&psfNextFolder)))  // modified by RG
            {
                pMalloc->Free(pidlNextItem);
                psfFolder->Release();
                pMalloc->Release();
                return bResult;
            }

            //
            // Release the IShellFolder pointer to the parent folder
            // and set psfFolder equal to the IShellFolder pointer for
            // the current folder.
            //
            psfFolder->Release();
            psfFolder = psfNextFolder;

            //
            // Release the storage for the 1-item item ID list we created
            // just a moment ago and initialize pidlItem so that it points
            // to the next item in pidlMain.
            //
            pMalloc->Free(pidlNextItem);
            pidlItem = GetNextItem (pidlItem);
        }

        //
        // Get a pointer to the item's IContextMenu interface and call
        // IContextMenu::QueryContextMenu to initialize a context menu.
        //
        ppidl = &pidlItem;
        if (SUCCEEDED (psfFolder->GetUIObjectOf(hwnd, 1, (LPCITEMIDLIST*)ppidl, IID_IContextMenu, NULL, (void**)&pContextMenu)))   // modified by RG
        {
            // try to see if we can upgrade to an IContextMenu3 
            // or IContextMenu2 interface pointer:
            int level = 1;
            void *pCM = NULL;
            if (pContextMenu->QueryInterface(IID_IContextMenu3, &pCM) == NOERROR)
            {
                pContextMenu->Release();
                pContextMenu = (LPCONTEXTMENU)pCM;
                level = 3;
            }
            else if (pContextMenu->QueryInterface(IID_IContextMenu2, &pCM) == NOERROR)
            {
                pContextMenu->Release();
                pContextMenu = (LPCONTEXTMENU)pCM;
                level = 2;
            }

            hMenu = CreatePopupMenu ();
            if (SUCCEEDED (pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_EXPLORE))) 
            {
                ClientToScreen (hwnd, &point);

                // install the subclassing "hook", for versions 2 or 3
                if (level >= 2) 
                {
                    g_pOldWndProc   = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (DWORD_PTR)HookWndProc);
                    g_pIContext2or3 = (LPCONTEXTMENU2)pContextMenu; // cast ok for ICMv3
                }
                else 
                {
                    g_pOldWndProc   = NULL;
                    g_pIContext2or3 = NULL;
                }

                //
                // Display the context menu.
                //
                nCmd = TrackPopupMenu (hMenu, TPM_LEFTALIGN |
                    TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                    point.x, point.y, 0, hwnd, NULL);

                // restore old wndProc
                if (g_pOldWndProc) 
                {
                    SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)g_pOldWndProc);
                }

                //
                // If a command was selected from the menu, execute it.
                //
                if (nCmd >= 1 && nCmd <= 0x7fff) 
                {
                    ZeroMemory(&ici, sizeof(ici));
                    ici.cbSize          = sizeof (CMINVOKECOMMANDINFO);
                    //ici.fMask           = 0;
                    ici.hwnd            = hwnd;
                    ici.lpVerb          = MAKEINTRESOURCE (nCmd - 1);
                    //ici.lpParameters    = NULL;
                    //ici.lpDirectory     = NULL;
                    ici.nShow           = SW_SHOWNORMAL;
                    //ici.dwHotKey        = 0;
                    //ici.hIcon           = NULL;

                    if (SUCCEEDED ( pContextMenu->InvokeCommand (&ici)))
                        bResult = TRUE;
                }
                /*else if (nCmd) 
                {
                    PostMessage(hwnd, WM_COMMAND, nCmd, NULL); // our command
                }*/
            }
            DestroyMenu (hMenu);
            pContextMenu->Release();
        }
    }

    //
    // Clean up and return.
    //
    psfFolder->Release();
    pMalloc->Release();

    return bResult;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Note: a special thanks goes out to Jeff Prosise for writing & publishing 
//        the following code!
//  
//  FUNCTION:       GetItemCount
//
//  DESCRIPTION:    Computes the number of item IDs in an item ID list.
//
//  INPUT:          pidl = Pointer to an item ID list.
//
//  RETURNS:        Number of item IDs in the list.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UINT GetItemCount (LPITEMIDLIST pidl)
{
    USHORT nLen;
    UINT nCount;

    nCount = 0;
    while ((nLen = pidl->mkid.cb) != 0) {
        pidl = GetNextItem (pidl);
        nCount++;
    }
    return nCount;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Note: a special thanks goes out to Jeff Prosise for writing & publishing 
//        the following code!
//  
//  FUNCTION:       GetNextItem
//
//  DESCRIPTION:    Finds the next item in an item ID list.
//
//  INPUT:          pidl = Pointer to an item ID list.
//
//  RETURNS:        Pointer to the next item.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LPITEMIDLIST GetNextItem (LPITEMIDLIST pidl)
{
    USHORT nLen;

    if ((nLen = pidl->mkid.cb) == 0)
        return NULL;
    
    return (LPITEMIDLIST) (((LPBYTE) pidl) + nLen);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Note: a special thanks goes out to Jeff Prosise for writing & publishing 
//        the following code!
//  
//  FUNCTION:       DuplicateItem
//
//  DESCRIPTION:    Makes a copy of the next item in an item ID list.
//
//  INPUT:          pMalloc = Pointer to an IMalloc interface.
//                  pidl    = Pointer to an item ID list.
//
//  RETURNS:        Pointer to an ITEMIDLIST containing the copied item ID.
//
//  NOTES:          It is the caller's responsibility to free the memory
//                  allocated by this function when the item ID is no longer
//                  needed. Example:
//
//                    pidlItem = DuplicateItem (pMalloc, pidl);
//                      .
//                      .
//                      .
//                    pMalloc->lpVtbl->Free (pMalloc, pidlItem);
//
//                  Failure to free the ITEMIDLIST will result in memory
//                  leaks.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LPITEMIDLIST DuplicateItem (LPMALLOC pMalloc, LPITEMIDLIST pidl)
{
    USHORT nLen;
    LPITEMIDLIST pidlNew;

    nLen = pidl->mkid.cb;
    if (nLen == 0)
        return NULL;

    pidlNew = (LPITEMIDLIST) pMalloc->Alloc (
        nLen + sizeof (USHORT));
    if (pidlNew == NULL)
        return NULL;

    CopyMemory (pidlNew, pidl, nLen);
    *((USHORT*) (((LPBYTE) pidlNew) + nLen)) = 0;

    return pidlNew;
}

//----------------------------------------------------------------------
// A special thanks goes out to Jeroen-bart Engelen (Yeep) for providing
// his source code for getting the position & label information for all 
// the icons on the desktop, as found below.  See his article at 
// http://www.digiwar.com/scripts/renderpage.php?section=2&subsection=2
//----------------------------------------------------------------------

void FindDesktopWindows(HWND *desktop_progman, HWND *desktopview_wnd, HWND *listview_wnd)
{
    *desktop_progman = NULL;
	*desktopview_wnd = NULL;
	*listview_wnd = NULL;

	*desktop_progman = FindWindow(NULL, ("Program Manager"));
	if(*desktop_progman == NULL)
	{
		//MessageBox(NULL, "Unable to get the handle to the Program Manager.", "Fatal error", MB_OK|MB_ICONERROR);
		return;
	}
	
	*desktopview_wnd = FindWindowEx(*desktop_progman, NULL, "SHELLDLL_DefView", NULL);
	if(*desktopview_wnd == NULL)
	{
		//MessageBox(NULL, "Unable to get the handle to the desktopview.", "Fatal error", MB_OK|MB_ICONERROR);
		return;
	}
	
	// Thanks ef_ef_ef@yahoo.com for pointing out this works in NT 4 and not the way I did it originally.
	*listview_wnd = FindWindowEx(*desktopview_wnd, NULL, "SysListView32", NULL);
	if(*listview_wnd == NULL)
	{
		//MessageBox(NULL, "Unable to get the handle to the folderview.", "Fatal error", MB_OK|MB_ICONERROR);
		return;
	}
}

//----------------------------------------------------------------------

int GetDesktopIconSize()
{
    int ret = 32;

    // reads the key: HKEY_CURRENT_USER\Control Panel, Desktop\WindowMetrics\Shell Icon Size
    unsigned char buf[64];
    unsigned long len = sizeof(buf);
    DWORD type;
    HKEY key;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", 0, KEY_READ, &key))
    {
        if (ERROR_SUCCESS == RegQueryValueEx(key, "Shell Icon Size", NULL, &type, (unsigned char*)buf, &len) &&
            type == REG_SZ)
        {
            int x = _atoi_l((char*)buf, g_use_C_locale);
            if (x>0 && x<=128)
                ret = x;
        }

        RegCloseKey(key);
    }

    return ret;
}

//----------------------------------------------------------------------

// handy functions for populating Combo Boxes:
int SelectItemByValue(HWND ctrl, DWORD value)
{
    int count = SendMessage(ctrl, CB_GETCOUNT, 0, 0);
	for (int i=0; i<count; i++)
	{
		DWORD value_i = SendMessage( ctrl, CB_GETITEMDATA, i, 0);
		if (value_i == value)
        {
			SendMessage( ctrl, CB_SETCURSEL, i, 0);
            return i;
        }
	}
    return -1;
}

bool ReadCBValue(HWND hwnd, DWORD ctrl_id, int* pRetValue)
{
    if (!pRetValue)
        return false;
    HWND ctrl = GetDlgItem( hwnd, ctrl_id );
	int t = SendMessage( ctrl, CB_GETCURSEL, 0, 0);
	if (t == CB_ERR) 
        return false;
    *pRetValue = (int)SendMessage( ctrl, CB_GETITEMDATA, t, 0);
    return true;
}

D3DXCREATEFONTW pCreateFontW=0;
D3DXMATRIXMULTIPLY pMatrixMultiply=0;
D3DXMATRIXTRANSLATION pMatrixTranslation=0;
D3DXMATRIXSCALING pMatrixScaling=0;
D3DXMATRIXROTATION pMatrixRotationX=0, pMatrixRotationY=0, pMatrixRotationZ=0;
D3DXCREATETEXTUREFROMFILEEXW pCreateTextureFromFileExW=0;
D3DXMATRIXORTHOLH pMatrixOrthoLH = 0;
D3DXCOMPILESHADER pCompileShader=0;
D3DXMATRIXLOOKATLH pMatrixLookAtLH=0;
D3DXCREATETEXTURE pCreateTexture=0;
//----------------------------------------------------------------------
HMODULE FindD3DX9(HWND winamp)
{
	HMODULE d3dx9 = (HMODULE)SendMessage(winamp,WM_WA_IPC, 0, IPC_GET_D3DX9);
	if (!d3dx9 || d3dx9 == (HMODULE)1)
	{
		
	// TODO: benski> this is a quick-fix, we should call FindFirstFile() on the system directory
	d3dx9=NULL;
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_36.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_35.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_34.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_33.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_32.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_31.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_30.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_29.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_28.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_27.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_26.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_25.dll");
	if (!d3dx9) d3dx9 = LoadLibrary("d3dx9_24.dll");
	}

	if (d3dx9)
	{
		pCreateFontW = (D3DXCREATEFONTW) GetProcAddress(d3dx9,"D3DXCreateFontW");
		pMatrixMultiply = (D3DXMATRIXMULTIPLY) GetProcAddress(d3dx9,"D3DXMatrixMultiply");
		pMatrixTranslation = (D3DXMATRIXTRANSLATION)GetProcAddress(d3dx9,"D3DXMatrixTranslation");
		pMatrixScaling = (D3DXMATRIXSCALING)GetProcAddress(d3dx9,"D3DXMatrixScaling");
		pMatrixRotationX = (D3DXMATRIXROTATION)GetProcAddress(d3dx9,"D3DXMatrixRotationX");
		pMatrixRotationY = (D3DXMATRIXROTATION)GetProcAddress(d3dx9,"D3DXMatrixRotationY");
		pMatrixRotationZ = (D3DXMATRIXROTATION)GetProcAddress(d3dx9,"D3DXMatrixRotationZ");
		pCreateTextureFromFileExW = (D3DXCREATETEXTUREFROMFILEEXW)GetProcAddress(d3dx9,"D3DXCreateTextureFromFileExW");
		pMatrixOrthoLH = (D3DXMATRIXORTHOLH)GetProcAddress(d3dx9,"D3DXMatrixOrthoLH");
		pCompileShader = (D3DXCOMPILESHADER)GetProcAddress(d3dx9,"D3DXCompileShader");
		pMatrixLookAtLH = (D3DXMATRIXLOOKATLH)GetProcAddress(d3dx9,"D3DXMatrixLookAtLH");
		pCreateTexture = (D3DXCREATETEXTURE)GetProcAddress(d3dx9,"D3DXCreateTexture");
		
		
		
	}
	
	return d3dx9;
}

LRESULT GetWinampVersion(HWND winamp)
{
	static LRESULT version=0;
	if (!version)
		version=SendMessage(winamp,WM_WA_IPC,0,0);
	return version;
}

void* GetTextResource(UINT id, int no_fallback){
	void* data = 0;
	HINSTANCE hinst = WASABI_API_LNG_HINST;
	HRSRC rsrc = FindResource(hinst,MAKEINTRESOURCE(id),"TEXT");
	if(!rsrc && !no_fallback) rsrc = FindResource((hinst = WASABI_API_ORIG_HINST),MAKEINTRESOURCE(id),"TEXT");
	if(rsrc){
	HGLOBAL resourceHandle = LoadResource(hinst,rsrc);
		data = LockResource(resourceHandle);
	}
	return data;
}