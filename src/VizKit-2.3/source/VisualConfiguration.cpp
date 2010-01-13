/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualConfiguration.cpp
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

#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"

#if TARGET_OS_MAC
#include <CoreFoundation/CFString.h> // CFStringRef
#endif

#include <string> // strlen

using namespace VizKit;



// ========= INDIVIDUAL VISUALIZER IDENTIFICATION CONFIGURATION (start) ========= //

const char* const VisualConfiguration::visualizerPluginIdentifierName = "VizKit";
const char* const VisualConfiguration::visualizerShowName = "VizKit";

void VisualConfiguration::prepareVisualizerShowNameUnicode() {
	
#if TARGET_OS_WIN
	wchar_t* visualizerShowNameUnicode_wchar = L"VizKit"; // utf-16
	this->visualizerShowNameUnicode = (uint16*)visualizerShowNameUnicode_wchar;
#endif

#if TARGET_OS_MAC
/*
	// if you malloc the show name, do not forget to free the allocated heap memory in VisualConfiguration's destructor
	
	char* visualizerShowNameUnicodePrep = (char*)malloc(10 * sizeof(char));
	visualizerShowNameUnicodePrep[0] = 'V';
	visualizerShowNameUnicodePrep[1] = 'i';
	visualizerShowNameUnicodePrep[2] = 'z';
	visualizerShowNameUnicodePrep[3] = 0x00E3; // KATAKANA LETTER ZU (Unicode: 30BA, UTF8: E3 82 BA)
	visualizerShowNameUnicodePrep[4] = 0x0082;
	visualizerShowNameUnicodePrep[5] = 0x00BA;
	visualizerShowNameUnicodePrep[6] = 'K';
	visualizerShowNameUnicodePrep[7] = 'i';
	visualizerShowNameUnicodePrep[8] = 't';
	visualizerShowNameUnicodePrep[9] = '\0';
	
	this->visualizerShowNameUnicode = (uint8*)visualizerShowNameUnicodePrep;
*/
	this->visualizerShowNameUnicode = (uint8*)VisualConfiguration::visualizerShowName; // defaults to utf8 being the same as the ascii name

#endif
}


#if TARGET_OS_MAC
const char* const VisualConfiguration::kVisualPluginDomainIdentifier = "de.imagomat.vizkit";
#endif
#if TARGET_OS_WIN
const char* const VisualConfiguration::kVisualPluginDomainIdentifier = "Imagomat";
#endif

const uint8 VisualConfiguration::kMajorReleaseNumber = 2;

const uint8 VisualConfiguration::kMinorReleaseNumber = 3;

const uint8 VisualConfiguration::kReleaseRevisionNumber = 0;


const char* const VisualConfiguration::kCurrentVersionInformationURL = "http://www.imagomat.de/vizkit/download/current_version.txt";


// ========= INDIVIDUAL VISUALIZER IDENTIFICATION CONFIGURATION (end) ========= //


VisualConfiguration* VisualConfiguration::theVisualConfiguration = NULL;


VisualConfiguration::VisualConfiguration() {
	unicodeShowName = NULL;
	prepareVisualizerShowNameUnicode();
}


VisualConfiguration::~VisualConfiguration() {
#if TARGET_OS_MAC
	if (theVisualConfiguration->unicodeShowName != NULL) {
		free(theVisualConfiguration->unicodeShowName);
	}
	//if (theVisualConfiguration->visualizerShowNameUnicode != NULL) {
		//free(theVisualConfiguration->visualizerShowNameUnicode);
	//}
#endif
}


VisualConfiguration* VisualConfiguration::getInstance() {
    if (theVisualConfiguration == NULL) {
		theVisualConfiguration = new VisualConfiguration;
    }
	if (theVisualConfiguration == NULL) {
		writeLog("ERR: init theVisualConfiguration failed");
	}
	return theVisualConfiguration;
}


void VisualConfiguration::dispose() {
	if (theVisualConfiguration != NULL) {
		delete theVisualConfiguration;
		theVisualConfiguration = NULL;
	}
}



uint16* VisualConfiguration::getVisualizerShowNameUnicode(uint32& numberOfCharactersOfVisualizerShowNameUnicode) {
	theVisualConfiguration = VisualConfiguration::getInstance();
#if TARGET_OS_MAC
	Boolean isExternalRepresentation = false;
	CFStringRef unicodeNameStr = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8*)theVisualConfiguration->visualizerShowNameUnicode, strlen((char*)theVisualConfiguration->visualizerShowNameUnicode), kCFStringEncodingUTF8, isExternalRepresentation);
	numberOfCharactersOfVisualizerShowNameUnicode = CFStringGetLength(unicodeNameStr);
	theVisualConfiguration->unicodeShowName = (UniChar*)malloc(numberOfCharactersOfVisualizerShowNameUnicode * sizeof(UniChar));
	CFStringGetCharacters(unicodeNameStr, CFRangeMake(0, numberOfCharactersOfVisualizerShowNameUnicode), theVisualConfiguration->unicodeShowName);
	CFRelease(unicodeNameStr);
#endif
#if TARGET_OS_WIN
	theVisualConfiguration->unicodeShowName = theVisualConfiguration->visualizerShowNameUnicode;
	numberOfCharactersOfVisualizerShowNameUnicode = wcslen((wchar_t*)theVisualConfiguration->visualizerShowNameUnicode);
#endif
	return theVisualConfiguration->unicodeShowName;
}
		

uint8 VisualConfiguration::getBCD(uint8 decimalNumber) {
	
	if (decimalNumber > 99) return getBCD(99);
	
	uint8 bcd = 16 * (decimalNumber / 10) + (decimalNumber%10);
	return bcd;
}


uint8 VisualConfiguration::getMajorReleaseNumberAsBCD(void) {
	return VisualConfiguration::getBCD(VisualConfiguration::kMajorReleaseNumber);
}


uint8 VisualConfiguration::getMinorReleaseNumberAsBCD(void) {
	return VisualConfiguration::getBCD(VisualConfiguration::kMinorReleaseNumber);
}


uint8 VisualConfiguration::getReleaseRevisionNumber(void) {
	return VisualConfiguration::kReleaseRevisionNumber;
}
