/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAnimation.cpp
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

#include "VisualAnimation.h"
#include "VisualTiming.h"
#include "VisualTimeline.h"
#include "VisualErrorHandling.h"
#include "VisualStageControl.h"
#include "VisualGraphics.h"



using namespace VizKit;


const UInt32 VisualAnimation::maxNumberOfDebugHistoryEntries = 700;


VisualAnimation::VisualAnimation(AnimatedProperty anAnimatedProperty) {
	animationIdentifier = new VisualItemIdentifier;
	theTimeline = new VisualTimeline;
	animatedProperty = anAnimatedProperty;
	remainingNumberOfRepeats = 0;
	currentValue = 0.0;
	currentValueIsSet = false;
	animateCallbackFunctionPtr = NULL;
	userData = NULL;
	startDelayInMilliseconds = 0;
	status = kIsNotRunning;
	debugMode = false;
	minimumClampValue = 0.0;
	minimumClampValueIsSet = false;
	maximumClampValue = 0.0;
	maximumClampValueIsSet = false;
	currDebugHistoryEntry = 0;
	debugHistoryIsInitialized = false;
}


VisualAnimation::~VisualAnimation() {
	delete theTimeline;
	delete animationIdentifier;
	debugHistory.clear();
}


VisualAnimation::VisualAnimation(const VisualAnimation& other) {
	copy(other);
}


VisualAnimation& VisualAnimation::operator=(const VisualAnimation& other) {
	if (this != &other) {
		delete this->theTimeline;
		delete this->animationIdentifier;
		this->copy(other);
	}
	return *this;
}


void VisualAnimation::copy(const VisualAnimation& other) {
	this->theTimeline = new VisualTimeline(*other.theTimeline);
	this->animationIdentifier = new VisualItemIdentifier(*other.animationIdentifier);
	this->animatedProperty = other.animatedProperty;
	this->remainingNumberOfRepeats = other.remainingNumberOfRepeats;
	this->currentValue = other.currentValue;
	this->currentValueIsSet = other.currentValueIsSet;
	this->animateCallbackFunctionPtr = other.animateCallbackFunctionPtr;
	this->userData = other.userData;
	this->startDelayInMilliseconds = other.startDelayInMilliseconds;
	this->status = other.status;
	this->debugMode = other.debugMode;
	this->minimumClampValue = other.minimumClampValue;
	this->minimumClampValueIsSet = other.minimumClampValueIsSet;
	this->maximumClampValue = other.maximumClampValue;
	this->maximumClampValueIsSet = other.maximumClampValueIsSet;
	this->currDebugHistoryEntry = other.currDebugHistoryEntry;
	this->debugHistory = other.debugHistory;
	this->debugHistoryIsInitialized = other.debugHistoryIsInitialized;
}


void VisualAnimation::setAnimateCallbackFunctionPtr(VisualAnimationCallback theCallback, void* someUserData) {
	this->animateCallbackFunctionPtr = theCallback;
	this->userData = someUserData;
}


OSStatus VisualAnimation::callAnimateCallbackFunction() {
	OSStatus osStatus = noErr;
	double currVal = this->theTimeline->getCurrentValue();
	
	if (this->debugMode == true) {
		this->debugHistory[this->currDebugHistoryEntry] = currVal;
		VisualGraphics::drawHistoryDiagram(this->debugHistory, this->currDebugHistoryEntry, 0.0, 1.0);
		if ((this->currDebugHistoryEntry + 1) < this->maxNumberOfDebugHistoryEntries) {
			this->currDebugHistoryEntry++;
		} else {
			this->currDebugHistoryEntry = 0;
		}
	}
	if ((this->minimumClampValueIsSet == true) && (currVal < this->minimumClampValue)) {
		osStatus = this->animateCallbackFunctionPtr(this->minimumClampValue, this->userData);
		this->status = kIsDead;
	} else if ((this->maximumClampValueIsSet == true) && (currVal > this->maximumClampValue)) {
		osStatus = this->animateCallbackFunctionPtr(this->maximumClampValue, this->userData);
		this->status = kIsDead;
	} else {
		osStatus = this->animateCallbackFunctionPtr(currVal, this->userData);
	}
	return osStatus;
}


VisualTimeline* VisualAnimation::getTimeline(void) {
	return this->theTimeline;
}


void VisualAnimation::start() {
	if (this->startDelayInMilliseconds > 0) {
		VisualTiming::resetTimestamp(*(this->animationIdentifier));
		// start() is called later by isRunning()
		return;
	}
	if (this->remainingNumberOfRepeats == 0) {
		this->remainingNumberOfRepeats = 1;
	}
	this->theTimeline->reset();
	this->theTimeline->start(VisualAnimation::durationIsExceeded, this);
	if (this->currentValueIsSet == true) {
		// if we have clamp values, they may never be reached if the current value beyond (and moving direction does not fit)
		// (e.g. if we call fadeOut with clampValue and continueCurrentValue when fadeIn just started)
		// the requested end value should always be aimed
		if ((this->minimumClampValueIsSet == true) && (this->currentValue < this->minimumClampValue)) {
			if ((this->theTimeline->getMovingDirection() == kForward) && (theTimeline->getStartValue() > theTimeline->getEndValue())) {
				this->theTimeline->setMovingDirection(kBackward);
			} else if ((this->theTimeline->getMovingDirection() == kBackward) && (theTimeline->getEndValue() > theTimeline->getStartValue())) {
				this->theTimeline->setMovingDirection(kForward);
			}
			this->maximumClampValue = this->minimumClampValue;
			this->maximumClampValueIsSet = true;
			this->minimumClampValue = 0.0;
			this->minimumClampValueIsSet = false;
		}
		if ((this->maximumClampValueIsSet == true) && (this->currentValue > this->maximumClampValue)) {
			if ((this->theTimeline->getMovingDirection() == kForward) && (theTimeline->getStartValue() < theTimeline->getEndValue())) {
				this->theTimeline->setMovingDirection(kBackward);
			} else if ((this->theTimeline->getMovingDirection() == kBackward) && (theTimeline->getEndValue() < theTimeline->getStartValue())) {
				this->theTimeline->setMovingDirection(kForward);
			}
			this->minimumClampValue = this->maximumClampValue;
			this->minimumClampValueIsSet = true;
			this->maximumClampValue = 0.0;
			this->maximumClampValueIsSet = false;
		}

		this->theTimeline->setCurrentValue(this->currentValue);

	}
	this->status = kIsRunning;
}


void VisualAnimation::stop() {
	this->status = kIsDead;
}


void VisualAnimation::setDurationInMilliseconds(UInt32 numberOfMilliseconds) {
	theTimeline->setDurationInMilliseconds(numberOfMilliseconds);
}


UInt32 VisualAnimation::getDurationInMilliseconds() {
	return theTimeline->getDurationInMilliseconds();
}


void VisualAnimation::setCurrentValue(double aCurrentValue) {
	this->currentValue = aCurrentValue;
	this->currentValueIsSet = true;
}


void VisualAnimation::setStartValue(double aStartValue) {
	theTimeline->setStartValue(aStartValue);
}


double VisualAnimation::getStartValue() {
	return theTimeline->getStartValue();
}


void VisualAnimation::setEndValue(double anEndValue) {
	theTimeline->setEndValue(anEndValue);
}


double VisualAnimation::getEndValue() {
	return theTimeline->getEndValue();
}


void VisualAnimation::setMinClampValue(double aMinClampValue) {
	this->minimumClampValue = aMinClampValue;
	this->minimumClampValueIsSet = true;
}


void VisualAnimation::setMaxClampValue(double aMaxClampValue) {
	this->maximumClampValue = aMaxClampValue;
	this->maximumClampValueIsSet = true;
}


void VisualAnimation::setLoopMode(LoopMode aLoopMode, SInt32 requestedNumberOfLoops) {
	if ((aLoopMode & kMirroredLoop) == kMirroredLoop) {
		theTimeline->setRepeatMode(kRepeatMirrored);
	} else if ((aLoopMode & kLoop) == kLoop) {
		theTimeline->setRepeatMode(kRepeatFromStart);
	}
	this->remainingNumberOfRepeats = requestedNumberOfLoops;
}


UInt32 VisualAnimation::getStartDelayInMilliseconds() {
	return this->startDelayInMilliseconds;
}


void VisualAnimation::setStartDelayInMilliseconds(UInt32 aStartDelayInMilliseconds) {
	this->startDelayInMilliseconds = aStartDelayInMilliseconds;
}


const AnimatedProperty VisualAnimation::getAnimatedProperty() const {
	return this->animatedProperty;
}


const bool VisualAnimation::isRunning(void) {
	if (this->startDelayInMilliseconds > 0) {
		UInt32 elapsedMillisecondsOfDelay = VisualTiming::getElapsedMilliSecsSinceReset(*(this->animationIdentifier));
		if (elapsedMillisecondsOfDelay > this->startDelayInMilliseconds) {
			this->startDelayInMilliseconds = 0;
			this->start();
		} else {
			return false;
		}
	}
	if (this->status == kIsRunning) {
		return true;
	} else {
		return false;
	}
}


const bool VisualAnimation::isDead(void) const {
	if (this->status == kIsDead) {
		return true;
	} else {
		return false;
	}
}


const VisualItemIdentifier* const VisualAnimation::getIdentifier() const {
	return this->animationIdentifier;
}


void VisualAnimation::setDebugMode(bool requestedDebugMode) {
	this->debugMode = requestedDebugMode;
	this->theTimeline->setDebugMode(requestedDebugMode);
	if (this->debugHistoryIsInitialized == false) {
		for (size_t i = 0; i < this->maxNumberOfDebugHistoryEntries; i++) {
			debugHistory.push_back(0.0);
		}
		this->debugHistoryIsInitialized = true;
	}
}


void VisualAnimation::durationIsExceeded(void* visualAnimation) {
	reinterpret_cast<VisualAnimation*>(visualAnimation)->decrementRemainingNumberOfRepeats();
}


void VisualAnimation::decrementRemainingNumberOfRepeats() {

	if (this->remainingNumberOfRepeats == kInfiniteRepetition) {
		return;
	}
	
	if (this->remainingNumberOfRepeats > 1) {
		this->remainingNumberOfRepeats--;
	} else {
		this->stop();
	}
	
}
