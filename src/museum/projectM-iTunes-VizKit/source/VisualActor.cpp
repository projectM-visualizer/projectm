/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualActor.cpp
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

#include "VisualActor.h"
#include "VisualPlayerState.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"



using namespace VizKit;


VisualActor::VisualActor() {
	strcpy(actorName, "UNKNOWN");
	state = kVisActOn;
}


VisualActor::~VisualActor() {
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


VisualActor* VisualActor::clone(void) const {
	return new VisualActor(*this);
}


void VisualActor::init(void) {
	// hook method that can be implemented by subclass
}


const char* const VisualActor::getName(void) {
	// interface method that does not need to be implemented by subclass
	return this->actorName;
}


void VisualActor::show(const VisualPlayerState& visualPlayerState) {
	// main interface method that should be implemented by subclass
	VisualActorGraphics::doFallbackActorShow(this->actorName);
}


VisualActorState VisualActor::getState() {
    // interface method that does not need to be implemented by subclass
    return this->state;
}


void VisualActor::setState(VisualActorState actorState) {
    // interface method that does not need to be implemented by subclass
    this->state = actorState;
}


void VisualActor::handleNotification(VisualNotification& aNotification) {
    // hook method that can be implemented by subclass
	// notification is logged in case the actor registered for a notification but did not implement it
	VisualNotificationKey aKey = aNotification.getKey();
	char notificationStr[64];
	char logStr[128];
	VisualNotification::convertNotificationKeyToString(aKey, notificationStr);
	sprintf(logStr, "VisualActor::handleNotification: %s -- %s", this->actorName, notificationStr);
	writeLog(logStr);
}


uint8 VisualActor::getError(char* errorString) {
    uint8 errorNum = 0;
    char anErrorString[256];

    errorNum = VisualActorGraphics::getError(anErrorString);
    if (errorNum > 0) {
        strcpy(errorString, anErrorString);
    }
    return errorNum;
}
