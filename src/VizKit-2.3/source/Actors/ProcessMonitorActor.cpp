/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: ProcessMonitorActor.cpp
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

#include "ProcessMonitorActor.h"
#include "ProcessMonitor.h"
#include "VisualPlayerState.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


ProcessMonitorActor::ProcessMonitorActor() {
	strcpy(actorName, "PROCESSMONITOR");
    state = kVisActOn;
	showAudioInfoBool = true;
	elapsedAudioTime = 0;
	remainingAudioTime = 0;
	processMonitor = new ProcessMonitor;
}


ProcessMonitorActor::~ProcessMonitorActor() {
	delete processMonitor;
}


void ProcessMonitorActor::init() {
	VisualNotification::registerNotification(this, kCanvasReshapeEvt);
}


void ProcessMonitorActor::show(const VisualPlayerState& visualPlayerState) {
	this->elapsedAudioTime = visualPlayerState.getElapsedAudioTime();
	this->remainingAudioTime = visualPlayerState.getRemainingAudioTime();
    processMonitor->prepareProcessMonitorShow();
	
    processMonitor->showInfoStrings();
	if (this->showAudioInfoBool == true) {
		processMonitor->showAudioInfo(elapsedAudioTime, remainingAudioTime);
	}
	processMonitor->finishProcessMonitorShow();
}


void ProcessMonitorActor::handleNotification(VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
		case kCanvasReshapeEvt:
			processMonitor->updateProgressMeterVertices();
			break;
		default:
			char notificationString[64];
			VisualNotification::convertNotificationKeyToString(notificationKey, notificationString);
			char errLog[256];
			sprintf(errLog, "Unhandled VisualNotificationKey %s in file: %s (line: %d) [%s])", notificationString, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			break;
	}

}


void ProcessMonitorActor::setState(VisualActorState aVisualActorState) {
    this->state = aVisualActorState;
}


void ProcessMonitorActor::registerProcessMonitorInfoMap(const std::map<std::string, std::string>* const processMonitorInfoMap) {
	processMonitor->registerProcessMonitorInfoMap(processMonitorInfoMap);
}


bool ProcessMonitorActor::isAudioInfoShown() {
	return this->showAudioInfoBool;
}


void ProcessMonitorActor::setShowAudioInfo(bool requestedAudioInfoState) {
	this->showAudioInfoBool = requestedAudioInfoState;
}
