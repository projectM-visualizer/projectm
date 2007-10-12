/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualHostCommunication.cpp
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

#include "VisualHostCommunication.h"
#include "VisualSignature.h"

using namespace VizKit;



UInt16 VisualHostCommunication::getCurrentTrackCoverArt(OSType* albumCoverArtworkFileType, Handle* albumCoverArtworkHandle) {

	UInt16 numberOfArtworks = 0;

	OSStatus status = PlayerGetCurrentTrackCoverArt(VisualSignature::getAppCookie(), VisualSignature::getAppProc(), albumCoverArtworkHandle, albumCoverArtworkFileType);
    if (status != noErr) {
        //writeLog("Error at VisualDataStore.evaluateCoverArtInfo");
    }
	HLock(*albumCoverArtworkHandle);
	Size coverArtHandleBlockSize = GetHandleSize(*albumCoverArtworkHandle); // win: malloc.h: _msize(coverArtHandle) (?)
	if (coverArtHandleBlockSize == 0) {
		*albumCoverArtworkFileType = (OSType)NULL;
    } else {
		numberOfArtworks = 1;
    }
	HUnlock(*albumCoverArtworkHandle);

	return numberOfArtworks;
}


OSStatus VisualHostCommunication::setPreferredDisplayResolution(UInt16 minBitsPerPixel, UInt16 maxBitsPerPixel, UInt16 preferredBitsPerPixel, UInt16 horizontalPixels, UInt16 verticalPixels) {

	OSStatus osStatus = PlayerSetFullScreenOptions(VisualSignature::getAppCookie(), 
											VisualSignature::getAppProc(), 
											minBitsPerPixel, 
											maxBitsPerPixel, 
											preferredBitsPerPixel, 
											horizontalPixels, 
											verticalPixels);

	return osStatus;
}
