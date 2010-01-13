/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualAsset.cpp
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

#include "VisualAsset.h"
#include "VisualItemIdentifier.h"
#include "VisualAnimationPackage.h"
#include "VisualAnimationQueue.h"
#include "VisualErrorHandling.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualGraphics.h"
#include "VisualGraphicTypes.h"
#include "VisualVertex.h"
#include "VisualCamera.h"


using namespace VizKit;


VisualAsset::VisualAsset() {
	image = NULL;
	camera = NULL;
	stageBox = NULL;
	defaultVertexChainIdentifier = new VisualItemIdentifier;
	rotationAngle = 0.0;
	opacityValue = 1.0;
	scaleFactor = 1.0;
	init();
}


VisualAsset::~VisualAsset() {
	
	VisualAnimationQueue::removeVisualAnimationsOfAsset(this);
	
	delete stageBox;
	delete camera;
	delete defaultVertexChainIdentifier;
	if (image != NULL) {
		delete image;
	}
	
}


VisualAsset::VisualAsset(const VisualAsset& other) : VisualObject(other) {
	copy(other);
}


VisualAsset& VisualAsset::operator=(const VisualAsset& other) {
	
	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	if (this->image != NULL) {
		delete this->image;
	}
	delete this->stageBox;
	delete this->camera;
	delete this->defaultVertexChainIdentifier;
	
	this->copy(other);
	
	return *this;
}


bool VisualAsset::operator<(const VisualAsset& other) const {
	return (this->VisualObject::getIdentifier() < other.VisualObject::getIdentifier());
}


bool VisualAsset::operator==(const VisualAsset& other) const {
	return (this->VisualObject::getIdentifier() == other.VisualObject::getIdentifier());
}


bool VisualAsset::operator!=(const VisualAsset& other) const {
	return !(*this == other);
}


VisualAsset* VisualAsset::clone(void) const {
	return new VisualAsset(*this);
}


void VisualAsset::copy(const VisualAsset& other) {
	
	if (other.image != NULL) {
		this->image = new VisualImage(*(other.image));
	} else {
		this->image = NULL;
	}
	this->camera = new VisualCamera(*(other.camera));
	this->stageBox = new VisualStageBox(*(other.stageBox));
	
	this->defaultVertexChainIdentifier = new VisualItemIdentifier();
	this->rotationAngle = other.rotationAngle;
	this->opacityValue = other.opacityValue;
	this->scaleFactor = other.scaleFactor;
	
	VisualAnimationQueue::copyAnimationsFromAssetToAsset(other, *this);
}


void VisualAsset::init() {
	this->camera = new VisualCamera;
	this->stageBox = new VisualStageBox(this);
	this->setCamera(*this->camera);
}


VisualStageBox* VisualAsset::getBox() const {
	return this->stageBox;
}


void VisualAsset::setCamera(const VisualCamera& cameraRef) {
	*(this->camera) = cameraRef;
	this->camera->activate();
	this->stageBox->setCoordDepth(cameraRef.getDepth());
	this->stageBox->update();
}


VisualCamera& VisualAsset::getCamera(void) const {
	return *(this->camera);
}


VisualStagePosition VisualAsset::getPosition() const {
	return this->stageBox->getVisualStagePosition();
}


void VisualAsset::setPosition(const VisualStagePosition& aPosition) {
	this->stageBox->setVisualStagePosition(aPosition);
}


void VisualAsset::updateCoords() {
	this->camera->activate();
	this->stageBox->update();
}


void VisualAsset::setImage(const VisualImage& anImage) {
	if (this->image != NULL) {
		delete this->image;
		this->image = NULL;
	}
	this->image = new VisualImage(anImage);
	
	this->stageBox->setContentPixelWidth(this->image->getWidth());
	this->stageBox->setContentPixelHeight(this->image->getHeight());
	
	this->stageBox->initializeVertexChain(*(this->defaultVertexChainIdentifier));
	
	VisualVertex* aVertex = NULL;
	aVertex = this->stageBox->createVertex(0.0, 1.0, 0.0, 0.0, 1.0);
	this->stageBox->addVertexToChain(*(this->defaultVertexChainIdentifier), aVertex);
	
	aVertex = this->stageBox->createVertex(0.0, 0.0, 0.0, 0.0, 0.0);
	this->stageBox->addVertexToChain(*(this->defaultVertexChainIdentifier), aVertex);
	
	aVertex = this->stageBox->createVertex(1.0, 0.0, 0.0, 1.0, 0.0);
	this->stageBox->addVertexToChain(*(this->defaultVertexChainIdentifier), aVertex);
	
	aVertex = this->stageBox->createVertex(1.0, 1.0, 0.0, 1.0, 1.0);
	this->stageBox->addVertexToChain(*(this->defaultVertexChainIdentifier), aVertex);
}


bool VisualAsset::hasImage() const {
	if (this->image == NULL) {
		return false;
	}
	return true;
}


VisualImage* VisualAsset::getImage() const {
	return this->image;
}


void VisualAsset::removeImage() {
	if (this->image != NULL) {
		delete this->image;
		this->image = NULL;
		this->stageBox->setContentPixelWidth(0);
		this->stageBox->setContentPixelHeight(0);
	}
}


void VisualAsset::draw(const VisualItemIdentifier& vertexChainName) const {
	VertexChain* aVertexChain = this->stageBox->getVertexChain(vertexChainName);
	this->doDraw(*aVertexChain);
}


void VisualAsset::draw() const {
	size_t numberOfVertexChains = this->stageBox->getNumberOfVertexChains();
	bool defaultVertexChainFound = false;
	for (size_t i = 0; i < numberOfVertexChains; i++) {
		if (const_cast<VisualItemIdentifier&>(this->stageBox->getVertexChainIdentifier(i)) != *(this->defaultVertexChainIdentifier)) {
			VertexChain* defaultVertexChain = this->stageBox->getVertexChain(i);
			this->doDraw(*defaultVertexChain);
		} else {
			defaultVertexChainFound = true;
		}
	}
	if (defaultVertexChainFound == true && numberOfVertexChains == 1) {
		VertexChain* defaultVertexChain = this->stageBox->getVertexChain(*(this->defaultVertexChainIdentifier));
		this->doDraw(*defaultVertexChain);
	}
}


void VisualAsset::doDraw(const VertexChain& aVertexChain) const {
	this->camera->activate();
	this->stageBox->updateIfNeeded();
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->rotateMatrix(this->rotationAngle, 0.0, 1.0, 0.0);
	if (this->image != NULL) {
		this->image->draw(&aVertexChain, this->stageBox->getDebugMode());
		if (this->stageBox->getDebugMode() == true) {
			theVisualGraphics->drawVertexChain(aVertexChain);
		}
	} else {
		theVisualGraphics->drawVertexChain(aVertexChain);
	}
	
	if (this->stageBox->getDebugMode() == true) {
		theVisualGraphics->drawDebugVertexChain(aVertexChain, *(this->camera));
		this->drawBoxOutline();
	}
}


void VisualAsset::drawBoxOutline() const {
	VertexColor yellow;
	yellow.r = 1.0;
	yellow.g = 1.0;
	yellow.b = 0.0;
	yellow.a = 1.0;
	Coord coordPoint;
	VisualVertex* aVertex = NULL;
	// front
	VertexChain* frontBoxChain = new VertexChain;
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	frontBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	frontBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	frontBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	frontBoxChain->push_back(aVertex);
	// back
	VertexChain* backBoxChain = new VertexChain;
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	backBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	backBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	backBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	backBoxChain->push_back(aVertex);
	// left
	VertexChain* leftBoxChain = new VertexChain;
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	leftBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	leftBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	leftBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	leftBoxChain->push_back(aVertex);
	// right
	VertexChain* rightBoxChain = new VertexChain;
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	rightBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	rightBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	rightBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	rightBoxChain->push_back(aVertex);
	// top
	VertexChain* topBoxChain = new VertexChain;
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	topBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	topBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	topBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getTopCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	topBoxChain->push_back(aVertex);
	// bottom
	VertexChain* bottomBoxChain = new VertexChain;
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	bottomBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getRightCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	bottomBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getBackPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	bottomBoxChain->push_back(aVertex);
	coordPoint.x = this->stageBox->getLeftCoord();
	coordPoint.y = this->stageBox->getBottomCoord();
	coordPoint.z = this->stageBox->getFrontPosition();
	aVertex = new VisualVertex(coordPoint, yellow);
	bottomBoxChain->push_back(aVertex);
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->drawVertexChain(*frontBoxChain);
	theVisualGraphics->drawVertexChain(*backBoxChain);
	theVisualGraphics->drawVertexChain(*leftBoxChain);
	theVisualGraphics->drawVertexChain(*rightBoxChain);
	theVisualGraphics->drawVertexChain(*topBoxChain);
	theVisualGraphics->drawVertexChain(*bottomBoxChain);
	
	for (VertexChainIterator chainIt = frontBoxChain->begin(); chainIt != frontBoxChain->end(); chainIt++) {
		delete *chainIt;
		chainIt = frontBoxChain->erase(chainIt);
	}
	delete frontBoxChain;
	for (VertexChainIterator chainIt = backBoxChain->begin(); chainIt != backBoxChain->end(); chainIt++) {
		delete *chainIt;
		chainIt = backBoxChain->erase(chainIt);
	}
	delete backBoxChain;
	for (VertexChainIterator chainIt = leftBoxChain->begin(); chainIt != leftBoxChain->end(); chainIt++) {
		delete *chainIt;
		chainIt = leftBoxChain->erase(chainIt);
	}
	delete leftBoxChain;
	for (VertexChainIterator chainIt = rightBoxChain->begin(); chainIt != rightBoxChain->end(); chainIt++) {
		delete *chainIt;
		chainIt = rightBoxChain->erase(chainIt);
	}
	delete rightBoxChain;
	for (VertexChainIterator chainIt = topBoxChain->begin(); chainIt != topBoxChain->end(); chainIt++) {
		delete *chainIt;
		chainIt = topBoxChain->erase(chainIt);
	}
	delete topBoxChain;
	for (VertexChainIterator chainIt = bottomBoxChain->begin(); chainIt != bottomBoxChain->end(); chainIt++) {
		delete *chainIt;
		chainIt = bottomBoxChain->erase(chainIt);
	}
	delete bottomBoxChain;
}


void VisualAsset::setScaleFactor(double aScaleFactor) {
	this->scaleFactor = aScaleFactor;
	this->stageBox->setScaleFactor(aScaleFactor);
}


double VisualAsset::getScaleFactor() const {
	return this->scaleFactor;
}


void VisualAsset::setOpacityValue(double anOpacityValue) {
	this->opacityValue = anOpacityValue;
	this->stageBox->setOpacityValue(anOpacityValue);
}


double VisualAsset::getOpacityValue() const {
	return this->opacityValue;
}


void VisualAsset::setRotationAngle(double aRotationAngle) {
	this->rotationAngle = aRotationAngle;
}


double VisualAsset::getRotationAngle() const {
	return this->rotationAngle;
}


void VisualAsset::setDebugMode(bool requestedDebugMode) {
	this->stageBox->setDebugMode(requestedDebugMode);
}


bool VisualAsset::getDebugMode(void) {
	return this->stageBox->getDebugMode();
}


void VisualAsset::addAnimation(VisualAnimationComponent& anAnimation) {
	anAnimation.preparePriorToAddingToAsset(*this);
	VisualAnimationQueue::push(anAnimation, this->VisualObject::getIdentifier());
}


void VisualAsset::removeAnimations(AnimatedProperty anAnimatedProperty) {
	VisualAnimationQueue::removeVisualAnimationsWithOwnerIdentifier(this->VisualObject::getIdentifier(), anAnimatedProperty);
}


const VisualAnimationComponent* const VisualAsset::getAnimation(const AnimatedProperty& anAnimatedProperty) const {
	return VisualAnimationQueue::getFirstVisualAnimation(this->VisualObject::getIdentifier(), anAnimatedProperty);
}


double VisualAsset::getCurrentAnimationValueOfProperty(const AnimatedProperty& anAnimatedProperty) const {
	double currentValue = 0.0;
	
	switch (anAnimatedProperty) {
		case kAnimatedSize:
			currentValue = this->scaleFactor;
			break;
		case kAnimatedOpacity:
			currentValue = this->opacityValue;
			break;
		case kAnimatedRotation:
			currentValue = (this->rotationAngle / 360.0);
			break;
		case kUndefinedAnimatedProperty:
		case kCustomAnimatedProperty:
			// unknown
			break;
		default:
			char errLog[256];
			sprintf(errLog, "unknown AnimatedProperty %d in file: %s (line: %d) [%s])", anAnimatedProperty, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
	}
	
	return currentValue;
	
}


void VisualAsset::updateLocation(double currPosition) {
	VisualStagePosition position = VisualStageBox::tweenVisualStagePosition(this->startValueVisualStagePosition, this->stopValueVisualStagePosition, currPosition, *this->stageBox);
	this->setPosition(position);
}


double VisualAsset::getCurrentAnimationValueForAnimatedLocation(const VisualStagePosition& startPosition, const VisualStagePosition& stopPosition) const {
	VisualStagePosition currStagePosition = this->stageBox->getVisualStagePosition();
	return 0.0;
}


void VisualAsset::setStartValueVisualStagePosition(const VisualStagePosition& position) {
	this->startValueVisualStagePosition = position;
}


void VisualAsset::setStopValueVisualStagePosition(const VisualStagePosition& position) {
	this->stopValueVisualStagePosition = position;
}


void VisualAsset::animateOpacity(double currentPosition, void* userData) {
	if (reinterpret_cast<VisualAsset*>(userData)->stageBox->getDebugMode() == true) {
		printf("VisualAsset::animateOpacity: %f\n", currentPosition);
	}
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(currentPosition);
}


void VisualAsset::animateRotation(double currentPosition, void* userData) {
	double rotationAngle = currentPosition * 360.0;
	reinterpret_cast<VisualAsset*>(userData)->setRotationAngle(rotationAngle);
}


void VisualAsset::animateScaleFactor(double currentPosition, void* userData) {
	reinterpret_cast<VisualAsset*>(userData)->setScaleFactor(currentPosition);
}


void VisualAsset::animateLocation(double currentPosition, void* userData) {
	reinterpret_cast<VisualAsset*>(userData)->updateLocation(currentPosition);
}
