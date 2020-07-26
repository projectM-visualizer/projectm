/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNotification.cpp
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

#include "VisualNotification.h"
#include "VisualNotificationQueue.h"
#include "VisualErrorHandling.h"
#include "VisualStageControl.h"


using namespace VizKit;


VisualNotification::VisualNotification() {
	notificationEnumKey = kNoNotificationKey;
	notificationData = NULL;
	notificationDataSize = 0;
	notificationPointer = NULL;
	notificationObject = NULL;
}


VisualNotification::VisualNotification(const VisualItemIdentifier& anIdentifier) : VisualObject(anIdentifier) {
	notificationEnumKey = kNoNotificationKey;
	notificationData = NULL;
	notificationDataSize = 0;
	notificationPointer = NULL;
	notificationObject = NULL;
}


VisualNotification::~VisualNotification() {
	if (notificationData != NULL) {
		free(notificationData);
	}
	if (notificationObject != NULL) {
		delete notificationObject;
	}
}


VisualNotification::VisualNotification(const VisualNotification& other) : VisualObject(other) {
	copy(other);
}


VisualNotification& VisualNotification::operator=(const VisualNotification& other) {
	
	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	if (this->notificationData != NULL) {
		free(this->notificationData);
	}
	if (this->notificationObject != NULL) {
		delete this->notificationObject;
	}
	this->copy(other);

	return *this;
}


void VisualNotification::copy(const VisualNotification& other) {
	this->notificationEnumKey = other.notificationEnumKey;
	if (other.notificationData != NULL) {
		this->notificationData = (void*)malloc(other.notificationDataSize * sizeof(char));
		memcpy(this->notificationData, other.notificationData, other.notificationDataSize);
	} else {
		this->notificationData = NULL;
	}
	this->notificationDataSize = other.notificationDataSize;
	this->notificationPointer = other.notificationPointer;
	if (other.notificationObject != NULL) {
		this->notificationObject = other.notificationObject->clone();
	} else {
		this->notificationObject = NULL;
	}
}


VisualNotification* VisualNotification::clone(void) const {
	return new VisualNotification(*this);
}


void VisualNotification::setKey(const VisualNotificationKey aKey) {
    this->notificationEnumKey = aKey;
}


VisualNotificationKey VisualNotification::getKey() const {
	return this->notificationEnumKey;
}


void VisualNotification::setData(const void* const someData, uint32 dataSizeInBytes) {
	if (this->notificationData != NULL) {
		free(this->notificationData);
		this->notificationData = NULL;
	}
	this->notificationData = (void*)malloc(dataSizeInBytes * sizeof(char));
	memcpy(this->notificationData, someData, dataSizeInBytes);
	this->notificationDataSize = dataSizeInBytes;
}


void* VisualNotification::getData(uint32& dataSizeInBytes) const {
	dataSizeInBytes = this->notificationDataSize;
    return this->notificationData;
}


void VisualNotification::setObject(const VisualObject& anObject) {
	if (this->notificationObject != NULL) {
		delete this->notificationObject;
	}
	this->notificationObject = anObject.clone();
}


VisualObject* VisualNotification::getObject() const {
	return this->notificationObject;
}


void VisualNotification::setPointer(void* pointer) {
	this->notificationPointer = pointer;
}


void* VisualNotification::getPointer() {
	return this->notificationPointer;
}


void VisualNotification::post() {
	VisualNotificationQueue::push(*this);
}


void VisualNotification::post(const VisualNotificationKey aKey) {
	VisualNotification aNotification;
	aNotification.setKey(aKey);
	VisualNotificationQueue::push(aNotification);
}


void VisualNotification::registerNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey) {
	VisualStageControl::registerObserverForNotification(aVisualActor, aNotificationKey);
}


void VisualNotification::removeNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey) {
	VisualStageControl::removeObserverOfNotification(aVisualActor, aNotificationKey);
}


void VisualNotification::convertNotificationKeyToString(const VisualNotificationKey aKey, char* outString) {
	const char* messageString;
	switch (aKey) {
		case kNoNotificationKey:
			messageString = "kNoNotificationKey";
			break;
		case kKeyPressedEvt:
			messageString = "kKeyPressedEvt";
			break;
		case kAudioPlayTrackChangedEvt:
			messageString = "kAudioPlayTrackChangedEvt";
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
		case kAudioPlayReachedFadeOutTimeEvt:
			messageString = "kAudioPlayReachedFadeOutTimeEvt";
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
		case kBeatImpulseEvt:
			messageString = "kBeatImpulseEvt";
			break;
		case kTrackInfoImageMsg:
			messageString = "kTrackInfoImageMsg";
			break;
		case kTrackLyricsImageMsg:
			messageString = "kTrackLyricsImageMsg";
			break;
		case kLoadingEncodedImageDataCompletedMsg:
			messageString = "kLoadingEncodedImageDataCompletedMsg";
			break;
		case kImageHistogramCompletedMsg:
			messageString = "kImageHistogramCompletedMsg";
			break;
		case kImageWithIdentifierLoadedAndCreatedMsg:
			messageString = "kImageWithIdentifierLoadedAndCreatedMsg";
			break;
		case kImageWithIdentifierMsg:
			messageString = "kImageWithIdentifierMsg";
			break;
		case kImageWriteToPNGFileMsg:
			messageString = "kImageWriteToPNGFileMsg";
			break;
		case kImageWriteToPNGFileAndDeleteMsg:
			messageString = "kImageWriteToPNGFileAndDeleteMsg";
			break;
		case kGetTrackInfoStringStyleMsg:
			messageString = "kGetTrackInfoStringStyleMsg";
			break;
		case kGetTrackLyricsStringStyleMsg:
			messageString = "kGetTrackLyricsStringStyleMsg";
			break;
		case kStyledStringMsg:
			messageString = "kStyledStringMsg";
			break;
		case kStringWithIdentifierLoadedAndCreatedMsg:
			messageString = "kStringWithIdentifierLoadedAndCreatedMsg";
			break;
		case kLyricsAreWrittenIntoTempFileMsg:
			messageString = "kLyricsAreWrittenIntoTempFileMsg";
			break;
		case kLyricsAreAvailableInMetadataMsg:
			messageString = "kLyricsAreAvailableInMetadataMsg";
			break;
		case kTrackInfoTextureChangedMsg:
			messageString = "kTrackInfoTextureChangedMsg";
			break;
			
		default:
			messageString = "unknownNotificationKey";
	}
	strcpy(outString, messageString);
}
