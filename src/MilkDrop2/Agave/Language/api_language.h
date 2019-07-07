#ifndef NULLSOFT_API_LANGUAGE_H
#define NULLSOFT_API_LANGUAGE_H

#include <bfc/dispatch.h>
#include "lang.h"
#include <locale.h>

#if (_MSC_VER <= 1200)
	struct threadlocaleinfostruct;
	struct threadmbcinfostruct;
	typedef struct threadlocaleinfostruct * pthreadlocinfo;
	typedef struct threadmbcinfostruct * pthreadmbcinfo;

	typedef struct localeinfo_struct
	{
		pthreadlocinfo locinfo;
		pthreadmbcinfo mbcinfo;
	} _locale_tstruct, *_locale_t;
#endif

class api_language : public Dispatchable
{
protected:
	api_language() {}
	~api_language() {}
public:
	char *GetString(HINSTANCE hinst, HINSTANCE owner, UINT uID, char *str=NULL, size_t maxlen=0);
	wchar_t *GetStringW(HINSTANCE hinst, HINSTANCE owner, UINT uID, wchar_t *str=NULL, size_t maxlen=0);

	char *GetStringFromGUID(const GUID guid, HINSTANCE owner, UINT uID, char *str=NULL, size_t maxlen=0);
	wchar_t *GetStringFromGUIDW(const GUID guid, HINSTANCE owner, UINT uID, wchar_t *str=NULL, size_t maxlen=0);

	HINSTANCE FindDllHandleByGUID(GUID guid);
	HINSTANCE FindDllHandleByString(const char* str);
	HINSTANCE FindDllHandleByStringW(const wchar_t* str);
	HINSTANCE StartLanguageSupport(HINSTANCE hinstance, const GUID guid);

	const wchar_t *GetLanguageFolder();

	#define LANG_IDENT_STR 0
	#define LANG_LANG_CODE 1
	#define LANG_COUNTRY_CODE 2
	const wchar_t *GetLanguageIdentifier(int mode);

	HWND CreateLDialogParam(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param);
	INT_PTR LDialogBoxParam(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param);
	HMENU LoadLMenu(HINSTANCE localised, HINSTANCE original, UINT id);

	HWND CreateLDialogParamW(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param);
	INT_PTR LDialogBoxParamW(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param);
	HMENU LoadLMenuW(HINSTANCE localised, HINSTANCE original, UINT id);

	void* LoadResourceFromFile(HINSTANCE hinst, HINSTANCE owner, LPCTSTR lpType, LPCTSTR lpName, DWORD* size);
	void* LoadResourceFromFileW(HINSTANCE hinst, HINSTANCE owner, LPCWSTR lpType, LPCWSTR lpName, DWORD* size);

	HACCEL LoadAcceleratorsA(HINSTANCE hinst, HINSTANCE owner, LPCSTR lpTableName);
	HACCEL LoadAcceleratorsW(HINSTANCE hinst, HINSTANCE owner, LPCWSTR lpTableName);

	// Implemented in 5.58+
	// When called this will attempt to set the locale used for numeric representation
	// to that of the user running the current Winamp instance as long as the language
	// and country identifiers match those reported within the language pack (if used)
	//
	// If you're running under a different thread then this will need to be called as
	// the locale is set on a per thread basis which generally means anything under the
	// Winamp process will be handled correctly unless a UI aspect is running under a
	// different thread. Internally this is called within winamp.exe and vis_milk2.dll
	BOOL UseUserNumericLocale();

	// Get_C_NumericLocale() is a wrapper for _create_locale(LC_NUMERIC, "C") which can
	// then be used in _atof_l(..), _sscanf_l(..) or other locale based functions when
	// you need to process numbers without localisation handling ie the "C" locale.
	// This function is provided for convenience unless you want to do it all manually.
	_locale_t Get_C_NumericLocale();

	// Implemented in 5.7+
	wchar_t* FormattedSizeString(wchar_t *out, int cchLen, __int64 size);

public:
	DISPATCH_CODES
	{
	    API_LANGUAGE_GETSTRING = 10,
		API_LANGUAGE_GETSTRINGW = 11,

	    API_LANGUAGE_GETSTRINGFROMGUID = 12,
		API_LANGUAGE_GETSTRINGFROMGUIDW = 13,

		API_LANGUAGE_GETHINSTANCEBYGUID = 20,
		API_LANGUAGE_GETHINSTANCEBYNAME = 21,
		API_LANGUAGE_GETHINSTANCEBYNAMEW = 22,

		API_LANGUAGE_STARTUP = 30,
		API_LANGUAGE_SHUTDOWN = 31,

		API_LANGUAGE_GETLANGUAGEFOLDER = 40,

		API_LANGUAGE_CREATELDIALOGPARAM = 50,
		API_LANGUAGE_LDIALOGBOXPARAM = 51,
		API_LANGUAGE_LOADLMENU = 52,
		API_LANGUAGE_CREATELDIALOGPARAMW = 53,
		API_LANGUAGE_LDIALOGBOXPARAMW = 54,
		API_LANGUAGE_LOADLMENUW = 55,

		API_LANGUAGE_GETLANGUAGEIDENTIFIER = 60,

		API_LANGUAGE_LOADRESOURCEFROMFILE = 70,
		API_LANGUAGE_LOADRESOURCEFROMFILEW = 71,

		API_LANGUAGE_LOADACCELERATORSA = 80,
		API_LANGUAGE_LOADACCELERATORSW = 81,

		// Implemented in 5.58+
		// See UseUserNumericLocale notes
		API_LANGUAGE_USEUSERNUMERICLOCALE = 90,
		API_LANGUAGE_GET_C_NUMERICLOCALE = 91,

		// Implemented in 5.7+
		API_LANGUAGE_FORMATTEDSIZESTRING = 100,
	};
};

inline char *api_language::GetString(HINSTANCE hinst, HINSTANCE owner, UINT uID, char *str, size_t maxlen)
{
	return _call(API_LANGUAGE_GETSTRING, (char * )0, hinst, owner, uID, str, maxlen);
}

inline wchar_t *api_language::GetStringW(HINSTANCE hinst, HINSTANCE owner, UINT uID, wchar_t *str, size_t maxlen)
{
	return _call(API_LANGUAGE_GETSTRINGW, (wchar_t * )0, hinst, owner, uID, str, maxlen);
}

inline char *api_language::GetStringFromGUID(const GUID guid, HINSTANCE owner, UINT uID, char *str, size_t maxlen)
{
	return _call(API_LANGUAGE_GETSTRINGFROMGUID, (char * )0, guid, owner, uID, str, maxlen);
}

inline wchar_t *api_language::GetStringFromGUIDW(const GUID guid, HINSTANCE owner, UINT uID, wchar_t *str, size_t maxlen)
{
	return _call(API_LANGUAGE_GETSTRINGFROMGUIDW, (wchar_t * )0, guid, owner, uID, str, maxlen);
}

inline HINSTANCE api_language::FindDllHandleByGUID(const GUID guid)
{
	return _call(API_LANGUAGE_GETHINSTANCEBYGUID, (HINSTANCE )0, guid);
}

inline HINSTANCE api_language::FindDllHandleByString(const char* str)
{
	return _call(API_LANGUAGE_GETHINSTANCEBYNAME, (HINSTANCE )0, str);
}

inline HINSTANCE api_language::FindDllHandleByStringW(const wchar_t* str)
{
	return _call(API_LANGUAGE_GETHINSTANCEBYNAMEW, (HINSTANCE )0, str);
}

inline HINSTANCE api_language::StartLanguageSupport(HINSTANCE hinstance, const GUID guid)
{
	return _call(API_LANGUAGE_STARTUP, (HINSTANCE )0, hinstance, guid);
}

inline const wchar_t *api_language::GetLanguageFolder()
{
	return _call(API_LANGUAGE_GETLANGUAGEFOLDER, (const wchar_t *)0);
}

inline HWND api_language::CreateLDialogParam(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param)
{
	return _call(API_LANGUAGE_CREATELDIALOGPARAM, (HWND)0, localised, original, id, parent, proc, param);
}

inline INT_PTR api_language::LDialogBoxParam(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param)
{
	return _call(API_LANGUAGE_LDIALOGBOXPARAM, (INT_PTR)0, localised, original, id, parent, proc, param);
}

inline HMENU api_language::LoadLMenu(HINSTANCE localised, HINSTANCE original, UINT id)
{
	return _call(API_LANGUAGE_LOADLMENU, (HMENU)0, localised, original, id);
}

inline HWND api_language::CreateLDialogParamW(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param)
{
	return _call(API_LANGUAGE_CREATELDIALOGPARAMW, (HWND)0, localised, original, id, parent, proc, param);
}

inline INT_PTR api_language::LDialogBoxParamW(HINSTANCE localised, HINSTANCE original, UINT id, HWND parent, DLGPROC proc, LPARAM param)
{
	return _call(API_LANGUAGE_LDIALOGBOXPARAMW, (INT_PTR)0, localised, original, id, parent, proc, param);
}

inline HMENU api_language::LoadLMenuW(HINSTANCE localised, HINSTANCE original, UINT id)
{
	return _call(API_LANGUAGE_LOADLMENUW, (HMENU)0, localised, original, id);
}

inline const wchar_t *api_language::GetLanguageIdentifier(int mode)
{
	return _call(API_LANGUAGE_GETLANGUAGEIDENTIFIER, (const wchar_t *)0, mode);
}

inline void *api_language::LoadResourceFromFile(HINSTANCE hinst, HINSTANCE owner, LPCTSTR lpType, LPCTSTR lpName, DWORD* size)
{
	return _call(API_LANGUAGE_LOADRESOURCEFROMFILE, (void*)0, hinst, owner, lpType, lpName, size);
}

inline void *api_language::LoadResourceFromFileW(HINSTANCE hinst, HINSTANCE owner, LPCWSTR lpType, LPCWSTR lpName, DWORD* size)
{
	return _call(API_LANGUAGE_LOADRESOURCEFROMFILEW, (void*)0, hinst, owner, lpType, lpName, size);
}

inline HACCEL api_language::LoadAcceleratorsA(HINSTANCE hinst, HINSTANCE owner, LPCSTR lpTableName)
{
	return _call(API_LANGUAGE_LOADACCELERATORSA, (HACCEL)NULL, hinst, owner, lpTableName);
}

inline HACCEL api_language::LoadAcceleratorsW(HINSTANCE hinst, HINSTANCE owner, LPCWSTR lpTableName)
{
	return _call(API_LANGUAGE_LOADACCELERATORSA, (HACCEL)NULL, hinst, owner, lpTableName);
}

inline BOOL api_language::UseUserNumericLocale()
{
	return _call(API_LANGUAGE_USEUSERNUMERICLOCALE, (BOOL)0);
}

inline _locale_t api_language::Get_C_NumericLocale()
{
	return _call(API_LANGUAGE_GET_C_NUMERICLOCALE, (_locale_t)0);
}

inline wchar_t *api_language::FormattedSizeString(wchar_t *out, int cchLen, __int64 size)
{
	return _call(API_LANGUAGE_FORMATTEDSIZESTRING, (wchar_t*)0, out, cchLen, size);
}


// utility macros and relevant predefined variables for use with the service + macros
extern api_language *languageManager;
#define WASABI_API_LNG languageManager

extern HINSTANCE api_localised_hinstance;
#define WASABI_API_LNG_HINST api_localised_hinstance

extern HINSTANCE api_orig_hinstance;
#define WASABI_API_ORIG_HINST api_orig_hinstance

#define WASABI_API_LNGSTR WASABI_API_LNG->GetString
// use this is you want a temp copy of the string
#define WASABI_API_LNGSTRING(uID) \
		WASABI_API_LNGSTR(WASABI_API_LNG_HINST,WASABI_API_ORIG_HINST,uID)
// use this is you want a temp copy of the string but need it to fallback to a different module
#define WASABI_API_LNGSTRING_HINST(hinst,uID) \
		WASABI_API_LNGSTR(WASABI_API_LNG_HINST,hinst,uID)
// use this is you want a copy of the string
#define WASABI_API_LNGSTRING_BUF(uID,buf,len) \
		WASABI_API_LNGSTR(WASABI_API_LNG_HINST,WASABI_API_ORIG_HINST,uID,buf,len)
// use this is you want a copy of the string but need it to fallback to a different module
#define WASABI_API_LNGSTRING_HINST_BUF(hinst,uID,buf,len) \
		WASABI_API_LNGSTR(WASABI_API_LNG_HINST,hinst,uID,buf,len)

// unicode versions of the above macros
#define WASABI_API_LNGSTRW WASABI_API_LNG->GetStringW
#define WASABI_API_LNGSTRINGW(uID) \
		WASABI_API_LNGSTRW(WASABI_API_LNG_HINST,WASABI_API_ORIG_HINST,uID)
#define WASABI_API_LNGSTRINGW_HINST(hinst,uID) \
		WASABI_API_LNGSTRW(WASABI_API_LNG_HINST,hinst,uID)
#define WASABI_API_LNGSTRINGW_BUF(uID,buf,len) \
		WASABI_API_LNGSTRW(WASABI_API_LNG_HINST,WASABI_API_ORIG_HINST,uID,buf,len)
#define WASABI_API_LNGSTRINGW_BUF_HINST(hinst,uID,buf,len) \
		WASABI_API_LNGSTRW(WASABI_API_LNG_HINST,hinst,uID,buf,len)

// Dialog handling functions (will revert back to the non-localised version if not valid/present)
#define WASABI_API_CREATEDIALOGPARAM(id, parent, proc, param) \
		WASABI_API_LNG->CreateLDialogParam(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, param)
#define WASABI_API_CREATEDIALOG(id, parent, proc) \
		WASABI_API_LNG->CreateLDialogParam(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, 0)

#define WASABI_API_CREATEDIALOGPARAMW(id, parent, proc, param) \
		WASABI_API_LNG->CreateLDialogParamW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, param)
#define WASABI_API_CREATEDIALOGW(id, parent, proc) \
		WASABI_API_LNG->CreateLDialogParamW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, 0)

#define WASABI_API_DIALOGBOXPARAM(id, parent, proc, param) \
		WASABI_API_LNG->LDialogBoxParam(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, param)
#define WASABI_API_DIALOGBOX(id, parent, proc) \
		WASABI_API_LNG->LDialogBoxParam(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, 0)

#define WASABI_API_DIALOGBOXPARAMW(id, parent, proc, param) \
		WASABI_API_LNG->LDialogBoxParamW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, param)
#define WASABI_API_DIALOGBOXW(id, parent, proc) \
		WASABI_API_LNG->LDialogBoxParamW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id, parent, (DLGPROC)proc, 0)

#define WASABI_API_LOADMENU(id) \
		WASABI_API_LNG->LoadLMenu(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id)
#define WASABI_API_LOADMENUW(id) \
		WASABI_API_LNG->LoadLMenuW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, id)


#define WASABI_API_LOADACCELERATORSA(__id) \
		WASABI_API_LNG->LoadAcceleratorsA(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, MAKEINTRESOURCEA(__id))
#define WASABI_API_LOADACCELERATORSW(__id) \
		WASABI_API_LNG->LoadAcceleratorsW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, MAKEINTRESOURCEW(__id))

#ifdef UNICODE
#define WASABI_API_LOADACCELERATORS WASABI_API_LOADACCELERATORSW
#else
#define WASABI_API_LOADACCELERATORS WASABI_API_LOADACCELERATORSA
#endif

#define WASABI_API_START_LANG(orig_hinst, guid) \
{ \
	WASABI_API_ORIG_HINST = orig_hinst; \
	WASABI_API_LNG_HINST = WASABI_API_LNG->StartLanguageSupport(WASABI_API_ORIG_HINST,guid); \
}

#define WASABI_API_LOADRESFROMFILE(lpType, lpName, size) \
		WASABI_API_LNG->LoadResourceFromFile(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, lpType, lpName, size)
#define WASABI_API_LOADRESFROMFILEW(lpType, lpName, size) \
		WASABI_API_LNG->LoadResourceFromFileW(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST, lpType, lpName, size)

// {30AED4E5-EF10-4277-8D49-27AB5570E891}
static const GUID languageApiGUID = 
{ 0x30aed4e5, 0xef10, 0x4277, { 0x8d, 0x49, 0x27, 0xab, 0x55, 0x70, 0xe8, 0x91 } };

#endif