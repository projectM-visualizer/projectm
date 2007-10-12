/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualActor.cpp
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

#include "VisualActor.h"
#include "VisualPlayerState.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"



using namespace VizKit;


VisualActor::VisualActor() {
    //writeLog("constructor of VisualActor called");
	strcpy(actorName, "UNKNOWN");
	state = kVisActNoShow;
}


VisualActor::~VisualActor() {
    //writeLog("destructor of VisualActor called");
}


VisualActor::VisualActor(const VisualActor& other) {
	copy(other);
}


VisualActor& VisualActor::operator=(const VisualActor& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualActor::copy(const VisualActor& other) {
	strcpy(this->actorName, other.actorName);
	this->state = other.state;
}


const char* const VisualActor::getName(void) {
	// interface method that doesn't need to be implemented by subclass
	return this->actorName;
}


void VisualActor::prepareShow(const VisualPlayerState& visualPlayerState) {
	// hook method that can be implemented by subclass
}


void VisualActor::show() {
	// interface method that should be implemented by subclass
	VisualActorGraphics::doFallbackActorShow(this->actorName);
}


void VisualActor::finishShow() {
    // hook method that can be implemented by subclass
}


VisualActorState VisualActor::getState() {
    // interface method that doesn't need to be implemented by subclass
    return this->state;
}


void VisualActor::handleNotification(const VisualNotification& aNotification) {
    // hook method that can be implemented by subclass
	VisualNotificationKey aKey = aNotification.getKey();
	char notificationStr[64];
	char logStr[128];
	VisualNotification::convertNotificationKeyToString(aKey, notificationStr);
	sprintf(logStr, "%s: Notification: %s", this->actorName, notificationStr);
	writeLog(logStr);
}


UInt8 VisualActor::getError(char* errorString) {
    UInt8 errorNum = 0;
    char anErrorString[256];

    errorNum = VisualActorGraphics::getError(anErrorString);
    if (errorNum > 0) {
        strcpy(errorString, anErrorString);
    }
    return errorNum;
}
