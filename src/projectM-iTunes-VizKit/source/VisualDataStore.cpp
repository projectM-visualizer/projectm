/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualDataStore.cpp
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

#include "VisualDataStore.h"
#include "VisualHostCommunication.h"
#include "VisualErrorHandling.h"
#include "VisualAudioLab.h"
#include "VisualConfiguration.h"
#include "VisualThreading.h"
#include "VisualNotification.h"
#include "VisualAudioMetaData.h"
#include "VisualString.h"
#include "VisualFile.h"
#include "VisualPlayerState.h"
#include "iTunesAPI.h" // ITTrackInfo, ITStreamInfo

#if TARGET_OS_MAC
#include "VisualAppleScript.h"
#endif

#if TARGET_OS_WIN
#include <shlobj.h> // SHGetFolderPath
#include <io.h> // _access
#endif


#if TARGET_OS_WIN
#include "iTunesCOMInterface.h"
#endif


using namespace VizKit;


const size_t VisualDataStore::audioMetaDataHistoryCount = 2;

bool VisualDataStore::lyricsCreationThreadIsRunning = false;

VisualDataStore* VisualDataStore::theVisualDataStore = NULL;  // singleton



VisualDataStore::VisualDataStore() {
    for (UInt8 i = 0; i < audioMetaDataHistoryCount; i++) {
		audioMetaDataHistory.push_back(new VisualAudioMetaData);
    }
	currAudioMetaDataHistoryIdx = audioMetaDataHistoryCount - 1;
	appVersionMajorNum = 0;
	appVersionMinorNum = 0;
	audioMetaDataHasBeenSentToEnsembleBool = false;
}


VisualDataStore::~VisualDataStore() {

	for (PreferenceVectorIterator it = preferenceVector.begin(); it != preferenceVector.end(); it++) {
		delete *it;
		*it = NULL;
	}
	preferenceVector.clear();

	processInfoMap.clear();
	
	for (AudioMetaDataVectorIterator it = audioMetaDataHistory.begin(); it != audioMetaDataHistory.end(); it++) {
		delete *it;
		*it = NULL;
	}
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


void VisualDataStore::setDefaultPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, const int aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			(*it)->defaultValueInt = aValue;
			(*it)->valueInt = 0;
			strcpy((*it)->defaultValueChar, "\0");
			strcpy((*it)->valueChar, "\0");
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->key = anIdentifier;
		aPreference->defaultValueInt = aValue;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kDefaultPref;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setDefaultPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, const float aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			(*it)->defaultValueFloat = aValue;
			(*it)->valueFloat = 0.0f;
			(*it)->defaultValueInt = 0;
			(*it)->valueInt = 0;
			strcpy((*it)->defaultValueChar, "\0");
			strcpy((*it)->valueChar, "\0");
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->key = anIdentifier;
		aPreference->defaultValueFloat = aValue;
		aPreference->valueFloat = 0.0f;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kDefaultPref;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setDefaultPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, const char* const aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy((*it)->defaultValueChar, aValue);
			strcpy((*it)->valueChar, "\0");
			(*it)->defaultValueInt = 0;
			(*it)->valueInt = 0;
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->key = anIdentifier;
		strcpy(aPreference->defaultValueChar, aValue);
		strcpy(aPreference->valueChar, "\0");
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->status = kDefaultPref;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, const int aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			(*it)->valueInt = aValue;
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->key = anIdentifier;
		aPreference->valueInt = aValue;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetButNotStored;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, const float aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			(*it)->valueFloat = aValue;
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->key = anIdentifier;
		aPreference->valueFloat = aValue;
		aPreference->defaultValueFloat = 0.0f;
		aPreference->valueInt = 0;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetButNotStored;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, const char* const aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	
	char errStr[256] = "\0";
	if (strlen(aValue) > 255) {
		strncat(errStr, aValue, 255);
		writeLog("pref char value too long");
		return;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy((*it)->valueChar, aValue);
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->key = anIdentifier;
		strcpy(aPreference->valueChar, aValue);
		strcpy(aPreference->defaultValueChar, "\0");
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->status = kPrefSetButNotStored;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setValueInt(const VisualConfiguration::PreferenceKey anIdentifier, const int aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			(*it)->valueInt = aValue;
			(*it)->status = kDoNotStore;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->key = anIdentifier;
		aPreference->valueInt = aValue;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kDoNotStore;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, const float aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			(*it)->valueFloat = aValue;
			(*it)->status = kDoNotStore;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->key = anIdentifier;
		aPreference->valueFloat = aValue;
		aPreference->defaultValueFloat = 0.0f;
		aPreference->valueInt = 0;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kDoNotStore;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


void VisualDataStore::setValueChar(const VisualConfiguration::PreferenceKey anIdentifier, const char* const aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	
	char errStr[256] = "\0";
	if (strlen(aValue) > 255) {
		strncat(errStr, aValue, 255);
		writeLog("pref char value too long");
		return;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy((*it)->valueChar, aValue);
			(*it)->status = kDoNotStore;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->key = anIdentifier;
		strcpy(aPreference->valueChar, aValue);
		strcpy(aPreference->defaultValueChar, "\0");
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->status = kDoNotStore;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}
}


int VisualDataStore::getValueInt(const VisualConfiguration::PreferenceKey anIdentifier, bool* wasSet) {

	int prefVal = 0;
	
	if (wasSet != NULL) {
		*wasSet = false;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (PreferenceVectorIterator it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			if ((*it)->status == kDoNotStore) {
				if (wasSet != NULL) {
					*wasSet = true;
				}
				return (*it)->valueInt;
			}
		}
	}
	
	return prefVal;
}


float VisualDataStore::getValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, bool* wasSet) {
	
	float prefVal = 0;
	
	if (wasSet != NULL) {
		*wasSet = false;
	}

	theVisualDataStore = VisualDataStore::getInstance();
	for (PreferenceVectorIterator it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			if ((*it)->status == kDoNotStore) {
				if (wasSet != NULL) {
					*wasSet = true;
				}
				return (*it)->valueFloat;
			}
		}
	}

	return prefVal;
}


void VisualDataStore::getValueChar(const VisualConfiguration::PreferenceKey anIdentifier, char* outPrefVal, bool* wasSet) {
	
	strcpy(outPrefVal, "\0");
	
	if (wasSet != NULL) {
		*wasSet = false;
	}
	
	theVisualDataStore = VisualDataStore::getInstance();
	for (PreferenceVectorIterator it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			if ((*it)->status == kDoNotStore) {
				if (wasSet != NULL) {
					*wasSet = true;
				}
				strncat(outPrefVal, (*it)->valueChar, strlen((*it)->valueChar));
				return;
			}
		}
	}
	
}


int VisualDataStore::getDefaultPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue) {
	char errLog[128];
	*isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			*isDefaultValue = true;
			return (*it)->defaultValueInt;
		}
	}
	char preferenceKeyStr[64];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Int not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return 0;
}


float VisualDataStore::getDefaultPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue) {
	char errLog[128];
	*isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			*isDefaultValue = true;
			return (*it)->defaultValueFloat;
		}
	}
	char preferenceKeyStr[64];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Float not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return 0.0f;
}


bool VisualDataStore::getDefaultPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, char* outPrefVal) {
	char errLog[128];
	bool isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	strcpy(outPrefVal, "\0");
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy(outPrefVal, (*it)->defaultValueChar);
			isDefaultValue = true;
			return isDefaultValue;
		}
	}
	char preferenceKeyStr[64];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Char not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return isDefaultValue;
}


int VisualDataStore::getPreferenceValueInt(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue) {

	bool isADefaultVal = false;
	bool wasStored = false;
	bool wasSetButNotStored = false;
	int prefVal = 0;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}

	theVisualDataStore = VisualDataStore::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			if ((*it)->status == kPrefSetAndStored) {
				return (*it)->valueInt;
			}
		}
	}
	
	// the preference was not set and stored
	// we have to check the underlying preference system for persistently stored values
#if TARGET_OS_MAC
	CFStringRef identifierStr;
	CFNumberRef intRef = NULL;
	
	char preferenceKeyStr[64];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	identifierStr = CFStringCreateWithCString(kCFAllocatorDefault, preferenceKeyStr, kCFStringEncodingISOLatin1);

	CFStringRef appID = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingISOLatin1);
	intRef = (CFNumberRef)CFPreferencesCopyValue(identifierStr, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
	CFRelease(appID);
	if (intRef) {
		if (!CFNumberGetValue(intRef, kCFNumberIntType, &prefVal)) {
			wasStored = false;
		} else {
			wasStored = true;
		}
		CFRelease(intRef);
	} else {
		wasStored = false;
	}
	
	CFRelease(identifierStr);

#endif


#if TARGET_OS_WIN

	TCHAR szAppData[1024];
	HRESULT hr;
	char prefPath[512];
	//hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
	BOOL createFolderIfNotAlreadyThere = true;
	hr = SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::kVisualPluginName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualDataStore->make_sure_path_exists(szAppData, false);

	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;

	char anIdentifierStr[128];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);

	if (theVisualDataStore->fileExists(szAppData)) {
	 
		hFile = CreateFile(szAppData,     			// file to create
						   GENERIC_READ,          // open for reading
						   FILE_SHARE_READ,       // do share (to avoid error 32)
						   NULL,                   // default security
						   OPEN_ALWAYS,            // open the file if it exists, else create new one
						   FILE_ATTRIBUTE_NORMAL, // normal file (FILE_FLAG_OVERLAPPED,   // asynchronous I/O)
						   NULL);                  // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) { 
			char str [1024];
			sprintf(str, "Could not open file %s for reading pref int val (error %d)\n", szAppData, GetLastError());
			writeLog(str);
			return 0;
		}

		BOOL success;
		char str[4096] = "\0";
		DWORD numberOfBytesRead;

		success = ReadFile(hFile, &str, 4096, &numberOfBytesRead, NULL);

		char* token = NULL;
		char seps[] = "\r\n";
		char keyStr[128];
		char equalityChar[2];
		char valueStr[128];
		token = strtok(str, seps);
		while (token != NULL) {

			if (strlen(token) > 4) {
				sscanf(token, "%s %s %s", keyStr, equalityChar, valueStr);

				if (strcmp(keyStr, anIdentifierStr) == 0) {
					prefVal = atoi(valueStr);
					wasStored = true;
				}
			}

			token = strtok(NULL, seps);
		}


		/*
		BOOL ReadFile(
		  HANDLE hFile,
		  LPVOID lpBuffer,
		  DWORD nNumberOfBytesToRead,
		  LPDWORD lpNumberOfBytesRead,
		  LPOVERLAPPED lpOverlapped
		);
		*/

		CloseHandle(hFile);
	}

#endif
	
	if (wasStored == false) {
		prefVal = theVisualDataStore->getDefaultPreferenceValueInt(anIdentifier, &isADefaultVal);
	}

	// check to see if we have to add the preference to the vector
	wasSetButNotStored = false;
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kIntPrefType) {
			if ((*it)->status == kPrefSetButNotStored) {
				wasSetButNotStored = true;
			}
		}
	}
	if ((isADefaultVal == false) && (wasStored == true) && (wasSetButNotStored == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->key = anIdentifier;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = prefVal;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetAndStored;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}

	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	return prefVal;
}


float VisualDataStore::getPreferenceValueFloat(const VisualConfiguration::PreferenceKey anIdentifier, bool* isDefaultValue) {
	
	bool isADefaultVal = false;
	bool wasStored = false;
	bool wasSetButNotStored = false;
	float prefVal = 0;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}

	theVisualDataStore = VisualDataStore::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			if ((*it)->status == kPrefSetAndStored) {
				return (*it)->valueFloat;
			}
		}
	}

	// the preference was not set and stored
	// we have to check the underlying preference system for persistently stored values
#if TARGET_OS_MAC
	CFStringRef identifierStr;
	CFNumberRef floatRef = NULL;

	char preferenceKeyStr[64];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	identifierStr = CFStringCreateWithCString(kCFAllocatorDefault, preferenceKeyStr, kCFStringEncodingISOLatin1);

	CFStringRef appID = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingISOLatin1);
	floatRef = (CFNumberRef)CFPreferencesCopyValue(identifierStr, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
	CFRelease(appID);
	if (floatRef) {
		if (!CFNumberGetValue(floatRef, kCFNumberFloatType, &prefVal)) {
			wasStored = false;
		} else {
			wasStored = true;
		}
		CFRelease(floatRef);
	} else {
		wasStored = false;
	}
	
	CFRelease(identifierStr);

#endif


#if TARGET_OS_WIN

	TCHAR szAppData[1024];
	HRESULT hr;
	char prefPath[512];
	//hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
	BOOL createFolderIfNotAlreadyThere = true;
	hr = SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::kVisualPluginName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualDataStore->make_sure_path_exists(szAppData, false);

	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;

	char anIdentifierStr[128];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);

	if (theVisualDataStore->fileExists(szAppData)) {
	 
		hFile = CreateFile(szAppData,     			// file to create
						   GENERIC_READ,          // open for reading
						   FILE_SHARE_READ,       // do share (to avoid error 32)
						   NULL,                   // default security
						   OPEN_ALWAYS,            // open the file if it exists, else create new one
						   FILE_ATTRIBUTE_NORMAL, // normal file (FILE_FLAG_OVERLAPPED,   // asynchronous I/O)
						   NULL);                  // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) { 
			char str [1024];
			sprintf(str, "Could not open file %s for reading pref float val (error %d)\n", szAppData, GetLastError());
			writeLog(str);
			return 0;
		}

		BOOL success;
		char str[4096] = "\0";
		DWORD numberOfBytesRead;

		success = ReadFile(hFile, &str, 4096, &numberOfBytesRead, NULL);

		char* token = NULL;
		char seps[] = "\r\n";
		char keyStr[128];
		char equalityChar[2];
		char valueStr[128];
		token = strtok(str, seps);
		while (token != NULL) {

			if (strlen(token) > 4) {
				sscanf(token, "%s %s %s", keyStr, equalityChar, valueStr);
				
				if (strcmp(keyStr, anIdentifierStr) == 0) {

					prefVal = (float)atof(valueStr);
					wasStored = true;
				}
			}

			token = strtok(NULL, seps);
		}


		/*
		BOOL ReadFile(
		  HANDLE hFile,
		  LPVOID lpBuffer,
		  DWORD nNumberOfBytesToRead,
		  LPDWORD lpNumberOfBytesRead,
		  LPOVERLAPPED lpOverlapped
		);
		*/

		CloseHandle(hFile);
	}

#endif
	
	if (wasStored == false) {
		prefVal = theVisualDataStore->getDefaultPreferenceValueFloat(anIdentifier, &isADefaultVal);
	}

	// check to see if we have to add the preference to the vector
	wasSetButNotStored = false;
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kFloatPrefType) {
			wasSetButNotStored = true;
		}
	}
	if ((isADefaultVal == false) && (wasStored == true) && (wasSetButNotStored == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->key = anIdentifier;
		aPreference->defaultValueFloat = 0.0f;
		aPreference->valueFloat = prefVal;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetAndStored;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}

	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	return prefVal;
}


void VisualDataStore::getPreferenceValueChar(const VisualConfiguration::PreferenceKey anIdentifier, char* outPrefVal, bool* isDefaultValue) {
	
	bool isADefaultVal = false;
	bool wasStored = false;
	bool wasSetButNotStored = false;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
	strcpy(outPrefVal, "\0");
	
	theVisualDataStore = VisualDataStore::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			if ((*it)->status == kPrefSetAndStored) {
				strncat(outPrefVal, (*it)->valueChar, strlen((*it)->valueChar));
				return;
			}
		}
	}

	// the preference was not set and stored
	// we have to check the underlying preference system for persistently stored values
#if TARGET_OS_MAC
	CFStringRef identifierStr;
	CFStringRef stringRef = NULL;

	char preferenceKeyStr[64];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	identifierStr = CFStringCreateWithCString(kCFAllocatorDefault, preferenceKeyStr, kCFStringEncodingISOLatin1);

	CFStringRef appID = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingISOLatin1);
	stringRef = (CFStringRef)CFPreferencesCopyValue(identifierStr, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
	CFRelease(appID);
	if (stringRef) {
		bool success = CFStringGetCString(stringRef, outPrefVal, 256, kCFStringEncodingISOLatin1);
		// CFStringGetCString must know if the provided c-buffer is large enough for the cString including the null-terminator byte.
		// So a bufferSize of 256 means a char array of size of 256 (255 characters and 1 terminating null-byte).
		if (success) {
			wasStored = true;
		} else {
			wasStored = false;
		}
		CFRelease(stringRef);
	} else {
		wasStored = false;
	}
	
	CFRelease(identifierStr);

#endif


#if TARGET_OS_WIN

	TCHAR szAppData[1024];
	HRESULT hr;
	char prefPath[512];
	//hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
	BOOL createFolderIfNotAlreadyThere = true;
	hr = SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::kVisualPluginName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualDataStore->make_sure_path_exists(szAppData, false);

	strcat(szAppData, "\\Preferences.txt");

	HANDLE hFile;

	char anIdentifierStr[128];
	VisualConfiguration::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);

	if (theVisualDataStore->fileExists(szAppData)) {
	 
		hFile = CreateFile(szAppData,     			// file to create
						   GENERIC_READ,          // open for reading
						   FILE_SHARE_READ,       // do share
						   NULL,                   // default security
						   OPEN_ALWAYS,            // open the file if it exists, else create new one
						   FILE_ATTRIBUTE_NORMAL, // | // normal file
						   //FILE_FLAG_OVERLAPPED,   // asynchronous I/O
						   //NULL,   // no asynchronous I/O
						   NULL);                  // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) { 
			char str [1024];
			sprintf(str, "Could not open file %s for reading pref char val (error %d)\n", szAppData, GetLastError());
			writeLog(str);
			return;
		}


		BOOL success;
		char str[4096] = "\0";
		DWORD numberOfBytesRead = 0;


		success = ReadFile(hFile, &str, 4096, &numberOfBytesRead, NULL);

		char* token = NULL;
		char seps[] = "\r\n";
		char keyStr[128];
		token = strtok(str, seps);
		//int count = 0;
		char* valueStr;
		char lineStr[255];
		while (token != NULL) {
			strcpy(lineStr, token);
			valueStr = strchr(lineStr, '=');
			if (valueStr) {
				valueStr++;
				valueStr++;
				sscanf(token, "%s", keyStr);
				if (strcmp(keyStr, anIdentifierStr) == 0) {
					// move past leading quotation mark
					valueStr++;
					// we copy the whole string
					// without trailing quotation mark
					strncat(outPrefVal, valueStr, strlen(valueStr) - 1);
					wasStored = true;
				}
			}
			token = strtok(NULL, seps);
		}


		/*
		BOOL ReadFile(
		  HANDLE hFile,
		  LPVOID lpBuffer,
		  DWORD nNumberOfBytesToRead,
		  LPDWORD lpNumberOfBytesRead,
		  LPOVERLAPPED lpOverlapped
		);
		*/

		CloseHandle(hFile);
	}

#endif

	if (wasStored == false) {
		isADefaultVal = theVisualDataStore->getDefaultPreferenceValueChar(anIdentifier, outPrefVal);
	}
	
	// check to see if we have to add the preference to the vector
	wasSetButNotStored = false;
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->key == anIdentifier && (*it)->dataType == kCharPrefType) {
			wasSetButNotStored = true;
		}
	}
	if ((isADefaultVal == false) && (wasStored == true) && (wasSetButNotStored == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->key = anIdentifier;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, outPrefVal);
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->status = kPrefSetAndStored;
		theVisualDataStore->preferenceVector.push_back(aPreference);
	}

	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
}


void VisualDataStore::storePreferences() {
	
#if TARGET_OS_WIN
	BOOL success;
#endif

	PreferenceVectorIterator it;
	theVisualDataStore = VisualDataStore::getInstance();
	
#if TARGET_OS_MAC
	CFStringRef appID = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingISOLatin1);
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		CFStringRef identifierStr;
		CFNumberRef intRef = NULL;
		CFNumberRef floatRef = NULL;
		CFStringRef prefString;

		if ((*it)->status == kPrefSetButNotStored) {
			char preferenceKeyStr[64];
			VisualConfiguration::convertPreferenceKeyToString((*it)->key, preferenceKeyStr);
			identifierStr = CFStringCreateWithCString(kCFAllocatorDefault, preferenceKeyStr, kCFStringEncodingISOLatin1);
			if ((*it)->dataType == kCharPrefType) {
				prefString = CFStringCreateWithCString(kCFAllocatorDefault, (*it)->valueChar, kCFStringEncodingISOLatin1);
				CFPreferencesSetValue(identifierStr, prefString, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
				CFRelease(prefString);
			} else if ((*it)->dataType == kIntPrefType) {
				intRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &(*it)->valueInt);
				CFPreferencesSetValue(identifierStr, intRef, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
				CFRelease(intRef);
			} else if ((*it)->dataType == kFloatPrefType) {
				floatRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloatType, &(*it)->valueFloat);
				CFPreferencesSetValue(identifierStr, floatRef, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
				CFRelease(floatRef);
			}
			CFRelease(identifierStr);
		}
	}

	CFPreferencesSynchronize(appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
	CFRelease(appID);
#endif


#if TARGET_OS_WIN

	// now we add the additional preferences possibly already stored in file system

	TCHAR szAppData[1024];
	HRESULT hr;
	char prefPath[512];
	//hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
	BOOL createFolderIfNotAlreadyThere = true;
	hr = SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::kVisualPluginName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualDataStore->make_sure_path_exists(szAppData, false);

	strcat(szAppData, "\\Preferences.txt");

	HANDLE hFile;
	
	if (theVisualDataStore->fileExists(szAppData)) {
	 
		hFile = CreateFile(szAppData,     			// file to create
						   GENERIC_READ,  // open for reading
						   FILE_SHARE_READ,    // do share
						   NULL,                   // default security
						   OPEN_ALWAYS,            // open the file if it exists, else create new file
						   FILE_ATTRIBUTE_NORMAL, // normal file
						   //FILE_FLAG_OVERLAPPED,   // asynchronous I/O
						   //NULL,   // no asynchronous I/O
						   NULL);                  // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) { 
			char str [1024];
			sprintf(str, "Could not open file %s for reading prefs before storing (error %d)\n", szAppData, GetLastError());
			writeLog(str);
			return;
		}

		char str[4096] = "\0";
		DWORD numberOfBytesRead = 0;

		success = ReadFile(hFile, &str, 4096, &numberOfBytesRead, NULL);

		char* token = NULL;
		char seps[] = "\r\n";
		//char seps[] = "\r\n";
		char keyStr[128];
		char* valueStr;
		char valueNormalizedStr[32];
		int intVal;
		float floatVal;
		char lineStr[255];
		bool found = false;
		bool isCharVal = false; // char value or int value
		UInt16 aPrefDataType = 0;

		if (numberOfBytesRead > 0) {

			token = strtok(str, seps);

			while (token != NULL) {

				strcpy(lineStr, token);
				valueStr = strchr(lineStr, '=');

				if (valueStr) {
					valueStr++;
					valueStr++;

					sscanf(token, "%s", keyStr);

					if (strchr(valueStr, 34) != NULL) {
						aPrefDataType = kCharPrefType;
					} else if (strchr(valueStr, 46) != NULL) {
						aPrefDataType = kFloatPrefType;
					} else {
						aPrefDataType = kIntPrefType;
					}
					
					found = false;
					for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
						char currKeyStr[128];
						VisualConfiguration::convertPreferenceKeyToString((*it)->key, currKeyStr);
						if (strcmp(currKeyStr, keyStr) == 0 && (*it)->dataType == aPrefDataType) {
							if ((*it)->status == kDefaultPref) {
								found = true;
								// the value was persistently stored
								// while preferenceVector up to now only knew the default value
								if (aPrefDataType == kCharPrefType) {
									// we have to re-save the already stored value
									valueStr++;
									strcpy(valueNormalizedStr, "\0");
									strncat(valueNormalizedStr, valueStr, strlen(valueStr) - 1);
									theVisualDataStore->setPreferenceValueChar((*it)->key, valueNormalizedStr);
								} else if (aPrefDataType == kFloatPrefType) {
									// we have to re-save the already stored value
									floatVal = (float)atof(valueStr);
									theVisualDataStore->setPreferenceValueFloat((*it)->key, floatVal);
								} else if (aPrefDataType == kIntPrefType) {
									// we have to re-save the already stored value
									intVal = atoi(valueStr);
									theVisualDataStore->setPreferenceValueInt((*it)->key, intVal);
								}
							} else {
								// new value has been set but not stored
								found = true;
							}
						}
					}

					/*
					if (found == false) {
						// the value was persistently stored
						// and not known to preferenceVector
						// because no default pref was set
						if (aPrefDataType == kCharPrefType) {
							valueStr++;
							strcpy(valueNormalizedStr, "\0");
							strncat(valueNormalizedStr, valueStr, strlen(valueStr) - 1);
							theVisualDataStore->setPreferenceValueChar(keyStr, valueNormalizedStr);
						} else if (aPrefDataType == kFloatPrefType) {
							floatVal = (float)atof(valueStr);
							theVisualDataStore->setPreferenceValueFloat(keyStr, floatVal);
						} else if (aPrefDataType == kIntPrefType) {
							intVal = atoi(valueStr);
							theVisualDataStore->setPreferenceValueInt(keyStr, intVal);
						}

					}
					*/
					
				}
				token = strtok(NULL, seps);
			}

		}

		CloseHandle(hFile);
		
	}

	DWORD numberOfBytesWritten;

	hFile = CreateFile(szAppData,     			// file to create
						GENERIC_WRITE,  // open for writing
						FILE_SHARE_READ, // do share
						NULL,                   // default security
						CREATE_ALWAYS,            // create new file
						FILE_ATTRIBUTE_NORMAL, // normal file
						NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE) { 
		char str [1024];
		sprintf(str, "Could not open file %s for writing prefs (error %d)\n", szAppData, GetLastError());
		writeLog(str);
		return;
	}

	char prefLineStr[256];
	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->status != kDefaultPref) {
			// store all pref vals persistently that are not pure default preferences
			if ((*it)->status != kDoNotStore) {
				strcpy(prefLineStr, "\0");
				char currKeyStr[128];
				VisualConfiguration::convertPreferenceKeyToString((*it)->key, currKeyStr);
				if ((*it)->dataType == kCharPrefType) {
					sprintf(prefLineStr, "%s = \"%s\"\r\n", currKeyStr, (*it)->valueChar);
				} else if ((*it)->dataType == kFloatPrefType) {
					sprintf(prefLineStr, "%s = %f\r\n", currKeyStr, (*it)->valueFloat);
				} else if ((*it)->dataType == kIntPrefType) {
					sprintf(prefLineStr, "%s = %d\r\n", currKeyStr, (*it)->valueInt);
				}
				if (strlen(prefLineStr) > 0) {
					success = WriteFile(hFile, prefLineStr, strlen(prefLineStr), &numberOfBytesWritten, NULL);
				}
			}
		}
	}

	CloseHandle(hFile);

#endif

	for (it = theVisualDataStore->preferenceVector.begin(); it != theVisualDataStore->preferenceVector.end(); it++) {
		if ((*it)->status == kPrefSetButNotStored) {
			(*it)->status = kPrefSetAndStored;
		}
	}

}


void VisualDataStore::setAppVersionNum(UInt8 majorVersionNum, UInt8 minorVersionNum) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->appVersionMajorNum = majorVersionNum;
	theVisualDataStore->appVersionMinorNum = minorVersionNum;
}


UInt8 VisualDataStore::getAppVersionMajorNum() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->appVersionMajorNum;
}


UInt8 VisualDataStore::getAppVersionMinorNum() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->appVersionMinorNum;
}


bool VisualDataStore::storeAudioTrackMetadata(const ITTrackInfo* const trackMetadata) {

	bool audioTrackDidChange = false;

	theVisualDataStore = VisualDataStore::getInstance();
	
	VisualAudioMetaData* currAudioMetaData = new VisualAudioMetaData;
	
	VisualString trackName((const UniChar*)trackMetadata->name + 1, (UInt32)trackMetadata->name[0]);
	VisualString trackArtist((const UniChar*)trackMetadata->artist + 1, (UInt32)trackMetadata->artist[0]);
	VisualString trackAlbum((const UniChar*)trackMetadata->album + 1, (UInt32)trackMetadata->album[0]);
	currAudioMetaData->setTrackName(trackName);
	currAudioMetaData->setTrackArtist(trackArtist);
	currAudioMetaData->setTrackAlbum(trackAlbum);
	currAudioMetaData->setTrackSizeInBytes(trackMetadata->sizeInBytes);
	currAudioMetaData->setIsStream(false);
	
	if (*currAudioMetaData != *theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]) {
		theVisualDataStore->advanceAudioMetaDataHistory();
		delete theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx];
		theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx] = currAudioMetaData;
		audioTrackDidChange = true;
	} else {
		delete currAudioMetaData;
	}
	
	VisualAudioLab* theVisualAudioLab;
	theVisualAudioLab = VisualAudioLab::getInstance();
	theVisualAudioLab->setTotalTimeOfCurrentTrack(trackMetadata->totalTimeInMS);
	theVisualAudioLab->setStartTimeOfCurrentTrack(trackMetadata->startTimeInMS);
	theVisualAudioLab->setStopTimeOfCurrentTrack(trackMetadata->stopTimeInMS);
	
	theVisualDataStore->audioMetaDataHasBeenSentToEnsembleBool = false;
	
	return audioTrackDidChange;
	
}


bool VisualDataStore::storeAudioStreamMetadata(const ITStreamInfo* const streamMetadata) {

	bool audioTrackDidChange = false;

	theVisualDataStore = VisualDataStore::getInstance();
	
	VisualAudioMetaData* currAudioMetaData = new VisualAudioMetaData;
	
	if (streamMetadata->streamTitle[0] > 0) {
		VisualString streamTitle((const UniChar*)streamMetadata->streamTitle + 1, (UInt32)streamMetadata->streamTitle[0]);
		currAudioMetaData->setTrackName(streamTitle);
	} else if (streamMetadata->streamMessage[0] > 0) {
		VisualString streamMessage((const UniChar*)streamMetadata->streamMessage + 1, (UInt32)streamMetadata->streamMessage[0]);
		currAudioMetaData->setTrackName(streamMessage);
	}
	currAudioMetaData->setIsStream(true);

	if (*currAudioMetaData != *theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]) {
		theVisualDataStore->advanceAudioMetaDataHistory();
		delete theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx];
		theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx] = currAudioMetaData;
		audioTrackDidChange = true;
	} else {
		delete currAudioMetaData;
	}
	
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	theVisualAudioLab->setTotalTimeOfCurrentTrack(0);
	
	theVisualDataStore->audioMetaDataHasBeenSentToEnsembleBool = false;
	
	return audioTrackDidChange;
	
}


bool VisualDataStore::audioMetaDataHasBeenSentToEnsemble() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHasBeenSentToEnsembleBool;
}


void VisualDataStore::setAudioMetaDataHasBeenSentToEnsemble(bool hasBeenSent) {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->audioMetaDataHasBeenSentToEnsembleBool = hasBeenSent;
}


void VisualDataStore::advanceAudioMetaDataHistory() {
	if ((this->currAudioMetaDataHistoryIdx + 1) >= this->audioMetaDataHistory.size()) {
		this->currAudioMetaDataHistoryIdx = 0;
	} else {
		this->currAudioMetaDataHistoryIdx++;
	}
}


UInt32 VisualDataStore::getTrackSizeInBytes() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getTrackSizeInBytes();
}


const VisualString& VisualDataStore::getNameOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getTrackName();
}


const VisualString& VisualDataStore::getArtistOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getTrackArtist();
}


const VisualString& VisualDataStore::getAlbumOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getTrackAlbum();
}


const VisualString& VisualDataStore::getLyricsOfCurrentTrack() {
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getTrackLyrics();
}


bool VisualDataStore::currentlyPlayingAudioIsStream(void) {
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getIsStream();
}


UInt16 VisualDataStore::getNumberOfCoverArtworksOfCurrentTrack() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getNumberOfArtworks();
}


UInt16 VisualDataStore::evaluateCoverArtInfo(OSType* albumCoverArtworkFileType, Handle* albumCoverArtworkHandle) {
	UInt16 numberOfArtworks = 0;
	numberOfArtworks = VisualHostCommunication::getCurrentTrackCoverArt(albumCoverArtworkFileType, albumCoverArtworkHandle);
	return numberOfArtworks;
}


OSType VisualDataStore::getCoverArtFileType() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getAlbumCoverArtworkFileType();
}


Handle VisualDataStore::getHandleOfCoverArt() {
	theVisualDataStore = VisualDataStore::getInstance();
	return theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->getAlbumCoverArtworkHandle();
}


void VisualDataStore::disposeHandleOfCoverArt() {
	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->disposeAlbumCoverArtworkHandle();
}


void VisualDataStore::setPreferredDisplayResolution(UInt16 minBitsPerPixel, UInt16 maxBitsPerPixel, UInt16 preferredBitsPerPixel, UInt16 horizontalPixels, UInt16 verticalPixels) {
	OSStatus status;
	theVisualDataStore = VisualDataStore::getInstance();
	if (horizontalPixels > 0 && verticalPixels > 0) {
		status = VisualHostCommunication::setPreferredDisplayResolution(minBitsPerPixel, maxBitsPerPixel, preferredBitsPerPixel, horizontalPixels, verticalPixels);
	} else {
		status = noErr;
	}
	if (status != noErr) {
		writeLog("err PlayerSetFullScreenOptions");
	}
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


void VisualDataStore::OSTypeToString(OSType x, char* dest) {
	// Uniform Type Identifiers Overview
	// UTCreateStringForOSType
	// UTType.h
   dest[0] = (char)(x>>24);
   dest[1] = (char)(x>>16);
   dest[2] = (char)(x>>8);
   dest[3] = (char)x;
   dest[4] = '\0';
}


#if TARGET_OS_WIN

int VisualDataStore::make_sure_path_exists(const char *iPath, bool FilenameIncluded) {
   char *Path=(char*)iPath, 
        *TmpPath=Path, 
        TmpSmb=0,
        *LastDPtr=NULL;
   while ((TmpPath=strpbrk(TmpPath+1,"\\/"))) {
      TmpSmb=Path[TmpPath-Path];
      Path[TmpPath-Path]=0;
      CreateDirectory(Path,NULL);
      Path[TmpPath-Path]=TmpSmb;
      LastDPtr=TmpPath;
   }

   int Res=1;
   if(!FilenameIncluded) {
      CreateDirectory(iPath,NULL);
      Res=!_access(iPath,0);
   }
   else {
      if(LastDPtr) {
         Path=(char*)iPath;
         TmpSmb=Path[LastDPtr-Path];
         Path[LastDPtr-Path]=0;
         Res=!_access(Path,0);
         Path[LastDPtr-Path]=TmpSmb;
      }
   }
   
   return Res;
}


bool VisualDataStore::fileExists(LPCSTR szFileName) {
	HANDLE handle;
	WIN32_FIND_DATA findData;

	handle = FindFirstFile(szFileName, &findData);
	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			return true;
		}
	}

	return false;
}

#endif


void VisualDataStore::createLyricsOfCurrentTrack() {
	if (VisualDataStore::lyricsCreationThreadIsRunning == true) return;
	VisualThreading::createThread((ThreadingFuncPtr)VisualDataStore::createLyricsOfCurrentTrackThread, "createLyricsOfCurrentTrackThread");
}


#if TARGET_OS_MAC
OSStatus VisualDataStore::createLyricsOfCurrentTrackThread(void* parameter) {

	VisualDataStore::lyricsCreationThreadIsRunning = true;

	OSStatus status = noErr;
	bool debug = false;
	
	if (debug == true) {
		writeLog("---------------------- createLyricsOfCurrentTrackThread (begin)");
	}
	
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	
	VisualString tempFileName = VisualString("VizKitLyrics.tmp.txt");
	VisualFile tempFile;
	status = tempFile.initWithDirectoryOfTemporaryItems();
	if (status != noErr) {
		return status;
	}
	status = tempFile.appendFileName(tempFileName);
	if (status != noErr) {
		return status;
	}

	if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) != kIsShowing) {
		return (OSStatus)1001;
	}

	if (debug == true) {
		writeLog("before executeScriptFile()");
	}
	
	VisualString appleScriptFileName = VisualString("getLyrics.applescript");
	VisualFile appleScriptFile;
	status = appleScriptFile.initWithResourcesDirectory();
	appleScriptFile.appendFileName(appleScriptFileName);
	VisualAppleScript::executeScriptFile(appleScriptFile);

	if (debug == true) {
		writeLog("after executeScriptFile()");
	}
	
	VisualString lyricsString;
	lyricsString.initWithContentsOfFile(tempFile);

	status = tempFile.remove();
	if (debug == true) {
		writeLog("tempFile.remove()");
	}
	if (status != noErr) {
		return status;
	}

	theVisualDataStore = VisualDataStore::getInstance();
	theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->setTrackLyrics(lyricsString);
	
	VisualNotification::post(kLyricsAreAvailableMsg);
	
	if (debug == true) {
		writeLog("---------------------- createLyricsOfCurrentTrackThread (end)");
	}
	
	VisualDataStore::lyricsCreationThreadIsRunning = false;
	
	return status;
}

#endif

#if TARGET_OS_WIN
DWORD WINAPI VisualDataStore::createLyricsOfCurrentTrackThread(LPVOID lpParam) {

	VisualDataStore::lyricsCreationThreadIsRunning = true;

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
				iTrackFile->get_Lyrics(&bstrLyricsTrack);
				theVisualDataStore = VisualDataStore::getInstance();

				UInt16 trackLyricsStringLength = lstrlenW(bstrLyricsTrack);
				VisualString lyrics((UniChar*)bstrLyricsTrack, trackLyricsStringLength);
				theVisualDataStore->audioMetaDataHistory[theVisualDataStore->currAudioMetaDataHistoryIdx]->setTrackLyrics(lyrics);
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

	VisualNotification::post(kLyricsAreAvailableMsg);

	VisualDataStore::lyricsCreationThreadIsRunning = false;

	return (DWORD)noErr;
}
#endif
