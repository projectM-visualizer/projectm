/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNetwork.cpp
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

#include "VisualNetwork.h"
#include "VisualDownloadQueue.h"
#include "VisualErrorHandling.h"
#include "VisualString.h"
#include "VisualDataStore.h"

#if TARGET_OS_WIN
#include <shlwapi.h> // UrlEscapeW
#endif

using namespace VizKit;


void VisualNetwork::dispose() {
	VisualDownloadQueue::dispose();
}


bool VisualNetwork::startDownloadThreadIfNeeded() {
	return VisualDownloadQueue::startDownloadThreadIfNeeded();
}


bool VisualNetwork::addToDownloadQueue(VisualString& aURL, VisualObject* visualObject, const VisualItemIdentifier& identifier) {
	bool success = VisualDownloadQueue::addToDownloadQueue(aURL, visualObject, identifier);
	return success;
}


bool VisualNetwork::removeDownload(const VisualItemIdentifier& identifier) {
	return VisualDownloadQueue::removeDownload(identifier);
}


bool VisualNetwork::removeDownloadForObject(const VisualItemIdentifier& visualObjectIdentifier) {
	return VisualDownloadQueue::removeDownloadForObject(visualObjectIdentifier);
}

/*
intshcut.h // url.dll
BOOL InetIsOffline(          DWORD dwFlags
);
*/


VisualString* VisualNetwork::createNormalizedURLString(const VisualString& str) {
	VisualString* normalizedURLString = NULL;
#if TARGET_OS_MAC

	CFStringRef urlStr = str.getCharactersPointer();

	CFStringRef normalizedCFString = CFURLCreateStringByReplacingPercentEscapes(kCFAllocatorDefault, urlStr, CFSTR(""));
	if (normalizedCFString == NULL) {
		return NULL;
	}
	
	CFStringRef charactersToLeaveUnescaped = NULL;
	CFStringRef legalURLCharactersToBeEscaped = NULL;
	CFStringRef escapedCFString = CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault, normalizedCFString, charactersToLeaveUnescaped, legalURLCharactersToBeEscaped, kCFStringEncodingUTF8);
	if (escapedCFString == NULL) {
		CFRelease(normalizedCFString);
		return NULL;
	}
	
	CFRelease(normalizedCFString);

	char* escapedCString = NULL;
	CFRange range = {0, CFStringGetLength(escapedCFString)};
	uint8 lossByte = 0;
	Boolean isExternalRepresentation = false;
	CFIndex maxBufLen = 0;
	CFIndex numberOfBytes = 0;
	CFIndex numberOfCharsConverted;
	numberOfCharsConverted = CFStringGetBytes(escapedCFString, range, kCFStringEncodingUTF8, lossByte, isExternalRepresentation, NULL, maxBufLen, &numberOfBytes);
	if ((numberOfCharsConverted == CFStringGetLength(escapedCFString)) && (numberOfBytes > 0)) {
		escapedCString = (char*)malloc(numberOfBytes + 1);
		numberOfCharsConverted = CFStringGetBytes(escapedCFString, range, kCFStringEncodingUTF8, lossByte, isExternalRepresentation, (uint8*)escapedCString, numberOfBytes, &numberOfBytes);
		escapedCString[numberOfBytes] = '\0';
	}
	CFRelease(escapedCFString);

	normalizedURLString = VisualString::createWithUTF8String(escapedCString);
	free(escapedCString);

#endif

#if TARGET_OS_WIN
	const wchar_t* const urlStr = str.getCharactersPointer();
	wchar_t SPC = L' ';
	LPCWSTR pch;
	DWORD cSpaces = 0;
	for (pch = urlStr; *pch; pch++)
    {
        if (*pch == SPC)
            cSpaces++;
    }
	const wchar_t* const urlStr2 = str.getCharactersPointer();

	wchar_t* escapedStr = (wchar_t*)malloc((lstrlenW(urlStr2) + (cSpaces * 2) + 1) * sizeof(wchar_t));
	wchar_t* escapedStrPtr = NULL;
	wchar_t* urlStrPtr = const_cast<wchar_t*>(urlStr2);
	
	for (escapedStrPtr = escapedStr; *urlStrPtr != L'\0'; urlStrPtr++) {
		if (*urlStrPtr == SPC) {
			*escapedStrPtr++ = L'%';
			*escapedStrPtr++ = L'2';
			*escapedStrPtr++ = L'0';
		} else {
			*escapedStrPtr++ = *urlStrPtr;
		}
	}
	*escapedStrPtr = L'\0';

	const wchar_t* const urlStr3 = str.getCharactersPointer();

	memcpy(escapedStr, urlStr3, (lstrlenW(urlStr3) + 1) * sizeof(wchar_t)); // todo

	normalizedURLString = VisualString::createWithUTF16Buffer((const uint16* const)escapedStr, lstrlenW(urlStr3));

	free(escapedStr);
#endif

	return normalizedURLString;

}

