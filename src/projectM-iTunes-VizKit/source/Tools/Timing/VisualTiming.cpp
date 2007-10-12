/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualTiming.cpp
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

#include "VisualTiming.h"
#include "VisualErrorHandling.h"

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
		theVisualTiming = new VisualTiming;
    }
	if (theVisualTiming == NULL) {
		writeLog("ERR: init theVisualTiming failed");
	}
	return theVisualTiming;
}


void VisualTiming::dispose() {
	if (theVisualTiming != NULL) {
		delete theVisualTiming;
		theVisualTiming = NULL;
	}
}


void VisualTiming::setTimestampOfPlayerCall() {
	theVisualTiming = VisualTiming::getInstance();
#if TARGET_OS_MAC
    struct timeval tod;
    SInt8 retVal;
    retVal = gettimeofday(&tod, NULL);
    
    if (retVal == 0) {
        theVisualTiming->timestampOfPlayerCall = tod.tv_sec + tod.tv_usec * 1.0E-6;
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
	QueryPerformanceCounter(&theVisualTiming->timestampOfPlayerCall);
#endif

}


UInt32 VisualTiming::getElapsedMilliSecsSinceReset(const char* const durationName) {
	VisualItemIdentifier* visualDurationIdentifier = VisualItemIdentifier::createVisualItemIdentifier(durationName);
	theVisualTiming = VisualTiming::getInstance();
	UInt32 elapsedMilliSeconds = theVisualTiming->getElapsedMilliSecsSinceReset(*visualDurationIdentifier);
	delete visualDurationIdentifier;
	return elapsedMilliSeconds;
}


UInt32 VisualTiming::getElapsedMilliSecsSinceReset(const VisualItemIdentifier& visualDurationIdentifier) {
	double timestampDelta;
	theVisualTiming = VisualTiming::getInstance();
	DurationMapIterator it;
	it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	if (it == theVisualTiming->durationMap.end()) {
		DurationItem* durationItem;
		durationItem = new DurationItem;
		durationItem->firstTimestamp = theVisualTiming->timestampOfPlayerCall;
		durationItem->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
		it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	}
	timestampDelta = theVisualTiming->getTimeDelta(theVisualTiming->timestampOfPlayerCall, it->second->firstTimestamp);
	it->second->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
	return (UInt32)(timestampDelta * 1000.0);

}


UInt32 VisualTiming::getElapsedMilliSecsSinceLastCall(const char* const durationName) {
	VisualItemIdentifier* visualDurationIdentifier = VisualItemIdentifier::createVisualItemIdentifier(durationName);
	theVisualTiming = VisualTiming::getInstance();
	UInt32 elapsedMilliSeconds = theVisualTiming->getElapsedMilliSecsSinceLastCall(*visualDurationIdentifier);
	delete visualDurationIdentifier;
	return elapsedMilliSeconds;
}


UInt32 VisualTiming::getElapsedMilliSecsSinceLastCall(const VisualItemIdentifier& visualDurationIdentifier) {
	double timestampDelta;
	theVisualTiming = VisualTiming::getInstance();
	DurationMapIterator it;
	it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	if (it == theVisualTiming->durationMap.end()) {
		DurationItem* durationItem;
		durationItem = new DurationItem;
		durationItem->firstTimestamp = theVisualTiming->timestampOfPlayerCall;
		durationItem->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
		it = theVisualTiming->durationMap.find(visualDurationIdentifier);
	}
	timestampDelta = theVisualTiming->getTimeDelta(theVisualTiming->timestampOfPlayerCall, it->second->lastTimestamp);
	it->second->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
	return (UInt32)(timestampDelta * 1000.0);

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
		durationItem->firstTimestamp = theVisualTiming->timestampOfPlayerCall;
		durationItem->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
	} else {
		it->second->firstTimestamp = theVisualTiming->timestampOfPlayerCall;
		it->second->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
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
    SInt8 retVal;
	
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
		durationItem->firstTimestamp = theVisualTiming->timestampOfPlayerCall;
		durationItem->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
		theVisualTiming->durationMap[visualDurationIdentifier] = durationItem;
	} else {
		it->second->firstTimestamp = theVisualTiming->timestampOfPlayerCall;
		it->second->lastTimestamp = theVisualTiming->timestampOfPlayerCall;
	}
}
