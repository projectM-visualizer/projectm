/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualThreading.cpp
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

#include "VisualThreading.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/Multiprocessing.h>
#endif



using namespace VizKit;


OSStatus VisualThreading::createThread(ThreadingFuncPtr aThreadingFuncPtr, char* name) {
	OSStatus osStatus = noErr;
#if TARGET_OS_MAC
	void* param = NULL;
	MPTaskID threadId;
	ByteCount stackSize = 0;
	osStatus = MPCreateTask(&(*aThreadingFuncPtr), param, stackSize, NULL, NULL, NULL, 0, &threadId);
#endif
#if TARGET_OS_WIN
	LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL;
	SIZE_T stackSize = 0;
	LPVOID param = NULL;
	DWORD dwCreationFlags = 0;
	LPDWORD threadId = 0;
	HANDLE hdl = CreateThread(lpThreadAttributes, stackSize, (LPTHREAD_START_ROUTINE)aThreadingFuncPtr, param, dwCreationFlags, threadId);
	/*
	WaitForSingleObject(hdl, 3000);
	DWORD exitCode;
	BOOL success = GetExitCodeThread(hdl, &exitCode);
	char logStr[64];
	sprintf(logStr, "%s - exitCode: %ld", name, exitCode);
	writeLog(logStr);
	CloseHandle(hdl);
	*/
#endif
	return osStatus;
}
