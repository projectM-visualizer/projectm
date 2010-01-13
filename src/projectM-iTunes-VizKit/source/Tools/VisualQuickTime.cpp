/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualQuickTime.cpp
 *
 */

/***************************************************************************

Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)


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

#include "VisualQuickTime.h"


#if TARGET_OS_WIN
#include <QuickTime/CIncludes/QTML.h> // Header file of QuickTime 7.3 SDK for Windows (http://developer.apple.com/sdk/index.html)
#endif

#if TARGET_OS_MAC
#include <Carbon/Carbon.h>
#endif


using namespace VizKit;


uint32 VisualQuickTime::getHandleSize(void* aHandle) {
	uint32 numberOfBytes = 0;
	HLock((Handle)aHandle);
	numberOfBytes = ::GetHandleSize((Handle)aHandle); // win: malloc.h: _msize(aHandle) (?)
	HUnlock((Handle)aHandle);
	return numberOfBytes;
}


void VisualQuickTime::disposeHandle(void* aHandle) {
	::DisposeHandle((Handle)aHandle);
}


bool VisualQuickTime::getPressedKeyOfEvent(void* theEvent, char& aChar) {
	bool success = false;
	EventRecord* eventPtr = reinterpret_cast<EventRecord*>(theEvent);
	if ((eventPtr) && ((eventPtr->what == keyDown) || (eventPtr->what == autoKey))) {
		aChar = (char)(eventPtr->message & charCodeMask);
		success = true;
	}
	return success;
}
