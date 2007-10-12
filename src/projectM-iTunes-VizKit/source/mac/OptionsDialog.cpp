/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: OptionsDialog.cpp
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

#include "OptionsDialog.h"
#include "VisualErrorHandling.h"
#include "VisualGraphics.h"
#include "VisualConfiguration.h"
#include "VisualDataStore.h"
#include "VisualNotification.h"


using namespace VizKit;


OptionsDialog* OptionsDialog::theOptionsDialog = NULL;  // singleton


const OSType OptionsDialog::signatureOfOptionsDialog = 'vizK';

const SInt32 OptionsDialog::displayResolutionMenuId = 6;
const ControlID OptionsDialog::displayResolutionMenuControlID = {signatureOfOptionsDialog, displayResolutionMenuId};

const SInt32 OptionsDialog::tabControlId = 128;
const SInt32 OptionsDialog::aboutTabPaneId = 129;
const SInt32 OptionsDialog::displayResolutionTabPaneId = 130;
const ControlID OptionsDialog::tabControlID = {signatureOfOptionsDialog, tabControlId};

const EventTypeSpec OptionsDialog::controlHitEvent = {kEventClassControl, kEventControlHit};
const EventTypeSpec OptionsDialog::windowCloseEvent = {kEventClassWindow, kEventWindowClose};


static pascal OSStatus optionsDialogControlHandler(EventHandlerCallRef inRef, EventRef inEvent, void* userData) {
	OptionsDialog* theOptionsDialog;
	theOptionsDialog = OptionsDialog::getOptionsDialog();
	theOptionsDialog->handleEvent(inRef, inEvent, userData);
    return noErr;
}


static pascal OSStatus optionsDialogWindowHandler(EventHandlerCallRef inRef, EventRef inEvent, void* userData) {
	OptionsDialog* theOptionsDialog;
	theOptionsDialog = OptionsDialog::getOptionsDialog();
	theOptionsDialog->handleEvent(inRef, inEvent, userData);
	return noErr;
}


void OptionsDialog::dispose() {
	if (theOptionsDialog != NULL) {
		DisposeMenu(theOptionsDialog->displayResolutionMenu);
		if (theOptionsDialog->optionsDialogWindow != NULL) {
			DisposeEventHandlerUPP(optionsDialogControlHandler);
			DisposeEventHandlerUPP(optionsDialogWindowHandler);
			DisposeWindow(theOptionsDialog->optionsDialogWindow);
			theOptionsDialog->optionsDialogWindow = NULL;
		}
		delete theOptionsDialog;
		theOptionsDialog = NULL;
	}
}


OptionsDialog::OptionsDialog() {

	optionsDialogWindow = NULL;
	
	tabPaneVector.push_back(aboutTabPaneId);
	tabPaneVector.push_back(displayResolutionTabPaneId);
	
	displayResolutionPopUpControl = NULL;
	displayResolutionMenuSelectedIdx = -1;
	
	tabControl = NULL;

}


OptionsDialog::~OptionsDialog() {
	tabPaneVector.clear();
}


OptionsDialog* OptionsDialog::getOptionsDialog() {
    if (theOptionsDialog == NULL) {
		theOptionsDialog = new OptionsDialog;
    }
	return theOptionsDialog;
}


void OptionsDialog::show() {
	int aLastTabIndex;
	OSStatus err;

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->evaluateFullscreenDisplayResolution();
	
	aLastTabIndex = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kPreferencePane);
	if (aLastTabIndex == 0) {
		aLastTabIndex = 1;
	}

	if (theOptionsDialog->optionsDialogWindow == NULL) {
	
		// find the bundle to load the nib inside of it
		IBNibRef nibRef;
		CFStringRef pluginName;
		pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingWindowsLatin1);
		CFBundleRef thePlugin = CFBundleGetBundleWithIdentifier(pluginName);
		err = CreateNibReferenceWithCFBundle(thePlugin, CFSTR("VisualOptions"), &nibRef);
		if (err != noErr) {
			writeLog("CreateNibReferenceWithCFBundle failed in OptionsDialog->show");
			return;
		}
		CreateWindowFromNib(nibRef, CFSTR("OptionsDialog"), &(theOptionsDialog->optionsDialogWindow));
		if (err != noErr) {
			writeLog("CreateWindowFromNib failed in OptionsDialog->show");
			return;
		}
		DisposeNibReference(nibRef);
		CFRelease(pluginName);
		
		// tab control
		GetControlByID(theOptionsDialog->optionsDialogWindow, &theOptionsDialog->tabControlID, &theOptionsDialog->tabControl);

		// display resolution menu pop up control
		err = GetControlByID(theOptionsDialog->optionsDialogWindow, &theOptionsDialog->displayResolutionMenuControlID, &(theOptionsDialog->displayResolutionPopUpControl));
		if (err != noErr) {
			writeLog("GetControlByID failed in OptionsDialog->show");
			return;
		}

		UInt32 count;
		MenuAttributes displayResolutionMenuAttributes = (MenuAttributes)NULL;
		err = CreateNewMenu(131, displayResolutionMenuAttributes, &(theOptionsDialog->displayResolutionMenu));
		if (err != noErr) {
			writeLog("CreateNewMenu failed in OptionsDialog->show");
			return;
		}

		CFStringRef displayResolutionShowStr;
		count = 0;
		char str[32];
		UInt8 isSelected = 0;
		
		while(theVisualGraphics->getNextAvailableDisplayResolution(str, &isSelected)) {
			displayResolutionShowStr = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%s"), str);
			err = AppendMenuItemTextWithCFString(theOptionsDialog->displayResolutionMenu, displayResolutionShowStr, 0, 0, NULL);
			if (err != noErr) {
				writeLog("Error while appending menu item");
			}
			count++;
			if (isSelected == 1) {
				theOptionsDialog->displayResolutionMenuSelectedIdx = count;
			}
			CFRelease(displayResolutionShowStr);
		}
		SetControlPopupMenuHandle(theOptionsDialog->displayResolutionPopUpControl, theOptionsDialog->displayResolutionMenu);
		SetControl32BitMaximum(theOptionsDialog->displayResolutionPopUpControl, count);
		
		InstallWindowEventHandler(theOptionsDialog->optionsDialogWindow, NewEventHandlerUPP(optionsDialogWindowHandler), 1, &windowCloseEvent, NULL, NULL);
		InstallWindowEventHandler(theOptionsDialog->optionsDialogWindow, NewEventHandlerUPP(optionsDialogControlHandler), 1, &controlHitEvent, NULL, NULL);
		
	}
	
	if (theOptionsDialog->displayResolutionMenuSelectedIdx == -1) {
		theOptionsDialog->displayResolutionMenuSelectedIdx = 1; // first menuItem is default
	}
	SetControl32BitValue(theOptionsDialog->displayResolutionPopUpControl, theOptionsDialog->displayResolutionMenuSelectedIdx);
	
	SetControl32BitValue(theOptionsDialog->tabControl, aLastTabIndex);
	theOptionsDialog->showSelectedPaneOfTabControl();
	
	ShowWindow(theOptionsDialog->optionsDialogWindow);
	SelectWindow(theOptionsDialog->optionsDialogWindow);

}


void OptionsDialog::handleEvent(EventHandlerCallRef inRef, EventRef inEvent, void* userData) {
    
	UInt32 eventClass;
	UInt32 eventKind;
	
	eventClass = GetEventClass(inEvent);
	eventKind = GetEventKind(inEvent);
	
	switch (eventClass) {
	
		case kEventClassWindow:
			// window event
			if (eventKind == kEventWindowClose) {
				HideWindow(theOptionsDialog->optionsDialogWindow);
			}
			break;
	
		case kEventClassControl:
			// control event
			ControlID controlID;
			ControlRef control = NULL;
			GetEventParameter(inEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &control);
			GetControlID(control, &controlID);
			UInt32 stringLength = 0;
			CFStringRef selectedText = NULL;
			
			switch(controlID.id) {

				case displayResolutionMenuId:
					{
						SInt32 selectedMenuItemIdx;
						selectedMenuItemIdx = GetControl32BitValue(control);
						theOptionsDialog->displayResolutionMenuSelectedIdx = selectedMenuItemIdx;
						char str[32];
						CopyMenuItemTextAsCFString(theOptionsDialog->displayResolutionMenu, selectedMenuItemIdx, &selectedText);
						stringLength = CFStringGetLength(selectedText);
						stringLength += 1; // incl. terminating null-byte
						CFStringGetCString(selectedText, str, stringLength, kCFStringEncodingASCII);
						CFRelease(selectedText);
						
						VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
						UInt16 horizontalPixels;
						UInt16 verticalPixels;
						UInt16 bitsPerPixel;
						UInt16 refreshRate;
						theVisualGraphics->matchDisplayResolutionShowStrWithPrefs(str, horizontalPixels, verticalPixels, bitsPerPixel, refreshRate);

						VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenWidth, horizontalPixels);
						VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenHeight, verticalPixels);
						VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenBitsPerPixel, bitsPerPixel);
						VisualDataStore::setPreferenceValueInt(VisualConfiguration::kFullscreenRefreshRate, refreshRate);
						
						VisualDataStore::storePreferences();
						
						UInt16 minBitsPerPixel = 24;
						UInt16 maxBitsPerPixel = 32;
						VisualDataStore::setPreferredDisplayResolution(minBitsPerPixel, maxBitsPerPixel, bitsPerPixel, horizontalPixels, verticalPixels);
					}
					break;
					
				case tabControlId:
					theOptionsDialog->showSelectedPaneOfTabControl();
					break;
			}
			break;
	}
}


void OptionsDialog::showSelectedPaneOfTabControl() {
    ControlRef userPane;
    ControlRef selectedPane = NULL;
    UInt16 idx = 0;
	SInt16 selectedPaneIdx;
	ControlID controlID;
	
	controlID.signature = theOptionsDialog->signatureOfOptionsDialog;

    selectedPaneIdx = GetControlValue(theOptionsDialog->tabControl);

	TabPaneVectorIterator it;

	for (it = theOptionsDialog->tabPaneVector.begin(); it != theOptionsDialog->tabPaneVector.end(); it++) {
		
		controlID.id = *it;
        GetControlByID(GetControlOwner(theOptionsDialog->tabControl), &controlID, &userPane);
		
		idx++;
        if (idx == selectedPaneIdx) {
            selectedPane = userPane;
        } else {
            SetControlVisibility(userPane, false, false);
            DisableControl(userPane);
        }
		
    }
    
    if (selectedPane != NULL) {
        EnableControl(selectedPane);
        SetControlVisibility(selectedPane, true, true);
    }
    
    Draw1Control(theOptionsDialog->tabControl);
	
	VisualDataStore::setPreferenceValueInt(VisualConfiguration::kPreferencePane, selectedPaneIdx);
	VisualDataStore::storePreferences();
	
}
