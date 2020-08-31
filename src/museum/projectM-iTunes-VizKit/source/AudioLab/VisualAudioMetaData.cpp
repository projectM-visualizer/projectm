/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAudioMetaData.cpp
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

#include "VisualAudioMetaData.h"
#include "VisualDataStore.h"
#include "VisualString.h"
#include "VisualErrorHandling.h"
#include "VisualMainAction.h"
#include "VisualItemIdentifier.h"


using namespace VizKit;


VisualAudioMetaData::VisualAudioMetaData() {
	trackIdentifier = NULL;
	init();
}


VisualAudioMetaData::~VisualAudioMetaData() {
	delete trackIdentifier;
}


VisualAudioMetaData::VisualAudioMetaData(const VisualAudioMetaData& other) {
	this->copy(other);
}


VisualAudioMetaData& VisualAudioMetaData::operator=(const VisualAudioMetaData& other) {
	
	if (this == &other) return *this;

	delete this->trackIdentifier;

	this->copy(other);

	return *this;
}


bool VisualAudioMetaData::operator==(const VisualAudioMetaData& other) {
	bool compareResult = false;
	if ((this->trackSizeInBytes == other.trackSizeInBytes) &&
		(this->trackName == other.trackName) &&
		(this->trackArtist == other.trackArtist) &&
		(this->trackAlbum == other.trackAlbum) &&
		(this->isStream == other.isStream) &&
		(this->streamTitle == other.streamTitle)) {
		compareResult = true;
	}
	return compareResult;
}


bool VisualAudioMetaData::operator!=(const VisualAudioMetaData& other) {
	return !(*this == other);
}


void VisualAudioMetaData::copy(const VisualAudioMetaData& other) {
	this->trackName = other.trackName;
	this->trackArtist = other.trackArtist;
	this->trackAlbum = other.trackAlbum;
	this->trackLyrics = other.trackLyrics;
	this->streamTitle = other.streamTitle;
	this->trackSizeInBytes = other.trackSizeInBytes;
	this->isStream = other.isStream;
	this->trackIdentifier = new VisualItemIdentifier(*(other.trackIdentifier));
}


void VisualAudioMetaData::setIsStream(bool isAStream) {
	this->isStream = isAStream;
}


void VisualAudioMetaData::setTrackName(const VisualString& aTrackName) {
	this->trackName = aTrackName;
}


void VisualAudioMetaData::setTrackArtist(const VisualString& aTrackArtist) {
	this->trackArtist = aTrackArtist;
}


void VisualAudioMetaData::setTrackAlbum(const VisualString& aTrackAlbum) {
	this->trackAlbum = aTrackAlbum;
}


void VisualAudioMetaData::setTrackLyrics(const VisualString& someTrackLyrics) {
	this->trackLyrics = someTrackLyrics;
}


void VisualAudioMetaData::setTrackComposer(const VisualString& aComposer) {
	this->trackComposer = aComposer;
}


void VisualAudioMetaData::setStreamTitle(const VisualString& aStreamTitle) {
	this->streamTitle = aStreamTitle;
}


void VisualAudioMetaData::setStreamMessage(const VisualString& aStreamMessage) {
	this->streamMessage = aStreamMessage;
}


void VisualAudioMetaData::setStreamURL(const VisualString& aStreamURL) {
	this->streamURL = aStreamURL;
}


void VisualAudioMetaData::setTrackSizeInBytes(uint32 aTrackSizeInBytes) {
	this->trackSizeInBytes = aTrackSizeInBytes;
}


void VisualAudioMetaData::setYear(uint16 aYear) {
	this->year = aYear;
}


const VisualString& VisualAudioMetaData::getTrackName() {
	return this->trackName;
}


const VisualString& VisualAudioMetaData::getTrackArtist() {
	return this->trackArtist;
}


const VisualString& VisualAudioMetaData::getTrackAlbum() {
	return this->trackAlbum;
}


const VisualString& VisualAudioMetaData::getTrackLyrics() {
	return this->trackLyrics;
}


const VisualString& VisualAudioMetaData::getTrackComposer() {
	return this->trackComposer;
}


const VisualString& VisualAudioMetaData::getStreamTitle() {
	return this->streamTitle;
}


const VisualString& VisualAudioMetaData::getStreamMessage() {
	return this->streamMessage;
}


const VisualString& VisualAudioMetaData::getStreamURL() {
	return this->streamURL;
}


uint32 VisualAudioMetaData::getTrackSizeInBytes() {
	return this->trackSizeInBytes;
}


uint16 VisualAudioMetaData::getYear() {
	return this->year;
}


VisualItemIdentifier VisualAudioMetaData::getTrackIdentifier() {
	return *this->trackIdentifier;
}


bool VisualAudioMetaData::getIsStream() {
	return this->isStream;
}


void VisualAudioMetaData::init() {
	this->isStream = false;
	this->trackSizeInBytes = 0;
	this->year = 0;
	this->trackName.clear();
	this->trackArtist.clear();
	this->trackAlbum.clear();
	this->trackLyrics.clear();
	this->trackComposer.clear();
	this->streamTitle.clear();
	this->streamMessage.clear();
	this->streamURL.clear();
	if (this->trackIdentifier != NULL) {
		delete this->trackIdentifier;
	}
	this->trackIdentifier = new VisualItemIdentifier;
}
