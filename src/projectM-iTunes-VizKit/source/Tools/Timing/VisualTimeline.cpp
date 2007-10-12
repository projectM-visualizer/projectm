/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualTimeline.cpp
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

#include "VisualTimeline.h"
#include "VisualErrorHandling.h"
#include "VisualTiming.h"
#include "VisualItemIdentifier.h"



using namespace VizKit;


VisualTimeline::VisualTimeline() {
	repeatMode = kRepeatFromStart;
	movingDirection = kForward;
	durationIdentifier = new VisualItemIdentifier;
	visualInterpolation = new VisualInterpolation;
	durationInMilliseconds = 3000;
	VisualTiming::addDurationItemToDurationMap(*durationIdentifier);
	elapsedMilliseconds = 0;
	offsetMilliseconds = 0;
	currentValue = 0.0;
	isStopped = false;
	debugMode = false;
	elapsedTimeGreaterThanDurationCallback = NULL;
	userData = NULL;
}


VisualTimeline::~VisualTimeline() {
	delete visualInterpolation;
	delete durationIdentifier;
}


VisualTimeline::VisualTimeline(const VisualTimeline& other) {
	copy(other);
}


VisualTimeline& VisualTimeline::operator=(const VisualTimeline& other) {
	if (this != &other) {
		delete this->visualInterpolation;
		delete this->durationIdentifier;
		this->copy(other);
	}
	return *this;
}


void VisualTimeline::copy(const VisualTimeline& other) {
	this->repeatMode = other.repeatMode;
	this->movingDirection = other.movingDirection;
	this->visualInterpolation = new VisualInterpolation(*other.visualInterpolation);
	this->durationIdentifier = new VisualItemIdentifier;
	VisualTiming::addDurationItemToDurationMap(*(this->durationIdentifier));
	this->durationInMilliseconds = other.durationInMilliseconds;
	this->elapsedMilliseconds = other.elapsedMilliseconds;
	this->offsetMilliseconds = other.offsetMilliseconds;
	this->currentValue = other.currentValue;
	this->isStopped = other.isStopped;
	this->debugMode = other.debugMode;
	this->elapsedTimeGreaterThanDurationCallback = other.elapsedTimeGreaterThanDurationCallback;
	this->userData = other.userData;
}


void VisualTimeline::setStartValue(double aStartValue) {
	this->visualInterpolation->setStartValue(aStartValue);
	this->offsetMilliseconds = 0;
}


double VisualTimeline::getStartValue() {
	return this->visualInterpolation->getStartValue();
}


void VisualTimeline::setEndValue(double anEndValue) {
	this->visualInterpolation->setEndValue(anEndValue);
	this->offsetMilliseconds = 0;
}


double VisualTimeline::getEndValue() {
	return this->visualInterpolation->getEndValue();
}


void VisualTimeline::setDurationInMilliseconds(UInt32 numberOfMilliseconds) {
	double currVal;
	currVal = this->getCurrentValue();
	this->durationInMilliseconds = numberOfMilliseconds;
	this->elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset(*(this->durationIdentifier));
	this->setCurrentValue(currVal);
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
}


UInt32 VisualTimeline::getDurationInMilliseconds() {
	return this->durationInMilliseconds;
}


void VisualTimeline::setRepeatMode(RepeatMode aRepeatMode) {
	this->repeatMode = aRepeatMode;
}


RepeatMode VisualTimeline::getRepeatMode() {
	return this->repeatMode;
}


void VisualTimeline::setCurrentValue(double newValue) {
	double currStartValue = this->visualInterpolation->getStartValue();
	double currEndValue = this->visualInterpolation->getEndValue();
	double minVal = (currStartValue < currEndValue) ? currStartValue : currEndValue;
	double maxVal = (currEndValue > currStartValue) ? currEndValue : currStartValue;
	/*
	if (newValue < minVal) {
		// distance expanded
		if (currStartValue < currEndValue) {
			this->setStartValue(newValue);
			currStartValue = newValue;
		} else {
			this->setEndValue(newValue);
			currEndValue = newValue;
		}
	}
	if (newValue > maxVal) {
		// distance expanded
		if (currStartValue < currEndValue) {
			this->setEndValue(newValue);
			currEndValue = newValue;
		} else {
			this->setStartValue(newValue);
			currStartValue = newValue;
		}
	}
	*/
	
	/* always aim to reach endValue */
	if (newValue < minVal) {
		if (currStartValue < currEndValue) {
			this->setStartValue(newValue);
			this->setEndValue(currEndValue - (minVal - newValue));
		} else {
			this->setEndValue(newValue);
			this->setStartValue(currStartValue - (minVal - newValue));
		}
		currStartValue = this->visualInterpolation->getStartValue();
		currEndValue = this->visualInterpolation->getEndValue();
	} else if (newValue > maxVal) {
		if (currStartValue < currEndValue) {
			this->setEndValue(newValue);
			this->setStartValue(currStartValue + (newValue - maxVal));
		} else {
			this->setStartValue(newValue);
			this->setEndValue(currEndValue + (newValue - maxVal));
		}
		currStartValue = this->visualInterpolation->getStartValue();
		currEndValue = this->visualInterpolation->getEndValue();
	}
	
	double distance = 0.0;
	if (currStartValue < currEndValue) {
		distance = currEndValue - currStartValue;
	} else {
		distance = currStartValue - currEndValue;
	}
	if (distance == 0.0) return; // prevent divison by zero
	if (this->movingDirection == kForward) {
		if (currStartValue < currEndValue) {
			this->offsetMilliseconds = (UInt32)((double)this->durationInMilliseconds * ((newValue - currStartValue) / distance));
		} else {
			this->offsetMilliseconds = (UInt32)((double)this->durationInMilliseconds * ((currStartValue - newValue) / distance));
		}
	} else if (this->movingDirection == kBackward) {
		if (currStartValue < currEndValue) {
			this->offsetMilliseconds = (UInt32)((double)this->durationInMilliseconds * ((currEndValue - newValue) / distance));
		} else {
			this->offsetMilliseconds = (UInt32)((double)this->durationInMilliseconds * ((newValue - currEndValue) / distance));
		}
	}
}


double VisualTimeline::getCurrentValue() {
	if (this->isStopped == true) {
		return this->currentValue;
	}
	if (this->durationInMilliseconds == 0) {
		return this->currentValue;
	}
	this->elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset(*(this->durationIdentifier)) + this->offsetMilliseconds;
	if (this->elapsedMilliseconds >= this->durationInMilliseconds) {
		if (this->repeatMode == kRepeatMirrored) {
			if (this->movingDirection == kForward) {
				this->currentValue = this->visualInterpolation->getEndValue();
				this->movingDirection = kBackward;
			} else if (this->movingDirection == kBackward) {
				this->currentValue = this->visualInterpolation->getStartValue();
				this->movingDirection = kForward;
			}
		} else if (this->repeatMode == kRepeatFromStart) {
			if (this->movingDirection == kForward) {
				this->currentValue = this->visualInterpolation->getEndValue();
			} else if (this->movingDirection == kBackward) {
				this->currentValue = this->visualInterpolation->getStartValue();
			}
		}
		VisualTiming::resetTimestamp(*(this->durationIdentifier));
		this->offsetMilliseconds = 0;
		this->elapsedTimeDidExceedDuration();
	} else {
		if (this->movingDirection == kForward) {
			this->currentValue = this->visualInterpolation->getValueAtPosition((double)this->elapsedMilliseconds / (double)this->durationInMilliseconds);
		} else if (this->movingDirection == kBackward) {
			this->currentValue = this->visualInterpolation->getValueAtPosition(1.0 - (double)this->elapsedMilliseconds / (double)this->durationInMilliseconds);
		}
	}
	return this->currentValue;
}


void VisualTimeline::reset() {
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
	this->offsetMilliseconds = 0;
	if (this->movingDirection == kForward) {
		this->currentValue = this->visualInterpolation->getStartValue();
	} else if (this->movingDirection == kBackward) {
		this->currentValue = this->visualInterpolation->getEndValue();
	}
}


void VisualTimeline::stop() {
	this->isStopped = true;
}


void VisualTimeline::start(VisualTimelineElapsedTimeDidExceedDurationCallback aCallbackFunction, void* someUserData) {
	this->isStopped = false;
	this->elapsedTimeGreaterThanDurationCallback = aCallbackFunction;
	this->userData = someUserData;
}


void VisualTimeline::resume() {
	UInt32 elapsedMillisecs = this->elapsedMilliseconds;
	VisualTiming::resetTimestamp(*(this->durationIdentifier));
	this->currentValue = this->visualInterpolation->getStartValue();
	if (this->movingDirection == kForward) {
		this->offsetMilliseconds = elapsedMillisecs;
	} else if (this->movingDirection == kBackward) {
		this->offsetMilliseconds = this->durationInMilliseconds - elapsedMillisecs;
	}
	this->isStopped = false;
}


void VisualTimeline::setInterpolationType(InterpolationType anInterpolationType) {
	this->visualInterpolation->setType(anInterpolationType);
}


void VisualTimeline::setMovingDirection(MovingDirection direction) {
	this->movingDirection = direction;
}


MovingDirection VisualTimeline::getMovingDirection() {
	return this->movingDirection;
}


void VisualTimeline::setDebugMode(bool requestedDebugMode) {
	this->debugMode = requestedDebugMode;
}


void VisualTimeline::elapsedTimeDidExceedDuration() {
	if (this->elapsedTimeGreaterThanDurationCallback != NULL) {
		this->elapsedTimeGreaterThanDurationCallback(this->userData);
	}
}
