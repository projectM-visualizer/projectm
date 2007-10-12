/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: BeatlightActor.cpp
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

#include "BeatlightActor.h"
#include "Beatlight.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


BeatlightActor::BeatlightActor() {
	strcpy(actorName, "BEATLIGHT");
	state = kVisActOn;
    beatlight = new Beatlight;
}


BeatlightActor::~BeatlightActor() {
    delete beatlight;
	beatlight = NULL;
}


void BeatlightActor::prepareShow(const VisualPlayerState& visualPlayerState) {
    this->beatlight->prepareBeatlightShow();
}


void BeatlightActor::show() {
    this->beatlight->showBeatlight();
}


void BeatlightActor::finishShow() {
    this->beatlight->finishBeatlightShow();
}


void BeatlightActor::handleNotification(const VisualNotification& aNotification) {

	//VisualActor::handleNotification(aNotification); // debug

	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
		case kCanvasReshapeEvt:
			this->beatlight->calcPositionOnScreen();
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unhandled Notification in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			break;
	}

}
