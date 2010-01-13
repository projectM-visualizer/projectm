/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: CoverArtActor.cpp
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

#include "CoverArtActor.h"
#include "CoverArt.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualPreferences.h"
#include "VisualTiming.h"
#include "VisualConfiguration.h"
#include "VisualImage.h"



using namespace VizKit;


CoverArtActor::CoverArtActor() {
	strcpy(actorName, "COVERART");
	state = kVisActOn;
    coverArt = new CoverArt;
	currentAudioTrackHasCoverArt = false;
}


CoverArtActor::~CoverArtActor() {
    delete coverArt;
}


void CoverArtActor::init() {
	VisualNotification::registerNotification(this, kImageWithIdentifierMsg);
	VisualNotification::registerNotification(this, kAudioPlayTrackChangedEvt);
	VisualNotification::registerNotification(this, kAudioPlayStoppedEvt);
	VisualNotification::registerNotification(this, kAudioPlayPausedEvt);
	VisualNotification::registerNotification(this, kAudioPlayResumedEvt);
	VisualNotification::registerNotification(this, kAudioPlayReachedFadeOutTimeEvt);
	VisualNotification::registerNotification(this, kCanvasReshapeEvt);
	VisualNotification::registerNotification(this, kTrackInfoTextureChangedMsg);
}


void CoverArtActor::show(const VisualPlayerState& visualPlayerState) {
	if ((this->currentAudioTrackHasCoverArt == true) && (this->state != kVisActOff)) {
		this->coverArt->prepareCoverAction();
		this->coverArt->showCover();
		this->coverArt->finishCoverAction();
	}
}


void CoverArtActor::handleNotification(VisualNotification& aNotification) {

	// VisualActor::handleNotification(aNotification); // debug

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
	
		case kImageWithIdentifierMsg:
			{
				if (aNotification.getIdentifier() == VisualDataStore::albumCoverArtworkImageId) {
					VisualImage* albumCoverArtworkImage = dynamic_cast<VisualImage*>(aNotification.getObject());
					this->coverArt->setAlbumCoverArtWorkImage(*albumCoverArtworkImage);
					this->coverArt->reshape();
					this->currentAudioTrackHasCoverArt = true;
					this->coverArt->rotate();
					this->coverArt->fadeIn(VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnPlayInMS));
				}
			}
			break;
			
		case kAudioPlayTrackChangedEvt:
			this->coverArt->clear();
			this->currentAudioTrackHasCoverArt = false;
			break;
			
		case kAudioPlayStoppedEvt:
			this->coverArt->clear();
			this->currentAudioTrackHasCoverArt = false;
			break;
			
		case kAudioPlayPausedEvt:
			this->coverArt->fadeOut(VisualPreferences::getValue(VisualPreferences::kFadeOutTimeOnPauseInMS), 0.15f);
			break;
			
		case kAudioPlayResumedEvt:
			this->coverArt->fadeIn(VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnResumeInMS));
			break;
			
		case kAudioPlayReachedFadeOutTimeEvt:
			this->coverArt->fadeOut(VisualPreferences::getValue(VisualPreferences::kFadeOutTimeBeforeEndOfTrackInMS));
			break;
			
		case kCanvasReshapeEvt:
			if (this->currentAudioTrackHasCoverArt == true) {
				this->coverArt->reshape();
			}
			break;
			
		case kTrackInfoTextureChangedMsg:
			if (this->currentAudioTrackHasCoverArt == true) {
				this->coverArt->reshape();
			}
			break;
			
		default:
			char notificationString[64];
			VisualNotification::convertNotificationKeyToString(notificationKey, notificationString);
			char errLog[256];
			sprintf(errLog, "Unhandled VisualNotificationKey %s in file: %s (line: %d) [%s])", notificationString, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			break;
	}

}
