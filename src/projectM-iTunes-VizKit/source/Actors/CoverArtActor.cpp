/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: CoverArtActor.cpp
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

#include "CoverArtActor.h"
#include "CoverArt.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualTiming.h"
#include "VisualConfiguration.h"



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


void CoverArtActor::prepareShow(const VisualPlayerState& visualPlayerState) {
	if ((this->currentAudioTrackHasCoverArt == true) && (this->state != kVisActOff)) {
		this->coverArt->prepareCoverAction();
	}
}


void CoverArtActor::show() {
	if ((this->currentAudioTrackHasCoverArt == true) && (this->state != kVisActOff)) {
		this->coverArt->showCover();
	}
}


void CoverArtActor::finishShow() {
	if ((this->currentAudioTrackHasCoverArt == true) && (this->state != kVisActOff)) {
		this->coverArt->finishCoverAction();
	}
}


void CoverArtActor::handleNotification(const VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
		case kAudioMetadataIsAvailableMsg:
			if (VisualDataStore::getNumberOfCoverArtworksOfCurrentTrack() > 0) {
				this->coverArt->makeImageOfCover();
				this->coverArt->calcPositionOnScreen();
				this->currentAudioTrackHasCoverArt = true;
			} else {
				this->currentAudioTrackHasCoverArt = false;
			}
			if (this->currentAudioTrackHasCoverArt == true) {
				VisualNotification::post(kCoverTextureIsAvailableMsg);
			}
			break;
		case kCoverTextureIsAvailableMsg:
			this->coverArt->rotate();
			this->coverArt->fadeIn(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnPlayInMS));
			break;
		case kAudioPlayStartedEvt:
			this->coverArt->clear();
			this->currentAudioTrackHasCoverArt = false;
			break;
		case kAudioPlayStoppedEvt:
			this->coverArt->clear();
			this->currentAudioTrackHasCoverArt = false;
			break;
		case kAudioPlayPausedEvt:
			this->coverArt->fadeOut(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeOnPauseInMS), 0.15f);
			break;
		case kAudioPlayResumedEvt:
			this->coverArt->fadeIn(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnResumeInMS));
			break;
		case kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt:
			this->coverArt->fadeOut(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeBeforeEndOfTrackInMS));
			break;
		case kCanvasReshapeEvt:
			if (this->currentAudioTrackHasCoverArt == true) {
				this->coverArt->calcPositionOnScreen();
			}
			break;
		case kTrackInfoTextureChangedMsg:
			if (this->currentAudioTrackHasCoverArt == true) {
				this->coverArt->calcPositionOnScreen();
			}
			break;
		default:
			writeLog("unhandled Notification in ProcessMonitorActor");
			break;
	}

}
