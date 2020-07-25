/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TrackLyricsActor.cpp
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

#include "TrackLyricsActor.h"
#include "TrackLyrics.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualPreferences.h"
#include "VisualString.h"
#include "VisualStringStyle.h"
#include "VisualImage.h"


using namespace VizKit;


TrackLyricsActor::TrackLyricsActor() {
	strcpy(actorName, "TRACKLYRICS");
	state = kVisActOn;
    trackLyrics = new TrackLyrics;
	textureOfCurrentTrackLyricsIsAvailable = false;
}


TrackLyricsActor::~TrackLyricsActor() {
    trackLyrics->clear();
	delete trackLyrics;
}


void TrackLyricsActor::init() {
	VisualNotification::registerNotification(this, kGetTrackLyricsStringStyleMsg);
	VisualNotification::registerNotification(this, kTrackLyricsImageMsg);
	VisualNotification::registerNotification(this, kAudioPlayTrackChangedEvt);
	VisualNotification::registerNotification(this, kAudioPlayStoppedEvt);
	VisualNotification::registerNotification(this, kAudioPlayPausedEvt);
	VisualNotification::registerNotification(this, kAudioPlayResumedEvt);
	VisualNotification::registerNotification(this, kAudioPlayReachedFadeOutTimeEvt);
	VisualNotification::registerNotification(this, kCanvasReshapeEvt);
	VisualNotification::registerNotification(this, kTrackInfoTextureChangedMsg);
}


void TrackLyricsActor::show(const VisualPlayerState& visualPlayerState) {
	if ((this->textureOfCurrentTrackLyricsIsAvailable == true) && (this->state != kVisActOff)) {
		this->trackLyrics->show();
	}
}


void TrackLyricsActor::handleNotification(VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug
	
	VisualString trackLyricsStr;

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
	
		case kGetTrackLyricsStringStyleMsg:
			{
				VisualStringStyle lyricsStringStyle = this->trackLyrics->getTrackLyricsStringStyle();
				aNotification.setObject(lyricsStringStyle);				
			}
			break;

		case kTrackLyricsImageMsg:
			{
				VisualImage* lyricsImage = dynamic_cast<VisualImage*>(aNotification.getObject());
				this->trackLyrics->setTrackLyricsImage(*lyricsImage);
				this->trackLyrics->reshape();
				//VisualDataStore::setValue(VisualDataStore::kTrackInfoTextureHeight, this->trackTitle->getTrackInfoTextureHeightInPixels());
				this->textureOfCurrentTrackLyricsIsAvailable = true;
				//VisualNotification::post(kLyricsTextureIsAvailableMsg);
				//VisualNotification::post(kTrackLyricsTextureChangedMsg);
				this->trackLyrics->fadeIn(VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnPlayInMS));
			}
			break;
			
		case kAudioPlayTrackChangedEvt:
			this->trackLyrics->clear();
			this->textureOfCurrentTrackLyricsIsAvailable = false;
			break;
			
		case kAudioPlayStoppedEvt:
			this->trackLyrics->clear();
			this->textureOfCurrentTrackLyricsIsAvailable = false;
			break;
			
		case kAudioPlayPausedEvt:
			this->trackLyrics->fadeOut(VisualPreferences::getValue(VisualPreferences::kFadeOutTimeOnPauseInMS), 0.15f);
			break;
			
		case kAudioPlayResumedEvt:
			this->trackLyrics->fadeIn(VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnResumeInMS));
			break;
			
		case kAudioPlayReachedFadeOutTimeEvt:
			this->trackLyrics->fadeOut(VisualPreferences::getValue(VisualPreferences::kFadeOutTimeBeforeEndOfTrackInMS));
			break;
			
		case kCanvasReshapeEvt:
			if (textureOfCurrentTrackLyricsIsAvailable == true) {
				this->trackLyrics->reshape();
			}
			break;
			
		case kTrackInfoTextureChangedMsg:
			if (textureOfCurrentTrackLyricsIsAvailable == true) {
				this->trackLyrics->reshape();
			}
			break;
			
		default:
			writeLog("unhandled Notification in TrackLyricsActor");
			break;
	}

}
