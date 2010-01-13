/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: BeatlightActor.cpp
 *
 */

/*************************************************************************

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

 *************************************************************************/


#include "BeatlightActor.h"

#include "Beatlight.h"
#include "VisualErrorHandling.h"
#include "VisualPlayerState.h"
#include "VisualNotification.h"

using namespace VizKit;


BeatlightActor::BeatlightActor() {
	strcpy(actorName, "BEATLIGHT");
	state = kVisActOn;
    beatlight = new Beatlight;
	
}


BeatlightActor::~BeatlightActor() {
    delete beatlight;
	beatlight = NULL;
}


void BeatlightActor::init() {
	VisualNotification::registerNotification(this, kBeatImpulseEvt);
	VisualNotification::registerNotification(this, kCanvasReshapeEvt);
}


void BeatlightActor::show(const VisualPlayerState& visualPlayerState) {
	bool audioIsPlaying = visualPlayerState.isAudioPlaying();
    this->beatlight->showBeatlight(audioIsPlaying);
	//beatlight->showBeatMeterLight(playerState->isAudioPlaying());
}



void BeatlightActor::handleNotification(VisualNotification& aNotification) {

	// VisualActor::handleNotification(aNotification); // debug

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
	
		case kBeatImpulseEvt:
			this->beatlight->applyBehavior();
			break;
		/*
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
		*/
		case kCanvasReshapeEvt:
			this->beatlight->reshape();
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
