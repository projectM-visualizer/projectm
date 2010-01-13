/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTiming.cpp
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

#include "VisualTiming.h"
#include "VisualErrorHandling.h"
#include "VisualThreading.h"
#include "VisualThreadingManager.h"

#if TARGET_OS_MAC
#include <sys/time.h> // gettimeofday(), timeval
#endif



using namespace VizKit;


VisualTiming* VisualTiming::theVisualTiming = NULL;


VisualTiming::VisualTiming() {
#if TARGET_OS_WIN
	QueryPerformanceFrequency(&freq);
#endif
}


VisualTiming::~VisualTiming() {
	for (DurationMapIterator mapIt = this->durationMap.begin(); mapIt != this->durationMap.end(); mapIt++) {
		delete (*mapIt).second;
		(*mapIt).second = NULL;
	}
	theVisualTiming->durationMap.clear();
}


VisualTiming* VisualTiming::getInstance() {
	if (theVisualTiming == NULL) {
#if TARGET_OS_MAC
		MPCriticalRegionID timeStoreSingletonCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::timeStoreSingletonMutex);
		OSStatus status = noErr;
		status = ::MPEnterCriticalRegion(timeStoreSingletonCriticalRegion, kDurationForever);
#endif
#if TARGET_OS_WIN
		LPCRITICAL_SECTION timeStoreSingletonCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::timeStoreSingletonMutex);
		::EnterCriticalSection(timeStoreSingletonCriticalSection);
#endif
		if (theVisualTiming == NULL) {
			theVisualTiming = new VisualTiming;
			if (theVisualTiming == NULL) {
				char errLog[256];
				sprintf(errLog, "ERR: init VisualTiming failed in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
			}
		}
#if TARGET_OS_MAC
		status = ::MPExitCriticalRegion(timeStoreSingletonCriticalRegion);
#endif
#if TARGET_OS_WIN
		::LeaveCriticalSection(timeStoreSingletonCriticalSection);
#endif
	}
	return theVisualTiming;
}


void VisualTiming::dispose() {
	if (theVisualTiming != NULL) {
		delete theVisualTiming;
		theVisualTiming = NULL;
	}
}


void VisualTiming::update() {
	theVisualTiming = VisualTiming::getInstance();
#if TARGET_OS_MAC
    struct timeval tod;
    sint8 retVal;
    retVal = gettimeofday(&tod, NULL);
    
    if (retVal == 0) {
        theVisualTiming->currentTime = tod.tv_sec + tod.tv_usec * 1.0E-6;
    } else {
        if (retVal == -1) {
            //writeLog("Error in file \"%s\" (line %d): function \"%s\":\n%s\n", __FILE__, __LINE__, __func__, strerror(errno));
			writeLog("Error in setTimestamp");
        } else {
            //writeLog("Unqualified error in file \"%s\" (line %d): function \"%s\"\n", __FILE__, __LINE__, __func__);
			writeLog("Unqualified error in setTimestamp");
        }
    }
#endif

#if TARGET_OS_WIN
	QueryPerformanceCounter(&theVisualTiming->currentTime);
#endif

}


uint32 VisualTiming::getElapsedMilliSecsSinceReset(const char* const durationName) {
	VisualItemIdentifier* visualDurationIdentifier = VisualItemIdentifier::createVisualItemIdentifier(durationName);
	theVisualTiming = VisualTiming::getInstance();
	uint32 elapsedMilliSeconds = theVisualTiming->getElapsedMilliSecsSinceReset(*visualDurationIdentifier);
	delete visualDurationIdentifier;
	return elapsedMilliSeconds;
}


uint32 VisualTiming::getElapsedMilliSecsSinceReset(const VisualItemIdentifier& visualDurationIdentifier) {
	double timestampDelta;
	theVisualTiming = VisualTiming::getInstance();

#if TARGET_OS_MAC
	MPCriticalRegionID timeStoreAccessCriticalRegion = VisualThreadingManager::getCriticalRegionID(VisualThreadingManager::timeStoreAccessMutex);
	OSStatus status = noErr;
	status = ::MPEnterCriticalRegion(timeStoreAccessCriticalRegion, kDurationForever);
#endif
#if TARGET_OS_WIN
	LPCRITICAL_SECTION timeStoreAccessCriticalSection = VisualThreadingManager::getCriticalSectionPointer(VisualThreadingManager::timeStoreAccessMutex);
	::EnterCriticalSection(timeStoreAccessCriticalSection);
#endif
	
	DurationMapIterator it;
	it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	if (it == theVisualTiming->durationMap.end()) {
		DurationItem* durationItem;
		durationItem = new DurationItem;
		durationItem->firstTimestamp = theVisualTiming->currentTime;
		durationItem->lastTimestamp = theVisualTiming->currentTime;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
		it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	}
	timestampDelta = theVisualTiming->getTimeDelta(theVisualTiming->currentTime, it->second->firstTimestamp);
	it->second->lastTimestamp = theVisualTiming->currentTime;

#if TARGET_OS_MAC
	status = ::MPExitCriticalRegion(timeStoreAccessCriticalRegion);
#endif
#if TARGET_OS_WIN
	::LeaveCriticalSection(timeStoreAccessCriticalSection);
#endif
	
	return (uint32)(timestampDelta * 1000.0);

}


uint32 VisualTiming::getElapsedMilliSecsSinceLastCall(const char* const durationName) {
	VisualItemIdentifier* visualDurationIdentifier = VisualItemIdentifier::createVisualItemIdentifier(durationName);
	theVisualTiming = VisualTiming::getInstance();
	uint32 elapsedMilliSeconds = theVisualTiming->getElapsedMilliSecsSinceLastCall(*visualDurationIdentifier);
	delete visualDurationIdentifier;
	return elapsedMilliSeconds;
}


uint32 VisualTiming::getElapsedMilliSecsSinceLastCall(const VisualItemIdentifier& visualDurationIdentifier) {
	double timestampDelta;
	theVisualTiming = VisualTiming::getInstance();
	DurationMapIterator it;
	it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	if (it == theVisualTiming->durationMap.end()) {
		DurationItem* durationItem;
		durationItem = new DurationItem;
		durationItem->firstTimestamp = theVisualTiming->currentTime;
		durationItem->lastTimestamp = theVisualTiming->currentTime;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
		it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	}
	timestampDelta = theVisualTiming->getTimeDelta(theVisualTiming->currentTime, it->second->lastTimestamp);
	it->second->lastTimestamp = theVisualTiming->currentTime;
	return (uint32)(timestampDelta * 1000.0);

}


void VisualTiming::resetTimestamp(const char* const durationName) {
	theVisualTiming = VisualTiming::getInstance();
	VisualItemIdentifier* visualDurationIdentifier = VisualItemIdentifier::createVisualItemIdentifier(durationName);
	theVisualTiming->resetTimestamp(*visualDurationIdentifier);
	delete visualDurationIdentifier;
}


void VisualTiming::resetTimestamp(const VisualItemIdentifier& visualDurationIdentifier) {
	theVisualTiming = VisualTiming::getInstance();
	DurationMapIterator it;
	it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	if (it == theVisualTiming->durationMap.end()) {
		DurationItem* durationItem;
		durationItem = new DurationItem;
		durationItem->firstTimestamp = theVisualTiming->currentTime;
		durationItem->lastTimestamp = theVisualTiming->currentTime;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
	} else {
		it->second->firstTimestamp = theVisualTiming->currentTime;
		it->second->lastTimestamp = theVisualTiming->currentTime;
	}
}


const char* const VisualTiming::getCurrentMillisecsDateTime() {

#if TARGET_OS_MAC
    double timestampVal;
#endif
#if TARGET_OS_WIN32
	LARGE_INTEGER timestampVal;

	SYSTEMTIME timeVal;
#endif
#if TARGET_OS_MAC
    struct timeval tod;
	struct tm* ptm;
    sint8 retVal;
	
	theVisualTiming = VisualTiming::getInstance();
	
    retVal = gettimeofday(&tod, NULL);
    
    if (retVal == 0) {
        timestampVal = tod.tv_sec + tod.tv_usec * 1.0E-6;
		ptm = localtime ((time_t*)&tod.tv_sec);
		strftime (theVisualTiming->dateTimeStr, sizeof (theVisualTiming->dateTimeStr), "%Y%m%d%H%M%S", ptm);
		//printf("test33: %s\n", time_string);
		//dateTimeInt = atoi(time_string);
		//printf("test44: %ld\n", dateTimeInt);
    } else {
        if (retVal == -1) {
            //writeLog("Error in file \"%s\" (line %d): function \"%s\":\n%s\n", __FILE__, __LINE__, __func__, strerror(errno));
			writeLog("Error in getCurrentMillisecsDateTime");
        } else {
            //writeLog("Unqualified error in file \"%s\" (line %d): function \"%s\"\n", __FILE__, __LINE__, __func__);
			writeLog("Unqualified error in getCurrentMillisecsDateTime");
        }
    }
#endif

#if TARGET_OS_WIN32
	QueryPerformanceCounter(&timestampVal);
	// DWORD(theVisualTiming->timestampVal * 1000 / theVisualTiming->freq)
	GetSystemTime(&timeVal);

	sprintf(theVisualTiming->dateTimeStr, "%d%d%d%d%d%d", timeVal.wYear, timeVal.wMonth, timeVal.wDay, timeVal.wHour, timeVal.wMinute, timeVal.wSecond);
	// SYSTEMTIME st
	// GetSystemTime()
#endif

	//printf("timeTest: %ld\n", theVisualTiming->timestampVal);
	return theVisualTiming->dateTimeStr;

}


#if TARGET_OS_MAC
double VisualTiming::getTimeDelta(double young_timestamp, double old_timestamp) {
	return (young_timestamp - old_timestamp);
#endif
#if TARGET_OS_WIN
double VisualTiming::getTimeDelta(LARGE_INTEGER young_timestamp, LARGE_INTEGER old_timestamp) {
	return (young_timestamp.QuadPart - old_timestamp.QuadPart)/((double)theVisualTiming->freq.QuadPart);
#endif
}


void VisualTiming::addDurationItemToDurationMap(const VisualItemIdentifier& visualDurationIdentifier) {
	theVisualTiming = VisualTiming::getInstance();
	DurationMapIterator it;
	it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	if (it == theVisualTiming->durationMap.end()) {
		DurationItem* durationItem = new DurationItem;
		durationItem->firstTimestamp = theVisualTiming->currentTime;
		durationItem->lastTimestamp = theVisualTiming->currentTime;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
	} else {
		it->second->firstTimestamp = theVisualTiming->currentTime;
		it->second->lastTimestamp = theVisualTiming->currentTime;
	}
}
