/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationSequence.cpp
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

#include "VisualAnimationSequence.h"
#include "VisualAnimationQueue.h"
#include "VisualItemIdentifier.h"
#include "VisualAsset.h"


using namespace VizKit;


VisualAnimationSequence::VisualAnimationSequence() {
	currAnimationIndex = -1;
	parentCollectionIdentifier = NULL;
	enclosingAsset = NULL;
	performAnyAdditionalActionCallback = NULL;
	performAnyAdditionalActionCallbackUserData = NULL;
	willDieCallback = NULL;
	willDieCallbackUserData = NULL;
}


VisualAnimationSequence::~VisualAnimationSequence() {
	for (AnimationVectorIterator it = animationVector.begin(); it != animationVector.end(); it++) {
		delete (*it);
	}
	animationVector.clear();
	if (parentCollectionIdentifier != NULL) {
		delete parentCollectionIdentifier;
	}
}


VisualAnimationSequence::VisualAnimationSequence(const VisualAnimationSequence& other) : VisualAnimationComponent(other) {
	copy(other);
}


VisualAnimationSequence& VisualAnimationSequence::operator=(const VisualAnimationSequence& other) {

	if (this == &other) return *this;
	
	VisualObject::operator=(other);

	if (this->parentCollectionIdentifier != NULL) {
		delete this->parentCollectionIdentifier;
	}
	
	this->copy(other);

	return *this;
}


VisualAnimationComponent* VisualAnimationSequence::clone(void) const {
	return new VisualAnimationSequence(*this);
}


void VisualAnimationSequence::copy(const VisualAnimationSequence& other) {
	// deep copy
	for (AnimationVectorConstIterator it = other.animationVector.begin(); it != other.animationVector.end(); it++) {
		this->animationVector.push_back((*it)->clone());
	}
	this->currAnimationIndex = other.currAnimationIndex;
	if (other.parentCollectionIdentifier != NULL) {
		this->parentCollectionIdentifier = new VisualItemIdentifier(*(other.parentCollectionIdentifier));
	} else {
		this->parentCollectionIdentifier = NULL;
	}
	this->enclosingAsset = other.enclosingAsset;
	this->performAnyAdditionalActionCallback = other.performAnyAdditionalActionCallback;
	this->performAnyAdditionalActionCallbackUserData = other.performAnyAdditionalActionCallbackUserData;
	this->willDieCallback = other.willDieCallback;
	this->willDieCallbackUserData = other.willDieCallbackUserData;
}


void VisualAnimationSequence::add(VisualAnimationComponent& anAnimation) {
	anAnimation.setIdentifierOfParentCollection(this->VisualObject::getIdentifier());
	this->animationVector.push_back(anAnimation.clone());
}


void VisualAnimationSequence::animate() {
	if (this->currAnimationIndex != -1) {
		if (this->animationVector[this->currAnimationIndex]->isReadyToDie() == false) {
			this->animationVector[this->currAnimationIndex]->animate();
		}
	}
	if (this->performAnyAdditionalActionCallback != NULL) {
		this->performAnyAdditionalActionCallback(this, this->performAnyAdditionalActionCallbackUserData);
	}
}


void VisualAnimationSequence::update(void) {
	if (this->currAnimationIndex != -1) {
		if (this->animationVector[this->currAnimationIndex]->isReadyToDie() == false) {
			this->animationVector[this->currAnimationIndex]->update();
		}
	}
}


bool VisualAnimationSequence::isRunning() const {
	bool returnValue = false;
	for (AnimationVectorConstIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		if ((*it)->isRunning() == true) {
			returnValue = true;
		}
	}
	return returnValue;
}


bool VisualAnimationSequence::isReadyToDie() const {
	bool returnValue = true;
	for (AnimationVectorConstIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		if ((*it)->isReadyToDie() == false) {
			returnValue = false;
		}
	}
	return returnValue;
}


void VisualAnimationSequence::start() {
	if (this->animationVector.size() > 0) {
		this->currAnimationIndex++;
		this->animationVector[this->currAnimationIndex]->start();
	}
}


void VisualAnimationSequence::stop() {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		(*it)->stop();
	}
}


AnimatedProperty VisualAnimationSequence::getAnimatedProperty(void) const {
	AnimatedProperty animatedProperty = kUndefinedAnimatedProperty;
	if (this->animationVector.size() == 1) {
		animatedProperty = this->animationVector[0]->getAnimatedProperty();
	}
	return animatedProperty;
}


void VisualAnimationSequence::preparePriorToAddingToAsset(VisualAsset& visualAsset) {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		(*it)->preparePriorToAddingToAsset(visualAsset);
	}
	this->enclosingAsset = &visualAsset;
}


const VisualAsset* const VisualAnimationSequence::getEnclosingAsset(void) const {
	return this->enclosingAsset;
}


void VisualAnimationSequence::setIdentifierOfParentCollection(const VisualItemIdentifier& aCollectionIdentifier) {
	if (this->parentCollectionIdentifier != NULL) {
		delete this->parentCollectionIdentifier;
	}
	this->parentCollectionIdentifier = new VisualItemIdentifier(aCollectionIdentifier);
}


void VisualAnimationSequence::handleOneCollectionItemAnimationDied() {
	if (this->currAnimationIndex < (sint32)((this->animationVector.size() - 1))) {
		this->currAnimationIndex++;
		this->animationVector[this->currAnimationIndex]->start();
	} else {
		if (this->parentCollectionIdentifier != NULL) {
			VisualAnimationComponent* animationCollection = VisualAnimationQueue::getVisualAnimation(*(this->parentCollectionIdentifier));
			animationCollection->handleOneCollectionItemAnimationDied();
		}
	}
}


void VisualAnimationSequence::setCallbackToPerformAnyAdditionalAction(VisualAnimationPerformAnyAdditionalActionCallback theCallback, void* userData) {
	this->performAnyAdditionalActionCallback = theCallback;
	this->performAnyAdditionalActionCallbackUserData = userData;
}


void VisualAnimationSequence::setCallbackToNotifyBeforeDeath(VisualAnimationWillDieCallback theCallback, void* userData) {
	this->willDieCallback = theCallback;
	this->willDieCallbackUserData = userData;
}


void VisualAnimationSequence::notifyBeforeDeath() {
	if (this->willDieCallback != NULL) {
		this->willDieCallback(this, this->willDieCallbackUserData);
	}
}


VisualAnimationComponent* VisualAnimationSequence::getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier) {
	if (VisualObject::getIdentifier() == anAnimationIdentifier) {
		return this;
	}
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		VisualAnimationComponent* animation = (*it)->getVisualAnimation(anAnimationIdentifier);
		if (animation != NULL) {
			return animation;
		}
	}
	return NULL;
}
