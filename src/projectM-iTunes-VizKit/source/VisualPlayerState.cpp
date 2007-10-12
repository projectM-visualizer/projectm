/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualPlayerState.cpp
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

#include "VisualPlayerState.h"
#include "VisualTiming.h"
#include "VisualAudioLab.h"
#include "VisualDataStore.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


VisualPlayerState* VisualPlayerState::theVisualPlayerState = NULL;


VisualPlayerState::VisualPlayerState() {
	audioPlayState = kAudioIsNotPlaying;
	trackPlayPositionInMS = 0;
	showMode = kIsNotShowing;
	debugMode = false;
	fadeOutEventHasBeenSent = false;
}


VisualPlayerState::~VisualPlayerState() {
	// null
}


VisualPlayerState* VisualPlayerState::getInstance() {
    if (theVisualPlayerState == NULL) {
		theVisualPlayerState = new VisualPlayerState;
    }
	if (theVisualPlayerState == NULL) {
		writeLog("ERR: init theVisualPlayerState failed");
	}
	return theVisualPlayerState;
}


void VisualPlayerState::dispose() {
	if (theVisualPlayerState != NULL) {
		delete theVisualPlayerState;
		theVisualPlayerState = NULL;
	}
}


bool VisualPlayerState::isAudioPlaying() const {
	if (audioPlayState == kAudioIsPlaying || audioPlayState == kAudioPlayStarted || audioPlayState == kAudioPlayResumed) {
		return true;
	} else {
		return false;
	}
}


AudioPlayState VisualPlayerState::getAudioPlayState() const {
	return this->audioPlayState;
}


PlayerShowMode VisualPlayerState::getPlayerShowMode() const {
	return this->showMode;
}


UInt32 VisualPlayerState::getElapsedAudioTime() const {
	return this->trackPlayPositionInMS;
}


bool VisualPlayerState::remainingAudioTimeIsKnown() const {
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	return theVisualAudioLab->remainingTimeOfCurrentTrackIsKnown();
}


UInt32 VisualPlayerState::getRemainingAudioTime() const {
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	return theVisualAudioLab->getRemainingTimeOfCurrentTrack(this->isAudioPlaying());
}


bool VisualPlayerState::fadeOutEventShouldBeSent() {
	bool retVal = false;
	if (this->fadeOutEventHasBeenSent == false) {
		if (this->remainingAudioTimeIsKnown() == true) {
			if (this->getRemainingAudioTime() < (UInt32)VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeBeforeEndOfTrackInMS)) {
				retVal = true;
				this->fadeOutEventHasBeenSent = true;
			}
		}
	}
	return retVal;
}


UInt32 VisualPlayerState::getElapsedAudioPlayStartTime() const {
	return VisualTiming::getElapsedMilliSecsSinceReset("trackPlayStart");
}


UInt32 VisualPlayerState::getElapsedAudioPlayStopTime() const {
	return VisualTiming::getElapsedMilliSecsSinceReset("trackPlayStop");
}


bool VisualPlayerState::isInDebugMode(void) const {
	return this->debugMode;
}


void VisualPlayerState::setAudioPlayState(AudioPlayState playState) {
	this->audioPlayState = playState;
	if (playState == kAudioPlayStarted) {
		this->fadeOutEventHasBeenSent = false;
	}
}


void VisualPlayerState::setPlayerShowMode(const PlayerShowMode aShowMode) {
	this->showMode = aShowMode;
}


void VisualPlayerState::setDebugMode(bool aDebugMode) {
	this->debugMode = aDebugMode;
}


void VisualPlayerState::setTrackPlayPositionInMS(const UInt32 positionInMS) {
	this->trackPlayPositionInMS = positionInMS;
}


void VisualPlayerState::convertAudioPlayStateToString(const AudioPlayState anAudioPlayState, char* outString) {
	char* messageString;
	switch (anAudioPlayState) {
		case kAudioIsNotPlaying:
			messageString = "kAudioIsNotPlaying";
			break;
		case kAudioPlayStarted:
			messageString = "kAudioPlayStarted";
			break;
		case kAudioIsPlaying:
			messageString = "kAudioIsPlaying";
			break;
		case kAudioPlayResumed:
			messageString = "kAudioPlayResumed";
			break;
		case kAudioPlayPaused:
			messageString = "kAudioPlayPaused";
			break;
		default:
			messageString = "unknownAudioPlayState";
	}
	strcpy(outString, messageString);
}


void VisualPlayerState::convertPlayerShowModeToString(const PlayerShowMode aShowMode, char* outString) {
	char* messageString;
	switch (aShowMode) {
		case kIsNotShowing:
			messageString = "kIsNotShowing";
			break;
		case kErrorState:
			messageString = "kErrorState";
			break;
		case kIsShowing:
		//case kIsShowingInWindow:
			messageString = "kIsShowing(InWindow)";
			break;
		case kIsShowingInFullScreen:
			messageString = "kIsShowingInFullScreen";
			break;
		default:
			messageString = "unknownShowMode";
	}
	strcpy(outString, messageString);
}
