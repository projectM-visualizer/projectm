/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualString.cpp
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

#include "VisualAudioMetaData.h"
#include "VisualDataStore.h"
#include "VisualString.h"
#include "VisualErrorHandling.h"

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacMemory.h>
#endif

#if TARGET_OS_WIN
#include <QT/macmemory.h> // DisposeHandle
#endif


using namespace VizKit;


VisualAudioMetaData::VisualAudioMetaData() {
	isStream = false;
	trackSizeInBytes = 0;
	albumCoverArtworkHasBeenEvaluated = false;
	numberOfArtworks = 0;
	albumCoverArtworkFileType = '    ';
	albumCoverArtworkHandle = NULL;
}


VisualAudioMetaData::~VisualAudioMetaData() {
	if (albumCoverArtworkHandle != NULL) {
		DisposeHandle(albumCoverArtworkHandle);
	}
}


VisualAudioMetaData::VisualAudioMetaData(const VisualAudioMetaData& other) {
	this->copy(other);
}


VisualAudioMetaData& VisualAudioMetaData::operator=(const VisualAudioMetaData& other) {
	if (this != &other) {
		if (albumCoverArtworkHandle != NULL) {
			DisposeHandle(albumCoverArtworkHandle);
		}
		this->copy(other);
	}
	return *this;
}


bool VisualAudioMetaData::operator==(const VisualAudioMetaData& other) {
	bool compareResult = false;
	if ((this->trackSizeInBytes == other.trackSizeInBytes) &&
		(this->trackName == other.trackName) &&
		(this->trackArtist == other.trackArtist) &&
		(this->trackAlbum == other.trackAlbum) &&
		(this->isStream == other.isStream)) {
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
	this->trackSizeInBytes = other.trackSizeInBytes;
	this->isStream = other.isStream;
	this->albumCoverArtworkHasBeenEvaluated = other.albumCoverArtworkHasBeenEvaluated;
	this->numberOfArtworks = other.numberOfArtworks;
	this->albumCoverArtworkFileType = other.albumCoverArtworkFileType;
	this->albumCoverArtworkHandle = other.albumCoverArtworkHandle;
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


void VisualAudioMetaData::setTrackSizeInBytes(UInt32 aTrackSizeInBytes) {
	this->trackSizeInBytes = aTrackSizeInBytes;
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


UInt32 VisualAudioMetaData::getTrackSizeInBytes() {
	return this->trackSizeInBytes;
}


bool VisualAudioMetaData::getIsStream() {
	return this->isStream;
}


UInt16 VisualAudioMetaData::getNumberOfArtworks() {
	if (this->albumCoverArtworkHasBeenEvaluated == false) {
		this->numberOfArtworks = VisualDataStore::evaluateCoverArtInfo(&this->albumCoverArtworkFileType, &this->albumCoverArtworkHandle);
		this->albumCoverArtworkHasBeenEvaluated = true;
	}
	return this->numberOfArtworks;
}


OSType VisualAudioMetaData::getAlbumCoverArtworkFileType() {
	if (this->albumCoverArtworkHasBeenEvaluated == false) {
		this->numberOfArtworks = VisualDataStore::evaluateCoverArtInfo(&this->albumCoverArtworkFileType, &this->albumCoverArtworkHandle);
		this->albumCoverArtworkHasBeenEvaluated = true;
	}
	return this->albumCoverArtworkFileType;
}


Handle VisualAudioMetaData::getAlbumCoverArtworkHandle() {
	if (this->albumCoverArtworkHasBeenEvaluated == false) {
		this->numberOfArtworks = VisualDataStore::evaluateCoverArtInfo(&this->albumCoverArtworkFileType, &this->albumCoverArtworkHandle);
		this->albumCoverArtworkHasBeenEvaluated = true;
	}
	return this->albumCoverArtworkHandle;
}


void VisualAudioMetaData::disposeAlbumCoverArtworkHandle() {
	if (this->albumCoverArtworkHandle != NULL) {
		DisposeHandle(this->albumCoverArtworkHandle);
		this->albumCoverArtworkHandle = NULL;
	}
	this->numberOfArtworks = 0;
	this->albumCoverArtworkFileType = '    ';
	this->albumCoverArtworkHasBeenEvaluated = false;
}
