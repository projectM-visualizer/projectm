#pragma once

#ifndef WIN32
#error this file is only for win32
#endif


// this should be the *only* place windows.h gets included!
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifndef _WIN32_WCE
#include <io.h>
#endif

#if defined(_MSC_VER)	// msvc
# define WASABIDLLEXPORT __declspec(dllexport)
# if _MSC_VER >= 1100
#  define NOVTABLE __declspec(novtable)
# endif
#endif



#define OSPIPE HANDLE


typedef WCHAR OSFNCHAR;
typedef LPWSTR OSFNSTR;
typedef LPCWSTR OSFNCSTR;

