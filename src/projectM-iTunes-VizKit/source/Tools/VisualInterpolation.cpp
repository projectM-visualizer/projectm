/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualInterpolation.cpp
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

#include "VisualInterpolation.h"
#include "VisualErrorHandling.h"

#include <math.h>
#include <stdio.h>



#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif


using namespace VizKit;


VisualInterpolation::VisualInterpolation(InterpolationType anInterpolationType) {
	startValue = 0.0;
	endValue = 1.0;
	calcDistance();
	interpolationType = anInterpolationType;
}


VisualInterpolation::~VisualInterpolation() {
	// NULL
}


VisualInterpolation::VisualInterpolation(const VisualInterpolation& other) {
	copy(other);
}


VisualInterpolation& VisualInterpolation::operator=(const VisualInterpolation& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualInterpolation::copy(const VisualInterpolation& other) {
	this->startValue = other.startValue;
	this->endValue = other.endValue;
	this->distance = other.distance;
	this->interpolationType = other.interpolationType;
}


void VisualInterpolation::setStartValue(double aStartValue) {
	this->startValue = aStartValue;
	this->calcDistance();
}

		
void VisualInterpolation::setEndValue(double anEndValue) {
	this->endValue = anEndValue;
	this->calcDistance();
}


double VisualInterpolation::getStartValue() {
	return this->startValue;
}
		

double VisualInterpolation::getEndValue() {
	return this->endValue;
}


double VisualInterpolation::getValueAtPosition(double position) {
	double valueAtPosition = 0.0;
	if (position < 0.0 || position > 1.0) {
		char errStr[64];
		printf("%f\n", position);
		sprintf(errStr, "invalid position %f in getValueAtPosition() of VisualInterpolation", position);
		writeLog(errStr);
		return valueAtPosition;
	}
	if (interpolationType == kSinusoidalInterpolation) {
		position = (-1.0 * cos(position * M_PI) / 2.0) + 0.5;
	}
	if (this->startValue > this->endValue) {
		valueAtPosition = this->startValue - (this->distance * position);
	} else if (this->startValue < this->endValue) {
		valueAtPosition = this->startValue + (this->distance * position);
	} else {
		valueAtPosition = this->startValue;
	}
	return valueAtPosition;
}


void VisualInterpolation::setType(InterpolationType anInterpolationType) {
	this->interpolationType = anInterpolationType;
}


void VisualInterpolation::calcDistance() {
	if (this->startValue > this->endValue) {
		this->distance = this->startValue - this->endValue;
	} else if (this->startValue < this->endValue) {
		this->distance = this->endValue - this->startValue;
	} else {
		this->distance = 0.0;
	}
}
