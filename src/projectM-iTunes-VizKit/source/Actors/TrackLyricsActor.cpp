/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: TrackLyricsActor.cpp
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

#include "TrackLyricsActor.h"
#include "TrackLyrics.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualString.h"



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


void TrackLyricsActor::show() {
	if ((this->textureOfCurrentTrackLyricsIsAvailable == true) && (this->state != kVisActOff)) {
		this->trackLyrics->show();
	}
}


void TrackLyricsActor::handleNotification(const VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug
	
	VisualString trackLyricsStr;
	const VisualString missingValueString("missing value"); // "missing value" is literally sent with streams that do not convey any lyrics

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
		case kAudioMetadataIsAvailableMsg:
			VisualDataStore::createLyricsOfCurrentTrack();
			break;
		case kLyricsAreAvailableMsg:
			trackLyricsStr = VisualDataStore::getLyricsOfCurrentTrack();
			if (trackLyricsStr.getNumberOfNonWhitespaceCharacters() > 0 && trackLyricsStr != missingValueString) {
				OSStatus status = this->trackLyrics->makeTextureOfTrackLyrics(trackLyricsStr);
				if (status == noErr) {
					this->trackLyrics->calcPositionOnScreen();
					VisualNotification::post(kLyricsTextureIsAvailableMsg);
				}
			}
			break;
		case kLyricsTextureIsAvailableMsg:
			this->textureOfCurrentTrackLyricsIsAvailable = true;
			this->trackLyrics->fadeIn(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnPlayInMS));
			break;
		case kAudioPlayStartedEvt:
			this->trackLyrics->clear();
			this->textureOfCurrentTrackLyricsIsAvailable = false;
			break;
		case kAudioPlayStoppedEvt:
			this->trackLyrics->clear();
			this->textureOfCurrentTrackLyricsIsAvailable = false;
			break;
		case kAudioPlayPausedEvt:
			this->trackLyrics->fadeOut(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeOnPauseInMS), 0.15f);
			break;
		case kAudioPlayResumedEvt:
			this->trackLyrics->fadeIn(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnResumeInMS));
			break;
		case kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt:
			this->trackLyrics->fadeOut(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeBeforeEndOfTrackInMS));
			break;
		case kCanvasReshapeEvt:
			if (textureOfCurrentTrackLyricsIsAvailable == true) {
				this->trackLyrics->calcPositionOnScreen();
			}
			break;
		case kTrackInfoTextureChangedMsg:
			if (textureOfCurrentTrackLyricsIsAvailable == true) {
				this->trackLyrics->calcPositionOnScreen();
			}
			break;
		default:
			writeLog("unhandled Notification in TrackLyricsActor");
			break;
	}

}


void TrackLyricsActor::clear() {
	this->trackLyrics->clear();
}
