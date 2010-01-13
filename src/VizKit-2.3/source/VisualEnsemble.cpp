/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualEnsemble.cpp
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

#include "VisualEnsemble.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"



using namespace VizKit;


VisualEnsemble::VisualEnsemble() {
    visualActorIterIndex = 0;
}


VisualEnsemble::~VisualEnsemble() {

	for (VisualEnsembleActorsIterator it = this->visualEnsembleActors.begin(); it != this->visualEnsembleActors.end(); it++) {
		delete *it;
	}

	visualEnsembleActors.clear();
	observerMap.clear();
}


VisualEnsemble::VisualEnsemble(const VisualEnsemble& other) {
	copy(other);
}


VisualEnsemble& VisualEnsemble::operator=(const VisualEnsemble& other) {

	if (this == &other) return *this;
	
	for (VisualEnsembleActorsIterator it = this->visualEnsembleActors.begin(); it != this->visualEnsembleActors.end(); it++) {
		delete *it;
	}
	visualEnsembleActors.clear();
	
	this->copy(other);

	return *this;
}


void VisualEnsemble::copy(const VisualEnsemble& other) {
	this->visualActorIterIndex = other.visualActorIterIndex;
	this->visualEnsembleActors = other.visualEnsembleActors;
	this->observerMap = other.observerMap;
}


void VisualEnsemble::showEnsemble(const VisualPlayerState& visualPlayerState) {
    uint16 graphicErrorNum;
    char graphicErrorString[256];
	VisualEnsembleActorsIterator it;

	for (it = this->visualEnsembleActors.begin(); it != this->visualEnsembleActors.end(); it++) {
	
		graphicErrorNum = (*it)->getError(graphicErrorString);
		if (graphicErrorNum != 0) {
			const char* visualActorName;
			char logStr[512];
			visualActorName = (*it)->getName();
			sprintf(logStr, "GL error: showEnsemble: detected before show of Actor: %s: %s", visualActorName, graphicErrorString);
			writeLog(logStr);
		}
		
        if ((*it)->getState() == kVisActNoShow) {
			continue;
		}
		
		VisualActorGraphics::resetModelViewMatrix();

		(*it)->show(visualPlayerState); // show

		graphicErrorNum = (*it)->getError(graphicErrorString);
		if (graphicErrorNum != 0) {
			const char* visualActorName;
			char logStr[512];
			visualActorName = (*it)->getName();
			sprintf(logStr, "GL error: showEnsemble: detected after show of Actor: %s: %s", visualActorName, graphicErrorString);
			writeLog(logStr);
		}
		
	}

}


void VisualEnsemble::addEnsembleMember(VisualActor* aVisualActor) {
	this->visualEnsembleActors.push_back(aVisualActor);
}


void VisualEnsemble::registerObserverForNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey) {
	this->observerMap.insert(std::make_pair(aNotificationKey, aVisualActor));
}


void VisualEnsemble::removeObserverOfNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey) {
	const char* firstName;
	const char* secondName;
	
	firstName = aVisualActor->getName();

	ObserverMapIterator it = this->observerMap.lower_bound(aNotificationKey);
	while (it != this->observerMap.upper_bound(aNotificationKey)) {
		secondName = it->second->getName();
		if (strcmp(firstName, secondName) == 0) {
			delete it->second;
			this->observerMap.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}
}


void VisualEnsemble::dispatchNotification(VisualNotification& aNotification) {
	ObserverMapIterator it;
	VisualNotificationKey aNotificationKey = aNotification.getKey();
	
	it = this->observerMap.find(aNotificationKey);
	if (it == this->observerMap.end()) return;
	
	for (it = this->observerMap.lower_bound(aNotificationKey); it != this->observerMap.upper_bound(aNotificationKey); it++) {
		it->second->handleNotification(aNotification);
	}
}


VisualActor* VisualEnsemble::getVisualActorByName(const char* const aVisualActorName) {
	for (VisualEnsembleActorsIterator it = visualEnsembleActors.begin(); it != visualEnsembleActors.end(); it++) {
		if (strcmp((*it)->getName(), aVisualActorName) == 0) {
			return (*it);
		}
	}
	return NULL;
}

