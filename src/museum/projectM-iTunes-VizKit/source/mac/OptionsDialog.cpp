/*
 * Project: VizKit
 * Version: 2.3	
 
 * Date: 20090823
 * File: OptionsDialog.cpp
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

#include "OptionsDialog.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualConfiguration.h"
#include "VisualDataStore.h"
#include "VisualPreferences.h"
#include "VisualNotification.h"
#include "VisualMainAction.h"
#include "VisualFontManager.h"
#include "VisualPlayerState.h"


using namespace VizKit;



OptionsDialog* OptionsDialog::theOptionsDialog = NULL;  // singleton


const OSType OptionsDialog::signatureOfOptionsDialog = 'vizK';

const SInt32 OptionsDialog::kFontPopUpControlId = 10;
const ControlID OptionsDialog::kFontPopUpControlID = {signatureOfOptionsDialog, kFontPopUpControlId};

const SInt32 OptionsDialog::kCheckForUpdateCheckboxId = 11;
const ControlID OptionsDialog::kCheckForUpdateCheckboxControlID = {signatureOfOptionsDialog, kCheckForUpdateCheckboxId};

const SInt32 OptionsDialog::tabControlId = 1;
const SInt32 OptionsDialog::aboutTabPaneId = 224;
const SInt32 OptionsDialog::optionsTabPaneId = 225;
const SInt32 OptionsDialog::updateTabPaneId = 226;
const ControlID OptionsDialog::tabControlID = {signatureOfOptionsDialog, tabControlId};

const EventTypeSpec OptionsDialog::controlHitEvent = {kEventClassControl, kEventControlHit};
const EventTypeSpec OptionsDialog::windowCloseEvent = {kEventClassWindow, kEventWindowClose};
const EventTypeSpec OptionsDialog::windowDragStarted = {kEventClassWindow, kEventWindowDragStarted};
const EventTypeSpec OptionsDialog::windowDragCompleted = {kEventClassWindow, kEventWindowDragCompleted};


OSStatus OptionsDialog::optionsDialogControlHandler(EventHandlerCallRef inRef, EventRef inEvent, void* userData) {
	theOptionsDialog = OptionsDialog::getInstance();
	theOptionsDialog->handleEvent(inRef, inEvent, userData);
    return noErr;
}


OSStatus OptionsDialog::optionsDialogWindowHandler(EventHandlerCallRef inRef, EventRef inEvent, void* userData) {
	theOptionsDialog = OptionsDialog::getInstance();
	theOptionsDialog->handleEvent(inRef, inEvent, userData);
	return noErr;
}


void OptionsDialog::dispose() {

	OSStatus err = noErr;

	if (theOptionsDialog != NULL) {

		if (theOptionsDialog->fontMenu != NULL) {
			VisualFontManager::removeFontMenuData(theOptionsDialog->fontMenu);
			DisposeMenu(theOptionsDialog->fontMenu);
			theOptionsDialog->fontMenu = NULL;
		}

		if (theOptionsDialog->optionsDialogWindow != NULL) {
			err = RemoveEventHandler(theOptionsDialog->windowEventsHandlerRef);
			err = RemoveEventHandler(theOptionsDialog->windowControlEventsHandlerRef);
			DisposeEventHandlerUPP(OptionsDialog::optionsDialogControlHandler);
			DisposeEventHandlerUPP(OptionsDialog::optionsDialogWindowHandler);
			DisposeWindow(theOptionsDialog->optionsDialogWindow);
			theOptionsDialog->optionsDialogWindow = NULL;
		}
		delete theOptionsDialog;
		theOptionsDialog = NULL;
	}
	
}


OptionsDialog::OptionsDialog() {

	optionsDialogWindow = NULL;
	
	isDisplayedNow = false;
	
	tabPaneVector.push_back(aboutTabPaneId);
	tabPaneVector.push_back(optionsTabPaneId);
	tabPaneVector.push_back(updateTabPaneId);
	
	fontMenu = NULL;
	
	selectedFontSubMenu = NULL;
	
	trackInfoFontSubMenuSelectedIdx = -1;
	trackInfoFontMenuSelectedIdx = -1;
	
	doGetFontList = true;
	
	windowEventsHandlerRef = NULL;
	windowControlEventsHandlerRef = NULL;
	
	windowAlphaValue = 0.9f;
	windowAlphaMoveValue = 0.75f;
	
	VisualFontManager::addFontBaseChangedObserver(OptionsDialog::fontBaseChanged);

}


OptionsDialog::~OptionsDialog() {
	tabPaneVector.clear();
	VisualFontManager::removeFontBaseChangedObserver(OptionsDialog::fontBaseChanged);
}


OptionsDialog* OptionsDialog::getInstance() {
    if (theOptionsDialog == NULL) {
		theOptionsDialog = new OptionsDialog;
    }
	return theOptionsDialog;
}


void OptionsDialog::show() {

	OSStatus err = noErr;
	bool success = false;
	
	int aLastTabIndex = VisualPreferences::getValue(VisualPreferences::kPreferencePane);
	if (aLastTabIndex == 0) {
		aLastTabIndex = 1;
	}

	if (theOptionsDialog->optionsDialogWindow == NULL) {
	
		success = this->loadWindowFromResource();
		if (!success) {
			return;
		}

		success = this->installWindowEventHandler();
		if (!success) {
			return;
		}
		
	}
	
	if (this->doGetFontList) {
	
		success = this->populateFontMenu();
		if (!success) {
			return;
		}

	}
	
	theOptionsDialog->checkTrackInfoFontMenuSelection();

	// Check For Update checkbox control
	ControlRef checkForUpdateControl = NULL;
	err = GetControlByID(theOptionsDialog->optionsDialogWindow, &kCheckForUpdateCheckboxControlID, &checkForUpdateControl);
	if ((err != noErr) || (!checkForUpdateControl)) {
		char errLog[256];
		sprintf(errLog, "GetControlByID returned no checkForUpdateControl (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	} else {
		bool doCheckForUpdate = VisualPreferences::getValue(VisualPreferences::kCheckForUpdate);
		if (doCheckForUpdate == true) {
			SetControl32BitValue(checkForUpdateControl, (SInt32)1);
		} else {
			SetControl32BitValue(checkForUpdateControl, (SInt32)0);
		}
	}
	
	// Tab control
	ControlRef tabControl = NULL;
	err = GetControlByID(theOptionsDialog->optionsDialogWindow, &theOptionsDialog->tabControlID, &tabControl);
	if ((err != noErr) || (!tabControl)) {
		char errLog[256];
		sprintf(errLog, "GetControlByID returned no tabControl (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	} else {
		SetControl32BitValue(tabControl, aLastTabIndex);
	}
	this->showSelectedPaneOfTabControl();
	
	ShowWindow(theOptionsDialog->optionsDialogWindow);
	SelectWindow(theOptionsDialog->optionsDialogWindow);
	
	this->isDisplayedNow = true;

}


void OptionsDialog::handleEvent(EventHandlerCallRef inRef, EventRef inEvent, void* userData) {
    
	UInt32 eventClass;
	UInt32 eventKind;
	
	eventClass = GetEventClass(inEvent);
	eventKind = GetEventKind(inEvent);
	
	OSStatus status;
	
	switch (eventClass) {
	
		case kEventClassWindow:
			// window event
			if (eventKind == kEventWindowClose) {
				HideWindow(theOptionsDialog->optionsDialogWindow);
				this->isDisplayedNow = false;
			} else if (eventKind == kEventWindowDragStarted) {
				status = SetWindowAlpha(theOptionsDialog->optionsDialogWindow, theOptionsDialog->windowAlphaMoveValue);
				if (status != noErr) {
					char errLog[256];
					sprintf(errLog, "SetWindowAlpha failed (status: %ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
					writeLog(errLog);
				}
			} else if (eventKind == kEventWindowDragCompleted) {
				status = SetWindowAlpha(theOptionsDialog->optionsDialogWindow, theOptionsDialog->windowAlphaValue);
				if (status != noErr) {
					char errLog[256];
					sprintf(errLog, "SetWindowAlpha failed (status: %ld) in file: %s (line: %d) [%s])", status, __FILE__, __LINE__, __FUNCTION__);
					writeLog(errLog);
				}
			}
			break;
	
		case kEventClassControl:
			{
				// control event
				ControlID controlID;
				ControlRef control = NULL;
				GetEventParameter(inEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof(ControlRef), NULL, &control);
				GetControlID(control, &controlID);
				
				switch(controlID.id) {
						
					case tabControlId:
						theOptionsDialog->showSelectedPaneOfTabControl();
						break;

					case kFontPopUpControlId:
					{
						SInt32 selectedMenuItem = GetControl32BitValue(control);
						
						theOptionsDialog->uncheckTrackInfoFontMenuSelection();
						
						MenuRef selectedSubMenu = NULL;
						MenuItemIndex selectedSubMenuItem = 0;
						char* selectedTrackInfoFontName = NULL;
						VisualFontManager::getSelectedFontName(theOptionsDialog->fontMenu, selectedMenuItem, selectedSubMenu, selectedSubMenuItem, &selectedTrackInfoFontName);
						
						theOptionsDialog->setTrackInfoFontMenuSelectedIdx(selectedMenuItem);
						if (selectedSubMenu) {
							theOptionsDialog->setTrackInfoFontSubMenuSelectedIdx(selectedSubMenuItem, selectedSubMenu);
						}

						VisualPreferences::setValue(VisualPreferences::kTrackInfoFont, selectedTrackInfoFontName);
						VisualPreferences::storeValues();
						free(selectedTrackInfoFontName);
						
						theOptionsDialog->checkTrackInfoFontMenuSelection();
						
						VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
						if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
							VisualMainAction::postTrackInfoOfCurrentAudioData();
						}
				
					}
						
						break;
						
					case kCheckForUpdateCheckboxId:
					{
						SInt32 doCheckForUpdate = GetControl32BitValue(control);
						
						if (doCheckForUpdate) {
							VisualPreferences::setValue(VisualPreferences::kCheckForUpdate, true);
						} else {
							VisualPreferences::setValue(VisualPreferences::kCheckForUpdate, false);
						}
						VisualPreferences::storeValues();
					}
						break;
				}
			}
				break;
	}
}


void OptionsDialog::fontBaseChanged() {
	theOptionsDialog = OptionsDialog::getInstance();
	if (theOptionsDialog->isDisplayedNow) {
		bool success = theOptionsDialog->populateFontMenu();
		if (!success) {
			return;
		}
		theOptionsDialog->checkTrackInfoFontMenuSelection();
	} else {
		theOptionsDialog->doGetFontList = true;
	}
}


bool OptionsDialog::loadWindowFromResource() {
	bool success = false;
	OSStatus err = noErr;
	IBNibRef nibRef;
	CFStringRef pluginName;
	pluginName = CFStringCreateWithCString(kCFAllocatorDefault, VisualConfiguration::kVisualPluginDomainIdentifier, kCFStringEncodingASCII);
	CFBundleRef thePlugin = CFBundleGetBundleWithIdentifier(pluginName);
	if (!thePlugin) {
		char errLog[256];
		sprintf(errLog, "CFBundleGetBundleWithIdentifier returned NULL in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return success;
	}
	err = CreateNibReferenceWithCFBundle(thePlugin, CFSTR("VisualOptions"), &nibRef);
	if (err != noErr) {
		char errLog[256];
		sprintf(errLog, "CreateNibReferenceWithCFBundle failed (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return success;
	}
	err = CreateWindowFromNib(nibRef, CFSTR("OptionsDialog"), &(this->optionsDialogWindow));
	if (err != noErr) {
		char errLog[256];
		sprintf(errLog, "CreateWindowFromNib failed (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return success;
	}
	DisposeNibReference(nibRef);
	CFRelease(pluginName);

	err = SetWindowAlpha(this->optionsDialogWindow, this->windowAlphaValue);
	if (err != noErr) {
		char errLog[256];
		sprintf(errLog, "SetWindowAlpha failed (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}

	success = true;
	return success;
}


bool OptionsDialog::installWindowEventHandler() {

	bool success = false;
	OSStatus err = noErr;
	
	EventHandlerUPP inEventHandler = NewEventHandlerUPP(OptionsDialog::optionsDialogWindowHandler);
	UInt32 inNumTypes = 3;
	const EventTypeSpec inEventTypeSpecList[] = {windowCloseEvent, windowDragStarted, windowDragCompleted};
	void* inUserData = NULL;
	
	err = InstallWindowEventHandler(this->optionsDialogWindow, inEventHandler, inNumTypes, inEventTypeSpecList, inUserData, &(this->windowEventsHandlerRef));
	if (err != noErr) {
		char errLog[256];
		sprintf(errLog, "InstallWindowEventHandler failed for window events (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return success;
	}

	inEventHandler = NewEventHandlerUPP(OptionsDialog::optionsDialogControlHandler);
	const EventTypeSpec* inEventTypeSpec = &controlHitEvent;
	inNumTypes = 1;
	
	err = InstallWindowEventHandler(this->optionsDialogWindow, inEventHandler, inNumTypes, inEventTypeSpec, inUserData, &(this->windowControlEventsHandlerRef));
	if (err != noErr) {
		char errLog[256];
		sprintf(errLog, "InstallWindowEventHandler failed for control events (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return success;
	}

	success = true;
	return success;
}


bool OptionsDialog::populateFontMenu() {
	bool success = false;
	
	if (this->fontMenu != NULL) {
		VisualFontManager::removeFontMenuData(this->fontMenu);
		DisposeMenu(this->fontMenu);
		this->fontMenu = NULL;
	}
	MenuAttributes fontMenuAttributes = NULL;
	MenuID fontMenuID = 0;
	OSStatus err = CreateNewMenu(fontMenuID, fontMenuAttributes, &(this->fontMenu));
	if (err != noErr) {
		char errLog[256];
		sprintf(errLog, "CreateNewMenu failed (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return false;
	}

	char currTrackInfoFont[512];
	VisualPreferences::getValue(VisualPreferences::kTrackInfoFont, currTrackInfoFont);
	if (VisualFontManager::fontIsAvailableAsVisualizerResource(currTrackInfoFont) == true) {
		VisualFontManager::removeFontBaseChangedObserver(OptionsDialog::fontBaseChanged);
		VisualFontActivationResult result = VisualFontManager::activateFont(currTrackInfoFont);
		VisualFontManager::addFontBaseChangedObserver(OptionsDialog::fontBaseChanged);
		if (result == fontActivationUnableToActivateFont) {
			VisualPreferences::getValue(VisualPreferences::kFallbackSystemFont, currTrackInfoFont);
		}
	}
	CFStringRef preferredTrackInfoFont = CFStringCreateWithCString(kCFAllocatorDefault, currTrackInfoFont, kCFStringEncodingUTF8);

	CFIndex numberOfFontNames = 0;
	VisualFontManager::populateFontMenu(this->fontMenu, numberOfFontNames, preferredTrackInfoFont, this->trackInfoFontMenuSelectedIdx, this->trackInfoFontSubMenuSelectedIdx, &(this->selectedFontSubMenu));
	
	ControlRef fontPopUpControl = NULL;
	err = GetControlByID(this->optionsDialogWindow, &kFontPopUpControlID, &fontPopUpControl);
	if ((err != noErr) || (!fontPopUpControl)) {
		char errLog[256];
		sprintf(errLog, "GetControlByID returned no fontPopUpControl (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return success;
	}
	SetControlPopupMenuHandle(fontPopUpControl, this->fontMenu);		
	SetControl32BitMaximum(fontPopUpControl, numberOfFontNames);
	
	this->doGetFontList = false;
	
	success = true;
	return success;
}


void OptionsDialog::showSelectedPaneOfTabControl() {
	OSStatus err = noErr;
	SInt16 selectedPaneIdx = -1;
	ControlID controlID;
	
	controlID.signature = this->signatureOfOptionsDialog;

	ControlRef tabControl = NULL;
	err = GetControlByID(this->optionsDialogWindow, &(this->tabControlID), &tabControl);
	if ((err != noErr) || (!tabControl)) {
		char errLog[256];
		sprintf(errLog, "GetControlByID returned no tabControl (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return;
	}
    selectedPaneIdx = GetControlValue(tabControl);
	SInt32 newValue = GetControl32BitValue(tabControl) - GetControl32BitMinimum(tabControl) + 1;
	
	HIViewRef newTabItemPane;
	OSStatus status = noErr;
	status = HIViewGetIndexedSubview(tabControl, newValue, &newTabItemPane);
	
	HIViewRef tabItemPane;
	for (tabItemPane = HIViewGetLastSubview(tabControl); tabItemPane != NULL; tabItemPane = HIViewGetPreviousView(tabItemPane))
		HIViewSetVisible(tabItemPane, (tabItemPane == newTabItemPane));

	VisualPreferences::setValue(VisualPreferences::kPreferencePane, selectedPaneIdx);
	VisualPreferences::storeValues();
	
}


void OptionsDialog::setTrackInfoFontMenuSelectedIdx(UInt32 selectedIdx) {
	this->trackInfoFontMenuSelectedIdx = selectedIdx;
}


void OptionsDialog::setTrackInfoFontSubMenuSelectedIdx(UInt32 selectedIdx, MenuRef subMenuRef) {
	this->trackInfoFontSubMenuSelectedIdx = selectedIdx;
	this->selectedFontSubMenu = subMenuRef;
}


void OptionsDialog::uncheckTrackInfoFontMenuSelection() {
	CheckMenuItem(this->fontMenu, this->trackInfoFontMenuSelectedIdx, false);
	if (this->trackInfoFontSubMenuSelectedIdx > -1) {
		CheckMenuItem(this->selectedFontSubMenu, this->trackInfoFontSubMenuSelectedIdx, false);
	}
	this->trackInfoFontMenuSelectedIdx = -1;
	this->trackInfoFontSubMenuSelectedIdx = -1;
	this->selectedFontSubMenu = NULL;
}


void OptionsDialog::checkTrackInfoFontMenuSelection() {
	if (this->trackInfoFontSubMenuSelectedIdx > -1) {
		SetItemMark(this->fontMenu, this->trackInfoFontMenuSelectedIdx, '-');
		CheckMenuItem(this->selectedFontSubMenu, this->trackInfoFontSubMenuSelectedIdx, true);
	} else {
		CheckMenuItem(this->fontMenu, this->trackInfoFontMenuSelectedIdx, true);
	}
	ControlRef fontPopUpControl = NULL;
	OSStatus err = GetControlByID(this->optionsDialogWindow, &kFontPopUpControlID, &fontPopUpControl);
	if ((err != noErr) || (!fontPopUpControl)) {
		char errLog[256];
		sprintf(errLog, "GetControlByID returned no fontPopUpControl  (err: %ld) in file: %s (line: %d) [%s])", err, __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
		return;
	}
	SetControl32BitValue(fontPopUpControl, this->trackInfoFontMenuSelectedIdx);
}
