/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimation.cpp
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

#include "VisualAnimation.h"
#include "VisualAsset.h"
#include "VisualItemIdentifier.h"
#include "VisualTiming.h"
#include "VisualTimeline.h"
#include "VisualErrorHandling.h"
#include "VisualAnimationQueue.h"
#include "VisualGraphics.h"
#include "VisualCamera.h"


using namespace VizKit;


const uint32 VisualAnimation::maxNumberOfDebugHistoryEntries = 700;


VisualAnimation::VisualAnimation(AnimatedProperty theAnimatedProperty, bool aDebugMode) {
	if (aDebugMode == true) {
		writeLog("VisualAnimation::VisualAnimation");
	}
	theTimeline = new VisualTimeline(aDebugMode);
	animatedProperty = theAnimatedProperty;
	remainingNumberOfRepeats = 0;
	animateCallback = NULL;
	animationUserData = NULL;
	performAnyAdditionalActionCallback = NULL;
	performAnyAdditionalActionCallbackUserData = NULL;
	startDelayInMilliseconds = 0;
	status = kIsNotRunning;
	debugMode = aDebugMode;
	currDebugHistoryEntry = 0;
	collectionIdentifier = NULL;
	enclosingAsset = NULL;
	doStartAnimationWithCurrentPropertyValue = true;
	startValue = 0.0;
	stopValue = 1.0;
	speed = theTimeline->getDistance() / theTimeline->getDurationInMilliseconds();
	setSpeed(speed);
	if (aDebugMode == true) {
		for (size_t i = 0; i < this->maxNumberOfDebugHistoryEntries; i++) {
			debugHistory.push_back(0.0);
		}
		debugHistoryIsInitialized = true;
	} else {
		debugHistoryIsInitialized = false;
	}
	willDieCallback = NULL;
	willDieCallbackUserData = NULL;
}


VisualAnimation::~VisualAnimation() {
	delete theTimeline;
	debugHistory.clear();
	if (collectionIdentifier != NULL) {
		delete collectionIdentifier;
	}
}


VisualAnimation::VisualAnimation(const VisualAnimation& other) : VisualAnimationComponent(other) {
	copy(other);
}


VisualAnimation& VisualAnimation::operator=(const VisualAnimation& other) {

	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	delete this->theTimeline;
	if (this->collectionIdentifier != NULL) {
		delete this->collectionIdentifier;
	}

	this->copy(other);

	return *this;
}


VisualAnimationComponent* VisualAnimation::clone(void) const {
	return new VisualAnimation(*this);
}


void VisualAnimation::copy(const VisualAnimation& other) {
	// deep copy
	if (other.debugMode == true) {
		writeLog("VisualAnimation::copy");
	}
	this->theTimeline = new VisualTimeline(*other.theTimeline);
	this->animatedProperty = other.animatedProperty;
	this->remainingNumberOfRepeats = other.remainingNumberOfRepeats;
	this->animateCallback = other.animateCallback;
	this->animationUserData = other.animationUserData;
	this->performAnyAdditionalActionCallback = other.performAnyAdditionalActionCallback;
	this->performAnyAdditionalActionCallbackUserData = other.performAnyAdditionalActionCallbackUserData;
	this->startDelayInMilliseconds = other.startDelayInMilliseconds;
	this->status = other.status;
	this->debugMode = other.debugMode;
	this->currDebugHistoryEntry = other.currDebugHistoryEntry;
	this->debugHistory = other.debugHistory;
	this->debugHistoryIsInitialized = other.debugHistoryIsInitialized;
	this->doStartAnimationWithCurrentPropertyValue = other.doStartAnimationWithCurrentPropertyValue;
	this->startValue = other.startValue;
	this->stopValue = other.stopValue;
	if (other.collectionIdentifier != NULL) {
		this->collectionIdentifier = new VisualItemIdentifier(*(other.collectionIdentifier));
	} else {
		this->collectionIdentifier = NULL;
	}
	this->enclosingAsset = other.enclosingAsset;
	this->willDieCallback = other.willDieCallback;
	this->willDieCallbackUserData = other.willDieCallbackUserData;
}


void VisualAnimation::setAnimateCallbackFunctionPtr(VisualAnimationAnimateCallback theCallback, void* someUserData) {
	this->animateCallback = theCallback;
	this->animationUserData = someUserData;
}


void VisualAnimation::setIdentifierOfParentCollection(const VisualItemIdentifier& aCollectionIdentifier) {
	if (this->collectionIdentifier != NULL) {
		delete this->collectionIdentifier;
	}
	this->collectionIdentifier = new VisualItemIdentifier(aCollectionIdentifier);
}


void VisualAnimation::handleOneCollectionItemAnimationDied() {
	char errLog[256];
	sprintf(errLog, "Err: VisualAnimation is unable to handle collection animations in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
	writeLog(errLog);
	return;
}


VisualAnimationComponent* VisualAnimation::getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier) {
	if (VisualObject::getIdentifier() == anAnimationIdentifier) {
		return this;
	} else {
		return NULL;
	}
}


void VisualAnimation::animate() {

	double currVal = this->theTimeline->getCurrentValue();
	
	if (this->debugMode == true) {
		this->debugHistory[this->currDebugHistoryEntry] = currVal;
		VisualCamera* aCamera = VisualCamera::createDefaultCamera();
		VisualGraphics::drawHistoryDiagram(this->debugHistory, this->currDebugHistoryEntry, 0.0, 1.0, *aCamera);
		delete aCamera;
		if ((this->currDebugHistoryEntry + 1) < this->maxNumberOfDebugHistoryEntries) {
			this->currDebugHistoryEntry++;
		} else {
			this->currDebugHistoryEntry = 0;
		}
		char logStr[128];
		sprintf(logStr, "VisualAnimation::animate currVal: %f", currVal);
		writeLog(logStr);
	}
	this->animateCallback(currVal, this->animationUserData);
	if (this->performAnyAdditionalActionCallback != NULL) {
		this->performAnyAdditionalActionCallback(this, this->performAnyAdditionalActionCallbackUserData);
	}
}


void VisualAnimation::start() {
	if (this->startDelayInMilliseconds > 0) {
		return;
	}
	if (this->remainingNumberOfRepeats == 0) {
		this->remainingNumberOfRepeats = 1;
	}
	this->theTimeline->start();
	this->status = kIsRunning;
	this->animate();
}


void VisualAnimation::stop() {
	this->status = kIsReadyToDie;
	if (this->collectionIdentifier != NULL) {
		VisualAnimationComponent* animationCollection = VisualAnimationQueue::getVisualAnimation(*(this->collectionIdentifier));
		if (animationCollection != NULL) {
			animationCollection->handleOneCollectionItemAnimationDied();
		} else {
			char errLog[256];
			sprintf(errLog, "CollectionAnimation not found in in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
		}

	}
}


void VisualAnimation::setDurationInMilliseconds(uint32 numberOfMilliseconds) {
	this->theTimeline->setDurationInMilliseconds(numberOfMilliseconds);
	this->speed = this->calcSpeed(this->theTimeline->getDistance(), this->theTimeline->getDurationInMilliseconds());
	this->durationSpeedConstraint = kDurationBound;
}


uint32 VisualAnimation::getDurationInMilliseconds() const {
	return this->theTimeline->getDurationInMilliseconds();
}


void VisualAnimation::setCurrentValue(double aCurrentValue) {
	AnimationSpeed prevSpeed = this->getSpeed();
	bool adjustedDistanceAndDuration = this->theTimeline->setCurrentValue(aCurrentValue);
	if (adjustedDistanceAndDuration == true) {
		if (this->durationSpeedConstraint == kSpeedBound) {
			this->setSpeed(prevSpeed);
		}
	}
	// aCurrentValue might be out of range of this->startValue and this->stopValue
	double maxValue = (this->startValue > this->stopValue) ? this->startValue : this->stopValue;
	double minValue = (this->startValue < this->stopValue) ? this->startValue : this->stopValue;
	if ((aCurrentValue < minValue) || (aCurrentValue > maxValue)) {
		this->startValue = aCurrentValue;
		this->doStartAnimationWithCurrentPropertyValue = true;
	}
	if (this->doStartAnimationWithCurrentPropertyValue == true) {
		// movingDirection might need to change (we want to arrive at the stop position by way of the shortest distance)
		MovingDirection movingDirection = this->theTimeline->getMovingDirection();
		double theCurrentValue = this->theTimeline->getCurrentValue();
		if ((movingDirection == kAscending) && theCurrentValue > this->stopValue) {
			this->theTimeline->toggleMovingDirection();
		} else if ((movingDirection == kDescending) && theCurrentValue < this->stopValue) {
			this->theTimeline->toggleMovingDirection();
		}
	}
	
}


double VisualAnimation::getCurrentValue() const {
	return this->theTimeline->getCurrentValue();
}


void VisualAnimation::setStartValue(double aStartValue, bool startAnimationWithCurrentPropertyValue) {
	AnimationSpeed prevSpeed = this->getSpeed();
	this->theTimeline->setStartValue(aStartValue);
	this->startValue = aStartValue;
	this->doStartAnimationWithCurrentPropertyValue = startAnimationWithCurrentPropertyValue;
	if (this->durationSpeedConstraint == kSpeedBound) {
		this->setSpeed(prevSpeed);
	}
}


void VisualAnimation::setStartValue(const VisualStagePosition& aPosition, bool startAnimationWithCurrentPropertyValue) {
	AnimationSpeed prevSpeed = this->getSpeed();
	//this->theTimeline->setStartValue(aStartValue);
	//this->startValue = aStartValue;
	this->startValueVisualStagePosition = aPosition;
	this->doStartAnimationWithCurrentPropertyValue = startAnimationWithCurrentPropertyValue;
	if (this->durationSpeedConstraint == kSpeedBound) {
		this->setSpeed(prevSpeed);
	}
}


double VisualAnimation::getStartValue() const {
	return this->startValue;
}


void VisualAnimation::setStopValue(double aStopValue) {
	AnimationSpeed prevSpeed = this->getSpeed();
	theTimeline->setStopValue(aStopValue);
	this->stopValue = aStopValue;
	if (this->durationSpeedConstraint == kSpeedBound) {
		this->setSpeed(prevSpeed);
	}
}


void VisualAnimation::setStopValue(const VisualStagePosition& aPosition) {
	AnimationSpeed prevSpeed = this->getSpeed();
	//theTimeline->setStopValue(aStopValue);
	//this->stopValue = aStopValue;
	this->stopValueVisualStagePosition = aPosition;
	if (this->durationSpeedConstraint == kSpeedBound) {
		this->setSpeed(prevSpeed);
	}
}


double VisualAnimation::getStopValue() const {
	return this->stopValue;
}


void VisualAnimation::setLoopMode(LoopMode aLoopMode, sint32 requestedNumberOfLoops) {
	if ((aLoopMode & kMirroredLoop) == kMirroredLoop) {
		theTimeline->setRepeatMode(kRepeatMirrored);
	} else if ((aLoopMode & kLoop) == kLoop) {
		theTimeline->setRepeatMode(kRepeatFromStart);
	}
	this->remainingNumberOfRepeats = requestedNumberOfLoops;
}


void VisualAnimation::setInterpolationType(InterpolationType anInterpolationType) {
	this->theTimeline->setInterpolationType(anInterpolationType);
}


uint32 VisualAnimation::getStartDelayInMilliseconds() const {
	return this->startDelayInMilliseconds;
}


void VisualAnimation::setStartDelayInMilliseconds(uint32 aStartDelayInMilliseconds) {
	this->startDelayInMilliseconds = aStartDelayInMilliseconds;
}


void VisualAnimation::setCallbackToPerformAnyAdditionalAction(VisualAnimationPerformAnyAdditionalActionCallback theCallback, void* userData) {
	this->performAnyAdditionalActionCallback = theCallback;
	this->performAnyAdditionalActionCallbackUserData = userData;
}


void VisualAnimation::setCallbackToNotifyBeforeDeath(VisualAnimationWillDieCallback theCallback, void* userData) {
	this->willDieCallback = theCallback;
	this->willDieCallbackUserData = userData;
}


void VisualAnimation::notifyBeforeDeath() {
	if (this->willDieCallback != NULL) {
		this->willDieCallback(this, this->willDieCallbackUserData);
	}
}


AnimatedProperty VisualAnimation::getAnimatedProperty() const {
	return this->animatedProperty;
}


MovingDirection VisualAnimation::getMovingDirection(void) const {
	return this->theTimeline->getMovingDirection();
}


RepeatMode VisualAnimation::getRepeatMode(void) const {
	return this->theTimeline->getRepeatMode();
}


void VisualAnimation::update() {
	if (this->status == kIsNotRunning) {
		if (this->startDelayInMilliseconds > 0) {
			uint32 elapsedMillisecondsOfDelay = VisualTiming::getElapsedMilliSecsSinceReset(VisualObject::getIdentifier());
			if (elapsedMillisecondsOfDelay > this->startDelayInMilliseconds) {
				this->startDelayInMilliseconds = 0;
				VisualTiming::resetTimestamp(VisualObject::getIdentifier());
				this->start();
			}
		}
	} else if (this->status == kIsRunning) {
		MovingDirection movingDirection = this->theTimeline->getMovingDirection();
		TimelineUpdateResult result = this->theTimeline->update();
		bool animateFinalValue = false;
		double endValue = 0.0;
		if (movingDirection == kAscending) {
			if (this->startValue < this->stopValue) {
				endValue = this->stopValue;
			} else {
				endValue = this->startValue;
			}
		} else if (movingDirection == kDescending) {
			if (this->startValue < this->stopValue) {
				endValue = this->startValue;
			} else {
				endValue = this->stopValue;
			}
		}
		if (result != kTimelineUpdateOK) {
			if (this->debugMode == true) {
				char resultStr[128];
				VisualTimeline::convertTimelineUpdateResultToString(result, resultStr);
				char logStr[128];
				sprintf(logStr, "VisualAnimation::update: TimelineUpdateResult == %s", resultStr);
				writeLog(logStr);
			}
			animateFinalValue = true;
		} else {
			if (movingDirection == kAscending) {
				if (this->theTimeline->getCurrentValue() >= endValue) {
					if (this->debugMode == true) {
						writeLog("VisualAnimation::update: this->theTimeline->getCurrentValue() >= endValue");
					}
					animateFinalValue = true;
				}
			} else if (movingDirection == kDescending) {
				if (this->theTimeline->getCurrentValue() <= endValue) {
					if (this->debugMode == true) {
						writeLog("VisualAnimation::update: this->theTimeline->getCurrentValue() <= endValue");
					}
					animateFinalValue = true;
				}
			}
		}
		if (animateFinalValue == true) {
			this->animateCallback(endValue, this->animationUserData);
			this->durationIsExceeded();
		}
	}
}


bool VisualAnimation::isRunning() const {
	if (this->status == kIsRunning) {
		return true;
	} else {
		return false;
	}
}


bool VisualAnimation::isReadyToDie(void) const {
	if (this->status == kIsReadyToDie) {
		return true;
	} else {
		return false;
	}
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


void VisualAnimation::durationIsExceeded() {
	if (this->remainingNumberOfRepeats != kInfiniteRepetition) {
		this->decrementRemainingNumberOfRepeats();
	}
}


void VisualAnimation::decrementRemainingNumberOfRepeats() {	
	if (this->remainingNumberOfRepeats > 1) {
		this->remainingNumberOfRepeats--;
	} else {
		this->stop();
	}
}


void VisualAnimation::preparePriorToAddingToAsset(VisualAsset& visualAsset) {

	VisualAnimationQueue::removeVisualAnimationsWithOwnerIdentifier(visualAsset.getIdentifier(), this->animatedProperty);
	
	if (this->doStartAnimationWithCurrentPropertyValue == true) {
		this->setCurrentTimelineValueToCurrentPropertyValue(visualAsset, this->animatedProperty);
	}

	if (this->animatedProperty == kAnimatedOpacity) {
		this->setAnimateCallbackFunctionPtr(VisualAsset::animateOpacity, (void*)&visualAsset);
	} else if (this->animatedProperty == kAnimatedRotation) {
		this->setAnimateCallbackFunctionPtr(VisualAsset::animateRotation, (void*)&visualAsset);
	} else if (this->animatedProperty == kAnimatedSize) {
		this->setAnimateCallbackFunctionPtr(VisualAsset::animateScaleFactor, (void*)&visualAsset);
	} else if (this->animatedProperty == kAnimatedLocation) {
		visualAsset.setStartValueVisualStagePosition(this->startValueVisualStagePosition);
		visualAsset.setStopValueVisualStagePosition(this->stopValueVisualStagePosition);
		this->setAnimateCallbackFunctionPtr(VisualAsset::animateLocation, (void*)&visualAsset);
	}
	
	this->enclosingAsset = &visualAsset;

}


const VisualAsset* const VisualAnimation::getEnclosingAsset(void) const {
	return this->enclosingAsset;
}


void VisualAnimation::setSpeed(AnimationSpeed animationSpeed) {
	if (this->debugMode == true) {
		char logStr[128];
		sprintf(logStr, "VisualAnimation::setSpeed: %f", animationSpeed);
		writeLog(logStr);
	}
	//double timelineStartValue = 0.0;
	//MovingDirection movingDirection = this->theTimeline->getMovingDirection();
	double distance = this->theTimeline->getDistance();
	uint32 newDuration = (uint32)(distance / (double)animationSpeed);
	if (this->debugMode == true) {
		char logStr[128];
		sprintf(logStr, "VisualAnimation::setSpeed: newDuration: %d (distance: %f)", newDuration, distance);
		writeLog(logStr);
	}
	this->theTimeline->setDurationInMilliseconds(newDuration);
	/*
	if (movingDirection == kAscending) {
		timelineStartValue = theTimeline->getMaxValue() - (this->theTimeline->getDurationInMilliseconds() * animationSpeed);
	} else if (movingDirection == kDescending) {
		timelineStartValue = theTimeline->getMinValue() + (this->theTimeline->getDurationInMilliseconds() * animationSpeed);
	}
	theTimeline->setStartValue(timelineStartValue);
	*/
	this->speed = animationSpeed;
	this->durationSpeedConstraint = kSpeedBound;
}


AnimationSpeed VisualAnimation::getSpeed(void) const {
	return this->speed;
}


AnimationSpeed VisualAnimation::calcSpeed(double aStartValue, double aStopValue, uint32 aDurationInMilliseconds) {
	double aDistance = 0.0;
	if (aStartValue < aStopValue) {
		aDistance = aStopValue - aStartValue;
	} else {
		aDistance = aStartValue - aStopValue;
	}
	return VisualAnimation::calcSpeed(aDistance, aDurationInMilliseconds);
}


AnimationSpeed VisualAnimation::calcSpeed(double aDistance, uint32 aDurationInMilliseconds) {
	return (aDistance / static_cast<double>(aDurationInMilliseconds));
}


uint32 VisualAnimation::calcDurationInMilliseconds(double aDistance, AnimationSpeed aSpeed) {
	return static_cast<uint32>(aDistance / static_cast<double>(aSpeed));
}


void VisualAnimation::setCurrentTimelineValueToCurrentPropertyValue(const VisualAsset& visualAsset, AnimatedProperty anAnimatedProperty) {
	AnimationSpeed prevSpeed = this->getSpeed();
	bool adjustedDistanceAndDuration = false;
	if (anAnimatedProperty == kAnimatedLocation) {
		adjustedDistanceAndDuration = this->theTimeline->setCurrentValue(visualAsset.getCurrentAnimationValueForAnimatedLocation(this->startValueVisualStagePosition, this->stopValueVisualStagePosition));
	} else {
		adjustedDistanceAndDuration = this->theTimeline->setCurrentValue(visualAsset.getCurrentAnimationValueOfProperty(anAnimatedProperty));
	}
	// movingDirection might need to change (we want to arrive at the stop position by way of the shortest distance)
	MovingDirection movingDirection = this->theTimeline->getMovingDirection();
	double theCurrentValue = this->theTimeline->getCurrentValue();
	if ((movingDirection == kAscending) && theCurrentValue > this->stopValue) {
		this->startValue = theCurrentValue;
		this->theTimeline->toggleMovingDirection();
	} else if ((movingDirection == kDescending) && theCurrentValue < this->stopValue) {
		this->startValue = theCurrentValue;
		this->theTimeline->toggleMovingDirection();
	}
	if (adjustedDistanceAndDuration == true) {
		if (this->durationSpeedConstraint == kSpeedBound) {
			this->setSpeed(prevSpeed);
		}
	}
}
