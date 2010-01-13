/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualFontManager.cpp
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

#include "VisualFontManager.h"
#include "VisualGraphicsCore.h"
#include "VisualErrorHandling.h"
#include "VisualConfiguration.h"

#include <algorithm> // sort on vector

#if TARGET_OS_WIN
#include "win/resource.h"
#include "VisualFile.h"
#endif


using namespace VizKit;


VisualFontManager* VisualFontManager::theVisualFontManager = NULL;  // singleton


#if TARGET_OS_WIN
	uint8 fontIsInstalledBool;
#endif



#if TARGET_OS_WIN

int CALLBACK EnumFontFamExProc(
				ENUMLOGFONTEX *lpelfe,    // logical-font data
				NEWTEXTMETRICEX *lpntme,  // physical-font data
				DWORD FontType,           // type of font
				LPARAM lParam             // application-defined data
)
{
	/*
  CString *pEnumerateFont = (CString*)lParam;
  *pEnumerateFont += CString("|") + ((char *)lpelfe->elfLogFont.lfFaceName);
  
  */
	//VisualGraphics* visualGraphics;
	//writeLog((char *)lpelfe->elfLogFont.lfFaceName);


	fontIsInstalledBool = 1;
	return 1;
}


uint8 VisualFontManager::fontIsInstalled(HDC currHdc, const char* const fontName) {

	LOGFONT lf;

	lf.lfCharSet = ANSI_CHARSET;
	lf.lfFaceName[0]='\0';
	strcpy(lf.lfFaceName, fontName);
	fontIsInstalledBool = 0;
	EnumFontFamiliesEx(currHdc, &lf, (FONTENUMPROC) EnumFontFamExProc, 0, 0);
	if (fontIsInstalledBool == 1) {
		return 1;
	} else {
		return 0;
	}

}

#endif


VisualFontManager::VisualFontManager() {
#if TARGET_OS_MAC
	fontBaseChangedNotificationRef = NULL;
	bundleRef = NULL;
	doFontIsAvailable = NULL;
	doActivateFont = NULL;
	doGetListOfFontNames = NULL;
	doReleaseCocoaAutoreleasePool = NULL;
#endif
}


VisualFontManager::~VisualFontManager() {
#if TARGET_OS_WIN
	std::vector<fontItem*>::iterator it2;
	for (it2 = this->FontVector.begin(); it2 != this->FontVector.end(); it2++) {
		delete *it2;
		*it2 = NULL;
	}
	FontVector.clear();
#endif
	cleanupFontBaseChangeObserver();
	fontBaseChangedCallbackVector.clear();
}


VisualFontManager* VisualFontManager::getInstance() {
    if (theVisualFontManager == NULL) {
		theVisualFontManager = new VisualFontManager;
    }
	return theVisualFontManager;
}


void VisualFontManager::dispose() {
	if (theVisualFontManager != NULL) {
		delete theVisualFontManager;
		theVisualFontManager = NULL;
	}
}

#if TARGET_OS_MAC
void VisualFontManager::initCocoaFunctionPointers() {
	CFStringRef pluginName;
	pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingASCII);
	bundleRef = CFBundleGetBundleWithIdentifier(pluginName);
    if (!bundleRef) {
			char errLog[256];
			sprintf(errLog, "Err: No bundleRef for %s in file: %s (line: %d) [%s])", VisualConfiguration::kVisualPluginDomainIdentifier, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
        return;
    } else {
		doFontIsAvailable = (FontIsAvailableProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("cocoaFontIsAvailable"));
		if (!doFontIsAvailable) {
			char errLog[256];
			sprintf(errLog, "Err: No function pointer for doFontIsAvailable in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
		doActivateFont = (ActivateFontProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("activateCocoaFont"));
		if (!doActivateFont) {
			char errLog[256];
			sprintf(errLog, "Err: No function pointer for doActivateFont in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
		doGetListOfFontNames = (GetListOfFontNamesProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("getCocoaListOfFontNames"));
		if (!doGetListOfFontNames) {
			char errLog[256];
			sprintf(errLog, "Err: No function pointer for doGetListOfFontNames in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
		doReleaseCocoaAutoreleasePool = (ReleaseCocoaAutoreleasePoolProc)CFBundleGetFunctionPointerForName(bundleRef, CFSTR("releaseTemporaryAutoreleasePool"));
		if (!doReleaseCocoaAutoreleasePool) {
			writeLog("no function pointer for doReleaseCocoaAutoreleasePool");
		}
    }
	CFRelease(pluginName);
}
#endif

#if TARGET_OS_MAC
void VisualFontManager::fontBaseDidChangeCallback(ATSFontNotificationInfoRef info, void* refCon) {
	theVisualFontManager = VisualFontManager::getInstance();
	for (FontBaseChangedCallbackVectorIterator it = theVisualFontManager->fontBaseChangedCallbackVector.begin(); it != theVisualFontManager->fontBaseChangedCallbackVector.end(); it++) {
		(*it)(); // notify observers by invoking registered callback function
	}
}
#endif


void VisualFontManager::setupFontBaseChangeObserver() {
#if TARGET_OS_MAC
	void* iRefCon = NULL;
	theVisualFontManager = VisualFontManager::getInstance();
	OSStatus status = ATSFontNotificationSubscribe(VisualFontManager::fontBaseDidChangeCallback, kATSFontNotifyOptionDefault, iRefCon, theVisualFontManager->fontBaseChangedNotificationRef);
	if (status != noErr) {
		char errLog[256];
		sprintf(errLog, "ATSFontNotificationSubscribe failed (OSStatus: %ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}
#endif
}


void VisualFontManager::cleanupFontBaseChangeObserver() {
#if TARGET_OS_MAC
	if (theVisualFontManager == NULL) {
		return;
	}
	theVisualFontManager = VisualFontManager::getInstance();
	if (theVisualFontManager->fontBaseChangedNotificationRef == NULL) {
		return;
	}
	theVisualFontManager = VisualFontManager::getInstance();
	OSStatus status = ATSFontNotificationUnsubscribe(*(theVisualFontManager->fontBaseChangedNotificationRef));
	if (status != noErr) {
		char errLog[256];
		sprintf(errLog, "ATSFontNotificationUnsubscribe failed (OSStatus: %ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}
#endif
}


void VisualFontManager::addFontBaseChangedObserver(FontBaseChangedCallback callback) {
	theVisualFontManager = VisualFontManager::getInstance();
	theVisualFontManager->fontBaseChangedCallbackVector.push_back(callback);
}


void VisualFontManager::removeFontBaseChangedObserver(FontBaseChangedCallback callback) {
	if (theVisualFontManager == NULL) {
		return;
	}
	FontBaseChangedCallbackVectorIterator found = std::find(theVisualFontManager->fontBaseChangedCallbackVector.begin(), theVisualFontManager->fontBaseChangedCallbackVector.end(), callback);
	if (found != theVisualFontManager->fontBaseChangedCallbackVector.end()) {
		theVisualFontManager->fontBaseChangedCallbackVector.erase(found);
	}

}


bool VisualFontManager::fontIsAvailableAsVisualizerResource(const char* const fontName) {
	bool fontIsAvailable = false;
	if (strcmp(fontName, "Devroye") == 0) {
		fontIsAvailable = true; 
	}
	return fontIsAvailable;
}


VisualFontActivationResult VisualFontManager::activateFont(const char* const fontName) {
	VisualFontActivationResult result = fontActivationUndefined;

	theVisualFontManager = VisualFontManager::getInstance();
#if TARGET_OS_MAC
	OSStatus status = noErr;
	size_t numberOfBytes = strlen(fontName);
	Boolean isExternalRepresentation = false;
	CFStringRef stringRef = CFStringCreateWithBytes(kCFAllocatorDefault, (const uint8*)fontName, numberOfBytes, kCFStringEncodingUTF8, isExternalRepresentation);
	if ((!(theVisualFontManager->doActivateFont)) || (!(theVisualFontManager->doActivateFont))) {
		theVisualFontManager->initCocoaFunctionPointers();
	}
	Boolean fontIsAvailable = theVisualFontManager->doFontIsAvailable(stringRef);
	if (!fontIsAvailable) {
		status = theVisualFontManager->doActivateFont(stringRef);
	} else {
		result = fontActivationFontAlreadyAvailable;
	}
	CFRelease(stringRef);
	if (status == noErr) {
		result = fontActivationNoError;
	} else if (result == fontActivationUndefined) {
		result = fontActivationUnableToActivateFont;
	}
#endif

#if TARGET_OS_WIN
	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	uint8 fontIsInstalledBool = theVisualFontManager->fontIsInstalled(theVisualGraphicsCore->getWindowDC(), fontName);
	if (fontIsInstalledBool == 0) {
		void* fontData = NULL;
		uint32 sizeOfFontResource = 0;
		bool success = VisualFile::getDataOfResource(IDR_DEVROYEFONT, "FONTRESOURCE", &fontData, sizeOfFontResource);
		if (success) {
			HANDLE fontResHandle = NULL;
			DWORD numberOfFonts = 1;
			fontResHandle = AddFontMemResourceEx(fontData, sizeOfFontResource, 0, &numberOfFonts);
			if (!fontResHandle || numberOfFonts == 0) {
				char errLog[256];
				sprintf(errLog, "AddFontMemResourceEx failed in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
				result = fontActivationUnableToActivateFont;
			} else {
				// system made its own copy
				DeleteObject(fontData);
				result = fontActivationNoError;
			}
		} else {
			char errLog[256];
			sprintf(errLog, "Err: Unable to get resource data in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			result = fontActivationUnableToActivateFont;
		}
	} else {
		result = fontActivationFontAlreadyAvailable;
	}
#endif
	return result;
}

#if TARGET_OS_MAC
CFArrayRef VisualFontManager::getListOfFontNames() {
	CFArrayRef fontNames = NULL;
	
	theVisualFontManager = VisualFontManager::getInstance();
	if (!(theVisualFontManager->doGetListOfFontNames)) {
		theVisualFontManager->initCocoaFunctionPointers();
	}
	fontNames = theVisualFontManager->doGetListOfFontNames();

	return fontNames;
}
#endif

#if TARGET_OS_WIN

struct CompareFontNamesAscending {

	bool operator()(const fontItem* x, const fontItem* y) {
		if (strcmp(x->name, y->name) > 0) {
			return false;
		} else {
			return true;
		}
	}

};


int CALLBACK VisualFontManager::EnumFontFamExProc2(
				ENUMLOGFONTEX *lpelfe,    // logical-font data
				NEWTEXTMETRICEX *lpntme,  // physical-font data
				DWORD FontType,           // type of font
				LPARAM lParam             // application-defined data
) {

	fontItem* aFontItem;

	VisualFontManager* theVisualFontManager = VisualFontManager::getInstance();
	
	if (!(FontType & RASTER_FONTTYPE)) {
	//} else {
		aFontItem = new fontItem;
		strcpy(aFontItem->name, (char*)lpelfe->elfFullName);
		//strcpy(aFontItem->name, (char*)lpelfe->elfLogFont.lfFaceName);
		
		theVisualFontManager->FontVector.push_back(aFontItem);
	}

	return 1;
}


void VisualFontManager::resetFontVectorIterIndex() {
	this->fontVectorIterIdx = 0;
}



void VisualFontManager::gatherFonts(void) {
	std::vector<fontItem*>::iterator it;
	LOGFONT lf;

	for (it = this->FontVector.begin(); it != this->FontVector.end(); it++) {
		delete *it;
		*it = NULL;
	}
	this->FontVector.clear();

	ZeroMemory(&lf, sizeof(lf));
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfFaceName[0] = '\0';

	VisualGraphicsCore* theVisualGraphicsCore = VisualGraphicsCore::getInstance();
	EnumFontFamiliesEx(theVisualGraphicsCore->getWindowDC(), &lf, (FONTENUMPROC) VisualFontManager::EnumFontFamExProc2, 0, 0);
	
}


void VisualFontManager::sortFontsByName() {
	std::sort(this->FontVector.begin(), this->FontVector.end(), CompareFontNamesAscending());
}


void VisualFontManager::addFont(const char* const fontName) {
	fontItem* aFontItem = new fontItem;
	strcpy(aFontItem->name, fontName);
	this->FontVector.push_back(aFontItem);
}


bool VisualFontManager::getNextFontName(char* fontName) {
	if (this->fontVectorIterIdx < this->FontVector.size()) {
		strcpy (fontName, this->FontVector[this->fontVectorIterIdx]->name);
		this->fontVectorIterIdx++;
		return true;
	} else {
		return false;
	}
}

#endif


#if TARGET_OS_MAC
void VisualFontManager::populateFontMenu(MenuRef fontMenu, CFIndex& numberOfFontNames, CFStringRef selectedFontName, SInt32& menuSelectedIdx, SInt32& subMenuSelectedIdx, MenuRef* fontSubMenu) {

	OSStatus err = noErr;
	
	CFArrayRef fontNameList = NULL;
	fontNameList = VisualFontManager::getListOfFontNames();
	if (fontNameList == NULL) {
		numberOfFontNames = 0;
	} else {
		numberOfFontNames = CFArrayGetCount(fontNameList);
	}
	
	CFStringRef fontNameStr;
	CFStringRef copiedFontNameData;
	CFArrayRef fontFamilyMembersArray;
	CFIndex numberOfFontFamilyMembers;
	CFIndex k;
	CFStringRef fontAttributeStr;
	CFMutableStringRef fontAttribute2Str;
	
	CFStringRef fontNameStringRef;
	MenuRef fontFamilyMembersMenu;
	
	menuSelectedIdx = 1;
	subMenuSelectedIdx = -1;
	*fontSubMenu = NULL;
					
	for (CFIndex i = 0; i < numberOfFontNames; i++) {

		fontNameStringRef = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontNameList, i), CFSTR("fontFamilyName"));
		err = AppendMenuItemTextWithCFString(fontMenu, fontNameStringRef, 0, 0, NULL);
		if (err != noErr) {
			writeLog("Error while appending font name as menu item");
		}
		
		numberOfFontFamilyMembers = 0;
		fontFamilyMembersArray = (CFArrayRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontNameList, i), CFSTR("fontFamilyMembersArray"));
		if (fontFamilyMembersArray) {
			numberOfFontFamilyMembers = CFArrayGetCount(fontFamilyMembersArray);
		}
		
		if (numberOfFontFamilyMembers > 1) {
			MenuAttributes fontMenuAttributes = NULL;
			err = CreateNewMenu(0, fontMenuAttributes, &fontFamilyMembersMenu);
			if (err != noErr) {
				writeLog("CreateNewMenu fontFamilyMembersMenu failed in OptionsDialog->show");
				return;
			}
			for (k = 0; k < numberOfFontFamilyMembers; k++) {
				fontNameStr = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontFamilyMembersArray, k), CFSTR("fontFamilyMemberName"));
				fontAttributeStr = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontFamilyMembersArray, k), CFSTR("fontFamilyMemberAttribute"));
				
				if (CFStringHasPrefix(fontAttributeStr, CFSTR("-"))) {
					// If the first character is a Minus-Sign, the menuItem will be a separator
					//fontAttribute2Str = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, fontAttributeStr);
					fontAttribute2Str = CFStringCreateMutable(kCFAllocatorDefault, 0);
					CFStringAppend(fontAttribute2Str, CFSTR(" "));
					CFStringAppend(fontAttribute2Str, fontAttributeStr);
					err = AppendMenuItemTextWithCFString(fontFamilyMembersMenu, fontAttribute2Str, 0, 0, NULL);
					CFRelease(fontAttribute2Str);
				} else {
					err = AppendMenuItemTextWithCFString(fontFamilyMembersMenu, fontAttributeStr, 0, 0, NULL);
				}
				
				
				if (err != noErr) {
					writeLog("Error while appending fontAttributeStr as menu item");
				}
				if (CFStringCompare(selectedFontName, fontNameStr, 0) == kCFCompareEqualTo) {
					subMenuSelectedIdx = k + 1;
					menuSelectedIdx = i + 1;
					*fontSubMenu = fontFamilyMembersMenu;
				}
				copiedFontNameData = CFStringCreateCopy(kCFAllocatorDefault, fontNameStr);
				SetMenuItemProperty(fontFamilyMembersMenu, k + 1, 'vizK', 'hook', sizeof(copiedFontNameData), &copiedFontNameData);
			}
			SetMenuItemHierarchicalMenu(fontMenu, i + 1, fontFamilyMembersMenu);
		} else {
			fontNameStr = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontFamilyMembersArray, 0), CFSTR("fontFamilyMemberName"));
			fontAttributeStr = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)CFArrayGetValueAtIndex(fontFamilyMembersArray, 0), CFSTR("fontFamilyMemberAttribute"));
			copiedFontNameData = CFStringCreateCopy(kCFAllocatorDefault, fontNameStr);				
			SetMenuItemProperty(fontMenu, i + 1, 'vizK', 'hook', sizeof(copiedFontNameData), &copiedFontNameData);
			
			if (CFStringCompare(selectedFontName, fontNameStr, 0) == kCFCompareEqualTo) {
				menuSelectedIdx = i + 1;
			}
		}
	}
	
	theVisualFontManager = VisualFontManager::getInstance();
	if (!(theVisualFontManager->doReleaseCocoaAutoreleasePool)) {
		theVisualFontManager->initCocoaFunctionPointers();
	}
	theVisualFontManager->doReleaseCocoaAutoreleasePool();

}


void VisualFontManager::getSelectedFontName(MenuRef fontMenu, SInt32 selectedMenuItem, MenuRef selectedSubMenu, MenuItemIndex selectedSubMenuItem, char** selectedFontName) {
	GetMenuItemHierarchicalMenu(fontMenu, selectedMenuItem, &selectedSubMenu);
	uint32 actualSize = 0;
	uint32 expectedSize = 0;
	CFStringRef fontNameCFStr = NULL;
	if (selectedSubMenu) {
		uint32 menuChoice = (uint32)MenuChoice();
		//MenuID clickedMenuID = HiWord(menuChoice);
		selectedSubMenuItem = LoWord(menuChoice);
		GetMenuItemPropertySize(selectedSubMenu, selectedSubMenuItem, 'vizK', 'hook', reinterpret_cast<UInt32*>(&expectedSize));
		OSStatus status = GetMenuItemProperty(selectedSubMenu, selectedSubMenuItem, 'vizK', 'hook', expectedSize, reinterpret_cast<UInt32*>(&actualSize), &fontNameCFStr);
		if (status != noErr) {
			char errLog[256];
			sprintf(errLog, "GetMenuItemProperty failed (1) (OSStatus: %ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		} else {
			*selectedFontName = (char*)malloc(256 * sizeof(char));
			CFStringGetCString(fontNameCFStr, *selectedFontName, 256, CFStringGetSystemEncoding());
			CFRelease(fontNameCFStr);
		}
	} else {
		selectedSubMenuItem = 0;
		GetMenuItemPropertySize(fontMenu, selectedMenuItem, 'vizK', 'hook', reinterpret_cast<UInt32*>(&expectedSize));
		OSStatus status = GetMenuItemProperty(fontMenu, selectedMenuItem, 'vizK', 'hook', expectedSize, reinterpret_cast<UInt32*>(&actualSize), &fontNameCFStr);
		if (status != noErr) {
			char errLog[256];
			sprintf(errLog, "GetMenuItemProperty failed (2) (OSStatus: %ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		} else {
			*selectedFontName = (char*)malloc(256 * sizeof(char));
			CFStringGetCString(fontNameCFStr, *selectedFontName, 256, CFStringGetSystemEncoding());
			CFRelease(fontNameCFStr);
		}
	}
}


void VisualFontManager::removeFontMenuData(MenuRef fontMenu) {
	MenuRef fontSubMenu;
	uint32 expectedSize;
	uint16 numberOfFontMenuItems = CountMenuItems(fontMenu);
	for (uint16 i = 0; i < numberOfFontMenuItems; i++) {
		GetMenuItemHierarchicalMenu(fontMenu, i + 1, &fontSubMenu);
		if (fontSubMenu) {
			uint16 numberOfFontSubMenuItems = CountMenuItems(fontSubMenu);
			for (uint16 k = 0; k < numberOfFontSubMenuItems; k++) {
				GetMenuItemPropertySize(fontSubMenu, i + 1, 'vizK', 'hook', reinterpret_cast<UInt32*>(&expectedSize));
				if (expectedSize > 0) {
					RemoveMenuItemProperty(fontSubMenu, i + 1, 'vizK', 'hook');
				}
			}
			DisposeMenu(fontSubMenu);
		}
		GetMenuItemPropertySize(fontMenu, i + 1, 'vizK', 'hook', reinterpret_cast<UInt32*>(&expectedSize));
		if (expectedSize > 0) {
			RemoveMenuItemProperty(fontMenu, i + 1, 'vizK', 'hook');
		}
	}
}

#endif
