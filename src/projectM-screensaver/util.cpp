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

// MessageName: this function returns the text name of the message.
char* MessageName(UINT msg)
{
	switch (msg)
	{
	case WM_NULL: return "WM_NULL";
    case WM_CREATE: return "WM_CREATE";
    case WM_DESTROY: return "WM_DESTROY";
    case WM_MOVE: return "WM_MOVE";
    case WM_SIZE: return "WM_SIZE";
    case WM_ACTIVATE: return "WM_ACTIVATE";
    case WM_SETFOCUS: return "WM_SETFOCUS";
    case WM_KILLFOCUS: return "WM_KILLFOCUS";
    case WM_ENABLE: return "WM_ENABLE";
    case WM_SETREDRAW: return "WM_SETREDRAW";
    case WM_SETTEXT: return "WM_SETTEXT";
    case WM_GETTEXT: return "WM_GETTEXT";
    case WM_GETTEXTLENGTH: return "WM_GETTEXTLENGTH";
    case WM_PAINT: return "WM_PAINT";
    case WM_CLOSE: return "WM_CLOSE";
    case WM_QUERYENDSESSION: return "WM_QUERYENDSESSION";
    case WM_QUIT: return "WM_QUIT";
    case WM_QUERYOPEN: return "WM_QUERYOPEN";
    case WM_ERASEBKGND: return "WM_ERASEBKGND";
    case WM_SYSCOLORCHANGE: return "WM_SYSCOLORCHANGE";
    case WM_ENDSESSION: return "WM_ENDSESSION";
    case WM_SHOWWINDOW: return "WM_SHOWWINDOW";
    case WM_SETTINGCHANGE: return "WM_SETTINGCHANGE";
    case WM_DEVMODECHANGE: return "WM_DEVMODECHANGE";
    case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
    case WM_FONTCHANGE: return "WM_FONTCHANGE";
    case WM_TIMECHANGE: return "WM_TIMECHANGE";
    case WM_CANCELMODE: return "WM_CANCELMODE";
    case WM_SETCURSOR: return "WM_SETCURSOR";
    case WM_MOUSEACTIVATE: return "WM_MOUSEACTIVATE";
    case WM_CHILDACTIVATE: return "WM_CHILDACTIVATE";
    case WM_QUEUESYNC: return "WM_QUEUESYNC";
    case WM_GETMINMAXINFO: return "WM_GETMINMAXINFO";
    case WM_PAINTICON: return "WM_PAINTICON";
    case WM_ICONERASEBKGND: return "WM_ICONERASEBKGND";
    case WM_NEXTDLGCTL: return "WM_NEXTDLGCTL";
    case WM_SPOOLERSTATUS: return "WM_SPOOLERSTATUS";
    case WM_DRAWITEM: return "WM_DRAWITEM";
    case WM_MEASUREITEM: return "WM_MEASUREITEM";
    case WM_DELETEITEM: return "WM_DELETEITEM";
    case WM_VKEYTOITEM: return "WM_VKEYTOITEM";
    case WM_CHARTOITEM: return "WM_CHARTOITEM";
    case WM_SETFONT: return "WM_SETFONT";
    case WM_GETFONT: return "WM_GETFONT";
    case WM_SETHOTKEY: return "WM_SETHOTKEY";
    case WM_GETHOTKEY: return "WM_GETHOTKEY";
    case WM_QUERYDRAGICON: return "WM_QUERYDRAGICON";
    case WM_COMPAREITEM: return "WM_COMPAREITEM";
    case WM_COMPACTING: return "WM_COMPACTING";
    case WM_COMMNOTIFY: return "WM_COMMNOTIFY";
    case WM_WINDOWPOSCHANGING: return "WM_WINDOWPOSCHANGING";
    case WM_WINDOWPOSCHANGED: return "WM_WINDOWPOSCHANGED";
    case WM_POWER: return "WM_POWER";
    case WM_COPYDATA: return "WM_COPYDATA";
    case WM_CANCELJOURNAL: return "WM_CANCELJOURNAL";
    case WM_NOTIFY: return "WM_NOTIFY";
    case WM_INPUTLANGCHANGEREQUEST: return "WM_INPUTLANGCHANGEREQUEST";
    case WM_INPUTLANGCHANGE: return "WM_INPUTLANGCHANGE";
    case WM_TCARD: return "WM_TCARD";
    case WM_HELP: return "WM_HELP";
    case WM_USERCHANGED: return "WM_USERCHANGED";
    case WM_NOTIFYFORMAT: return "WM_NOTIFYFORMAT";
    case WM_CONTEXTMENU: return "WM_CONTEXTMENU";
    case WM_STYLECHANGING: return "WM_STYLECHANGING";
    case WM_STYLECHANGED: return "WM_STYLECHANGED";
    case WM_DISPLAYCHANGE: return "WM_DISPLAYCHANGE";
    case WM_GETICON: return "WM_GETICON";
    case WM_SETICON: return "WM_SETICON";
    case WM_NCCREATE: return "WM_NCCREATE";
    case WM_NCDESTROY: return "WM_NCDESTROY";
    case WM_NCCALCSIZE: return "WM_NCCALCSIZE";
    case WM_NCHITTEST: return "WM_NCHITTEST";
    case WM_NCPAINT: return "WM_NCPAINT";
    case WM_NCACTIVATE: return "WM_NCACTIVATE";
    case WM_GETDLGCODE: return "WM_GETDLGCODE";
    case WM_NCMOUSEMOVE: return "WM_NCMOUSEMOVE";
    case WM_NCLBUTTONDOWN: return "WM_NCLBUTTONDOWN";
    case WM_NCLBUTTONUP: return "WM_NCLBUTTONUP";
    case WM_NCLBUTTONDBLCLK: return "WM_NCLBUTTONDBLCLK";
    case WM_NCRBUTTONDOWN: return "WM_NCRBUTTONDOWN";
    case WM_NCRBUTTONUP: return "WM_NCRBUTTONUP";
    case WM_NCRBUTTONDBLCLK: return "WM_NCRBUTTONDBLCLK";
    case WM_NCMBUTTONDOWN: return "WM_NCMBUTTONDOWN";
    case WM_NCMBUTTONUP: return "WM_NCMBUTTONUP";
    case WM_NCMBUTTONDBLCLK: return "WM_NCMBUTTONDBLCLK";
    case WM_KEYDOWN: return "WM_KEYDOWN";
    case WM_KEYUP: return "WM_KEYUP";
    case WM_CHAR: return "WM_CHAR";
    case WM_DEADCHAR: return "WM_DEADCHAR";
    case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
    case WM_SYSKEYUP: return "WM_SYSKEYUP";
    case WM_SYSCHAR: return "WM_SYSCHAR";
    case WM_SYSDEADCHAR: return "WM_SYSDEADCHAR";
    case WM_IME_STARTCOMPOSITION: return "WM_IME_STARTCOMPOSITION";
    case WM_IME_ENDCOMPOSITION: return "WM_IME_ENDCOMPOSITION";
    case WM_IME_COMPOSITION: return "WM_IME_COMPOSITION";
    case WM_INITDIALOG: return "WM_INITDIALOG";
    case WM_COMMAND: return "WM_COMMAND";
    case WM_SYSCOMMAND: return "WM_SYSCOMMAND";
    case WM_TIMER: return "WM_TIMER";
    case WM_HSCROLL: return "WM_HSCROLL";
    case WM_VSCROLL: return "WM_VSCROLL";
    case WM_INITMENU: return "WM_INITMENU";
    case WM_INITMENUPOPUP: return "WM_INITMENUPOPUP";
    case WM_MENUSELECT: return "WM_MENUSELECT";
    case WM_MENUCHAR: return "WM_MENUCHAR";
    case WM_ENTERIDLE: return "WM_ENTERIDLE";
    case WM_CTLCOLORMSGBOX: return "WM_CTLCOLORMSGBOX";
    case WM_CTLCOLOREDIT: return "WM_CTLCOLOREDIT";
    case WM_CTLCOLORLISTBOX: return "WM_CTLCOLORLISTBOX";
    case WM_CTLCOLORBTN: return "WM_CTLCOLORBTN";
    case WM_CTLCOLORDLG: return "WM_CTLCOLORDLG";
    case WM_CTLCOLORSCROLLBAR: return "WM_CTLCOLORSCROLLBAR";
    case WM_CTLCOLORSTATIC: return "WM_CTLCOLORSTATIC";
    case WM_MOUSEMOVE: return "WM_MOUSEMOVE";
    case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
    case WM_LBUTTONUP: return "WM_LBUTTONUP";
    case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
    case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
    case WM_RBUTTONUP: return "WM_RBUTTONUP";
    case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
    case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
    case WM_MBUTTONUP: return "WM_MBUTTONUP";
    case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
		//case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
    case WM_PARENTNOTIFY: return "WM_PARENTNOTIFY";
    case WM_ENTERMENULOOP: return "WM_ENTERMENULOOP";
    case WM_EXITMENULOOP: return "WM_EXITMENULOOP";
    case WM_NEXTMENU: return "WM_NEXTMENU";
    case WM_SIZING: return "WM_SIZING";
    case WM_CAPTURECHANGED: return "WM_CAPTURECHANGED";
    case WM_MOVING: return "WM_MOVING";
    case WM_POWERBROADCAST: return "WM_POWERBROADCAST";
    case WM_DEVICECHANGE: return "WM_DEVICECHANGE";
    case WM_IME_SETCONTEXT: return "WM_IME_SETCONTEXT";
    case WM_IME_NOTIFY: return "WM_IME_NOTIFY";
    case WM_IME_CONTROL: return "WM_IME_CONTROL";
    case WM_IME_COMPOSITIONFULL: return "WM_IME_COMPOSITIONFULL";
    case WM_IME_SELECT: return "WM_IME_SELECT";
    case WM_IME_CHAR: return "WM_IME_CHAR";
    case WM_IME_KEYDOWN: return "WM_IME_KEYDOWN";
    case WM_IME_KEYUP: return "WM_IME_KEYUP";
    case WM_MDICREATE: return "WM_MDICREATE";
    case WM_MDIDESTROY: return "WM_MDIDESTROY";
    case WM_MDIACTIVATE: return "WM_MDIACTIVATE";
    case WM_MDIRESTORE: return "WM_MDIRESTORE";
    case WM_MDINEXT: return "WM_MDINEXT";
    case WM_MDIMAXIMIZE: return "WM_MDIMAXIMIZE";
    case WM_MDITILE: return "WM_MDITILE";
    case WM_MDICASCADE: return "WM_MDICASCADE";
    case WM_MDIICONARRANGE: return "WM_MDIICONARRANGE";
    case WM_MDIGETACTIVE: return "WM_MDIGETACTIVE";
    case WM_MDISETMENU: return "WM_MDISETMENU";
    case WM_ENTERSIZEMOVE: return "WM_ENTERSIZEMOVE";
    case WM_EXITSIZEMOVE: return "WM_EXITSIZEMOVE";
    case WM_DROPFILES: return "WM_DROPFILES";
    case WM_MDIREFRESHMENU: return "WM_MDIREFRESHMENU";
		//case WM_MOUSEHOVER: return "WM_MOUSEHOVER";
		//case WM_MOUSELEAVE: return "WM_MOUSELEAVE";
    case WM_CUT: return "WM_CUT";
    case WM_COPY: return "WM_COPY";
    case WM_PASTE: return "WM_PASTE";
    case WM_CLEAR: return "WM_CLEAR";
    case WM_UNDO: return "WM_UNDO";
    case WM_RENDERFORMAT: return "WM_RENDERFORMAT";
    case WM_RENDERALLFORMATS: return "WM_RENDERALLFORMATS";
    case WM_DESTROYCLIPBOARD: return "WM_DESTROYCLIPBOARD";
    case WM_DRAWCLIPBOARD: return "WM_DRAWCLIPBOARD";
    case WM_PAINTCLIPBOARD: return "WM_PAINTCLIPBOARD";
    case WM_VSCROLLCLIPBOARD: return "WM_VSCROLLCLIPBOARD";
    case WM_SIZECLIPBOARD: return "WM_SIZECLIPBOARD";
    case WM_ASKCBFORMATNAME: return "WM_ASKCBFORMATNAME";
    case WM_CHANGECBCHAIN: return "WM_CHANGECBCHAIN";
    case WM_HSCROLLCLIPBOARD: return "WM_HSCROLLCLIPBOARD";
    case WM_QUERYNEWPALETTE: return "WM_QUERYNEWPALETTE";
    case WM_PALETTEISCHANGING: return "WM_PALETTEISCHANGING";
    case WM_PALETTECHANGED: return "WM_PALETTECHANGED";
    case WM_HOTKEY: return "WM_HOTKEY";
    case WM_PRINT: return "WM_PRINT";
    case WM_PRINTCLIENT: return "WM_PRINTCLIENT";
    case WM_HANDHELDFIRST: return "WM_HANDHELDFIRST";
    case WM_HANDHELDLAST: return "WM_HANDHELDLAST";
    case WM_AFXFIRST: return "WM_AFXFIRST";
    case WM_AFXLAST: return "WM_AFXLAST";
    case WM_PENWINFIRST: return "WM_PENWINFIRST";
    case WM_PENWINLAST: return "WM_PENWINLAST";
#ifdef __VCL
    case CM_ACTIVATE: return "CM_ACTIVATE";
    case CM_DEACTIVATE: return "CM_DEACTIVATE";
    case CM_GOTFOCUS: return "CM_GOTFOCUS";
    case CM_LOSTFOCUS: return "CM_LOSTFOCUS";
    case CM_CANCELMODE: return "CM_CANCELMODE";
    case CM_DIALOGKEY: return "CM_DIALOGKEY";
    case CM_DIALOGCHAR: return "CM_DIALOGCHAR";
    case CM_FOCUSCHANGED: return "CM_FOCUSCHANGED";
    case CM_PARENTFONTCHANGED: return "CM_PARENTFONTCHANGED";
    case CM_PARENTCOLORCHANGED: return "CM_PARENTCOLORCHANGED";
    case CM_HITTEST: return "CM_HITTEST";
    case CM_VISIBLECHANGED: return "CM_VISIBLECHANGED";
    case CM_ENABLEDCHANGED: return "CM_ENABLEDCHANGED";
    case CM_COLORCHANGED: return "CM_COLORCHANGED";
    case CM_FONTCHANGED: return "CM_FONTCHANGED";
    case CM_CURSORCHANGED: return "CM_CURSORCHANGED";
    case CM_CTL3DCHANGED: return "CM_CTL3DCHANGED";
    case CM_PARENTCTL3DCHANGED: return "CM_PARENTCTL3DCHANGED";
    case CM_TEXTCHANGED: return "CM_TEXTCHANGED";
    case CM_MOUSEENTER: return "CM_MOUSEENTER";
    case CM_MOUSELEAVE: return "CM_MOUSELEAVE";
    case CM_MENUCHANGED: return "CM_MENUCHANGED";
    case CM_APPKEYDOWN: return "CM_APPKEYDOWN";
    case CM_APPSYSCOMMAND: return "CM_APPSYSCOMMAND";
    case CM_BUTTONPRESSED: return "CM_BUTTONPRESSED";
    case CM_SHOWINGCHANGED: return "CM_SHOWINGCHANGED";
    case CM_ENTER: return "CM_ENTER";
    case CM_EXIT: return "CM_EXIT";
    case CM_DESIGNHITTEST: return "CM_DESIGNHITTEST";
    case CM_ICONCHANGED: return "CM_ICONCHANGED";
    case CM_WANTSPECIALKEY: return "CM_WANTSPECIALKEY";
    case CM_INVOKEHELP: return "CM_INVOKEHELP";
    case CM_WINDOWHOOK: return "CM_WINDOWHOOK";
    case CM_RELEASE: return "CM_RELEASE";
    case CM_SHOWHINTCHANGED: return "CM_SHOWHINTCHANGED";
    case CM_PARENTSHOWHINTCHANGED: return "CM_PARENTSHOWHINTCHANGED";
    case CM_SYSCOLORCHANGE: return "CM_SYSCOLORCHANGE";
    case CM_WININICHANGE: return "CM_WININICHANGE";
    case CM_FONTCHANGE: return "CM_FONTCHANGE";
    case CM_TIMECHANGE: return "CM_TIMECHANGE";
    case CM_TABSTOPCHANGED: return "CM_TABSTOPCHANGED";
    case CM_UIACTIVATE: return "CM_UIACTIVATE";
    case CM_UIDEACTIVATE: return "CM_UIDEACTIVATE";
    case CM_DOCWINDOWACTIVATE: return "CM_DOCWINDOWACTIVATE";
    case CM_CONTROLLISTCHANGE: return "CM_CONTROLLISTCHANGE";
    case CM_GETDATALINK: return "CM_GETDATALINK";
    case CM_CHILDKEY: return "CM_CHILDKEY";
    case CM_DRAG: return "CM_DRAG";
    case CM_HINTSHOW: return "CM_HINTSHOW";
    case CM_DIALOGHANDLE: return "CM_DIALOGHANDLE";
    case CM_ISTOOLCONTROL: return "CM_ISTOOLCONTROL";
    case CM_RECREATEWND: return "CM_RECREATEWND";
    case CM_INVALIDATE: return "CM_INVALIDATE";
    case CM_SYSFONTCHANGED: return "CM_SYSFONTCHANGED";
    case CM_CONTROLCHANGE: return "CM_CONTROLCHANGE";
    case CM_CHANGED: return "CM_CHANGED";
    case CN_CHARTOITEM: return "CN_CHARTOITEM";
    case CN_COMMAND: return "CN_COMMAND";
    case CN_COMPAREITEM: return "CN_COMPAREITEM";
    case CN_CTLCOLORBTN: return "CN_CTLCOLORBTN";
    case CN_CTLCOLORDLG: return "CN_CTLCOLORDLG";
    case CN_CTLCOLOREDIT: return "CN_CTLCOLOREDIT";
    case CN_CTLCOLORLISTBOX: return "CN_CTLCOLORLISTBOX";
    case CN_CTLCOLORMSGBOX: return "CN_CTLCOLORMSGBOX";
    case CN_CTLCOLORSCROLLBAR: return "CN_CTLCOLORSCROLLBAR";
    case CN_CTLCOLORSTATIC: return "CN_CTLCOLORSTATIC";
    case CN_DELETEITEM: return "CN_DELETEITEM";
    case CN_DRAWITEM: return "CN_DRAWITEM";
    case CN_HSCROLL: return "CN_HSCROLL";
    case CN_MEASUREITEM: return "CN_MEASUREITEM";
    case CN_PARENTNOTIFY: return "CN_PARENTNOTIFY";
    case CN_VKEYTOITEM: return "CN_VKEYTOITEM";
    case CN_VSCROLL: return "CN_VSCROLL";
    case CN_KEYDOWN: return "CN_KEYDOWN";
    case CN_KEYUP: return "CN_KEYUP";
    case CN_CHAR: return "CN_CHAR";
    case CN_SYSKEYDOWN: return "CN_SYSKEYDOWN";
    case CN_SYSCHAR: return "CN_SYSCHAR";
    case CN_NOTIFY: return "CN_NOTIFY";
#endif
    default: return "WM_????";
  }
}


