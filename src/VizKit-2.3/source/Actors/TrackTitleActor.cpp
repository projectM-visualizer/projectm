/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TrackTitleActor.cpp
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

#include "TrackTitleActor.h"
#include "TrackTitle.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualPreferences.h"
#include "VisualString.h"
#include "VisualStyledString.h"
#include "VisualImage.h"



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


void TrackTitleActor::init() {
	VisualNotification::registerNotification(this, kGetTrackInfoStringStyleMsg);
	VisualNotification::registerNotification(this, kTrackInfoImageMsg);
	VisualNotification::registerNotification(this, kAudioPlayTrackChangedEvt);
	VisualNotification::registerNotification(this, kAudioPlayStoppedEvt);
	VisualNotification::registerNotification(this, kAudioPlayPausedEvt);
	VisualNotification::registerNotification(this, kAudioPlayResumedEvt);
	VisualNotification::registerNotification(this, kAudioPlayReachedFadeOutTimeEvt);
	VisualNotification::registerNotification(this, kCanvasReshapeEvt);
}


void TrackTitleActor::show(const VisualPlayerState& visualPlayerState) {
	if ((this->textureOfCurrentTrackTitleIsAvailable == true) && (this->state != kVisActOff)) {
		this->trackTitle->show();
	}
}


void TrackTitleActor::handleNotification(VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug
	
	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
	
		case kGetTrackInfoStringStyleMsg:
			{
				VisualStringStyle trackInfoStringStyle = this->trackTitle->getTrackInfoStringStyle();
				aNotification.setObject(trackInfoStringStyle);				
			}
			break;
			
		case kTrackInfoImageMsg:
			{
				VisualImage* trackInfoImage = dynamic_cast<VisualImage*>(aNotification.getObject());
				this->trackTitle->setTrackInfoImage(*trackInfoImage);
				this->trackTitle->reshape();
				VisualDataStore::setValue(VisualDataStore::kTrackInfoTextureHeight, this->trackTitle->getTrackInfoTextureHeightInPixels());
				this->textureOfCurrentTrackTitleIsAvailable = true;
				VisualNotification::post(kTrackInfoTextureChangedMsg);
				VisualItemIdentifier currentTrackIdentifier = VisualDataStore::getIdentifierOfCurrentTrack();
				if (this->trackTitleTrackIdentifier != currentTrackIdentifier) {
					this->trackTitle->addMoveAnimation();
					this->trackTitle->fadeIn(VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnPlayInMS));
					this->trackTitleTrackIdentifier = currentTrackIdentifier;
				}
			}
			break;

		case kAudioPlayTrackChangedEvt:
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
			this->trackTitle->fadeIn(VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnResumeInMS));
			break;
			
		case kAudioPlayReachedFadeOutTimeEvt:
			this->trackTitle->fadeOut(VisualPreferences::getValue(VisualPreferences::kFadeOutTimeBeforeEndOfTrackInMS));
			break;
			
		case kCanvasReshapeEvt:
			this->trackTitle->reshape();
			VisualDataStore::setValue(VisualDataStore::kTrackInfoTextureHeight, this->trackTitle->getTrackInfoTextureHeightInPixels());
			break;
			
		default:
			writeLog("unhandled Notification in TrackTitleActor");
			break;
	}

}
