/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualTimeline.cpp
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

#include "VisualTimeline.h"
#include "VisualErrorHandling.h"
#include "VisualTiming.h"
#include "VisualInterpolation.h"
#include "VisualItemIdentifier.h"



using namespace VizKit;


VisualTimeline::VisualTimeline(bool aDebugMode) {
	if (aDebugMode == true) {
		writeLog("VisualTimeline::VisualTimeline");
	}
	repeatMode = kRepeatFromStart;
	durationIdentifier = new VisualItemIdentifier;
	visualInterpolation = new VisualInterpolation;
	durationInMilliseconds = 3000;
	minValue = 0.0;
	maxValue = 1.0;
	movingDirection = kAscending;
	distance = 1.0;
	VisualTiming::addDurationItemToDurationMap(*durationIdentifier);
	elapsedMilliseconds = 0;
	offsetMilliseconds = 0;
	currentValue = 0.0;
	isStopped = false;
	debugMode = aDebugMode;
}


VisualTimeline::~VisualTimeline() {
	delete visualInterpolation;
	delete durationIdentifier;
}


VisualTimeline::VisualTimeline(const VisualTimeline& other) {
	copy(other);
}


VisualTimeline& VisualTimeline::operator=(const VisualTimeline& other) {
	
	if (this == &other) return *this;
	
	delete this->visualInterpolation;
	delete this->durationIdentifier;

	this->copy(other);

	return *this;
}


void VisualTimeline::copy(const VisualTimeline& other) {
	this->repeatMode = other.repeatMode;
	this->movingDirection = other.movingDirection;
	this->visualInterpolation = new VisualInterpolation(*other.visualInterpolation);
	this->durationIdentifier = new VisualItemIdentifier;
	VisualTiming::addDurationItemToDurationMap(*(this->durationIdentifier));
	this->durationInMilliseconds = other.durationInMilliseconds;
	this->minValue = other.minValue;
	this->maxValue = other.maxValue;
	this->distance = other.distance;
	this->elapsedMilliseconds = other.elapsedMilliseconds;
	this->offsetMilliseconds = other.offsetMilliseconds;
	this->currentValue = other.currentValue;
	this->isStopped = other.isStopped;
	this->debugMode = other.debugMode;
}


void VisualTimeline::setStartValue(double aStartValue) {
	if (this->debugMode == true) {
		char logStr[128];
		sprintf(logStr, "VisualTimeline::setStartValue: %f", aStartValue);
		writeLog(logStr);
	}
	if (this->movingDirection == kAscending) {
		this->minValue = aStartValue;
	} else if (this->movingDirection == kDescending) {
		this->maxValue = aStartValue;
	}
	if (this->maxValue < this->minValue) {
		this->toggleMovingDirection();
		double temp = this->minValue;
		this->minValue = this->maxValue;
		this->maxValue = temp;
	}
	this->offsetMilliseconds = 0;
	this->distance = this->maxValue - this->minValue;
	if (this->currentValue < this->minValue) {
		this->currentValue = this->minValue;
	}
	if (this->currentValue > this->maxValue) {
		this->currentValue = this->maxValue;
	}
}


void VisualTimeline::setStopValue(double aStopValue) {
	if (this->debugMode == true) {
		char logStr[128];
		sprintf(logStr, "VisualTimeline::setStopValue: %f", aStopValue);
		writeLog(logStr);
	}
	if (this->movingDirection == kAscending) {
		this->maxValue = aStopValue;
	} else if (this->movingDirection == kDescending) {
		this->minValue = aStopValue;
	}
	if (this->maxValue < this->minValue) {
		this->toggleMovingDirection();
		double temp = this->minValue;
		this->minValue = this->maxValue;
		this->maxValue = temp;
	}
	this->offsetMilliseconds = 0;
	this->distance = this->maxValue - this->minValue;
	if (this->currentValue < this->minValue) {
		this->currentValue = this->minValue;
	}
	if (this->currentValue > this->maxValue) {
		this->currentValue = this->maxValue;
	}
}


double VisualTimeline::getMinValue() const {
	return this->minValue;
}


double VisualTimeline::getMaxValue() const {
	return this->maxValue;
}


double VisualTimeline::getDistance(void) const {
	return this->distance;
}


void VisualTimeline::setDurationInMilliseconds(uint32 numberOfMilliseconds) {
	if (this->debugMode == true) {
		char logStr[128];
		sprintf(logStr, "VisualTimeline::setDurationInMilliseconds: %d", numberOfMilliseconds);
		writeLog(logStr);
	}
	this->durationInMilliseconds = numberOfMilliseconds;
}


uint32 VisualTimeline::getDurationInMilliseconds() const {
	return this->durationInMilliseconds;
}


void VisualTimeline::setRepeatMode(RepeatMode aRepeatMode) {
	this->repeatMode = aRepeatMode;
}


RepeatMode VisualTimeline::getRepeatMode() const {
	return this->repeatMode;
}


bool VisualTimeline::setCurrentValue(double newCurrValue) {

	char logStr[128];
	if (this->debugMode == true) {
		sprintf(logStr, "VisualTimeline::setCurrentValue: %f", newCurrValue);
		writeLog(logStr);
	}

	bool adjustDistanceAndDuration = false;
	if (newCurrValue < this->minValue) {
		this->minValue = newCurrValue;
		adjustDistanceAndDuration = true;
	}
	if (newCurrValue > this->maxValue) {
		this->maxValue = newCurrValue;	
		adjustDistanceAndDuration = true;
	}
	
	if (adjustDistanceAndDuration == true) {
		double distanceBefore = this->distance;
		this->distance = this->maxValue - this->minValue;
		this->durationInMilliseconds = (uint32)((double)this->durationInMilliseconds * (distanceBefore / this->distance));
	}
	
	// calc offsetMilliseconds
	if (this->movingDirection == kAscending) {
		this->offsetMilliseconds = (uint32)((double)this->durationInMilliseconds * ((newCurrValue - this->minValue) / this->distance));
		if (this->debugMode == true) {
			sprintf(logStr, "VisualTimeline::setCurrentValue: offsetMilliseconds1: %d", this->offsetMilliseconds);
			writeLog(logStr);
		}
	} else if (this->movingDirection == kDescending) {
		this->offsetMilliseconds = (uint32)((double)this->durationInMilliseconds * ((this->maxValue - newCurrValue) / this->distance));
		if (this->debugMode == true) {
			sprintf(logStr, "VisualTimeline::setCurrentValue: offsetMilliseconds2: %d", this->offsetMilliseconds);
			writeLog(logStr);
		}
	}
	this->currentValue = newCurrValue;
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
	return adjustDistanceAndDuration;
}


TimelineUpdateResult VisualTimeline::update() {
	TimelineUpdateResult result = kTimelineUpdateOK;
	char logStr[256];
	if (this->debugMode == true) {
		sprintf(logStr, "VisualTimeline::update1: this->currentValue: %f, this->offsetMilliseconds: %d, this->elapsedMilliseconds: %d", this->currentValue, this->offsetMilliseconds, this->elapsedMilliseconds);
		writeLog(logStr);
	}
	if (this->isStopped == true) {
		return kTimelineIsStopped;
	}
	if (this->durationInMilliseconds == 0) {
		return kNoDurationTime;
	}
	this->elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset(*(this->durationIdentifier)) + this->offsetMilliseconds;
	if (this->elapsedMilliseconds >= this->durationInMilliseconds) {
		if (this->debugMode == true) {
			sprintf(logStr, "VisualTimeline::update2: this->elapsedMilliseconds >= this->durationInMilliseconds (%d >= %d)", this->elapsedMilliseconds, this->durationInMilliseconds);
			writeLog(logStr);
		}
		result = kElapsedTimeDidExceedDuration;
	} else {
		if (this->debugMode == true) {
			sprintf(logStr, "VisualTimeline::update3: this->elapsedMilliseconds < this->durationInMilliseconds (%d < %d)", this->elapsedMilliseconds, this->durationInMilliseconds);
			writeLog(logStr);
		}
		double interpolationValue = 0.0;
		if (this->movingDirection == kAscending) {
			if (this->debugMode == true) {
				sprintf(logStr, "VisualTimeline::update4: (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds: %f (%d / %d: %f)", (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds, this->elapsedMilliseconds, this->durationInMilliseconds, (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds);
				writeLog(logStr);
			}
			interpolationValue = this->visualInterpolation->getValueAtPosition((double)this->elapsedMilliseconds / (double)this->durationInMilliseconds);
		} else if (this->movingDirection == kDescending) {
			if (this->debugMode == true) {
				sprintf(logStr, "VisualTimeline::update5: 1.0 - (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds: %f (1.0 - %d / %d: %f)", 1.0 - (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds, this->elapsedMilliseconds, this->durationInMilliseconds, (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds);
				writeLog(logStr);
			}
			interpolationValue = this->visualInterpolation->getValueAtPosition(1.0 - (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds);
		}
		double expandedValue = interpolationValue * this->distance;
		if (this->debugMode == true) {
			sprintf(logStr, "VisualTimeline::update6: expandedValue: %f", expandedValue);
			writeLog(logStr);
		}
		this->currentValue = expandedValue + this->minValue;
	}
	if (this->debugMode == true) {
		sprintf(logStr, "VisualTimeline::update7: this->currentValue: %f", this->currentValue);
		writeLog(logStr);
	}
	if (result == kTimelineUpdateOK) {
		if (this->movingDirection == kAscending) {
			if (this->currentValue >= this->maxValue) {
				result = kStopValueHit;
			}
		}
		if (this->movingDirection == kDescending) {
			if (this->currentValue <= this->minValue) {
				result = kStopValueHit;
			}
		}
	}

	if ((result == kElapsedTimeDidExceedDuration) || (result == kStopValueHit)) {
		if (this->repeatMode == kRepeatMirrored) {
			this->toggleMovingDirection();
		}
		this->reset();
	}
	
	return result;
}


double VisualTimeline::getCurrentValue() const {
	return this->currentValue;
}


void VisualTimeline::reset() {
	if (this->debugMode == true) {
		writeLog("VisualTimeline::reset");
	}
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
	this->elapsedMilliseconds = 0;
	this->offsetMilliseconds = 0;
	if (this->movingDirection == kAscending) {
		this->currentValue = this->minValue;
	} else if (this->movingDirection == kDescending) {
		this->currentValue = this->maxValue;
	}
}


void VisualTimeline::stop() {
	this->isStopped = true;
}


void VisualTimeline::start() {
	if (this->debugMode == true) {
		writeLog("VisualTimeline::start");
	}
	this->isStopped = false;
	this->elapsedMilliseconds = 0;
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
}


void VisualTimeline::resume() {
	if (this->debugMode == true) {
		writeLog("VisualTimeline::resume");
	}
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
	if (this->movingDirection == kAscending) {
		this->offsetMilliseconds = this->elapsedMilliseconds;
	} else if (this->movingDirection == kDescending) {
		this->offsetMilliseconds = this->durationInMilliseconds - this->elapsedMilliseconds;
	}
	this->isStopped = false;
}


void VisualTimeline::setInterpolationType(InterpolationType anInterpolationType) {
	this->visualInterpolation->setType(anInterpolationType);
}


MovingDirection VisualTimeline::getMovingDirection() const {
	return this->movingDirection;
}


void VisualTimeline::toggleMovingDirection() {
	if (this->movingDirection == kAscending) {
		this->movingDirection = kDescending;
	} else if (this->movingDirection == kDescending) {
		this->movingDirection = kAscending;
	}
	if (this->debugMode == true) {
		char logStr[128];
		if (this->movingDirection == kAscending) {
			sprintf(logStr, "VisualTimeline::toggleMovingDirection: kAscending (this->offsetMilliseconds: %d) (this->durationInMilliseconds: %d)", this->offsetMilliseconds, this->durationInMilliseconds);
		} else if (this->movingDirection == kDescending) {
			sprintf(logStr, "VisualTimeline::toggleMovingDirection: kDescending (this->offsetMilliseconds: %d) (this->durationInMilliseconds: %d)", this->offsetMilliseconds, this->durationInMilliseconds);
		}
		writeLog(logStr);
	}
}


void VisualTimeline::setDebugMode(bool requestedDebugMode) {
	this->debugMode = requestedDebugMode;
}


void VisualTimeline::convertTimelineUpdateResultToString(const TimelineUpdateResult aResult, char* outString) {
	const char* messageString;
	switch (aResult) {
		case kTimelineUpdateOK:
			messageString = "kTimelineUpdateOK";
			break;
		case kElapsedTimeDidExceedDuration:
			messageString = "kElapsedTimeDidExceedDuration";
			break;
		case kStopValueHit:
			messageString = "kStopValueHit";
			break;
		case kTimelineIsStopped:
			messageString = "kTimelineIsStopped";
			break;
		case kNoDurationTime:
			messageString = "kNoDurationTime";
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unhandled TimelineUpdateResult %d in file: %s (line: %d) [%s])", aResult, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			messageString = "unknownTimelineUpdateResult";
	}
	strcpy(outString, messageString);
}
