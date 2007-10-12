/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualNotificationQueue.cpp
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

#include "VisualNotificationQueue.h"
#include "VisualErrorHandling.h"



using namespace VizKit;


VisualNotificationQueue* VisualNotificationQueue::theVisualNotificationQueue = NULL;


VisualNotificationQueue::VisualNotificationQueue() {
	// null
}


VisualNotificationQueue::~VisualNotificationQueue() {
	theVisualNotificationQueue->notificationQueue.clear();
}


VisualNotificationQueue* VisualNotificationQueue::getInstance() {
	if (theVisualNotificationQueue == NULL) {
		theVisualNotificationQueue = new VisualNotificationQueue;
	}
	return theVisualNotificationQueue;
}


void VisualNotificationQueue::dispose() {
	if (theVisualNotificationQueue != NULL) {
		delete theVisualNotificationQueue;
		theVisualNotificationQueue = NULL;
	}
}


void VisualNotificationQueue::push(VisualNotification& aNotification) {
	theVisualNotificationQueue = VisualNotificationQueue::getInstance();
	theVisualNotificationQueue->notificationQueue.push_back(aNotification);
}


VisualNotification VisualNotificationQueue::pop() {
	VisualNotification aNotification;
	theVisualNotificationQueue = VisualNotificationQueue::getInstance();
	aNotification = theVisualNotificationQueue->notificationQueue.front();
	theVisualNotificationQueue->notificationQueue.pop_front();
	return aNotification;
}


size_t VisualNotificationQueue::size() {
	theVisualNotificationQueue = VisualNotificationQueue::getInstance();
	return theVisualNotificationQueue->notificationQueue.size();
}
