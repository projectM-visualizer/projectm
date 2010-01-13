/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationCluster.cpp
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

#include "VisualAnimationCluster.h"
#include "VisualAnimationQueue.h"
#include "VisualItemIdentifier.h"
#include "VisualAsset.h"


using namespace VizKit;


VisualAnimationCluster::VisualAnimationCluster() {
	parentCollectionIdentifier = NULL;
	enclosingAsset = NULL;
	performAnyAdditionalActionCallback = NULL;
	performAnyAdditionalActionCallbackUserData = NULL;
	willDieCallback = NULL;
	willDieCallbackUserData = NULL;
}


VisualAnimationCluster::~VisualAnimationCluster() {
	for (AnimationVectorIterator it = animationVector.begin(); it != animationVector.end(); it++) {
		delete (*it);
	}
	animationVector.clear();
	if (parentCollectionIdentifier != NULL) {
		delete parentCollectionIdentifier;
	}
}


VisualAnimationCluster::VisualAnimationCluster(const VisualAnimationCluster& other) : VisualAnimationComponent(other) {
	copy(other);
}


VisualAnimationCluster& VisualAnimationCluster::operator=(const VisualAnimationCluster& other) {

	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	if (this->parentCollectionIdentifier != NULL) {
		delete this->parentCollectionIdentifier;
	}
	
	this->copy(other);

	return *this;
}


VisualAnimationComponent* VisualAnimationCluster::clone(void) const {
	return new VisualAnimationCluster(*this);
}


void VisualAnimationCluster::copy(const VisualAnimationCluster& other) {
	// deep copy
	for (AnimationVectorConstIterator it = other.animationVector.begin(); it != other.animationVector.end(); it++) {
		this->animationVector.push_back((*it)->clone());
	}
	//this->itemIdentifier = VisualItemIdentifier(other.itemIdentifier);
	this->enclosingAsset = other.enclosingAsset;
	if (other.parentCollectionIdentifier != NULL) {
		this->parentCollectionIdentifier = new VisualItemIdentifier(*(other.parentCollectionIdentifier));
	} else {
		this->parentCollectionIdentifier = NULL;
	}
	this->performAnyAdditionalActionCallback = other.performAnyAdditionalActionCallback;
	this->performAnyAdditionalActionCallbackUserData = other.performAnyAdditionalActionCallbackUserData;
	this->willDieCallback = other.willDieCallback;
	this->willDieCallbackUserData = other.willDieCallbackUserData;
}


void VisualAnimationCluster::add(VisualAnimationComponent& anAnimation) {
	anAnimation.setIdentifierOfParentCollection(this->VisualObject::getIdentifier());
	this->animationVector.push_back(anAnimation.clone());
}


void VisualAnimationCluster::animate() {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		if ((*it)->isReadyToDie() == false) {
			(*it)->animate();
		}
	}
	if (this->performAnyAdditionalActionCallback != NULL) {
		this->performAnyAdditionalActionCallback(this, this->performAnyAdditionalActionCallbackUserData);
	}
}


void VisualAnimationCluster::update(void) {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		(*it)->update();
	}
}


bool VisualAnimationCluster::isRunning() const {
	bool returnValue = false;
	for (AnimationVectorConstIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		if ((*it)->isRunning() == true) {
			returnValue = true;
		}
	}
	return returnValue;
}


bool VisualAnimationCluster::isReadyToDie() const {
	bool returnValue = true;
	for (AnimationVectorConstIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		if ((*it)->isReadyToDie() == false) {
			returnValue = false;
		}
	}
	return returnValue;
}


void VisualAnimationCluster::start() {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		(*it)->start();
	}
}


void VisualAnimationCluster::stop() {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		(*it)->stop();
	}
}


AnimatedProperty VisualAnimationCluster::getAnimatedProperty(void) const {
	AnimatedProperty animatedProperty = kUndefinedAnimatedProperty;
	if (this->animationVector.size() == 1) {
		animatedProperty = this->animationVector[0]->getAnimatedProperty();
	}
	return animatedProperty;
}


void VisualAnimationCluster::preparePriorToAddingToAsset(VisualAsset& visualAsset) {
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		(*it)->preparePriorToAddingToAsset(visualAsset);
	}
	this->enclosingAsset = &visualAsset;
}


const VisualAsset* const VisualAnimationCluster::getEnclosingAsset(void) const {
	return this->enclosingAsset;
}


void VisualAnimationCluster::setIdentifierOfParentCollection(const VisualItemIdentifier& aCollectionIdentifier) {
	if (this->parentCollectionIdentifier != NULL) {
		delete this->parentCollectionIdentifier;
	}
	this->parentCollectionIdentifier = new VisualItemIdentifier(aCollectionIdentifier);
}


void VisualAnimationCluster::handleOneCollectionItemAnimationDied() {
	bool clusterDied = true;
	for (AnimationVectorIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		if ((*it)->isReadyToDie() != true) {
			clusterDied = false;
		}
	}
	if (clusterDied == true) {
		if (this->parentCollectionIdentifier != NULL) {
			VisualAnimationComponent* animationCollection = VisualAnimationQueue::getVisualAnimation(*(this->parentCollectionIdentifier));
			animationCollection->handleOneCollectionItemAnimationDied();
		}
	}
}


void VisualAnimationCluster::setCallbackToPerformAnyAdditionalAction(VisualAnimationPerformAnyAdditionalActionCallback theCallback, void* userData) {
	this->performAnyAdditionalActionCallback = theCallback;
	this->performAnyAdditionalActionCallbackUserData = userData;
}


void VisualAnimationCluster::setCallbackToNotifyBeforeDeath(VisualAnimationWillDieCallback theCallback, void* userData) {
	this->willDieCallback = theCallback;
	this->willDieCallbackUserData = userData;
}


void VisualAnimationCluster::notifyBeforeDeath() {
	if (this->willDieCallback != NULL) {
		this->willDieCallback(this, this->willDieCallbackUserData);
	}
}


VisualAnimationComponent* VisualAnimationCluster::getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier) {
	if (VisualObject::getIdentifier() == anAnimationIdentifier) {
		return this;
	}
	for (AnimationVectorConstIterator it = this->animationVector.begin(); it != this->animationVector.end(); it++) {
		VisualAnimationComponent* animation = (*it)->getVisualAnimation(anAnimationIdentifier);
		if (animation != NULL) {
			return animation;
		}
	}
	return NULL;
}
