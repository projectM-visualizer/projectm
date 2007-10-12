/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: TrackTitleActor.cpp
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

#include "TrackTitleActor.h"
#include "TrackTitle.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualString.h"



using namespace VizKit;


TrackTitleActor::TrackTitleActor() {
	strcpy(actorName, "TRACKTITLE");
	state = kVisActOn;
    trackTitle = new TrackTitle;
	textureOfCurrentTrackTitleIsAvailable = false;
}


TrackTitleActor::~TrackTitleActor() {
    trackTitle->clear();
	delete trackTitle;
}


void TrackTitleActor::show() {
	if ((this->textureOfCurrentTrackTitleIsAvailable == true) && (this->state != kVisActOff)) {
		this->trackTitle->show();
	}
}


void TrackTitleActor::clear() {
	this->trackTitle->clear();
}


void TrackTitleActor::handleNotification(const VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug
	
	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
		case kAudioMetadataIsAvailableMsg:
			{
				const VisualString trackName = VisualDataStore::getNameOfCurrentTrack();
				if (trackName.getNumberOfNonWhitespaceCharacters() > 0) {
					OSStatus status = this->trackTitle->makeTextureOfTrackTitle(trackName);
					if (status == noErr) {
						this->trackTitle->calcPositionOnScreen();
						VisualDataStore::setValueInt(VisualConfiguration::kTrackInfoTextureHeight, this->trackTitle->getTrackInfoTextureHeightInPixels());
						VisualNotification::post(kTrackInfoTextureIsAvailableMsg);
					} else {
						VisualDataStore::setValueInt(VisualConfiguration::kTrackInfoTextureHeight, 0);
					}
					VisualNotification::post(kTrackInfoTextureChangedMsg);
				}
			}
			break;
		case kTrackInfoTextureIsAvailableMsg:
			this->textureOfCurrentTrackTitleIsAvailable = true;
			this->trackTitle->fadeIn(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnPlayInMS));
			break;
		case kAudioPlayStartedEvt:
			this->trackTitle->clear();
			this->textureOfCurrentTrackTitleIsAvailable = false;
			break;
		case kAudioPlayStoppedEvt:
			this->trackTitle->clear();
			this->textureOfCurrentTrackTitleIsAvailable = false;
			break;
		case kAudioPlayPausedEvt:
			this->trackTitle->pulsate();
			break;
		case kAudioPlayResumedEvt:
			this->trackTitle->fadeIn(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnResumeInMS));
			break;
		case kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt:
			this->trackTitle->fadeOut(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeBeforeEndOfTrackInMS));
			break;
		case kCanvasReshapeEvt:
			this->trackTitle->calcPositionOnScreen();
			VisualDataStore::setValueInt(VisualConfiguration::kTrackInfoTextureHeight, this->trackTitle->getTrackInfoTextureHeightInPixels());
			break;
		default:
			writeLog("unhandled Notification in TrackTitleActor");
			break;
	}

}
