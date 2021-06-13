/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
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
 */
/**
 * $Id: wprojectMsaver.cpp,v 1.4 2004/11/15 15:09:44 cvs Exp $
 *
 * projectM-based screensaver for Windows
 *
 */

#include <windows.h>
#include <stdio.h>
#include <regstr.h>
#include <math.h>
#include "resource.h"
#include "tsaver.h"
#ifdef WIN32
extern "C" {
#include <projectM/projectM.hpp>
  }
#endif /** WIN32 */

#ifdef DEBUG
FILE *debugFile = NULL;
#endif

//#pragma warning(disable: 4800)
//#pragma warning(disable: 4305)
//#pragma warning(disable: 4244)

HINSTANCE hInstance=NULL;
HWND hScrWindow=NULL;

TSaverSettings *ss=NULL;

/** Name */
TCHAR szAppName[32];

/** Visualiser */
projectM_t *globalPM = NULL;

void initGL(int width, int height)
{
    projectM_resetGL( globalPM, width, height );

    switchPreset( RANDOM_NEXT, HARD_CUT );
}

void display() {

    int i;
    short pcm_data[2][512];

    /** Produce some fake PCM data to stuff into projectM */
    if ( globalPM->count % 5 == 0 ) {
        for ( i = 0 ; i < 512 ; i++ ) {
            pcm_data[0][i] = 0;
            pcm_data[1][i] = 0;
          }
      } else {
        for ( i = 0 ; i < 512 ; i++ ) {
            if ( i % 2 == 0 ) {
                pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i%14) ) );
                pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i/2%14) ) );
              } else {
                pcm_data[0][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i/2%14) ) );
                pcm_data[1][i] = (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i%14) ) );
              }
            if ( i % 2 == 1 ) {
                pcm_data[0][i] = -pcm_data[0][i];
                pcm_data[1][i] = -pcm_data[1][i];
              }
        }
    }

    /** Add the waveform data */
    addPCM16( pcm_data );

    /** Render the new frame */
    renderFrame( globalPM );
  }

BOOL VerifyPassword(HWND hwnd)
{
	// Under NT, we return true immediately. This lets the saver quit, and the system manages passwords.
	// Under '95, we call VerifyScreenSavePwd. This checks the appropriate registry key and, if necessary, pops up a verify dialog
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	
	GetVersionEx(&osv);

	if( osv.dwPlatformId==VER_PLATFORM_WIN32_NT )
		return true;

	HINSTANCE hpwdcpl = ::LoadLibrary("PASSWORD.CPL");
	if( hpwdcpl==NULL )
		return true;

	typedef BOOL (WINAPI *VERIFYSCREENSAVEPWD)(HWND hwnd);
	VERIFYSCREENSAVEPWD VerifyScreenSavePwd;
	VerifyScreenSavePwd = (VERIFYSCREENSAVEPWD)GetProcAddress(hpwdcpl,"VerifyScreenSavePwd");

	if( VerifyScreenSavePwd==NULL )
	{
		FreeLibrary(hpwdcpl);
		return true;
	}
	
	BOOL bres = VerifyScreenSavePwd(hwnd);
	FreeLibrary(hpwdcpl);
	return bres;
}

void ChangePassword(HWND hwnd)
{
	// This only ever gets called under '95, when started with the /a option.
	HINSTANCE hmpr = ::LoadLibrary("MPR.DLL");
	if( hmpr==NULL )
		return;
	
	typedef VOID (WINAPI *PWDCHANGEPASSWORD) (LPCSTR lpcRegkeyname,HWND hwnd,UINT uiReserved1,UINT uiReserved2);
	PWDCHANGEPASSWORD PwdChangePassword=(PWDCHANGEPASSWORD)::GetProcAddress(hmpr,"PwdChangePasswordA");
	if( PwdChangePassword==NULL )
	{
		FreeLibrary(hmpr);
		return;
	}
	
	PwdChangePassword("SCRSAVE",hwnd,0,0); FreeLibrary(hmpr);
}

  
LRESULT CALLBACK SaverWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	// If you have a problem that's really not going away, put a debug in here:
	// Debug(MessageName(msg));
	// This will make a log of every single message that gets sent to the window.
    static PAINTSTRUCT ps;
	switch (msg)
	{
	case WM_CREATE:
		{
			ss->hwnd=hwnd;
			GetCursorPos(&(ss->InitCursorPos));
			ss->InitTime=GetTickCount();
		} break;
    case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_NCACTIVATE:
		{
			if( ScrMode==smSaver && !ss->IsDialogActive && LOWORD(wParam)==WA_INACTIVE )
				ss->CloseSaverWindow();
		} break;
	case WM_SETCURSOR:
		{
			if( ScrMode==smSaver && !ss->IsDialogActive )
				SetCursor(NULL);
			else
				SetCursor(LoadCursor(NULL,IDC_ARROW));
		} break;
    case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYDOWN:
		{
#ifdef DEBUG
            fprintf( debugFile, "key: %d\n", (int)wParam );
            fflush( debugFile );
#endif
            if ( ScrMode == smSaver && !ss->IsDialogActive ) {
                if ( (int)wParam == 'R' ) {
                    switchPreset( RANDOM_NEXT, HARD_CUT );
                  } else {
                    if ( (int)wParam == 'N' ) {
                        switchPreset( ALPHA_NEXT, HARD_CUT );
                      } else {
                        if ( (int)wParam == 'P' ) {
                            switchPreset( ALPHA_PREVIOUS, HARD_CUT );
                          }
                      }
                  }
              } else {
				ss->CloseSaverWindow();
              }
		} break;
	case WM_MOUSEMOVE:
		{
			if( ScrMode==smSaver && !ss->IsDialogActive )
			{
				POINT pt;
				GetCursorPos(&pt);
				int dx = pt.x-ss->InitCursorPos.x;
				if( dx<0 )
					dx = -dx;
				int dy = pt.y-ss->InitCursorPos.y;
				if( dy<0 )
					dy = -dy;
				if( dx>(int)ss->MouseThreshold || dy>(int)ss->MouseThreshold )
					ss->CloseSaverWindow();
			}
		} break;
	case (WM_SYSCOMMAND):
		{
			if( ScrMode==smSaver )
			{
				if( wParam==SC_SCREENSAVE )
					return false;
				if( wParam==SC_CLOSE )
					return false;
			}
		} break;
	case (WM_CLOSE):
		{
			if( ScrMode==smSaver && ss->ReallyClose && !ss->IsDialogActive )
			{
				BOOL CanClose = true;
				if( GetTickCount()-ss->InitTime > 1000*ss->PasswordDelay )
				{
					ss->StartDialog();
					CanClose = VerifyPassword(hwnd);
					ss->EndDialog();
				}
				if( CanClose )
					DestroyWindow(hwnd);
			}
			if( ScrMode==smSaver )
				return false; // so that DefWindowProc doesn't get called, because it would just DestroyWindow
		} break;
	case (WM_DESTROY):
		{
			if( ss->idTimer!=0 )
				KillTimer(hwnd,ss->idTimer);
			ss->idTimer=0;
			PostQuitMessage(0);
		} break;
  }
  return DefWindowProc(hwnd,msg,wParam,lParam);
}

void DoSaver(HWND hparwnd, int nCmdShow)
{
	WNDCLASS wc;
	wc.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	= SaverWindowProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInstance;
	wc.hIcon		= NULL;
	wc.hCursor		= NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName	= NULL;
	wc.lpszClassName	= "OpenGL";

	if( !RegisterClass(&wc) ) {
		MessageBox(NULL, "RegisterClass() failed:  "
			"Cannot register window class.", "Error", MB_OK);
		return;
	}

	int cx, cy;
	if( ScrMode==smPreview )
	{
		RECT rc;
		GetWindowRect(hparwnd,&rc);
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
		hScrWindow =
			CreateWindow("OpenGL", "SaverWindow", WS_CHILD|WS_VISIBLE,0, 0, cx, cy, hparwnd, NULL, hInstance, NULL);
	}
	else
	{
		cx = GetSystemMetrics(SM_CXSCREEN);
		cy = GetSystemMetrics(SM_CYSCREEN);
		DWORD exstyle, style;

		exstyle = WS_EX_TOPMOST;
		style = WS_POPUP|WS_VISIBLE;

		hScrWindow = CreateWindow   ("OpenGL", "SaverWindow", style,0, 0, cx, cy, NULL, NULL, hInstance, NULL);

	}

	if( hScrWindow==NULL )
		return;

	UINT oldval;

	if( ScrMode==smSaver )
		SystemParametersInfo(SPI_SCREENSAVERRUNNING,1,&oldval,0);

	HDC hDC = ::GetDC(hScrWindow);

	PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32;
	
	int pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
		MessageBox(NULL, "ChoosePixelFormat() failed:  "
			"Cannot find a suitable pixel format.", "Error", MB_OK); 
		return;
    } 
	
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
		MessageBox(NULL, "SetPixelFormat() failed:  "
			"Cannot set format specified.", "Error", MB_OK);
		return;
    } 

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	HGLRC hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
	
	initGL(cx, cy);

	ShowWindow(hScrWindow, nCmdShow);
	
	MSG msg;

	bool done = false;
	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
				done=TRUE;							// If So done=TRUE
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
//			display(cx, cy );
            display();
			SwapBuffers(hDC);					// Swap Buffers (Double Buffering)

			//Sleep(10);
		}
	}

	wglMakeCurrent(NULL, NULL);
    ReleaseDC(hScrWindow, hDC);
    wglDeleteContext(hRC);

	if( ScrMode==smSaver )
		SystemParametersInfo(SPI_SCREENSAVERRUNNING,0,&oldval,0);
	return;
}

BOOL CALLBACK ConfigDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton(hwnd,IDC_FLASH,ss->Rotate);
			return true;
		}
    case WM_COMMAND:
		{
			int id=LOWORD(wParam);
			if( id==IDOK )
			{
				ss->Rotate = (IsDlgButtonChecked(hwnd,IDC_FLASH)==BST_CHECKED);
				ss->WriteConfigRegistry();
			}
			if( id==IDOK || id==IDCANCEL )
				EndDialog(hwnd,id);
		} break;
	}
	return false;
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE,LPSTR,int nCmdShow)
{
	hInstance = h;

    LoadString( hInstance, IDS_APPNAME, szAppName, 31 );

	char *c = GetCommandLine();
	if(*c == '\"' )
	{
		c++;
		while( *c!=0 && *c!='\"' )
			c++;
	}
	else
	{
		while( *c!=0 && *c!=' ' )
			c++;
	}
	
	if( *c!=0 )
		c++;
	
	while( *c==' ' )
		c++;

	HWND hwnd=NULL;
	if( *c==0 )
	{
		ScrMode = smConfig;
		hwnd=NULL;
	}
	else
	{
		if( *c=='-' || *c=='/' )
			c++;
		if( *c=='p' || *c=='P' || *c=='l' || *c=='L' )
		{
			c++;
			while( *c==' ' || *c==':' )
				c++;
			hwnd = (HWND)atoi(c);
			
			ScrMode = smPreview;
		}
		else if( *c=='s' || *c=='S' )
		{
			ScrMode=smSaver;
		}
		else if( *c=='c' || *c=='C' )
		{
			c++;
			while( *c==' ' || *c==':' )
				c++;
			if( *c==0 )
				hwnd = GetForegroundWindow();
			else
				hwnd = (HWND)atoi(c);
			ScrMode = smConfig;
		}
		else if( *c=='a' || *c=='A' )
		{
			c++;
			while( *c==' ' || *c==':' )
				c++;
			hwnd = (HWND)atoi(c);
			ScrMode = smPassword;
		}
	}

    /** Set the app name */
//    LoadString( hInstance, IDS_APPNAME, szAppName, 31 );

	// We create a global TSaverSettings here, for convenience. It will get used by the config dialog and
	// by the saver as it runs
	ss = new TSaverSettings();
	ss->ReadGeneralRegistry();
	ss->ReadConfigRegistry();

	if( ScrMode==smPassword )
		ChangePassword(hwnd);
	if( ScrMode==smConfig )
		DialogBox(hInstance,MAKEINTRESOURCE(DLG_CONFIG),hwnd,ConfigDialogProc);
	if( ScrMode == smSaver || ScrMode==smPreview ) {

#ifdef DEBUG
        debugFile = fopen( "c:\\wprojectMsaver.txt", "w" );
#endif

        /** Initialise projectM */
        globalPM = (projectM_t *)wipemalloc( sizeof( projectM_t ) );
        projectM_reset( globalPM );

#ifdef DEBUG
        fprintf( debugFile, "here1\n" );
        fflush( debugFile );
#endif

        globalPM->fullscreen = 0;
        globalPM->texsize = 512;

        globalPM->fontURL = (char *)malloc( sizeof( char ) * 512 );
        strcpy( globalPM->fontURL, "c:\\Program Files\\projectM\\fonts" );

        globalPM->presetURL = (char *)malloc( sizeof( char ) * 512 );
        strcpy( globalPM->presetURL, "c:\\Program Files\\projectM\\presets" );

        projectM_init( globalPM );

#ifdef DEBUG
        fprintf( debugFile, "here2\n" );
        fflush( debugFile );
#endif

		DoSaver(hwnd, nCmdShow);
      }
	delete ss;

	return 0;
}


