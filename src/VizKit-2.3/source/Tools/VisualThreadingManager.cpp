/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualThreadingManager.cpp
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

#include "VisualThreadingManager.h"
#include "VisualErrorHandling.h"
#include "VisualTiming.h"
#include "VisualDispatch.h"



using namespace VizKit;



VisualThreadingManager* VisualThreadingManager::theVisualThreadingManager = NULL;


VisualThreadingManager::VisualThreadingManager() {
	terminateAllThreads = false;
#if TARGET_OS_MAC
	notificationQueue = NULL;
	OSStatus status = noErr;
	status = MPCreateQueue(&notificationQueue);
	status = MPCreateCriticalRegion(&downloadQueueSingletonCriticalRegion);
	status = MPCreateCriticalRegion(&downloadQueueAccessCriticalRegion);
	status = MPCreateCriticalRegion(&downloadQueueAddRequestCriticalRegion);
	status = MPCreateCriticalRegion(&downloadQueueStopRequestCriticalRegion);
	status = MPCreateCriticalRegion(&notificationQueueSingletonCriticalRegion);
	status = MPCreateCriticalRegion(&notificationQueueAccessCriticalRegion);
	status = MPCreateCriticalRegion(&inputNotificationQueueAccessCriticalRegion);
	status = MPCreateCriticalRegion(&timeStoreSingletonCriticalRegion);
	status = MPCreateCriticalRegion(&timeStoreAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::InitializeCriticalSection(&downloadQueueSingletonCriticalSection);
	::InitializeCriticalSection(&downloadQueueAccessCriticalSection);
	::InitializeCriticalSection(&downloadQueueAddRequestCriticalSection);
	::InitializeCriticalSection(&downloadQueueStopRequestCriticalSection);
	::InitializeCriticalSection(&notificationQueueSingletonCriticalSection);
	::InitializeCriticalSection(&notificationQueueAccessCriticalSection);
	::InitializeCriticalSection(&inputNotificationQueueAccessCriticalSection);
	::InitializeCriticalSection(&timeStoreSingletonCriticalSection);
	::InitializeCriticalSection(&timeStoreAccessCriticalSection);
#endif
}


VisualThreadingManager::~VisualThreadingManager() {
	for (ThreadVectorIterator it = threadVector.begin(); it != threadVector.end(); it++) {
		delete (*it).threadIdentifier;
		(*it).threadIdentifier = NULL;
	}
#if TARGET_OS_MAC
	OSStatus status = noErr;
	status = MPDeleteQueue(notificationQueue);
#endif
}


VisualThreadingManager* VisualThreadingManager::getInstance() {
    if (theVisualThreadingManager == NULL) {
		theVisualThreadingManager = new VisualThreadingManager;
		if (theVisualThreadingManager == NULL) {
			char errLog[256];
			sprintf(errLog, "ERR: init VisualThreadingManager failed in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}
	}
	return theVisualThreadingManager;
}


void VisualThreadingManager::dispose() {
	if (theVisualThreadingManager != NULL) {
		theVisualThreadingManager->terminateAllThreads = true;
		VisualItemIdentifier terminateAllThreadsId;
		uint32 elapsedMilliseconds = 0;
		while ((VisualThreadingManager::getNumberOfCurrentlyRunningThreads() > 0) && elapsedMilliseconds < 2000) {
			elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset(terminateAllThreadsId);
			VisualTiming::update(); // manual update of current timestamp
			// wait
		}
		// aborted after max 2 sec. of wait time
		ThreadVectorIterator it = theVisualThreadingManager->threadVector.begin();
		while (it != theVisualThreadingManager->threadVector.end()) {
#if TARGET_OS_MAC
			OSStatus terminationStatus = noErr;
			OSStatus terminationResult = noErr;
			terminationResult = MPTerminateTask(it->threadId, terminationStatus);
#endif
#if TARGET_OS_WIN
			DWORD dwExitCode = 0;
			BOOL success = TerminateThread(it->threadId, dwExitCode);
#endif
			it = theVisualThreadingManager->threadVector.erase(it);
		}
		delete theVisualThreadingManager;
		theVisualThreadingManager = NULL;
	}
}


uint32 VisualThreadingManager::getNumberOfCurrentlyRunningThreads() {
	theVisualThreadingManager = VisualThreadingManager::getInstance();
	return theVisualThreadingManager->threadVector.size();
}


bool VisualThreadingManager::createThread(ThreadingFuncPtr aThreadingFuncPtr, void* param, VisualThreading::ThreadDidDieCallback callbackAfterThreadDidDie) {
	bool success = false;
	theVisualThreadingManager = VisualThreadingManager::getInstance();
	if (theVisualThreadingManager->terminateAllThreads == true) {
		return success;
	}
	VisualItemIdentifier* threadIdentifier = new VisualItemIdentifier;
#if TARGET_OS_MAC
	MPTaskID threadId;
	ByteCount stackSize = 0;
	void* terminationParameter1 = threadIdentifier;
	void* terminationParameter2 = NULL;
	OSStatus osStatus = MPCreateTask(&(*aThreadingFuncPtr), param, stackSize, theVisualThreadingManager->notificationQueue, terminationParameter1, terminationParameter2, 0, &threadId);
	if (osStatus == noErr)
	{
		success = true;
	}
#endif
#if TARGET_OS_WIN
	LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL;
	SIZE_T stackSize = 0;
	DWORD dwCreationFlags = 0;
	LPDWORD threadId = 0;
	HANDLE hdl = CreateThread(lpThreadAttributes, stackSize, (LPTHREAD_START_ROUTINE)aThreadingFuncPtr, (LPVOID)param, dwCreationFlags, threadId);
	if (hdl)
	{
		success = true;
	}
#endif
	if (success == true) {
		theVisualThreadingManager->addThread(threadId, threadIdentifier, callbackAfterThreadDidDie);
	} else {
		delete threadIdentifier;
	}
	return success;	
}


#if TARGET_OS_MAC
void VisualThreadingManager::addThread(MPTaskID threadId, VisualItemIdentifier* threadIdentifier, VisualThreading::ThreadDidDieCallback callbackAfterThreadDidDie) {
#endif
#if TARGET_OS_WIN
void VisualThreadingManager::addThread(HANDLE threadId, VisualItemIdentifier* threadIdentifier, VisualThreading::ThreadDidDieCallback callbackAfterThreadDidDie) {
#endif
	theVisualThreadingManager = VisualThreadingManager::getInstance();
	ThreadItem threadItem;
	threadItem.threadId = threadId;
	threadItem.threadIdentifier = threadIdentifier;
	threadItem.callbackAfterDeathOfThread = callbackAfterThreadDidDie;
	theVisualThreadingManager->threadVector.push_back(threadItem);
}


void VisualThreadingManager::update() {
	theVisualThreadingManager = VisualThreadingManager::getInstance();
	ThreadVectorIterator it = theVisualThreadingManager->threadVector.begin();
	while (it != theVisualThreadingManager->threadVector.end()) {
#if TARGET_OS_MAC
		void* param1 = (void*)((*it).threadIdentifier);
		void* param2 = NULL;
		void* param3= NULL;
		Duration timeout = kDurationImmediate;
		OSStatus status = MPWaitOnQueue(theVisualThreadingManager->notificationQueue, &param1, &param2, &param3, timeout);
		if (status == noErr) {
#endif
#if TARGET_OS_WIN
		DWORD exitCode = 0;
		BOOL success = ::GetExitCodeThread((*it).threadId, &exitCode);
		if (exitCode != STILL_ACTIVE) {
#endif
			// thread terminated
			// a) call registered callback
			if ((*it).callbackAfterDeathOfThread != NULL) {
				(*it).callbackAfterDeathOfThread();
			}
			// b) unregister thread from queue
			it = theVisualThreadingManager->threadVector.erase(it);
		} else {
			it++;
		}
	}
}

#if TARGET_OS_MAC
MPCriticalRegionID VisualThreadingManager::getCriticalRegionID(VisualCriticalRegion criticalRegionId) {
	theVisualThreadingManager = VisualThreadingManager::getInstance();
	switch (criticalRegionId)
	{
		case downloadQueueSingletonMutex:
			return theVisualThreadingManager->downloadQueueSingletonCriticalRegion;
			break;
		case downloadQueueAccessMutex:
			return theVisualThreadingManager->downloadQueueAccessCriticalRegion;
			break;
		case addRequestDownloadQueueAccess:
			return theVisualThreadingManager->downloadQueueAddRequestCriticalRegion;
			break;
		case stopRequestDownloadQueueAccess:
			return theVisualThreadingManager->downloadQueueStopRequestCriticalRegion;
			break;
		case notificationQueueSingletonMutex:
			return theVisualThreadingManager->notificationQueueSingletonCriticalRegion;
			break;
		case notificationQueueAccessMutex:
			return theVisualThreadingManager->notificationQueueAccessCriticalRegion;
			break;
		case inputNotificationQueueAccessMutex:
			return theVisualThreadingManager->inputNotificationQueueAccessCriticalRegion;
			break;
		case timeStoreSingletonMutex:
			return theVisualThreadingManager->timeStoreSingletonCriticalRegion;
			break;
		case timeStoreAccessMutex:
			return theVisualThreadingManager->timeStoreAccessCriticalRegion;
			break;
		default:
			return NULL;
	}
	return NULL;
}
#endif


#if TARGET_OS_WIN
LPCRITICAL_SECTION VisualThreadingManager::getCriticalSectionPointer(VisualCriticalRegion criticalRegionId) {
	theVisualThreadingManager = VisualThreadingManager::getInstance();
	switch (criticalRegionId)
	{
		case downloadQueueSingletonMutex:
			return &(theVisualThreadingManager->downloadQueueSingletonCriticalSection);
			break;
		case downloadQueueAccessMutex:
			return &(theVisualThreadingManager->downloadQueueAccessCriticalSection);
			break;
		case addRequestDownloadQueueAccess:
			return &(theVisualThreadingManager->downloadQueueAddRequestCriticalSection);
			break;
		case stopRequestDownloadQueueAccess:
			return &(theVisualThreadingManager->downloadQueueStopRequestCriticalSection);
			break;
		case notificationQueueSingletonMutex:
			return &(theVisualThreadingManager->notificationQueueSingletonCriticalSection);
			break;
		case notificationQueueAccessMutex:
			return &(theVisualThreadingManager->notificationQueueAccessCriticalSection);
			break;
		case inputNotificationQueueAccessMutex:
			return &(theVisualThreadingManager->inputNotificationQueueAccessCriticalSection);
			break;
		case timeStoreSingletonMutex:
			return &(theVisualThreadingManager->timeStoreSingletonCriticalSection);
			break;
		case timeStoreAccessMutex:
			return &(theVisualThreadingManager->timeStoreAccessCriticalSection);
			break;
		default:
			return NULL;
	}
	return NULL;
}
#endif
