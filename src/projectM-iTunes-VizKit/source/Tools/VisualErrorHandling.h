/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualErrorHandling.h
 *
 */

/***************************************************************************

Copyright (c) 2004-2007 Heiko Wichmann (http://www.imagomat.de/vizkit)


This software is provided 'as-is', without any expressed or implied warranty. 
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#ifndef VisualErrorHandling_h
#define VisualErrorHandling_h


#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#endif

#if TARGET_OS_WIN
#include <QT/MacTypes.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* ErrorHandling can be used to write log information to a log file. */


/**
 * Writes a 8-bit string to the system log file.
 * @param infoStr The 8-bit string to write to log file.
 */
void writeLog(const char* const infoStr);

/**
 * Writes a wchar_t string to the system log file.
 * @param infoStr The wchar_t string to write to log file.
 */
void writeLogW(const wchar_t* const infoStr);

/**
 * Writes a string to the system log file if player is in debug mode.
 * @param infoStr The string to write to log file.
 */
void writeDebugLog(const char* const infoStr);

#if TARGET_OS_WIN
	/**
	 * Writes the last error to the system log file.\ Windows-only.
	 * @param lpszFunction The name of the function where the error occured.
	 */
	void writeLastErrorLog(char* lpszFunction);


	void addSourceToRegistry(char* pszAppname, char* pszMsgDLL);

	void reportAnIEvent(UInt32 dwIdEvent, UInt16 cStrings, char* pszStrings);

	void reportAnWEvent(UInt32 dwIdEvent, UInt16 cStrings, char* pszStrings);

	void reportAnEEvent(UInt32 dwIdEvent, UInt16 cStrings, char* pszStrings);

#endif

#ifdef __cplusplus
}
#endif

#endif /* VisualErrorHandling_h */
