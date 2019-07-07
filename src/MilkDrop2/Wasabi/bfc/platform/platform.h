#pragma once

#include <bfc/platform/types.h>
#include <bfc/std_mkncc.h>  // for MKnCC

#ifdef WIN32
# include <bfc/platform/win32.h>

#define OSMODULEHANDLE  HINSTANCE
#define INVALIDOSMODULEHANDLE  ((OSMODULEHANDLE)0)
#define OSWINDOWHANDLE  HWND
#define INVALIDOSWINDOWHANDLE  ((OSWINDOWHANDLE)0)
#define OSICONHANDLE  HICON
#define INVALIDOSICONHANDLE  ((OSICONHANDLE)0)
#define OSCURSORHANDLE  HICON
#define INVALIDOSCURSORHANDLE  ((OSCURSORHANDLE)0)
#define OSTHREADHANDLE  HANDLE
#define INVALIDOSTHREADHANDLE ((OSTHREADHANDLE)0)
#define OSREGIONHANDLE  HRGN
#define INVALIDOSREGIONHANDLE  ((OSREGIONHANDLE)0)
typedef HMENU OSMENUHANDLE;

#define RGBA(r,g,b,a)          ((ARGB32)((uint8_t)(r) | ((uint8_t)(g) << 8) | ((uint8_t)(b) << 16) | ((uint8_t)(a) << 24)))

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#elif defined(LINUX)
# include <bfc/platform/linux.h>
#elif defined(__APPLE__)
#include <Carbon/Carbon.h>

typedef HIShapeRef OSREGIONHANDLE;
typedef int OSCURSOR; // TODO: find a good one for this
typedef int OSCURSORHANDLE; // TODO: find a good one for this
typedef HIWindowRef OSWINDOWHANDLE;
typedef void *OSMODULEHANDLE; // TODO:
typedef CGContextRef HDC; // TODO: find a better name
typedef MenuRef OSMENUHANDLE;
typedef CGImageRef OSICONHANDLE;

#ifdef __LITTLE_ENDIAN__
#define RGBA(r,g,b,a)          ((ARGB32)((uint8_t)(r) | ((uint8_t)(g) << 8) | ((uint8_t)(b) << 16) | ((uint8_t)(a) << 24)))
#elif defined(__BIG_ENDIAN__)
#define RGBA(r,g,b,a)          ((ARGB32)((uint8_t)(a) | ((uint8_t)(r) << 8) | ((uint8_t)(g) << 16) | ((uint8_t)(b) << 24)))
#else
#error endian preprocessor symbol not defined
#endif

#define RGB(r,g,b)          RGBA(r,g,b,0xFF)

static const HIWindowRef INVALIDOSWINDOWHANDLE = 0; // TODO: maybe there's an apple-defined name for this
#define INVALIDOSMODULEHANDLE 0
#define INVALIDOSCURSORHANDLE 0

typedef char OSFNCHAR;
typedef char *OSFNSTR;

typedef const char OSFNCCHAR;
typedef const char *OSFNCSTR;

#define FNT(x) x

typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;
}
RECT;
typedef RECT * LPRECT;

inline RECT RECTFromHIRect(const HIRect *r)
{
	RECT rect;
	rect.left = r->origin.x;
	rect.right = r->origin.x + r->size.width;
	rect.top = r->origin.y;
	rect.bottom = r->origin.y + r->size.height;
	return rect;
}

inline HIRect HIRectFromRECT(const RECT *r)
{
	HIRect rect;
	rect.origin.x = r->left;
	rect.origin.y = r->top;
	rect.size.width = r->right - r->left;
	rect.size.height = r->bottom - r->top;
	return rect;
}

typedef struct tagPOINT
{
	int x;
	int y;
}
POINT;
typedef struct tagPOINT * LPPOINT;

inline HIPoint HIPointFromPOINT(const POINT *pt)
{
	HIPoint p;
	p.x = pt->x;
	p.y = pt->y;
	return p;
}

inline int MulDiv(int a, int b, int c)
{
	int s;
	int v;

	s = 0;
	if (a < 0)
	{
		s = !s;
		a = -a;
	}
	if (b < 0)
	{
		s = !s;
		b = -b;
	}
	if (c < 0)
	{
		s = !s;
		c = -c;
	}
	double d;
	d = ((double)a * (double)b) / (double)c;
	if (d >= 4294967296.)
		return -1;
	v = d;
	if (s)
		v = -v;
	return v;
}

#else
#error port me
// Windows API dependant definitions for non-windows platforms

#define __cdecl
#define __stdcall
#define WINAPI
#define WINBASEAPI
#define WINUSERAPI
#define WINGDIAPI
#define WINOLEAPI
#define CALLBACK
#define FARPROC void *

#define FALSE  0
#define TRUE  1

#define ERROR  0

#define CONST const
#define VOID void

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef long LONG;
typedef int INT;
typedef int BOOL;
typedef short SHORT;
typedef void * PVOID;
typedef void * LPVOID;

typedef char CHAR;
typedef unsigned short WCHAR;
typedef char * LPSTR;
typedef WCHAR * LPWSTR;
typedef const char * LPCSTR;
typedef const WCHAR * LPCWSTR;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR LPCTSTR;
typedef char TCHAR;
typedef WCHAR OLECHAR;

typedef void * HANDLE;
typedef void * HWND;
typedef void * HDC;
typedef void * HFONT;
typedef void * HBITMAP;
typedef void * HINSTANCE;
typedef void * HICON;
typedef void * HRGN;
typedef void * HPEN;
typedef void * HBRUSH;
typedef void * HRSRC;
typedef void * HGLOBAL;
typedef void * HACCEL;
typedef void * HMODULE;
typedef void * HMENU;
typedef void * HGDIOBJ;

typedef void * ATOM;
typedef void * CRITICAL_SECTION;
typedef void * LPCRITICAL_SECTION;

typedef UINT WPARAM;
typedef UINT LPARAM;
typedef LONG LRESULT;
typedef UINT COLORREF;

typedef LRESULT(*WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL CALLBACK WNDENUMPROC(HWND, LPARAM);
typedef VOID CALLBACK *TIMERPROC(HWND, UINT, UINT, DWORD);

typedef struct tagPOINT
{
	LONG x;
	LONG y;
}
POINT;
typedef struct tagPOINT * LPPOINT;

typedef struct tagSIZE
{
	LONG cx;
	LONG cy;
}
SIZE;


typedef struct tagRECT
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
}
RECT;
typedef RECT * LPRECT;

typedef struct _COORD
{
	SHORT X;
	SHORT Y;
}
COORD, *PCOORD;

typedef struct tagPAINTSTRUCT
{
	HDC hdc;
	BOOL fErase;
	RECT rcPaint;
	BOOL fRestore;
	BOOL fIncUpdate;
	BYTE rgbReserved[32];
}
PAINTSTRUCT;

typedef struct tagBITMAP
{  /* bm */
	int     bmType;
	int     bmWidth;
	int     bmHeight;
	int     bmWidthBytes;
	BYTE    bmPlanes;
	BYTE    bmBitsPixel;
	LPVOID  bmBits;
}
BITMAP;

typedef struct tagRGBQUAD
{
	BYTE rgbRed;
	BYTE rgbGreen;
	BYTE rgbBlue;
	BYTE rgbReserved;
}
RGBQUAD;

typedef struct tagBITMAPINFOHEADER
{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
}
BITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD     bmiColors[1];
}
BITMAPINFO, *LPBITMAPINFO;

typedef struct tagMSG
{
	HWND        hwnd;
	UINT        message;
	WPARAM      wParam;
	LPARAM      lParam;
	DWORD       time;
	POINT       pt;
}
MSG;

typedef MSG * LPMSG;

typedef struct _RGNDATAHEADER
{
	DWORD   dwSize;
	DWORD   iType;
	DWORD   nCount;
	DWORD   nRgnSize;
	RECT    rcBound;
}
RGNDATAHEADER, *PRGNDATAHEADER;

typedef struct _RGNDATA
{
	RGNDATAHEADER   rdh;
	char            Buffer[1];
}
RGNDATA, *PRGNDATA;

// Windows messages

#define WM_SYSCOMMAND 0x112
#define WM_LBUTTONDOWN 0x201
#define WM_LBUTTONUP 0x202
#define WM_RBUTTONDOWN 0x204
#define WM_RBUTTONUP 0x205

#define WM_USER 0x400

#define WS_EX_TOOLWINDOW 0x00000080L

#define WS_OVERLAPPED 0x00000000L
#define WS_MAXIMIZEBOX 0x00010000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_SYSMENU 0x00080000L
#define WS_CAPTION 0x00C00000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_VISIBLE 0x10000000L
#define WS_CHILD 0x40000000L
#define WS_POPUP 0x80000000L

#define HWND_TOP ((HWND)0)
#define HWND_TOPMOST ((HWND)-1)
#define HWND_NOTOPMOST ((HWND)-2)

#define GWL_STYLE  (-16)

#define GW_HWNDFIRST  0
#define GW_HWNDNEXT  2

#define SWP_NOMOVE 0x0002
#define SWP_NOSIZE 0x0001
#define SWP_SHOWWINDOW 0x0040
#define SWP_DEFERERASE 0x2000
#define SWP_NOZORDER 0x0004
#define SWP_NOACTIVATE 0x0010

#define SW_SHOW 5

#define SC_MINIMIZE 0xF020
#define SC_MAXIMIZE 0xF030
#define SC_RESTORE 0xF120

#define GCL_HICONSM (-34)
#define GCL_HICON (-14)

#define MB_OK    0
#define MB_OKCANCEL   1
#define MB_TASKMODAL  0x2000L

#define IDOK    1
#define IDCANCEL  2

#define VK_SHIFT   0x10
#define VK_CONTROL   0x11
#define VK_MENU    0x12

#define RT_RCDATA  10

#define IMAGE_BITMAP  0

#define LR_LOADFROMFILE  0x0010

#define DIB_RGB_COLORS  0

#define MAX_PATH  1024
#define _MAX_PATH  MAX_PATH
#define _MAX_DRIVE  3
#define _MAX_DIR  256
#define _MAX_FNAME  256
#define _MAX_EXT  256

#define GMEM_FIXED  0x0
#define GMEM_ZEROINIT  0x40
#define GPTR    (GMEM_FIXED | GMEM_ZEROINIT)

#define SPI_GETWORKAREA  48

#define SM_CXDOUBLECLK  36
#define SM_CYDOUBLECLK  37

#define COLORONCOLOR  3

#define SRCCOPY    (DWORD)0x00CC0020

#define BI_RGB    0L

#define NULLREGION  1

#define DT_LEFT    0x00000000
#define DT_CENTER  0x00000001
#define DT_RIGHT  0x00000002
#define DT_VCENTER  0x00000004
#define DT_WORDBREAK  0x00000010
#define DT_SINGLELINE  0x00000020
#define DT_CALCRECT  0x00000400
#define DT_NOPREFIX  0x00000800
#define DT_PATH_ELLIPSIS  0x00004000
#define DT_END_ELLIPSIS  0x00008000
#define DT_MODIFYSTRING  0x00010000

#define FW_NORMAL  400
#define FW_BOLD    700

#define FF_DONTCARE  (0<<4)

#define BLACK_BRUSH  4
#define NULL_BRUSH  5

#define PS_SOLID  0
#define PS_DOT    2

#define TRANSPARENT  1
#define OPAQUE    2

#define ANSI_CHARSET  0
#define ANSI_VAR_FONT  12

#define OUT_DEFAULT_PRECIS  0
#define CLIP_DEFAULT_PRECIS  0

#define PROOF_QUALITY  2

#define VARIABLE_PITCH  2

#define RGN_AND    1
#define RGN_OR    2
#define RGN_DIFF  4
#define RGN_COPY  5

#define RDH_RECTANGLES  1

#define MAXLONG    0x7fffffff

// define GUID
#include <bfc/platform/guid.h>

#endif /* not WIN32 */

#include <stdio.h>
#include <stdlib.h>
//#ifdef __cplusplus
//#include <new>
//#else
//#include <new.h>
//#endif
#include <limits.h>


#ifdef WIN32
#define OSPIPE HANDLE
#define OSPROCESSID int
#endif

// Ode macro keyworkds
#define DISPATCH_   // makes a method dispatchable, automatically assigns a free ID (requires Interface)
#define DISPATCH(x) // makes a method dispatchable and specifies its ID (not duplicate check, requires Interface)
#define NODISPATCH  // prevents a method from being dispatched if the class is marked for dispatching by default
#define EVENT       // marks a method as being an event to which other classes can connect to receive notification (used by Script and DependentItem helpers)
#define SCRIPT      // exposes a method to script interfaces (requires Script helper)
#define IN          // Input parameter
#define OUT         // Output parameter
#define INOUT       // Input/Output parameter

