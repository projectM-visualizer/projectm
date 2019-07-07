#ifndef AUTOWIDEH
#define AUTOWIDEH
#ifdef WIN32
#include <windows.h>

inline wchar_t *AutoWideDup(const char *convert, UINT codePage=CP_ACP)
{
	wchar_t *wide = 0;
	if (!convert)
		return 0;

	int size = MultiByteToWideChar(codePage, 0, convert, -1, 0,0);
	if (!size)
		return 0;

	wide = (wchar_t *)malloc(size*sizeof(wchar_t));
	if (!MultiByteToWideChar(codePage, 0, convert, -1, wide,size))
	{
		free(wide);
		wide=0;
	}
	return wide;
}

class AutoWide
{
public:
	AutoWide(const char *convert, UINT codePage=CP_ACP) :  wide(0)
	{
		wide = AutoWideDup(convert, codePage);
	}
	~AutoWide()
	{
		free(wide);
		wide=0;
	}
	operator wchar_t *()
	{
		return wide;
	}
private:
	wchar_t *wide;
};
#elif defined(__APPLE__)
#include <string.h>
inline wchar_t *AutoWideDup(const char *convert)
{
	wchar_t *wide=0;
	if (!convert)
		return 0;
	int size = strlen(convert)+1;
	if (!size)
		return 0;

	wide = (wchar_t *)malloc(size*sizeof(wchar_t));
	if (mbstowcs(wide, convert, size) == (size_t)-1)
	{
		free(wide);
		wide=0;
	}
	return wide;
}

class AutoWide
{
public:
	AutoWide(const char *convert) :  wide(0)
	{
		wide = AutoWideDup(convert);
	}
	~AutoWide()
	{
		free(wide);
		wide=0;
	}
	operator wchar_t *()
	{
		return wide;
	}
private:
	wchar_t *wide;
};

#endif

#endif