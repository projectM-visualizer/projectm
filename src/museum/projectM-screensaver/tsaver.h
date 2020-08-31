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

#ifndef tsaver_h
#define tsaver_h

#define REGSTR_PATH_PLUSSCR (REGSTR_PATH_SETUP "\\Screen Savers")
#define REGSTR_PATH_CONFIG  ("Software\\projectM\\wprojectMsaver")

enum TScrMode { smNone, smConfig, smPassword, smPreview, smSaver};
TScrMode ScrMode=smNone;

class TSaverSettings
{
public:
	HWND hwnd;
	DWORD PasswordDelay;   // in seconds
	DWORD MouseThreshold;  // in pixels
	BOOL  MuteSound;
	POINT InitCursorPos;
	DWORD InitTime;        // in ms
	UINT  idTimer;         // a timer id, because this particular saver uses a timer
	BOOL  IsDialogActive;
	BOOL  ReallyClose;     // for NT, so we know if a WM_CLOSE came from us or it.
	BOOL  Rotate;     // this is a user-configuration option, particular to this example saver
	TSaverSettings();
	void ReadGeneralRegistry();
	void ReadConfigRegistry();
	void WriteConfigRegistry();
	void CloseSaverWindow();
	void StartDialog();
	void EndDialog();
};

TSaverSettings::TSaverSettings()
{
	hwnd=NULL;
	ReallyClose = false;
	idTimer=0;
}

void TSaverSettings::ReadGeneralRegistry()
{
	PasswordDelay = 15;
	MouseThreshold = 50;
	IsDialogActive = false; // default values in case they're not in registry
	LONG res;
	HKEY skey;
	DWORD valtype, valsize, val;
	res = RegOpenKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_PLUSSCR,0,KEY_ALL_ACCESS,&skey);
	if( res!=ERROR_SUCCESS )
		return;
	
	valsize = sizeof(val);
	res = RegQueryValueEx(skey,"Password Delay",0,&valtype,(LPBYTE)&val,&valsize);
	if( res==ERROR_SUCCESS )
		PasswordDelay=val;
	
	valsize = sizeof(val);
	res = RegQueryValueEx(skey,"Mouse Threshold",0,&valtype,(LPBYTE)&val,&valsize);
	if( res==ERROR_SUCCESS )
		MouseThreshold=val;
	
	valsize = sizeof(val);
	res = RegQueryValueEx(skey,"Mute Sound",0,&valtype,(LPBYTE)&val,&valsize);
	if( res==ERROR_SUCCESS )
		MuteSound=val;
	
	RegCloseKey(skey);
}

void TSaverSettings::ReadConfigRegistry()
{
	Rotate=true;
	LONG res;
	HKEY skey;
	DWORD valtype, valsize, val;
	res = RegOpenKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_CONFIG,0,KEY_ALL_ACCESS,&skey);
	if( res!=ERROR_SUCCESS )
		return;
	
	valsize = sizeof(val);
	res = RegQueryValueEx(skey,"Rotate",0,&valtype,(LPBYTE)&val,&valsize);
	
	if( res==ERROR_SUCCESS )
		Rotate=val;
	
	RegCloseKey(skey);
}  

void TSaverSettings::WriteConfigRegistry()
{
	LONG res;
	HKEY skey;
	DWORD val, disp;
	res = RegCreateKeyEx(HKEY_CURRENT_USER,REGSTR_PATH_CONFIG,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
	if( res!=ERROR_SUCCESS )
		return;

	val = Rotate;
	RegSetValueEx(skey,"Rotate",0,REG_DWORD,(CONST BYTE*)&val,sizeof(val));
	RegCloseKey(skey);
}

void TSaverSettings::CloseSaverWindow()
{
	ReallyClose = true;
	PostMessage(hwnd,WM_CLOSE,0,0);
}

void TSaverSettings::StartDialog()
{
	IsDialogActive = true;
	SendMessage(hwnd,WM_SETCURSOR,0,0);
}

void TSaverSettings::EndDialog()
{
	IsDialogActive = false;
	SendMessage(hwnd,WM_SETCURSOR,0,0);
	GetCursorPos(&InitCursorPos);
}

#endif
