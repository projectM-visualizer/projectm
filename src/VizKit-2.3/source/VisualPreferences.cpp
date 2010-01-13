/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualPreferences.cpp
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

#include "VisualPreferences.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"

#if TARGET_OS_MAC
#include <Carbon/Carbon.h> // Persistent preference system
#endif


#if TARGET_OS_WIN
#include <shlobj.h> // SHGetFolderPath
#include <io.h> // _access
#endif




using namespace VizKit;



VisualPreferences* VisualPreferences::theVisualPreferences = NULL;  // singleton



VisualPreferences::VisualPreferences() {
	// null
}


VisualPreferences::~VisualPreferences() {
	for (PreferenceVectorIterator it = preferenceVector.begin(); it != preferenceVector.end(); it++) {
		delete *it;
		*it = NULL;
	}
	preferenceVector.clear();
}


VisualPreferences* VisualPreferences::getInstance() {
    if (theVisualPreferences == NULL) {
		theVisualPreferences = new VisualPreferences;
    }
	return theVisualPreferences;
}


void VisualPreferences::dispose() {
	if (theVisualPreferences != NULL) {
		delete theVisualPreferences;
		theVisualPreferences = NULL;
	}
}


void VisualPreferences::setDefault(const PreferenceKeyInt anIdentifier, const int aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyInt == anIdentifier && (*it)->dataType == kIntPrefType) {
			(*it)->defaultValueInt = aValue;
			(*it)->valueInt = 0;
			strcpy((*it)->defaultValueChar, "\0");
			strcpy((*it)->valueChar, "\0");
			(*it)->defaultValueBool = false;
			(*it)->valueBool = false;
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->keyInt = anIdentifier;
		aPreference->defaultValueInt = aValue;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->defaultValueBool = false;
		aPreference->valueBool = false;
		aPreference->status = kDefaultPref;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setDefault(const PreferenceKeyFloat anIdentifier, const float aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyFloat == anIdentifier && (*it)->dataType == kFloatPrefType) {
			(*it)->defaultValueFloat = aValue;
			(*it)->valueFloat = 0.0f;
			(*it)->defaultValueInt = 0;
			(*it)->valueInt = 0;
			strcpy((*it)->defaultValueChar, "\0");
			strcpy((*it)->valueChar, "\0");
			(*it)->defaultValueBool = false;
			(*it)->valueBool = false;
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->keyFloat = anIdentifier;
		aPreference->defaultValueFloat = aValue;
		aPreference->valueFloat = 0.0f;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->defaultValueBool = false;
		aPreference->valueBool = false;
		aPreference->status = kDefaultPref;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setDefault(const PreferenceKeyChar anIdentifier, const char* const aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyChar == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy((*it)->defaultValueChar, aValue);
			strcpy((*it)->valueChar, "\0");
			(*it)->defaultValueInt = 0;
			(*it)->valueInt = 0;
			(*it)->defaultValueBool = false;
			(*it)->valueBool = false;
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->keyChar = anIdentifier;
		strcpy(aPreference->defaultValueChar, aValue);
		strcpy(aPreference->valueChar, "\0");
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->defaultValueBool = false;
		aPreference->valueBool = false;
		aPreference->status = kDefaultPref;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setDefault(const PreferenceKeyBool anIdentifier, const bool aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyBool == anIdentifier && (*it)->dataType == kBoolPrefType) {
			(*it)->defaultValueBool = aValue;
			(*it)->valueBool = false;
			strcpy((*it)->defaultValueChar, "\0");
			strcpy((*it)->valueChar, "\0");
			(*it)->defaultValueInt = 0;
			(*it)->valueInt = 0;
			(*it)->status = kDefaultPref;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kBoolPrefType;
		aPreference->keyBool = anIdentifier;
		aPreference->defaultValueBool = aValue;
		aPreference->valueBool = false;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kDefaultPref;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setValue(const PreferenceKeyInt anIdentifier, const int aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyInt == anIdentifier && (*it)->dataType == kIntPrefType) {
			(*it)->valueInt = aValue;
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->keyInt = anIdentifier;
		aPreference->valueInt = aValue;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetButNotStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setValue(const PreferenceKeyFloat anIdentifier, const float aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyFloat == anIdentifier && (*it)->dataType == kFloatPrefType) {
			(*it)->valueFloat = aValue;
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->keyFloat = anIdentifier;
		aPreference->valueFloat = aValue;
		aPreference->defaultValueFloat = 0.0f;
		aPreference->valueInt = 0;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetButNotStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setValue(const PreferenceKeyChar anIdentifier, const char* const aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	
	char errStr[256] = "\0";
	if (strlen(aValue) > 255) {
		strncat(errStr, aValue, 255);
		writeLog("pref char value too long");
		return;
	}
	
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyChar == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy((*it)->valueChar, aValue);
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->keyChar = anIdentifier;
		strcpy(aPreference->valueChar, aValue);
		strcpy(aPreference->defaultValueChar, "\0");
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->status = kPrefSetButNotStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


void VisualPreferences::setValue(const PreferenceKeyBool anIdentifier, const bool aValue) {
	bool found = false;
	Preference* aPreference;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyBool == anIdentifier && (*it)->dataType == kBoolPrefType) {
			(*it)->valueBool = aValue;
			(*it)->status = kPrefSetButNotStored;
			found = true;
		}
	}
	if (found == false) {
		aPreference = new Preference;
		aPreference->dataType = kBoolPrefType;
		aPreference->keyBool = anIdentifier;
		aPreference->valueBool = aValue;
		aPreference->defaultValueBool = false;
		aPreference->valueInt = 0;
		aPreference->defaultValueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetButNotStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
}


int VisualPreferences::getDefault(const PreferenceKeyInt anIdentifier, bool* isDefaultValue) {
	char errLog[128];
	*isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyInt == anIdentifier && (*it)->dataType == kIntPrefType) {
			*isDefaultValue = true;
			return (*it)->defaultValueInt;
		}
	}
	char preferenceKeyStr[64];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Int not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return 0;
}


float VisualPreferences::getDefault(const PreferenceKeyFloat anIdentifier, bool* isDefaultValue) {
	char errLog[128];
	*isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyFloat == anIdentifier && (*it)->dataType == kFloatPrefType) {
			*isDefaultValue = true;
			return (*it)->defaultValueFloat;
		}
	}
	char preferenceKeyStr[64];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Float not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return 0.0f;
}


bool VisualPreferences::getDefault(const PreferenceKeyChar anIdentifier, char* outPrefVal) {
	char errLog[128];
	bool isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	strcpy(outPrefVal, "\0");
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyChar == anIdentifier && (*it)->dataType == kCharPrefType) {
			strcpy(outPrefVal, (*it)->defaultValueChar);
			isDefaultValue = true;
			return isDefaultValue;
		}
	}
	char preferenceKeyStr[64];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Char not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return isDefaultValue;
}


bool VisualPreferences::getDefault(const PreferenceKeyBool anIdentifier, bool* isDefaultValue) {
	char errLog[128];
	*isDefaultValue = false;
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyBool == anIdentifier && (*it)->dataType == kBoolPrefType) {
			*isDefaultValue = true;
			return (*it)->defaultValueBool;
		}
	}
	char preferenceKeyStr[64];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	sprintf(errLog, "Default Preference Bool not found: %s", preferenceKeyStr);
	writeLog(errLog);
	return false;
}


int VisualPreferences::getValue(const PreferenceKeyInt anIdentifier, bool* isDefaultValue) {
	
	bool isADefaultVal = false;
	bool wasStored = false;
	int prefVal = 0;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
	theVisualPreferences = VisualPreferences::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyInt == anIdentifier && (*it)->dataType == kIntPrefType) {
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
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
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
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::visualizerPluginIdentifierName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualPreferences->make_sure_path_exists(szAppData, false);
	
	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;
	
	char anIdentifierStr[128];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);
	
	if (theVisualPreferences->fileExists(szAppData)) {
		
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
	
	bool foundInVector = false;
	if (wasStored == false) {
		prefVal = theVisualPreferences->getDefault(anIdentifier, &isADefaultVal);
		foundInVector = true;
	} else {
		for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
			if ((*it)->keyInt == anIdentifier && (*it)->dataType == kIntPrefType) {
				(*it)->valueInt = prefVal;
				(*it)->status = kPrefSetAndStored;
				foundInVector = true;
			}
		}
	}
	
	if ((wasStored == true) && (foundInVector == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kIntPrefType;
		aPreference->keyInt = anIdentifier;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = prefVal;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->defaultValueBool = false;
		aPreference->valueBool = false;
		aPreference->status = kPrefSetAndStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
	
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	return prefVal;
}


float VisualPreferences::getValue(const PreferenceKeyFloat anIdentifier, bool* isDefaultValue) {
	
	bool isADefaultVal = false;
	bool wasStored = false;
	float prefVal = 0;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
	theVisualPreferences = VisualPreferences::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyFloat == anIdentifier && (*it)->dataType == kFloatPrefType) {
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
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
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
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::visualizerPluginIdentifierName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualPreferences->make_sure_path_exists(szAppData, false);
	
	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;
	
	char anIdentifierStr[128];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);
	
	if (theVisualPreferences->fileExists(szAppData)) {
		
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
	
	bool foundInVector = false;
	if (wasStored == false) {
		prefVal = theVisualPreferences->getDefault(anIdentifier, &isADefaultVal);
		foundInVector = true;
	} else {
		for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
			if ((*it)->keyFloat == anIdentifier && (*it)->dataType == kFloatPrefType) {
				(*it)->valueFloat = prefVal;
				(*it)->status = kPrefSetAndStored;
				foundInVector = true;
			}
		}
	}
	
	if ((wasStored == true) && (foundInVector == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kFloatPrefType;
		aPreference->keyFloat = anIdentifier;
		aPreference->defaultValueFloat = 0.0f;
		aPreference->valueFloat = prefVal;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->defaultValueBool = false;
		aPreference->valueBool = false;
		aPreference->status = kPrefSetAndStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
	
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	return prefVal;
}


void VisualPreferences::getValue(const PreferenceKeyChar anIdentifier, char* outPrefVal, bool* isDefaultValue) {
	
	bool isADefaultVal = false;
	bool wasStored = false;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
	strcpy(outPrefVal, "\0");
	
	theVisualPreferences = VisualPreferences::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyChar == anIdentifier && (*it)->dataType == kCharPrefType) {
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
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
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
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::visualizerPluginIdentifierName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualPreferences->make_sure_path_exists(szAppData, false);
	
	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;
	
	char anIdentifierStr[128];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);
	
	if (theVisualPreferences->fileExists(szAppData)) {
		
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
	
	bool foundInVector = false;
	if (wasStored == false) {
		isADefaultVal = theVisualPreferences->getDefault(anIdentifier, outPrefVal);
	} else {
		for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
			if ((*it)->keyChar == anIdentifier && (*it)->dataType == kCharPrefType) {
				strcpy((*it)->valueChar, outPrefVal);
				(*it)->status = kPrefSetAndStored;
				foundInVector = true;
			}
		}
	}
	
	if ((wasStored == true) && (foundInVector == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kCharPrefType;
		aPreference->keyChar = anIdentifier;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, outPrefVal);
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		aPreference->defaultValueBool = false;
		aPreference->valueBool = false;
		aPreference->status = kPrefSetAndStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
	
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
}


bool VisualPreferences::getValue(const PreferenceKeyBool anIdentifier, bool* isDefaultValue) {
	
	bool isADefaultVal = false;
	bool wasStored = false;
	bool prefVal = false;
	PreferenceVectorIterator it;
	
	isADefaultVal = false;
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	
	theVisualPreferences = VisualPreferences::getInstance();
	// check the preference vector to see if the value is already marked as set and stored
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->keyBool == anIdentifier && (*it)->dataType == kBoolPrefType) {
			if ((*it)->status == kPrefSetAndStored) {
				return (*it)->valueBool;
			}
		}
	}
	
	// the preference was not set and stored
	// we have to check the underlying preference system for persistently stored values
#if TARGET_OS_MAC
	CFStringRef identifierStr;
	CFBooleanRef boolRef = NULL;
	//Boolean macPrefVal = kCFBooleanFalse;
	
	char preferenceKeyStr[64];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, preferenceKeyStr);
	identifierStr = CFStringCreateWithCString(kCFAllocatorDefault, preferenceKeyStr, kCFStringEncodingISOLatin1);
	
	CFStringRef appID = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingISOLatin1);
	boolRef = (CFBooleanRef)CFPreferencesCopyValue(identifierStr, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
	CFRelease(appID);
	if (boolRef != NULL) {
		wasStored = true;
		//macPrefVal = CFBooleanGetValue(boolRef);
		if (boolRef == kCFBooleanFalse) {
			prefVal = false;
		} else {
			prefVal = true;
		}
		CFRelease(boolRef);
	} else {
		wasStored = false;
	}
	
	CFRelease(identifierStr);
	
#endif
	
	
#if TARGET_OS_WIN
	
	int winPrefVal = 0;
	TCHAR szAppData[1024];
	HRESULT hr;
	char prefPath[512];
	//hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szAppData);
	BOOL createFolderIfNotAlreadyThere = true;
	hr = SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::visualizerPluginIdentifierName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualPreferences->make_sure_path_exists(szAppData, false);
	
	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;
	
	char anIdentifierStr[128];
	VisualPreferences::convertPreferenceKeyToString(anIdentifier, anIdentifierStr);
	
	if (theVisualPreferences->fileExists(szAppData)) {
		
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
					wasStored = true;
					
					if (strcmp(valueStr, "true") == 0) {
						prefVal = true;
					} else {
						prefVal = false;
					}
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
	
	bool foundInVector = false;
	if (wasStored == false) {
		prefVal = theVisualPreferences->getDefault(anIdentifier, &isADefaultVal);
		foundInVector = true;
	} else {
		for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
			if ((*it)->keyBool == anIdentifier && (*it)->dataType == kBoolPrefType) {
				(*it)->valueBool = prefVal;
				(*it)->status = kPrefSetAndStored;
				foundInVector = true;
			}
		}
	}
	
	if ((wasStored == true) && (foundInVector == false)) {
		Preference* aPreference;
		aPreference = new Preference;
		aPreference->dataType = kBoolPrefType;
		aPreference->keyBool = anIdentifier;
		aPreference->defaultValueBool = false;
		aPreference->valueBool = prefVal;
		aPreference->defaultValueInt = 0;
		aPreference->valueInt = 0;
		strcpy(aPreference->defaultValueChar, "\0");
		strcpy(aPreference->valueChar, "\0");
		aPreference->status = kPrefSetAndStored;
		theVisualPreferences->preferenceVector.push_back(aPreference);
	}
	
	if (isDefaultValue) {
		*isDefaultValue = isADefaultVal;
	}
	return prefVal;
}


void VisualPreferences::storeValues() {
	
#if TARGET_OS_WIN
	BOOL success;
#endif
	
	PreferenceVectorIterator it;
	theVisualPreferences = VisualPreferences::getInstance();
	
#if TARGET_OS_MAC
	CFStringRef appID = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingISOLatin1);
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		CFStringRef identifierStr;
		CFNumberRef intRef = NULL;
		CFNumberRef floatRef = NULL;
		CFBooleanRef boolRef = NULL;
		CFStringRef prefString;
		
		if ((*it)->status == kPrefSetButNotStored) {
			char preferenceKeyStr[64];
			if ((*it)->dataType == kCharPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyChar, preferenceKeyStr);
			} else if ((*it)->dataType == kIntPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyInt, preferenceKeyStr);
			} else if ((*it)->dataType == kFloatPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyFloat, preferenceKeyStr);
			} else if ((*it)->dataType == kBoolPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyBool, preferenceKeyStr);
			}
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
			} else if ((*it)->dataType == kBoolPrefType) {
				boolRef = kCFBooleanFalse;
				if ((*it)->valueBool == true) {
					boolRef = kCFBooleanTrue;
				}
				//boolRef = CFBooleanCreate(kCFAllocatorDefault, kCFBooleanType, &(*it)->valueBool);
				//boolRef = CFBooleanCreate(kCFAllocatorDefault, kCFBooleanType, &boolVal);
				CFPreferencesSetValue(identifierStr, boolRef, appID, kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
				//CFRelease(boolRef);
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
	BOOL createFolderIfNotAlreadyThere = true;
	hr = SHGetSpecialFolderPath(NULL, szAppData, CSIDL_APPDATA, createFolderIfNotAlreadyThere);
	sprintf(prefPath, "\\%s\\%s", VisualConfiguration::kVisualPluginDomainIdentifier, VisualConfiguration::visualizerPluginIdentifierName);
	strcat(szAppData, prefPath);
	//PathAppend(szAppData);
	int createFolderResult;
	//createFolderResult = SHCreateDirectoryEx(NULL, szAppData, NULL);
	createFolderResult = theVisualPreferences->make_sure_path_exists(szAppData, false);
	
	strcat(szAppData, "\\Preferences.txt");
	
	HANDLE hFile;
	
	if (theVisualPreferences->fileExists(szAppData)) {
		
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
		char keyStr[128];
		char* valueStr;
		char valueNormalizedStr[64];
		int intVal;
		float floatVal;
		bool boolVal;
		char lineStr[255];
		bool found = false;
		bool isCharVal = false; // char value or int value
		uint16 aPrefDataType = 0;
		
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
					} else if (strcmp(valueStr, "true") == 0 || strcmp(valueStr, "false") == 0) {
						aPrefDataType = kBoolPrefType;
					} else {
						aPrefDataType = kIntPrefType;
					}
					
					found = false;
					char currKeyStr[128];
					for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
						if ((*it)->dataType == aPrefDataType) {
							if (aPrefDataType == kCharPrefType) {
								VisualPreferences::convertPreferenceKeyToString((*it)->keyChar, currKeyStr);
							} else if (aPrefDataType == kFloatPrefType) {
								VisualPreferences::convertPreferenceKeyToString((*it)->keyFloat, currKeyStr);
							} else if (aPrefDataType == kIntPrefType) {
								VisualPreferences::convertPreferenceKeyToString((*it)->keyInt, currKeyStr);
							} else if (aPrefDataType == kBoolPrefType) {
								VisualPreferences::convertPreferenceKeyToString((*it)->keyBool, currKeyStr);
							}
							if (strcmp(currKeyStr, keyStr) == 0) {
								if ((*it)->status == kDefaultPref) {
									found = true;
									// the value was persistently stored
									// while preferenceVector up to now only knew the default value
									if (aPrefDataType == kCharPrefType) {
										// we have to re-save the already stored value
										valueStr++;
										strcpy(valueNormalizedStr, "\0");
										strncat(valueNormalizedStr, valueStr, strlen(valueStr) - 1);
										theVisualPreferences->setValue((*it)->keyChar, valueNormalizedStr);
									} else if (aPrefDataType == kFloatPrefType) {
										// we have to re-save the already stored value
										floatVal = (float)atof(valueStr);
										theVisualPreferences->setValue((*it)->keyFloat, floatVal);
									} else if (aPrefDataType == kIntPrefType) {
										// we have to re-save the already stored value
										intVal = atoi(valueStr);
										theVisualPreferences->setValue((*it)->keyInt, intVal);
									} else if (aPrefDataType == kBoolPrefType) {
										// we have to re-save the already stored value
										if (strcmp(valueStr, "false") == 0) {
											boolVal = false;
										} else {
											boolVal = true;
										}
										theVisualPreferences->setValue((*it)->keyBool, boolVal);
									}
								} else {
									// new value has been set but not yet stored
									found = true;
								}
							}
						}
					}
					
					if (found == false) {
						// the value was persistently stored
						// and not known to preferenceVector
						// (because no default pref was set)
						if (aPrefDataType == kCharPrefType) {
							valueStr++;
							strcpy(valueNormalizedStr, "\0");
							strncat(valueNormalizedStr, valueStr, strlen(valueStr) - 1);
							theVisualPreferences->setValue(VisualPreferences::convertPreferenceKeyCharStringToKey(keyStr), valueNormalizedStr);
						} else if (aPrefDataType == kFloatPrefType) {
							floatVal = (float)atof(valueStr);
							theVisualPreferences->setValue(VisualPreferences::convertPreferenceKeyFloatStringToKey(keyStr), floatVal);
						} else if (aPrefDataType == kIntPrefType) {
							intVal = atoi(valueStr);
							theVisualPreferences->setValue(VisualPreferences::convertPreferenceKeyIntStringToKey(keyStr), intVal);
						} else if (aPrefDataType == kBoolPrefType) {
							bool boolVal = false;
							if (strcmp(valueStr, "true") == 0) {
								boolVal = true;
							}
							theVisualPreferences->setValue(VisualPreferences::convertPreferenceKeyBoolStringToKey(keyStr), boolVal);
						}
						
					}
					
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
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->status != kDefaultPref) {
			// store all pref vals persistently that are not pure default preferences
			strcpy(prefLineStr, "\0");
			char currKeyStr[128];
			if ((*it)->dataType == kCharPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyChar, currKeyStr);
				if (strcmp((*it)->valueChar, (*it)->defaultValueChar) != 0) {
					sprintf(prefLineStr, "%s = \"%s\"\r\n", currKeyStr, (*it)->valueChar);
				}
			} else if ((*it)->dataType == kFloatPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyFloat, currKeyStr);
				if ((*it)->valueFloat != (*it)->defaultValueFloat) {
					sprintf(prefLineStr, "%s = %.6f\r\n", currKeyStr, (*it)->valueFloat);
				}
			} else if ((*it)->dataType == kIntPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyInt, currKeyStr);
				if ((*it)->valueInt != (*it)->defaultValueInt) {
					sprintf(prefLineStr, "%s = %d\r\n", currKeyStr, (*it)->valueInt);
				}
			} else if ((*it)->dataType == kBoolPrefType) {
				VisualPreferences::convertPreferenceKeyToString((*it)->keyBool, currKeyStr);
				const char* kTrueStr = "true";
				const char* kFalseStr = "false";
				if ((*it)->valueBool != (*it)->defaultValueBool) {
					if ((*it)->valueBool == false) {
						sprintf(prefLineStr, "%s = %s\r\n", currKeyStr, kFalseStr);
					} else {
						sprintf(prefLineStr, "%s = %s\r\n", currKeyStr, kTrueStr);
					}
				}
			}
			if (strlen(prefLineStr) > 0) {
				success = WriteFile(hFile, prefLineStr, strlen(prefLineStr), &numberOfBytesWritten, NULL);
			}
		}
	}
	
	CloseHandle(hFile);
	
#endif
	
	for (it = theVisualPreferences->preferenceVector.begin(); it != theVisualPreferences->preferenceVector.end(); it++) {
		if ((*it)->status == kPrefSetButNotStored) {
			(*it)->status = kPrefSetAndStored;
		}
	}
	
}


void VisualPreferences::convertPreferenceKeyToString(const PreferenceKeyInt aKey, char* outString) {
	const char* messageString;
	switch (aKey) {
		case kFadeInTimeOnPlayInMS:
			messageString = "kFadeInTimeOnPlayInMS";
			break;
		case kFadeInTimeOnResumeInMS:
			messageString = "kFadeInTimeOnResumeInMS";
			break;
		case kFadeOutTimeBeforeEndOfTrackInMS:
			messageString = "kFadeOutTimeBeforeEndOfTrackInMS";
			break;
		case kFadeOutTimeOnPauseInMS:
			messageString = "kFadeOutTimeOnPauseInMS";
			break;
		case kTrackInfoFontSize:
			messageString = "kTrackInfoFontSize";
			break;
		case kTrackLyricsFontSize:
			messageString = "kTrackLyricsFontSize";
			break;
		case kPreferencePane:
			messageString = "kPreferencePane";
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unhandled PreferenceKey %d in file: %s (line: %d) [%s])", aKey, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			messageString = "kUnknownPref";
	}
	strcpy(outString, messageString);
}


void VisualPreferences::convertPreferenceKeyToString(const PreferenceKeyFloat aKey, char* outString) {
	const char* messageString;
	switch (aKey) {
		case kTrackInfoFontColorRedFloat:
			messageString = "kTrackInfoFontColorRedFloat";
			break;
		case kTrackInfoFontColorGreenFloat:
			messageString = "kTrackInfoFontColorGreenFloat";
			break;
		case kTrackInfoFontColorBlueFloat:
			messageString = "kTrackInfoFontColorBlueFloat";
			break;
		case kTrackInfoFontColorAlphaFloat:
			messageString = "kTrackInfoFontColorAlphaFloat";
			break;
		case kTrackInfoTextureColorTopLeftRed:
			messageString = "kTrackInfoTextureColorTopLeftRed";
			break;
		case kTrackInfoTextureColorTopLeftGreen:
			messageString = "kTrackInfoTextureColorTopLeftGreen";
			break;
		case kTrackInfoTextureColorTopLeftBlue:
			messageString = "kTrackInfoTextureColorTopLeftBlue";
			break;
		case kTrackInfoTextureColorTopLeftAlpha:
			messageString = "kTrackInfoTextureColorTopLeftAlpha";
			break;
		case kTrackInfoTextureColorBottomLeftRed:
			messageString = "kTrackInfoTextureColorBottomLeftRed";
			break;
		case kTrackInfoTextureColorBottomLeftGreen:
			messageString = "kTrackInfoTextureColorBottomLeftGreen";
			break;
		case kTrackInfoTextureColorBottomLeftBlue:
			messageString = "kTrackInfoTextureColorBottomLeftBlue";
			break;
		case kTrackInfoTextureColorBottomLeftAlpha:
			messageString = "kTrackInfoTextureColorBottomLeftAlpha";
			break;
		case kTrackInfoTextureColorBottomRightRed:
			messageString = "kTrackInfoTextureColorBottomRightRed";
			break;
		case kTrackInfoTextureColorBottomRightGreen:
			messageString = "kTrackInfoTextureColorBottomRightGreen";
			break;
		case kTrackInfoTextureColorBottomRightBlue:
			messageString = "kTrackInfoTextureColorBottomRightBlue";
			break;
		case kTrackInfoTextureColorBottomRightAlpha:
			messageString = "kTrackInfoTextureColorBottomRightAlpha";
			break;
		case kTrackInfoTextureColorTopRightRed:
			messageString = "kTrackInfoTextureColorTopRightRed";
			break;
		case kTrackInfoTextureColorTopRightGreen:
			messageString = "kTrackInfoTextureColorTopRightGreen";
			break;
		case kTrackInfoTextureColorTopRightBlue:
			messageString = "kTrackInfoTextureColorTopRightBlue";
			break;
		case kTrackInfoTextureColorTopRightAlpha:
			messageString = "kTrackInfoTextureColorTopRightAlpha";
			break;
		case kTrackLyricsFontColorRedFloat:
			messageString = "kTrackLyricsFontColorRedFloat";
			break;
		case kTrackLyricsFontColorGreenFloat:
			messageString = "kTrackLyricsFontColorGreenFloat";
			break;
		case kTrackLyricsFontColorBlueFloat:
			messageString = "kTrackLyricsFontColorBlueFloat";
			break;
		case kTrackLyricsFontColorAlphaFloat:
			messageString = "kTrackLyricsFontColorAlphaFloat";
			break;
		case kTrackLyricsTextureColorTopLeftRed:
			messageString = "kTrackLyricsTextureColorTopLeftRed";
			break;
		case kTrackLyricsTextureColorTopLeftGreen:
			messageString = "kTrackLyricsTextureColorTopLeftGreen";
			break;
		case kTrackLyricsTextureColorTopLeftBlue:
			messageString = "kTrackLyricsTextureColorTopLeftBlue";
			break;
		case kTrackLyricsTextureColorTopLeftAlpha:
			messageString = "kTrackLyricsTextureColorTopLeftAlpha";
			break;
		case kTrackLyricsTextureColorBottomLeftRed:
			messageString = "kTrackLyricsTextureColorBottomLeftRed";
			break;
		case kTrackLyricsTextureColorBottomLeftGreen:
			messageString = "kTrackLyricsTextureColorBottomLeftGreen";
			break;
		case kTrackLyricsTextureColorBottomLeftBlue:
			messageString = "kTrackLyricsTextureColorBottomLeftBlue";
			break;
		case kTrackLyricsTextureColorBottomLeftAlpha:
			messageString = "kTrackLyricsTextureColorBottomLeftAlpha";
			break;
		case kTrackLyricsTextureColorBottomRightRed:
			messageString = "kTrackLyricsTextureColorBottomRightRed";
			break;
		case kTrackLyricsTextureColorBottomRightGreen:
			messageString = "kTrackLyricsTextureColorBottomRightGreen";
			break;
		case kTrackLyricsTextureColorBottomRightBlue:
			messageString = "kTrackLyricsTextureColorBottomRightBlue";
			break;
		case kTrackLyricsTextureColorBottomRightAlpha:
			messageString = "kTrackLyricsTextureColorBottomRightAlpha";
			break;
		case kTrackLyricsTextureColorTopRightRed:
			messageString = "kTrackLyricsTextureColorTopRightRed";
			break;
		case kTrackLyricsTextureColorTopRightGreen:
			messageString = "kTrackLyricsTextureColorTopRightGreen";
			break;
		case kTrackLyricsTextureColorTopRightBlue:
			messageString = "kTrackLyricsTextureColorTopRightBlue";
			break;
		case kTrackLyricsTextureColorTopRightAlpha:
			messageString = "kTrackLyricsTextureColorTopRightAlpha";
			break;
		case kCanvasBackgroundColorRed:
			messageString = "kCanvasBackgroundColorRed";
			break;
		case kCanvasBackgroundColorGreen:
			messageString = "kCanvasBackgroundColorGreen";
			break;
		case kCanvasBackgroundColorBlue:
			messageString = "kCanvasBackgroundColorBlue";
			break;
		case kCanvasBackgroundColorAlpha:
			messageString = "kCanvasBackgroundColorAlpha";
			break;
		case kSurfaceBackgroundColorRed:
			messageString = "kSurfaceBackgroundColorRed";
			break;
		case kSurfaceBackgroundColorGreen:
			messageString = "kSurfaceBackgroundColorGreen";
			break;
		case kSurfaceBackgroundColorBlue:
			messageString = "kSurfaceBackgroundColorBlue";
			break;
		case kSurfaceBackgroundColorAlpha:
			messageString = "kSurfaceBackgroundColorAlpha";
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unhandled PreferenceKey %d in file: %s (line: %d) [%s])", aKey, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			messageString = "kUnknownPref";
	}
	strcpy(outString, messageString);
}


void VisualPreferences::convertPreferenceKeyToString(const PreferenceKeyChar aKey, char* outString) {
	const char* messageString;
	switch (aKey) {
		case kTrackInfoDisplayItemsDelimiterCharacters:
			messageString = "kTrackInfoDisplayItemsDelimiterCharacters";
			break;
		case kTrackInfoFont:
			messageString = "kTrackInfoFont";
			break;
		case kTrackLyricsFont:
			messageString = "kTrackLyricsFont";
			break;
		case kTrackLyricsAlignment:
			messageString = "kTrackLyricsAlignment";
			break;
		case kFallbackSystemFont:
			messageString = "kFallbackSystemFont";
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unhandled PreferenceKey %d in file: %s (line: %d) [%s])", aKey, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			messageString = "kUnknownPref";
	}
	strcpy(outString, messageString);
}


void VisualPreferences::convertPreferenceKeyToString(const PreferenceKeyBool aKey, char* outString) {
	const char* messageString;
	switch (aKey) {
		case (kUnknownPrefBool):
			messageString = "kUnknownPrefBool";
			break;
		case (kCheckForUpdate):
			messageString = "kCheckForUpdate";
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unhandled PreferenceKey %d in file: %s (line: %d) [%s])", aKey, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			messageString = "kUnknownPref";
	}
	strcpy(outString, messageString);
}


VisualPreferences::PreferenceKeyInt VisualPreferences::convertPreferenceKeyIntStringToKey(const char* const aPreferenceKeyString) {
	if (strcmp(aPreferenceKeyString, "kFadeInTimeOnPlayInMS") == 0) {
		return kFadeInTimeOnPlayInMS;
	}
	if (strcmp(aPreferenceKeyString, "kFadeInTimeOnResumeInMS") == 0) {
		return kFadeInTimeOnResumeInMS;
	}
	if (strcmp(aPreferenceKeyString, "kFadeOutTimeBeforeEndOfTrackInMS") == 0) {
		return kFadeOutTimeBeforeEndOfTrackInMS;
	}
	if (strcmp(aPreferenceKeyString, "kFadeOutTimeOnPauseInMS") == 0) {
		return kFadeOutTimeOnPauseInMS;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoFontSize") == 0) {
		return kTrackInfoFontSize;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsFontSize") == 0) {
		return kTrackLyricsFontSize;
	}
	if (strcmp(aPreferenceKeyString, "kPreferencePane") == 0) {
		return kPreferencePane;
	}
	
	char errLog[256];
	sprintf(errLog, "unknown PreferenceKeyStr %s in file: %s (line: %d) [%s])", aPreferenceKeyString, __FILE__, __LINE__, __FUNCTION__);
	writeLog(errLog);
	
	return kUnknownPrefInt;
}


VisualPreferences::PreferenceKeyFloat VisualPreferences::convertPreferenceKeyFloatStringToKey(const char* const aPreferenceKeyString) {
	if (strcmp(aPreferenceKeyString, "kTrackInfoFontColorRedFloat") == 0) {
		return kTrackInfoFontColorRedFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoFontColorGreenFloat") == 0) {
		return kTrackInfoFontColorGreenFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoFontColorBlueFloat") == 0) {
		return kTrackInfoFontColorBlueFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoFontColorAlphaFloat") == 0) {
		return kTrackInfoFontColorAlphaFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopLeftRed") == 0) {
		return kTrackInfoTextureColorTopLeftRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopLeftGreen") == 0) {
		return kTrackInfoTextureColorTopLeftGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopLeftBlue") == 0) {
		return kTrackInfoTextureColorTopLeftBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopLeftAlpha") == 0) {
		return kTrackInfoTextureColorTopLeftAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomLeftRed") == 0) {
		return kTrackInfoTextureColorBottomLeftRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomLeftGreen") == 0) {
		return kTrackInfoTextureColorBottomLeftGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomLeftBlue") == 0) {
		return kTrackInfoTextureColorBottomLeftBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomLeftAlpha") == 0) {
		return kTrackInfoTextureColorBottomLeftAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomRightRed") == 0) {
		return kTrackInfoTextureColorBottomRightRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomRightGreen") == 0) {
		return kTrackInfoTextureColorBottomRightGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomRightBlue") == 0) {
		return kTrackInfoTextureColorBottomRightBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorBottomRightAlpha") == 0) {
		return kTrackInfoTextureColorBottomRightAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopRightRed") == 0) {
		return kTrackInfoTextureColorTopRightRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopRightGreen") == 0) {
		return kTrackInfoTextureColorTopRightGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopRightBlue") == 0) {
		return kTrackInfoTextureColorTopRightBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoTextureColorTopRightAlpha") == 0) {
		return kTrackInfoTextureColorTopRightAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsFontColorRedFloat") == 0) {
		return kTrackLyricsFontColorRedFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsFontColorGreenFloat") == 0) {
		return kTrackLyricsFontColorGreenFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsFontColorBlueFloat") == 0) {
		return kTrackLyricsFontColorBlueFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsFontColorAlphaFloat") == 0) {
		return kTrackLyricsFontColorAlphaFloat;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopLeftRed") == 0) {
		return kTrackLyricsTextureColorTopLeftRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopLeftGreen") == 0) {
		return kTrackLyricsTextureColorTopLeftGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopLeftBlue") == 0) {
		return kTrackLyricsTextureColorTopLeftBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopLeftAlpha") == 0) {
		return kTrackLyricsTextureColorTopLeftAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomLeftRed") == 0) {
		return kTrackLyricsTextureColorBottomLeftRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomLeftGreen") == 0) {
		return kTrackLyricsTextureColorBottomLeftGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomLeftBlue") == 0) {
		return kTrackLyricsTextureColorBottomLeftBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomLeftAlpha") == 0) {
		return kTrackLyricsTextureColorBottomLeftAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomRightRed") == 0) {
		return kTrackLyricsTextureColorBottomRightRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomRightGreen") == 0) {
		return kTrackLyricsTextureColorBottomRightGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomRightBlue") == 0) {
		return kTrackLyricsTextureColorBottomRightBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorBottomRightAlpha") == 0) {
		return kTrackLyricsTextureColorBottomRightAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopRightRed") == 0) {
		return kTrackLyricsTextureColorTopRightRed;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopRightGreen") == 0) {
		return kTrackLyricsTextureColorTopRightGreen;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopRightBlue") == 0) {
		return kTrackLyricsTextureColorTopRightBlue;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsTextureColorTopRightAlpha") == 0) {
		return kTrackLyricsTextureColorTopRightAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kCanvasBackgroundColorRed") == 0) {
		return kCanvasBackgroundColorRed;
	}
	if (strcmp(aPreferenceKeyString, "kCanvasBackgroundColorGreen") == 0) {
		return kCanvasBackgroundColorGreen;
	}
	if (strcmp(aPreferenceKeyString, "kCanvasBackgroundColorBlue") == 0) {
		return kCanvasBackgroundColorBlue;
	}
	if (strcmp(aPreferenceKeyString, "kCanvasBackgroundColorAlpha") == 0) {
		return kCanvasBackgroundColorAlpha;
	}
	if (strcmp(aPreferenceKeyString, "kSurfaceBackgroundColorRed") == 0) {
		return kSurfaceBackgroundColorRed;
	}
	if (strcmp(aPreferenceKeyString, "kSurfaceBackgroundColorGreen") == 0) {
		return kSurfaceBackgroundColorGreen;
	}
	if (strcmp(aPreferenceKeyString, "kSurfaceBackgroundColorBlue") == 0) {
		return kSurfaceBackgroundColorBlue;
	}
	if (strcmp(aPreferenceKeyString, "kSurfaceBackgroundColorAlpha") == 0) {
		return kSurfaceBackgroundColorAlpha;
	}
	
	char errLog[256];
	sprintf(errLog, "unknown PreferenceKeyStr %s in file: %s (line: %d) [%s])", aPreferenceKeyString, __FILE__, __LINE__, __FUNCTION__);
	writeLog(errLog);
	
	return kUnknownPrefFloat;
}


VisualPreferences::PreferenceKeyChar VisualPreferences::convertPreferenceKeyCharStringToKey(const char* const aPreferenceKeyString) {
	if (strcmp(aPreferenceKeyString, "kTrackInfoDisplayItemsDelimiterCharacters") == 0) {
		return kTrackInfoDisplayItemsDelimiterCharacters;
	}
	if (strcmp(aPreferenceKeyString, "kTrackInfoFont") == 0) {
		return kTrackInfoFont;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsFont") == 0) {
		return kTrackLyricsFont;
	}
	if (strcmp(aPreferenceKeyString, "kTrackLyricsAlignment") == 0) {
		return kTrackLyricsAlignment;
	}
	
	char errLog[256];
	sprintf(errLog, "unknown PreferenceKeyStr %s in file: %s (line: %d) [%s])", aPreferenceKeyString, __FILE__, __LINE__, __FUNCTION__);
	writeLog(errLog);
	
	return kUnknownPrefChar;
}


VisualPreferences::PreferenceKeyBool VisualPreferences::convertPreferenceKeyBoolStringToKey(const char* const aPreferenceKeyString) {
	if (strcmp(aPreferenceKeyString, "kCheckForUpdate") == 0) {
		return kCheckForUpdate;
	}
	char errLog[256];
	sprintf(errLog, "unknown PreferenceKeyStr %s in file: %s (line: %d) [%s])", aPreferenceKeyString, __FILE__, __LINE__, __FUNCTION__);
	writeLog(errLog);
	
	return kUnknownPrefBool;
}


void VisualPreferences::setDefaultValues() {
	
	// Factory Preferences
	VisualPreferences::setDefault(VisualPreferences::kFadeInTimeOnPlayInMS, 5000);
	VisualPreferences::setDefault(VisualPreferences::kFadeInTimeOnResumeInMS, 2000);
	VisualPreferences::setDefault(VisualPreferences::kFadeOutTimeBeforeEndOfTrackInMS, 5000);
	VisualPreferences::setDefault(VisualPreferences::kFadeOutTimeOnPauseInMS, 2000);
	
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoDisplayItemsDelimiterCharacters, " : ");
	
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoFont, "Devroye");
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoFontSize, 48);
	
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoFontColorRedFloat, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoFontColorGreenFloat, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoFontColorBlueFloat, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoFontColorAlphaFloat, 1.0f);
	
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopLeftRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopLeftGreen, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopLeftBlue, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopLeftAlpha, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomLeftRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomLeftGreen, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomLeftBlue, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomLeftAlpha, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomRightRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomRightGreen, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomRightBlue, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorBottomRightAlpha, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopRightRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopRightGreen, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopRightBlue, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackInfoTextureColorTopRightAlpha, 1.0f);
	
#if TARGET_OS_MAC
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsFont, "Lucida Grande");
#endif
#if TARGET_OS_WIN
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsFont, "Tahoma");
#endif
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsFontSize, 18);
	
	VisualPreferences::setDefault(kTrackLyricsFontColorRedFloat, 1.0f);
	VisualPreferences::setDefault(kTrackLyricsFontColorGreenFloat, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsFontColorBlueFloat, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsFontColorAlphaFloat, 1.0f);
	
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopLeftRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopLeftGreen, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopLeftBlue, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopLeftAlpha, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomLeftRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomLeftGreen, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomLeftBlue, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomLeftAlpha, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomRightRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomRightGreen, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomRightBlue, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorBottomRightAlpha, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopRightRed, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopRightGreen, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopRightBlue, 1.0f);
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsTextureColorTopRightAlpha, 1.0f);
	
	VisualPreferences::setDefault(VisualPreferences::kTrackLyricsAlignment, "center");
	
#if TARGET_OS_MAC
	VisualPreferences::setDefault(VisualPreferences::kFallbackSystemFont, "Lucida Grande");
#endif
#if TARGET_OS_WIN
	VisualPreferences::setDefault(VisualPreferences::kFallbackSystemFont, "Tahoma");
#endif
	
	VisualPreferences::setDefault(VisualPreferences::kCanvasBackgroundColorRed, (12.0f / 255.0f));
	VisualPreferences::setDefault(VisualPreferences::kCanvasBackgroundColorGreen, (9.0f / 255.0f));
	VisualPreferences::setDefault(VisualPreferences::kCanvasBackgroundColorBlue, (172.0f / 255.0f));
	VisualPreferences::setDefault(VisualPreferences::kCanvasBackgroundColorAlpha, 1.0f);
	
	VisualPreferences::setDefault(VisualPreferences::kSurfaceBackgroundColorRed, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kSurfaceBackgroundColorGreen, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kSurfaceBackgroundColorBlue, 0.0f);
	VisualPreferences::setDefault(VisualPreferences::kSurfaceBackgroundColorAlpha, 1.0f);
	
#if TARGET_OS_MAC
	VisualPreferences::setDefault(VisualPreferences::kPreferencePane, 1);
#endif
#if TARGET_OS_WIN
	VisualPreferences::setDefault(VisualPreferences::kPreferencePane, 0);
#endif
	
	VisualPreferences::setDefault(VisualPreferences::kCheckForUpdate, true);
	
}


#if TARGET_OS_WIN

int VisualPreferences::make_sure_path_exists(const char *iPath, bool FilenameIncluded) {
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


bool VisualPreferences::fileExists(const char* szFileName) {
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
