/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualDataStore.cpp
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

#include "VisualDataStore.h"
#include "VisualHostCommunication.h"
#include "VisualErrorHandling.h"
#include "VisualAudioLab.h"
#include "VisualConfiguration.h"
#include "VisualPreferences.h"
#include "VisualThreading.h"
#include "VisualNotification.h"
#include "VisualString.h"
#include "VisualFile.h"
#include "VisualStyledString.h"
#include "VisualImage.h"
#include "VisualPlayerState.h"
#include "VisualQuickTime.h"

#if TARGET_OS_MAC
#include "VisualAppleScript.h"
#endif
#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacMemory.h> // HUnlock
#endif


#if TARGET_OS_WIN
#include "iTunesCOMInterface.h"
#endif


using namespace VizKit;


const size_t VisualDataStore::audioMetaDataHistoryCount = 2;

bool VisualDataStore::lyricsCreationThreadIsRunning = false;

VisualDataStore* VisualDataStore::theVisualDataStore = NULL;  // singleton


const VisualItemIdentifier VisualDataStore::albumCoverArtworkImageId;
VisualItemIdentifier VisualDataStore::fileWithLyricsStringId;
VisualItemIdentifier VisualDataStore::trackIdentifierOfLyricsMetadata;
const VisualItemIdentifier VisualDataStore::styledTrackInfoStringId;
const VisualItemIdentifier VisualDataStore::styledTrackLyricsStringId;
const VisualItemIdentifier VisualDataStore::updateInformationStringId;


VisualDataStore::VisualDataStore() {
	VisualAudioMetaData emptyVisualAudioMetaData;
    for (uint8 i = 0; i < audioMetaDataHistoryCount; i++) {
		audioMetaDataHistory.push_back(emptyVisualAudioMetaData);
    }
	currAudioMetaDataHistoryIdx = audioMetaDataHistoryCount - 1;
	appVersionMajorNum = 0;
	appVersionMinorNum = 0;
}


VisualDataStore::~VisualDataStore() {

	processInfoMap.clear();
	
	audioMetaDataHistory.clear();
}


VisualDataStore* VisualDataStore::getInstance() {
    if (theVisualDataStore == NULL) {
		theVisualDataStore = new VisualDataStore;
    }
	return theVisualDataStore;
}


void VisualDataStore::dispose() {
	if (theVisualDataStore != NULL) {
		delete theVisualDataStore;
		theVisualDataStore = NULL;
	}
}


void VisualDataStore::setValue(const ValueKey anIdentifier, const int anIntValue) {
	bool found = false;

	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			(*it)->valueInt = anIntValue;
			found = true;
		}
	}
	if (found == false) {
		Value* aValue = new Value;
		aValue->dataType = kIntPrefType;
		aValue->key = anIdentifier;
		aValue->valueInt = anIntValue;
		strcpy(aValue->valueChar, "\0");
		aValue->valueBool = false;
		theVisualDataStore->valueVector.push_back(aValue);
	}
}


void VisualDataStore::setValue(const ValueKey anIdentifier, const float aFloatValue) {
	bool found = false;

	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			(*it)->valueFloat = aFloatValue;
			found = true;
		}
	}
	if (found == false) {
		Value* aValue = new Value;
		aValue->dataType = kFloatPrefType;
		aValue->key = anIdentifier;
		aValue->valueFloat = aFloatValue;
		aValue->valueInt = 0;
		strcpy(aValue->valueChar, "\0");
		aValue->valueBool = false;
		theVisualDataStore->valueVector.push_back(aValue);
	}
}


void VisualDataStore::setValue(const ValueKey anIdentifier, const char* const aCharValue) {
	bool found = false;
	
	char errStr[256] = "\0";
	if (strlen(aCharValue) > 255) {
		strncat(errStr, aCharValue, 255);
		writeLog("char value too long");
		return;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy((*it)->valueChar, aCharValue);
			found = true;
		}
	}
	if (found == false) {
		Value* aValue = new Value;
		aValue->dataType = kCharPrefType;
		aValue->key = anIdentifier;
		strcpy(aValue->valueChar, aCharValue);
		aValue->valueInt = 0;
		aValue->valueBool = false;
		theVisualDataStore->valueVector.push_back(aValue);
	}
}


void VisualDataStore::setValue(const ValueKey anIdentifier, const bool aBoolValue) {
	bool found = false;
	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kBoolPrefType) {
			(*it)->valueBool = aBoolValue;
			found = true;
		}
	}
	if (found == false) {
		Value* aValue = new Value;
		aValue->dataType = kBoolPrefType;
		aValue->key = anIdentifier;
		aValue->valueBool = aBoolValue;
		aValue->valueInt = 0;
		aValue->valueFloat = 0.0f;
		strcpy(aValue->valueChar, "\0");
		theVisualDataStore->valueVector.push_back(aValue);
	}
}


int VisualDataStore::getValueInt(const ValueKey anIdentifier, bool* wasSet) {

	int prefVal = 0;
	
	if (wasSet != NULL) {
		*wasSet = false;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			if (wasSet != NULL) {
				*wasSet = true;
			}
			return (*it)->valueInt;
		}
	}
	
	return prefVal;
}


float VisualDataStore::getValueFloat(const ValueKey anIdentifier, bool* wasSet) {
	
	float prefVal = 0;
	
	if (wasSet != NULL) {
		*wasSet = false;
	}

	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			if (wasSet != NULL) {
				*wasSet = true;
			}
			return (*it)->valueFloat;
		}
	}

	return prefVal;
}


void VisualDataStore::getValueChar(const ValueKey anIdentifier, char* outPrefVal, bool* wasSet) {
	
	strcpy(outPrefVal, "\0");
	
	if (wasSet != NULL) {
		*wasSet = false;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			if (wasSet != NULL) {
				*wasSet = true;
			}
			strncat(outPrefVal, (*it)->valueChar, strlen((*it)->valueChar));
			return;
		}
	}
	
}


bool VisualDataStore::getValueBool(const ValueKey anIdentifier, bool* wasSet) {

	bool prefVal = false;
	
	if (wasSet != NULL) {
		*wasSet = false;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (ValueVectorIterator it = theVisualDataStore->valueVector.begin(); it != theVisualDataStore->valueVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kBoolPrefType) {
			if (wasSet != NULL) {
				*wasSet = true;
			}
			return (*it)->valueBool;
		}
	}
	
	return prefVal;
}


void VisualDataStore::setAppVersionNum(uint8 majorVersionNum, uint8 minorVersionNum) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->appVersionMajorNum = majorVersionNum;
	theVisualDataStore->appVersionMinorNum = minorVersionNum;
}


uint8 VisualDataStore::getAppVersionMajorNum() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->appVersionMajorNum;
}


uint8 VisualDataStore::getAppVersionMinorNum() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->appVersionMinorNum;
}


void VisualDataStore::resetCurrAudioMetaData(void) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->currAudioMetaData.init();
}


void VisualDataStore::setAudioTrackName(const uint16* const audioTrackName, uint32 audioTrackNameLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString trackName(audioTrackName, audioTrackNameLength);
	theVisualDataStore->currAudioMetaData.setTrackName(trackName);
}


void VisualDataStore::setAudioTrackArtistName(const uint16* const audioTrackArtistName, uint32 audioTrackArtistNameLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString trackArtistName(audioTrackArtistName, audioTrackArtistNameLength);
	theVisualDataStore->currAudioMetaData.setTrackArtist(trackArtistName);
}


void VisualDataStore::setAudioTrackAlbumName(const uint16* const audioTrackAlbumName, uint32 audioTrackAlbumNameLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString trackAlbumName(audioTrackAlbumName, audioTrackAlbumNameLength);
	theVisualDataStore->currAudioMetaData.setTrackAlbum(trackAlbumName);
}


void VisualDataStore::setAudioTrackComposer(const uint16* const audioTrackComposer, uint32 audioTrackComposerLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString trackComposer(audioTrackComposer, audioTrackComposerLength);
	theVisualDataStore->currAudioMetaData.setTrackComposer(trackComposer);
}


void VisualDataStore::setAudioStreamTitle(const char* const audioStreamTitle, uint32 audioStreamTitleLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString streamTitle(audioStreamTitle, audioStreamTitleLength);
	theVisualDataStore->currAudioMetaData.setStreamTitle(streamTitle);
}


void VisualDataStore::setAudioStreamTitle(const uint16* const audioStreamTitle, uint32 audioStreamTitleLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString streamTitle(audioStreamTitle, audioStreamTitleLength);
	theVisualDataStore->currAudioMetaData.setStreamTitle(streamTitle);
}


void VisualDataStore::setAudioStreamMessage(const char* const audioStreamMessage, uint32 audioStreamMessageLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString streamMessage(audioStreamMessage, audioStreamMessageLength);
	theVisualDataStore->currAudioMetaData.setStreamMessage(streamMessage);
}


void VisualDataStore::setAudioStreamMessage(const uint16* const audioStreamMessage, uint32 audioStreamMessageLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString streamMessage(audioStreamMessage, audioStreamMessageLength);
	theVisualDataStore->currAudioMetaData.setStreamMessage(streamMessage);
}


void VisualDataStore::setAudioStreamURL(const char* const audioStreamURL, uint32 audioStreamURLLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString streamURL(audioStreamURL, audioStreamURLLength);
	theVisualDataStore->currAudioMetaData.setStreamURL(streamURL);
}


void VisualDataStore::setAudioStreamURL(const uint16* const audioStreamURL, uint32 audioStreamURLLength) {
	theVisualDataStore = VisualDataStore::getInstance();
	VisualString streamURL(audioStreamURL, audioStreamURLLength);
	theVisualDataStore->currAudioMetaData.setStreamURL(streamURL);
}


void VisualDataStore::setAudioTrackSizeInBytes(int sizeInBytes) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->currAudioMetaData.setTrackSizeInBytes(sizeInBytes);
}


void VisualDataStore::setAudioTrackYear(uint16 aYear) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->currAudioMetaData.setYear(aYear);
}


void VisualDataStore::setAudioDataIsStream(bool isStream) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->currAudioMetaData.setIsStream(isStream);
}


void VisualDataStore::setLyricsOfCurrentTrack(const VisualString& lyricsString) {
	theVisualDataStore = VisualDataStore::getInstance();

	/*
	VisualFile aFile;
	aFile.initWithUserDesktopDirectory();
	VisualString fileName;
	fileName.initWithUTF8Buffer("lyricsTest.txt", 15);
	aFile.appendFileName(fileName);
	const uint16* const data = lyricsString.clone()->getUtf16Representation();
	uint32 length = lyricsString.getNumberOfCharacters();
	VisualFile::writeDataToFile((void**)&data, length * sizeof(uint16), aFile);
	 */
	
	VisualString* normalizedLyrics = lyricsString.clone();
	if (normalizedLyrics) {
		normalizedLyrics->normalizeLineEndings();
		theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].setTrackLyrics(*normalizedLyrics);
		delete normalizedLyrics;
	}
}


bool VisualDataStore::analyzeCurrentlySetMetadata() {
	bool audioTrackDidChange = false;
	theVisualDataStore = VisualDataStore::getInstance();
	if (theVisualDataStore->currAudioMetaData != theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]) {
		theVisualDataStore->advanceAudioMetaDataHistory();
		theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx] = theVisualDataStore->currAudioMetaData;
		audioTrackDidChange = true;
	}
	return audioTrackDidChange;
}


void VisualDataStore::advanceAudioMetaDataHistory() {
	if ((this->currAudioMetaDataHistoryIdx + 1) >= this->audioMetaDataHistory.size()) {
		this->currAudioMetaDataHistoryIdx = 0;
	} else {
		this->currAudioMetaDataHistoryIdx++;
	}
}


VisualString VisualDataStore::getInfoOfCurrentAudioDataForDisplay() {
	VisualString displayInfo;
	if (VisualDataStore::currentlyPlayingAudioIsStream()) {
		const VisualString& streamTitle = theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getStreamTitle();
		const VisualString& streamMessage = theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getStreamMessage();
		if (!streamTitle.isEmpty()) {
			displayInfo = streamTitle;
		} else if (!streamMessage.isEmpty()) {
			displayInfo = streamMessage;
		}
	} else {
		const VisualString& trackArtist = theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackArtist();
		if (!trackArtist.isEmpty()) {
			displayInfo = trackArtist;
		}
		const VisualString& trackName = theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackName();
		if (!trackName.isEmpty()) {
			if (!displayInfo.isEmpty()) {
				char trackInfoDisplayItemsDelimiterCharacters[32];
				VisualPreferences::getValue(VisualPreferences::kTrackInfoDisplayItemsDelimiterCharacters, trackInfoDisplayItemsDelimiterCharacters);
				displayInfo = (displayInfo + trackInfoDisplayItemsDelimiterCharacters);
			}
			displayInfo = (displayInfo + trackName);
		}
	}
	return displayInfo;
}


uint32 VisualDataStore::getTrackSizeInBytes() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackSizeInBytes();
}


uint16 VisualDataStore::getTrackYear() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getYear();
}


VisualString VisualDataStore::getNameOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackName();
}


VisualString VisualDataStore::getArtistOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackArtist();
}


VisualString VisualDataStore::getAlbumOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackAlbum();
}


VisualString VisualDataStore::getLyricsOfCurrentTrack() {
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackLyrics();
}


VisualItemIdentifier VisualDataStore::getIdentifierOfCurrentTrack() {
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getTrackIdentifier();
}


bool VisualDataStore::currentlyPlayingAudioIsStream(void) {
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].getIsStream();
}


void VisualDataStore::setProcessInfo(const char* const labelStr, const char* const valueStr) {
	std::string labelString(labelStr);
	std::string valueString(valueStr);
	ProcessInfoMapIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	it = theVisualDataStore->processInfoMap.find(labelString);
	if (it == theVisualDataStore->processInfoMap.end()) {
		theVisualDataStore->processInfoMap[labelString] = valueString;
	} else {
		it->second = valueString;
	}
}


const std::map<std::string, std::string>* const VisualDataStore::getProcessInfoMap() {
	theVisualDataStore = VisualDataStore::getInstance();
	return &theVisualDataStore->processInfoMap;
}


VisualImage* VisualDataStore::createCoverArtImage() {

	size_t numberOfBytes = 0;
	uint16 numberOfArtworks = 0;
	void* albumCoverArtworkImageData = NULL;

	numberOfArtworks = VisualHostCommunication::getCurrentTrackCoverArt(&albumCoverArtworkImageData, (uint32&)numberOfBytes);

	if (numberOfArtworks == 0) return NULL;
	
	VisualImage* anImage = new VisualImage;
	const char* coverArtDataPtr = (const char*)albumCoverArtworkImageData;
	bool success = anImage->initWithEncodedData(coverArtDataPtr, numberOfBytes);

	VisualHostCommunication::dispose();
	
	if (!success) {
		return NULL;
	} else if (anImage->isEmpty()) {
		delete anImage;
		return NULL;
	}
	
	return anImage;
}


bool VisualDataStore::createLyricsOfCurrentTrack() {
	bool success = false;
	if (VisualDataStore::lyricsCreationThreadIsRunning == true) return false;
	success = VisualThreading::createThread((ThreadingFuncPtr)VisualDataStore::writeLyricsToFile);
	return success;
}


#if TARGET_OS_MAC
OSStatus VisualDataStore::writeLyricsToFile(void* parameter) {

	bool success = true;

	VisualDataStore::lyricsCreationThreadIsRunning = true;
	
	VisualItemIdentifier currTrackId = VisualDataStore::getIdentifierOfCurrentTrack();
	VisualDataStore::trackIdentifierOfLyricsMetadata = currTrackId;
	
	bool debug = false;
	
	if (debug == true) {
		writeLog("writeLyricsToFile (begin)");
	}
	
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	
	VisualString tempFileName = VisualString("VizKitLyrics.tmp.txt");
	VisualFile tempFile;
	success = tempFile.initWithDirectoryOfTemporaryItems();
	if (!success) {
		return (OSStatus)1001;
	}
	success = tempFile.appendFileName(tempFileName);
	if (!success) {
		return (OSStatus)1002;
	}

	if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) != kIsShowing) {
		return (OSStatus)1003;
	}
	
	VisualFile appleScriptFile;
	success = appleScriptFile.initWithResourcesDirectory();
	if (!success) {
		return (OSStatus)1004;
	}
	VisualString appleScriptFileName = VisualString("getLyrics.applescript");
	success = appleScriptFile.appendFileName(appleScriptFileName);
	if (!success) {
		return (OSStatus)1005;
	}

	if (debug == true) {
		VisualString appleScriptFilePath;
		appleScriptFile.getFilePath(appleScriptFilePath);
		VisualString tempFilePath;
		tempFile.getFilePath(tempFilePath);
		char logStr[256];
		sprintf(logStr, "before executeScriptFile(), %s %s", appleScriptFilePath.getUtf8Representation(), tempFilePath.getUtf8Representation());
		writeLog(logStr);
	}

	VisualAppleScript::executeScriptFile(appleScriptFile);

	if (debug == true) {
		writeLog("after executeScriptFile()");
	}
	
	VisualNotification lyricsAreWrittenIntoTempFileNotification;
	lyricsAreWrittenIntoTempFileNotification.setKey(kLyricsAreWrittenIntoTempFileMsg);
	lyricsAreWrittenIntoTempFileNotification.setObject(tempFile);
	lyricsAreWrittenIntoTempFileNotification.post();

	if (debug == true) {
		writeLog("---------------------- writeLyricsToFile (end)");
	}

	VisualDataStore::lyricsCreationThreadIsRunning = false;
	
	return noErr;
}

#endif

#if TARGET_OS_WIN
DWORD WINAPI VisualDataStore::writeLyricsToFile(LPVOID lpParam) {

	VisualDataStore::lyricsCreationThreadIsRunning = true;

	DWORD retVal = 0;

	IiTunes* iITunes = 0;
	IITTrack* iITrack = 0;

	CoInitialize(0);
	//HRESULT result = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	
	try
	{
		HRESULT hRes = ::CoCreateInstance(CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, IID_IiTunes, (PVOID *)&iITunes);

		if(hRes == S_OK && iITunes) {
			iITunes->get_CurrentTrack(&iITrack);
			if(iITrack) {

				BSTR bstrTrack = 0;

				iITrack->get_Name((BSTR *)&bstrTrack);

				BSTR bstrLyricsTrack = ::SysAllocString((const OLECHAR*)(""));
				//BSTR bstrLyricsTrack = 0;
				IITFileOrCDTrack* iTrackFile;
				iITrack->QueryInterface(IID_IITFileOrCDTrack, (void **)&iTrackFile);
				if (iTrackFile) {
					iTrackFile->get_Lyrics(&bstrLyricsTrack);
				}
				theVisualDataStore = VisualDataStore::getInstance();

				uint16 trackLyricsStringLength = lstrlenW(bstrLyricsTrack);
				VisualString lyrics((uint16*)bstrLyricsTrack, trackLyricsStringLength);
				VisualDataStore::setLyricsOfCurrentTrack(lyrics);
				//theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx].setTrackLyrics(lyrics);
				//dynamic_cast<IITFileOrCDTrack*>(iITrack)->get_Lyrics((BSTR *)&bstrLyricsTrack);
				//dynamic_cast<IITFileOrCDTrack*>(iITrack)->get_Lyrics((BSTR *)&theVisualDataStore->trackLyrics);

			}
		}
		
		iITunes->Release();

	}
	catch(...)
	{
		try
		{
			if(iITunes)
				iITunes->Release();
					
			if(iITrack)
				iITrack->Release();
		}
		catch(...)
		{
		}
	}
	
	CoUninitialize();


/*
	iTunesLib.iTunesAppClass itl = new iTunesLib.iTunesAppClass();
	string currTrack = itl.CurrentTrack.Name.ToString();
	iTunesLib.IITTrack myTrack = (iTunesLib.IITTrack)itl.CurrentTrack;
	iTunesLib.IITFileOrCDTrack test = (iTunesLib.IITFileOrCDTrack)myTrack;
	string myLyrics = test.Lyrics;

	MessageBox.Show("Track: "+currTrack+", Lyrics: "+myLyrics);
*/

	VisualNotification::post(kLyricsAreAvailableInMetadataMsg);

	VisualDataStore::lyricsCreationThreadIsRunning = false;

	return retVal;
}
#endif



#if TARGET_OS_MAC
OSStatus VisualDataStore::readFileAndSendNotificationWithString(void* fileWithStringContent) {
	OSStatus retVal = noErr;
#endif
#if TARGET_OS_WIN
DWORD VisualDataStore::readFileAndSendNotificationWithString(LPVOID fileWithStringContent) {
	DWORD retVal = 0;
#endif

	bool debug = false;

	if (debug == true) {
		writeLog("readFileAndSendNotificationWithString (start)");
	}
	
	VisualFile* visualFile = reinterpret_cast<VisualFile*>(fileWithStringContent);

	VisualString stringContentsOfFile;
	stringContentsOfFile.initWithContentsOfFile(*visualFile);

	if (debug == true) {
		writeLog("after stringContentsOfFile.initWithContentsOfFile()");
	}
	if (stringContentsOfFile.getNumberOfCharacters() == 0) {
		if (debug == true) {
			writeLog("stringContentsOfFile.getNumberOfCharacters() == 0");
		}
#if TARGET_OS_MAC
		retVal = (OSStatus)1001;
#endif
#if TARGET_OS_WIN
		retVal = (DWORD)1001;
#endif
		return retVal;
	}

	VisualNotification aNotification(visualFile->getIdentifier());
	aNotification.setKey(kStringWithIdentifierLoadedAndCreatedMsg);
	aNotification.setObject(stringContentsOfFile);

	delete visualFile;
	visualFile = NULL;

	aNotification.post();
	
	if (debug == true) {
		writeLog("readFileAndSendNotificationWithString (end)");
	}

	return retVal;
}


#if TARGET_OS_MAC
OSStatus VisualDataStore::readFileAndRemoveFileAndSendNotificationWithString(void* fileWithStringContent) {
	OSStatus retVal = noErr;
#endif
#if TARGET_OS_WIN
DWORD VisualDataStore::readFileAndRemoveFileAndSendNotificationWithString(LPVOID fileWithStringContent) {
	DWORD retVal = 0;
#endif
	
	bool debug = false;

	if (debug == true) {
		writeLog("readFileAndRemoveFileAndSendNotificationWithString (start)");
	}
	
	VisualFile* visualFile = reinterpret_cast<VisualFile*>(fileWithStringContent);

	if (!visualFile) {
#if TARGET_OS_MAC
		retVal = (OSStatus)1001;
#endif
#if TARGET_OS_WIN
		retVal = (DWORD)1001;
#endif
		return retVal;
	}

	VisualString stringContentsOfFile;
	stringContentsOfFile.initWithContentsOfFile(*visualFile);

	if (debug == true) {
		writeLog("after stringContentsOfFile.initWithContentsOfFile()");
	}
	if (stringContentsOfFile.getNumberOfCharacters() == 0) {
		if (debug == true) {
			writeLog("stringContentsOfFile.getNumberOfCharacters() == 0");
		}
#if TARGET_OS_MAC
		retVal = (OSStatus)1002;
#endif
#if TARGET_OS_WIN
		retVal = (DWORD)1002;
#endif
		return retVal;
	}
	
	VisualItemIdentifier fileIdentifier = visualFile->getIdentifier();
	VisualNotification aNotification(fileIdentifier);
	aNotification.setKey(kStringWithIdentifierLoadedAndCreatedMsg);
	aNotification.setObject(stringContentsOfFile);

	bool success = visualFile->remove();
	if (debug == true) {
		writeLog("tempFile.remove()");
	}
	if (!success) {
		if (debug == true) {
			writeLog("no success after remove()");
		}
#if TARGET_OS_MAC
		retVal = (OSStatus)1003;
#endif
#if TARGET_OS_WIN
		retVal = (DWORD)1003;
#endif
	}
	
	delete visualFile;
	visualFile = NULL;

	aNotification.post();
	
	if (debug == true) {
		writeLog("readFileAndRemoveFileAndSendNotificationWithString (end)");
	}

	return retVal;
}
