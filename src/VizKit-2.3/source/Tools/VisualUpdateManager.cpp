/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualUpdateManager.cpp
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

#include "VisualUpdateManager.h"
#include "VisualTypes.h"
#include "VisualErrorHandling.h"
#include "VisualPreferences.h"
#include "VisualConfiguration.h"
#include "VisualDataStore.h"

#if TARGET_OS_MAC
#include <Carbon/Carbon.h> // CFURLRef
#endif


using namespace VizKit;


VisualUpdateManager* VisualUpdateManager::theVisualUpdateManager = NULL;


VisualUpdateManager::VisualUpdateManager() {
	updateInformation = NULL;
}


VisualUpdateManager::~VisualUpdateManager() {
	if (updateInformation != NULL) {
		delete updateInformation;
	}
}


VisualUpdateManager* VisualUpdateManager::getInstance() {
    if (theVisualUpdateManager == NULL) {
		theVisualUpdateManager = new VisualUpdateManager;
    }
	if (theVisualUpdateManager == NULL) {
		writeLog("ERR: init theVisualUpdateManager failed");
	}
	return theVisualUpdateManager;
}


void VisualUpdateManager::dispose() {
	if (theVisualUpdateManager != NULL) {
		delete theVisualUpdateManager;
		theVisualUpdateManager = NULL;
	}
}


void VisualUpdateManager::checkForUpdate() {
	bool doCheck = VisualPreferences::getValue(VisualPreferences::kCheckForUpdate);
	if (doCheck) {
		theVisualUpdateManager = VisualUpdateManager::getInstance();
		theVisualUpdateManager->updateInformation = VisualString::createWithContentsOfURL(VisualConfiguration::kCurrentVersionInformationURL, VisualDataStore::updateInformationStringId);
	}
}


void VisualUpdateManager::getMoreInfoAboutAvailableUpdate() {
	const char* URLBytes = theVisualUpdateManager->infoURL.getUtf8Representation();
	theVisualUpdateManager = VisualUpdateManager::getInstance();
	theVisualUpdateManager->launchBrowserToOpenURL(URLBytes);
}


void VisualUpdateManager::doDownloadAvailableUpdate(void) {
	const char* URLBytes = theVisualUpdateManager->downloadURL.getUtf8Representation();
	theVisualUpdateManager = VisualUpdateManager::getInstance();
	theVisualUpdateManager->launchBrowserToOpenURL(URLBytes);
}


void VisualUpdateManager::launchBrowserToOpenURL(const char* const URL) {
#if TARGET_OS_MAC
	CFIndex length = strlen(URL);
	CFStringEncoding encoding = kCFStringEncodingASCII;
	CFURLRef baseURL = NULL;
	CFURLRef url = CFURLCreateWithBytes(kCFAllocatorDefault, (UInt8*)URL, length, encoding, baseURL);
	CFArrayCallBacks* callBacks = NULL;
	CFMutableArrayRef inURLs = CFArrayCreateMutable(kCFAllocatorDefault, 1, callBacks);
	CFArrayAppendValue(inURLs, url);
	LSRolesMask inRole = kLSRolesViewer;
	const AEKeyDesc* inAEParam = NULL;
	const LSApplicationParameters* inAppParams = NULL;
	ProcessSerialNumber* outPSNs = NULL;
	CFIndex inMaxPSNCount = 0;
	OSStatus osStatus = LSOpenURLsWithRole(inURLs, inRole, inAEParam, inAppParams, outPSNs, inMaxPSNCount);
	if (osStatus == noErr) {
		printf("no Err\n");
	} else {
		printf("Err launch: %ld\n", osStatus);
	}
#endif
#if TARGET_OS_WIN
	ShellExecute(NULL, "open", URL , NULL, NULL, SW_SHOW);
#endif
}


bool VisualUpdateManager::evaluateUpdateInformation() {
	bool updateIsAvailable = false;
	theVisualUpdateManager = VisualUpdateManager::getInstance();
	VisualString delimiterVerticalLine;
	delimiterVerticalLine.initWithUTF8Buffer("|", 1);
	std::vector<VisualString> stringVector = theVisualUpdateManager->updateInformation->splitWithDelimiter(delimiterVerticalLine);
	int count = 0;
	for (std::vector<VisualString>::iterator it = stringVector.begin(); it != stringVector.end(); it++) {
		if (count == 0) {
			theVisualUpdateManager->currentVersionString = *(it);
			theVisualUpdateManager->currentVersionString.trim();
		} else if (count == 1) {
			theVisualUpdateManager->infoURL = *(it);
			theVisualUpdateManager->infoURL.trim();
		} else if (count == 2) {
			theVisualUpdateManager->downloadURL = *(it);
			theVisualUpdateManager->downloadURL.trim();
		}
		count++;
	}
	uint8 currMajorVersion = 0;
	uint8 currMinorVersion = 0;
	uint8 currReleaseRevision = 0;
	VisualString delimiterPoint;
	delimiterPoint.initWithUTF8Buffer(".", 1);
	std::vector<VisualString> versionStringVector = theVisualUpdateManager->currentVersionString.splitWithDelimiter(delimiterPoint);
	count = 0;
	for (std::vector<VisualString>::iterator it = versionStringVector.begin(); it != versionStringVector.end(); it++) {
		if (count == 0) {
			currMajorVersion = atoi(it->getUtf8Representation());
		} else if (count == 1) {
			currMinorVersion = atoi(it->getUtf8Representation());
		} else if (count == 2) {
			currReleaseRevision = atoi(it->getUtf8Representation());
		}
		count++;
	}
	if (currMajorVersion > VisualConfiguration::kMajorReleaseNumber) {
		updateIsAvailable = true;
	} else if (currMajorVersion == VisualConfiguration::kMajorReleaseNumber) {
		if (currMinorVersion > VisualConfiguration::kMinorReleaseNumber) {
			updateIsAvailable = true;
		} else if (currMinorVersion == VisualConfiguration::kMinorReleaseNumber) {
			if (currReleaseRevision > VisualConfiguration::kReleaseRevisionNumber) {
				updateIsAvailable = true;
			}
		}
	}
	return updateIsAvailable;
}
