/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualCamera.cpp
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

#include "VisualCamera.h"
#include "VisualGraphics.h"


using namespace VizKit;


VisualCamera::VisualCamera() {
	maxTopCoord = 0.0;
	maxLeftCoord = 0.0;
	maxBottomCoord = 0.0;
	maxRightCoord = 0.0;
	maxNearPos = 0.0;
	maxFarPos = 0.0;
	zoomFactor = 1.0;
	setOrthographicProjection();
}


VisualCamera::~VisualCamera() {
	// NULL
}


VisualCamera::VisualCamera(const VisualCamera& other) {
	copy(other);
}


VisualCamera& VisualCamera::operator=(const VisualCamera& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualCamera::copy(const VisualCamera& other) {
	this->maxTopCoord = other.maxTopCoord;
	this->maxLeftCoord = other.maxLeftCoord;
	this->maxBottomCoord = other.maxBottomCoord;
	this->maxRightCoord = other.maxRightCoord;
	this->maxNearPos = other.maxNearPos;
	this->maxFarPos = other.maxFarPos;
	this->zoomFactor = other.zoomFactor;
	this->projectionMode = other.projectionMode;
}


void VisualCamera::setOrthographicProjection() {
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	double zoom = 1.0 / zoomFactor;
	
	RelationalRect relationalRect = theVisualGraphics->getViewportOrientationAndAspectRatio();

	if (relationalRect.orientation == kPortrait) {
    	this->maxLeftCoord = -1.0;
        this->maxRightCoord = 1.0;
		this->maxBottomCoord = -1.0 * relationalRect.aspectRatio;
		this->maxTopCoord = 1.0 * relationalRect.aspectRatio;
    } else if (relationalRect.orientation == kLandscape) {
		this->maxLeftCoord = -1.0 * relationalRect.aspectRatio;
		this->maxRightCoord = 1.0 * relationalRect.aspectRatio;
        this->maxBottomCoord = -1.0;
        this->maxTopCoord = 1.0;
    } else if (relationalRect.orientation == kSquare) {
		this->maxLeftCoord = -1.0;
		this->maxRightCoord = 1.0;
        this->maxBottomCoord = -1.0;
        this->maxTopCoord = 1.0;
    }
	
	this->maxNearPos = 0.0;
	this->maxFarPos = 10.0;

	theVisualGraphics->setOrthographicProjection(this->maxLeftCoord, this->maxRightCoord, this->maxBottomCoord, this->maxTopCoord, this->maxNearPos, this->maxFarPos, zoom);
	
	this->projectionMode = kOrthographic;
}


void VisualCamera::setPerspectiveProjection(double nearClippingPlanePos) {

	double zoom = 1.0 / zoomFactor;
	
	this->maxNearPos = nearClippingPlanePos;
	
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	RelationalRect relationalRect = theVisualGraphics->getViewportOrientationAndAspectRatio();

	if (relationalRect.orientation == kPortrait) {
    	this->maxLeftCoord = -1.0;
        this->maxRightCoord = 1.0;
		this->maxBottomCoord = -1.0 * relationalRect.aspectRatio;
		this->maxTopCoord = 1.0 * relationalRect.aspectRatio;
		this->maxFarPos = (this->maxBottomCoord * -1.0) + this->maxTopCoord + this->maxNearPos;
	} else if (relationalRect.orientation == kLandscape) {
		this->maxLeftCoord = -1.0 * relationalRect.aspectRatio;
		this->maxRightCoord = 1.0 * relationalRect.aspectRatio;
        this->maxBottomCoord = -1.0;
        this->maxTopCoord = 1.0;
		this->maxFarPos = (maxLeftCoord * -1.0) + maxRightCoord + this->maxNearPos;
    } else if (relationalRect.orientation == kSquare) {
		this->maxLeftCoord = -1.0;
		this->maxRightCoord = 1.0;
        this->maxBottomCoord = -1.0;
        this->maxTopCoord = 1.0;
		this->maxFarPos = 2.0 + this->maxNearPos;
	}
	
	theVisualGraphics->setPerspectiveProjection(this->maxLeftCoord, this->maxRightCoord, this->maxBottomCoord, this->maxTopCoord, this->maxNearPos, this->maxFarPos, zoom);
	
	this->projectionMode = kPerspective;
}


void VisualCamera::lookAt(Point3D eye, Point3D center, Vector up) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->lookAt(eye, center, up);
}


void VisualCamera::activate() {
	if (this->projectionMode == kOrthographic) {
		this->setOrthographicProjection();
	} else if (this->projectionMode == kPerspective) {
		this->setPerspectiveProjection(this->maxNearPos);
	}
}


VisualCamera* VisualCamera::createDefaultCamera() {
	VisualCamera* aCamera = new VisualCamera;
	aCamera->setOrthographicProjection();
	return aCamera;
}


double VisualCamera::getMaxTopCoord() const {
	return this->maxTopCoord;
}


double VisualCamera::getMaxLeftCoord() const {
	return this->maxLeftCoord;
}


double VisualCamera::getMaxBottomCoord() const {
	return this->maxBottomCoord;
}


double VisualCamera::getMaxRightCoord() const {
	return this->maxRightCoord;
}


double VisualCamera::getMaxNearPos() const {
	return this->maxNearPos;
}


double VisualCamera::getMaxFarPos() const {
	return this->maxFarPos;
}


CoordSize3D VisualCamera::getSize() const {
	CoordSize3D size;
	size.width = this->getCoordWidth();
	size.height = this->getCoordHeight();
	size.depth = this->getDepth();
	return size;
}


double VisualCamera::getCoordHeight() const {
	return this->maxTopCoord - this->maxBottomCoord;
}


double VisualCamera::getCoordWidth() const {
	return this->maxRightCoord - this->maxLeftCoord;
}


double VisualCamera::getDepth() const {
	return this->maxFarPos - this->maxNearPos;
}
