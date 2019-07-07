#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <wchar.h>
// first, some standard int types
typedef unsigned int UINT;
typedef signed int SINT;

typedef unsigned char UCHAR;
typedef signed char SCHAR;

typedef unsigned long ARGB32;
typedef unsigned long RGB32;

typedef unsigned long ARGB24;
typedef unsigned long RGB24;

typedef unsigned short ARGB16;
typedef unsigned short RGB16;

typedef unsigned long FOURCC;

typedef wchar_t nsxml_char_t;
typedef wchar_t ns_char_t;
typedef wchar_t nsfilename_char_t;

typedef int socklen_t;

#if defined(_WIN32) && !defined(__GNUC__)
#include <stddef.h>
#if _MSC_VER >= 1600
#include <stdint.h>
#else
	// since windows doesn't have stdint.h
	typedef unsigned __int64 uint64_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int8 uint8_t;
	typedef __int64 int64_t;
	typedef __int32 int32_t;
	typedef __int16 int16_t;
	typedef __int8  int8_t;
#ifdef _M_IX86
	typedef int64_t intptr2_t;
#else if defined(_M_IX64)
	typedef unsigned __int128 uint128_t;
	typedef __int128 int128_t;
	typedef int128_t intptr2_t
#endif
#endif
#else
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#endif

	#ifndef GUID_DEFINED
  #define GUID_DEFINED

  typedef struct _GUID 
	{
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t Data4[8];
	} GUID;
/*
#ifndef _REFCLSID_DEFINED
#define REFGUID const GUID &
#define _REFCLSID_DEFINED
#endif
*/
#endif

// this is for GUID == and !=
#include <objbase.h>
#ifndef GUID_EQUALS_DEFINED
  #define GUID_EQUALS_DEFINED
#endif

	typedef SSIZE_T ssize_t;
#ifdef NULL
  #undef NULL
#endif
#ifndef NULL
  #define NULL 0
#endif

#ifdef _WIN32_WCE
typedef int intptr_t;
#endif
