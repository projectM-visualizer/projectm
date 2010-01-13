/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNotificationQueue.cpp
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

#include "VisualNotificationQueue.h"
#include "VisualThreading.h"
#include "VisualThreadingManager.h"
#include "VisualErrorHandling.h"


using namespace VizKit;


VisualNotificationQueue* VisualNotificationQueue::visualNotificationQueueInstance = NULL;


VisualNotificationQueue::VisualNotificationQueue() {
	// null
}


VisualNotificationQueue::~VisualNotificationQueue() {
	notificationQueue.clear();
	inputNotificationQueue.clear();
}


VisualNotificationQueue* VisualNotificationQueue::getInstance() {
	if (VisualNotificationQueue::visualNotificationQueueInstance == NULL) {
#if TARGET_OS_MAC
		MPCriticalRegionID visualNotificationQueueCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::notificationQueueSingletonMutex);
		OSStatus status = noErr;
		status = ::MPEnterCriticalRegion(visualNotificationQueueCriticalRegion, kDurationForever);
		if (status != noErr) return NULL;
#endif
#if TARGET_OS_WIN
		LPCRITICAL_SECTION visualNotificationQueueCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::notificationQueueSingletonMutex);
		::EnterCriticalSection(visualNotificationQueueCriticalSection);
#endif
		if (VisualNotificationQueue::visualNotificationQueueInstance == NULL) {
			VisualNotificationQueue::visualNotificationQueueInstance = new VisualNotificationQueue;
			if (VisualNotificationQueue::visualNotificationQueueInstance == NULL) {
				char errLog[256];
				sprintf(errLog, "ERR: init VisualNotificationQueue failed in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
			}
		}
#if TARGET_OS_MAC
		status = ::MPExitCriticalRegion(visualNotificationQueueCriticalRegion);
#endif
#if TARGET_OS_WIN
		::LeaveCriticalSection(visualNotificationQueueCriticalSection);
#endif
	}
	return VisualNotificationQueue::visualNotificationQueueInstance;
}


void VisualNotificationQueue::dispose() {
	if (VisualNotificationQueue::visualNotificationQueueInstance != NULL) {
		delete VisualNotificationQueue::visualNotificationQueueInstance;
		VisualNotificationQueue::visualNotificationQueueInstance = NULL;
	}
}


void VisualNotificationQueue::push(VisualNotification& aNotification) {
	
	VisualNotificationQueue* theVisualNotificationQueue = VisualNotificationQueue::getInstance();
	
#if TARGET_OS_MAC
	MPCriticalRegionID inputNotificationQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::inputNotificationQueueAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(inputNotificationQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION inputNotificationQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::inputNotificationQueueAccessMutex);
	::EnterCriticalSection(inputNotificationQueueAccessCriticalSection);
#endif
	
	theVisualNotificationQueue->inputNotificationQueue.push_back(aNotification);
	
#if TARGET_OS_MAC
	status = ::MPExitCriticalRegion(inputNotificationQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(inputNotificationQueueAccessCriticalSection);
#endif

}


VisualNotification VisualNotificationQueue::pop() {
	VisualNotification aNotification;
	VisualNotificationQueue* theVisualNotificationQueue = VisualNotificationQueue::getInstance();

#if TARGET_OS_MAC
	MPCriticalRegionID visualNotificationQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::notificationQueueAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(visualNotificationQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return aNotification;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION visualNotificationQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::notificationQueueAccessMutex);
	::EnterCriticalSection(visualNotificationQueueAccessCriticalSection);
#endif
	
	aNotification = theVisualNotificationQueue->notificationQueue.front();
	theVisualNotificationQueue->notificationQueue.pop_front();
	
#if TARGET_OS_MAC
	status = ::MPExitCriticalRegion(visualNotificationQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(visualNotificationQueueAccessCriticalSection);
#endif
	
	return aNotification;
}


size_t VisualNotificationQueue::size() {
	
	VisualNotificationQueue* theVisualNotificationQueue = VisualNotificationQueue::getInstance();

#if TARGET_OS_MAC
	MPCriticalRegionID visualNotificationQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::notificationQueueAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(visualNotificationQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return 0;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION visualNotificationQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::notificationQueueAccessMutex);
	::EnterCriticalSection(visualNotificationQueueAccessCriticalSection);
#endif
	
	return theVisualNotificationQueue->notificationQueue.size();

#if TARGET_OS_MAC
	status = ::MPExitCriticalRegion(visualNotificationQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(visualNotificationQueueAccessCriticalSection);
#endif

}


void VisualNotificationQueue::update() {

	VisualNotificationQueue* theVisualNotificationQueue = VisualNotificationQueue::getInstance();

	// inputNotificationQueue
#if TARGET_OS_MAC
	OSStatus status = noErr;
	MPCriticalRegionID inputNotificationQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::inputNotificationQueueAccessMutex);
	status = ::MPEnterCriticalRegion(inputNotificationQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION inputNotificationQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::inputNotificationQueueAccessMutex);
	::EnterCriticalSection(inputNotificationQueueAccessCriticalSection);
#endif

	// notificationQueue
#if TARGET_OS_MAC
	MPCriticalRegionID notificationQueueAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::notificationQueueAccessMutex);
	status = ::MPEnterCriticalRegion(notificationQueueAccessCriticalRegion, kDurationForever);
	if (status != noErr) return;
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION notificationQueueAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::notificationQueueAccessMutex);
	::EnterCriticalSection(notificationQueueAccessCriticalSection);
#endif

	for (NotificationQueueIterator it = theVisualNotificationQueue->inputNotificationQueue.begin(); it != theVisualNotificationQueue->inputNotificationQueue.end(); it++) {
		theVisualNotificationQueue->notificationQueue.push_back(*it);
	}
	theVisualNotificationQueue->inputNotificationQueue.clear();

	// notificationQueue
#if TARGET_OS_MAC
	status = ::MPExitCriticalRegion(notificationQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(notificationQueueAccessCriticalSection);
#endif

	// inputNotificationQueue
#if TARGET_OS_MAC
	status = ::MPExitCriticalRegion(inputNotificationQueueAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(inputNotificationQueueAccessCriticalSection);
#endif
}
