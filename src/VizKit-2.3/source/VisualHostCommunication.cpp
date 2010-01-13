/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualHostCommunication.cpp
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

#if TARGET_OS_WIN
#include <stdio.h> // sprintf
#endif

#include "VisualHostCommunication.h"
#include "VisualSignature.h"
#include "VisualMainAction.h"
#include "VisualErrorHandling.h"
#include "VisualQuickTime.h"


#include "iTunesAPI.h" // PlayerGetCurrentTrackCoverArt


using namespace VizKit;



VisualHostCommunication* VisualHostCommunication::theVisualHostCommunication = NULL;


VisualHostCommunication::VisualHostCommunication() {
	// null
}


VisualHostCommunication::~VisualHostCommunication() {
	//theVisualHostCommunication->notificationQueue.clear();
}


VisualHostCommunication* VisualHostCommunication::getInstance() {
	if (theVisualHostCommunication == NULL) {
		theVisualHostCommunication = new VisualHostCommunication;
	}
	return theVisualHostCommunication;
}


void VisualHostCommunication::dispose() {
	if (theVisualHostCommunication != NULL) {
		if (theVisualHostCommunication->coverArtImageDataHandle != NULL) {
			VisualQuickTime::disposeHandle((Handle)(theVisualHostCommunication->coverArtImageDataHandle));
		}
		delete theVisualHostCommunication;
		theVisualHostCommunication = NULL;
	}
}


int VisualHostCommunication::getCurrentTrackCoverArt(void** albumCoverArtworkImageData, uint32& numberOfBytes) {

	uint16 numberOfArtworks = 0;
	OSType albumCoverArtworkFileType;

	theVisualHostCommunication = VisualHostCommunication::getInstance();
	Handle coverArtHandle = NULL;
	OSStatus status = PlayerGetCurrentTrackCoverArt(VisualSignature::getAppCookie(), VisualSignature::getAppProc(), &coverArtHandle, &albumCoverArtworkFileType);
    if (status != noErr) {
		char errLog[256];
		sprintf(errLog, "PlayerGetCurrentTrackCoverArt err (%ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return numberOfArtworks;
    }

	numberOfBytes = VisualQuickTime::getHandleSize(coverArtHandle);

	if (numberOfBytes > 0) {
		numberOfArtworks = 1;
		*albumCoverArtworkImageData = *coverArtHandle;
	}

	if (coverArtHandle) {
		theVisualHostCommunication->coverArtImageDataHandle = coverArtHandle;
	}

	return numberOfArtworks;
}
