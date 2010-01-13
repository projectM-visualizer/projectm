/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAnimationQueue.cpp
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

#include "VisualAnimationQueue.h"
#include "VisualAnimationComponent.h"
#include "VisualAsset.h"
#include "VisualErrorHandling.h"
#include <algorithm>

using namespace VizKit;


VisualAnimationQueue* VisualAnimationQueue::theVisualAnimationQueue = NULL;


VisualAnimationQueue::VisualAnimationQueue() {
	// null
}


VisualAnimationQueue::~VisualAnimationQueue() {
	for (AnimationQueueIterator it = animationQueue.begin(); it != animationQueue.end(); it++) {
		delete it->second;
	}
	animationQueue.clear();
}


VisualAnimationQueue* VisualAnimationQueue::getInstance() {
	if (theVisualAnimationQueue == NULL) {
		theVisualAnimationQueue = new VisualAnimationQueue;
	}
	return theVisualAnimationQueue;
}


void VisualAnimationQueue::dispose() {
	if (theVisualAnimationQueue != NULL) {
		delete theVisualAnimationQueue;
		theVisualAnimationQueue = NULL;
	}
}


void VisualAnimationQueue::push(const VisualAnimationComponent& anAnimation, const VisualItemIdentifier& anOwnerIdentifier) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.insert(std::make_pair(anOwnerIdentifier, anAnimation.clone()));
	it->second->start();
}


size_t VisualAnimationQueue::size() {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	return (theVisualAnimationQueue->animationQueue.size());
}


void VisualAnimationQueue::removeVisualAnimationsWithOwnerIdentifier(const VisualItemIdentifier& anOwnerIdentifier, const AnimatedProperty anAnimatedProperty) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.lower_bound(anOwnerIdentifier);
	while (it != theVisualAnimationQueue->animationQueue.upper_bound(anOwnerIdentifier)) {
		if (it->second->getAnimatedProperty() == anAnimatedProperty) {
			delete it->second;
			it->second = NULL;
			theVisualAnimationQueue->animationQueue.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}
}


void VisualAnimationQueue::removeVisualAnimationsWithOwnerIdentifier(const VisualItemIdentifier& anOwnerIdentifier) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.lower_bound(anOwnerIdentifier);
	while (it != theVisualAnimationQueue->animationQueue.upper_bound(anOwnerIdentifier)) {
		delete it->second;
		it->second = NULL;
		theVisualAnimationQueue->animationQueue.erase(it++); // increment iterator, then call erase with old iterator
		// after erase, accessing iterator (for increment) is invalid
	}
}


void VisualAnimationQueue::removeVisualAnimationsOfAsset(const VisualAsset* const anAsset) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.begin();
	while (it != theVisualAnimationQueue->animationQueue.end()) {
		if (it->second->getEnclosingAsset() == anAsset) {
			delete it->second;
			theVisualAnimationQueue->animationQueue.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}
}


void VisualAnimationQueue::copyAnimationsFromAssetToAsset(const VisualAsset& sourceAsset, VisualAsset& destinationAsset) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	std::vector<VisualAnimationComponent*> animationsToCopy;
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.begin();
	while (it != theVisualAnimationQueue->animationQueue.end()) {
		if (*(it->second->getEnclosingAsset()) == sourceAsset) {
			animationsToCopy.push_back(it->second->clone());
		}
		it++;
	}
	for (std::vector<VisualAnimationComponent*>::iterator temp_it = animationsToCopy.begin(); temp_it != animationsToCopy.end(); temp_it++) {
		destinationAsset.addAnimation(*(*temp_it));
	}
}


void VisualAnimationQueue::removeVisualAnimation(const VisualItemIdentifier& animationIdentifier) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.begin();
	while (it != theVisualAnimationQueue->animationQueue.end()) {
		if (it->second->getIdentifier() == animationIdentifier) {
			delete it->second;
			it->second = NULL;
			theVisualAnimationQueue->animationQueue.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}
}


VisualAnimationComponent* VisualAnimationQueue::getVisualAnimation(const VisualItemIdentifier& anAnimationIdentifier) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	for (ConstAnimationQueueIterator it = theVisualAnimationQueue->animationQueue.begin(); it != theVisualAnimationQueue->animationQueue.end(); it++) {
		VisualAnimationComponent* animation = it->second->getVisualAnimation(anAnimationIdentifier);
		if (animation != NULL) {
			return animation;
		}
	}
	return NULL;
}


const VisualAnimationComponent* const VisualAnimationQueue::getFirstVisualAnimation(const VisualItemIdentifier& anOwnerIdentifier, const AnimatedProperty& anAnimatedProperty) {
	theVisualAnimationQueue = VisualAnimationQueue::getInstance();
	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.lower_bound(anOwnerIdentifier);
	while (it != theVisualAnimationQueue->animationQueue.upper_bound(anOwnerIdentifier)) {
		if (it->second->getAnimatedProperty() == anAnimatedProperty) {
			return it->second;
		}
		it++;
	}
	return NULL;
}


void VisualAnimationQueue::processAnimations() {

	theVisualAnimationQueue = VisualAnimationQueue::getInstance();

	AnimationQueueIterator it = theVisualAnimationQueue->animationQueue.begin();
	while (it != theVisualAnimationQueue->animationQueue.end()) {
		it->second->update();
		if (it->second->isReadyToDie() == true) {
			it->second->notifyBeforeDeath();
			delete it->second;
			it->second = NULL;
			theVisualAnimationQueue->animationQueue.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			if (it->second->isRunning() == true) {
				it->second->animate();
			}
			it++;
		}
	}

}
