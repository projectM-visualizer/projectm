/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualProgressIndicator.cpp
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

#include "VisualProgressIndicator.h"


using namespace VizKit;


const double VisualProgressIndicator::kProgressNotStarted = -1.0;

const double VisualProgressIndicator::kProgressUnknown = -2.0;


VisualProgressIndicator::VisualProgressIndicator() {
	currentProgressState = kProgressNotStarted;
}


VisualProgressIndicator::~VisualProgressIndicator() {
	// NULL
}


VisualProgressIndicator::VisualProgressIndicator(const VisualProgressIndicator& other) {
	copy(other);
}


VisualProgressIndicator& VisualProgressIndicator::operator=(const VisualProgressIndicator& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualProgressIndicator::copy(const VisualProgressIndicator& other) {
	this->currentProgressState = other.currentProgressState;
}


double VisualProgressIndicator::getState() const {
	return this->currentProgressState;
}


void VisualProgressIndicator::setState(double state) {
	this->currentProgressState = state;
}
