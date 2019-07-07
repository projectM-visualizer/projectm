#ifndef NULLSOFT_UTILITY_AUTOCHARFN_H
#define NULLSOFT_UTILITY_AUTOCHARFN_H

/* Winamp defines this, but this little block lets us use this thing outside of Winamp */
#ifndef FILENAME_SIZE
#define FILENAME_SIZE (MAX_PATH*4)
#define REMOVE_FILENAME_SIZE
#endif


#include <windows.h>
#include <shlwapi.h>

class AutoCharFn
{
public:
	AutoCharFn(const wchar_t *filename)
	{
		out[0]=0;
		if (!filename)
			return;
		if (PathIsURLW(filename))
		{
			WideCharToMultiByte(CP_ACP, 0, filename, -1, out, FILENAME_SIZE, NULL, NULL);
			return ;
		}

		BOOL unconvertable = FALSE;
		WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, filename, -1, out, FILENAME_SIZE, NULL, &unconvertable);

		if (unconvertable)
		{
			wchar_t temp[MAX_PATH];
			if (GetShortPathNameW(filename, temp, MAX_PATH))
				WideCharToMultiByte(CP_ACP, 0, temp, -1, out, FILENAME_SIZE, NULL, NULL);
				
		}
	}

	operator char *() { return out; }
private:
	char out[FILENAME_SIZE];
};


#ifdef REMOVE_FILENAME_SIZE
#undef FILENAME_SIZE
#endif

#endif
