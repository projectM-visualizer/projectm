/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualNotification.cpp
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

#include "VisualNotification.h"
#include "VisualNotificationQueue.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


VisualNotification::VisualNotification() {
	notificationEnumKey = kNoNotificationKey;
	notificationValue = NULL;
	notificationValueLength = 0;
	callbackFunction = NULL;
	userData = NULL;
}


VisualNotification::~VisualNotification() {
	if (notificationValue != NULL) {
		free(notificationValue);
	}
}


VisualNotification::VisualNotification(const VisualNotification& other) {
	copy(other);
}


VisualNotification& VisualNotification::operator=(const VisualNotification& other) {
	if (this != &other) {
		if (this->notificationValue != NULL) {
			free(this->notificationValue);
		}
		this->copy(other);
	}
	return *this;
}


void VisualNotification::copy(const VisualNotification& other) {
	this->notificationEnumKey = other.notificationEnumKey;
	if (other.notificationValue != NULL) {
		this->notificationValue = (void*)malloc(other.notificationValueLength * sizeof(unsigned char));
		memcpy(this->notificationValue, other.notificationValue, other.notificationValueLength);
	} else {
		this->notificationValue = NULL;
	}
	this->notificationValueLength = other.notificationValueLength;
	this->callbackFunction = other.callbackFunction;
	this->userData = other.userData;
}


void VisualNotification::setKey(const VisualNotificationKey aKey) {
    this->notificationEnumKey = aKey;
}


VisualNotificationKey VisualNotification::getKey() const {
	return this->notificationEnumKey;
}


void VisualNotification::setValue(const void* const aValue, UInt32 valueLengthInBytes) {
	if (this->notificationValue != NULL) {
		free(this->notificationValue);
		this->notificationValue = NULL;
	}
	this->notificationValue = (void*)malloc(valueLengthInBytes * sizeof(char));
	memcpy(this->notificationValue, aValue, valueLengthInBytes);
	this->notificationValueLength = valueLengthInBytes;
}


const void* const VisualNotification::getValue(UInt32& numberOfBytes) const {
	numberOfBytes = this->notificationValueLength;
    return this->notificationValue;
}


void VisualNotification::setCallbackFunction(VisualNotificationCallback aCallbackFunction, void* someUserData) {
	this->callbackFunction = aCallbackFunction;
	this->userData = someUserData;
}


void VisualNotification::post() {
	VisualNotificationQueue::push(*this);
}


void VisualNotification::callCallbackFunction() {
	this->callbackFunction(this->userData);
}


void VisualNotification::post(const VisualNotificationKey aKey) {
	VisualNotification aNotification;
	aNotification.setKey(aKey);
	VisualNotificationQueue::push(aNotification);
}


void VisualNotification::convertNotificationKeyToString(const VisualNotificationKey aKey, char* outString) {
	char* messageString;
	switch (aKey) {
		case kNoNotificationKey:
			messageString = "kNoNotificationKey";
			break;
		case kKeyPressedEvt:
			messageString = "kKeyPressedEvt";
			break;
		case kAudioPlayStartedEvt:
			messageString = "kAudioPlayStartedEvt";
			break;
		case kAudioPlayStoppedEvt:
			messageString = "kAudioPlayStoppedEvt";
			break;
		case kAudioPlayPausedEvt:
			messageString = "kAudioPlayPausedEvt";
			break;
		case kAudioPlayResumedEvt:
			messageString = "kAudioPlayResumedEvt";
			break;
		case kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt:
			messageString = "kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt";
			break;
		case kToggleShowProcessMonitorMsg:
			messageString = "kToggleShowProcessMonitorMsg";
			break;
		case kToggleProcessMonitorAudioInfoMsg:
			messageString = "kToggleProcessMonitorAudioInfoMsg";
			break;
		case kCanvasReshapeEvt:
			messageString = "kCanvasReshapeEvt";
			break;
		case kAudioMetadataIsAvailableMsg:
			messageString = "kAudioMetadataIsAvailableMsg";
			break;
		case kTrackInfoTextureIsAvailableMsg:
			messageString = "kTrackInfoTextureIsAvailableMsg";
			break;
		case kCoverTextureIsAvailableMsg:
			messageString = "kCoverTextureIsAvailableMsg";
			break;
		case kLyricsAreAvailableMsg:
			messageString = "kLyricsAreAvailableMsg";
			break;
		case kLyricsTextureIsAvailableMsg:
			messageString = "kLyricsTextureIsAvailableMsg";
			break;
		case kTrackInfoTextureChangedMsg:
			messageString = "kTrackInfoTextureChangedMsg";
			break;
		default:
			messageString = "unknownNotificationKey";
	}
	strcpy(outString, messageString);
}
