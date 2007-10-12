/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualStagePosition.cpp
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

#include "VisualStagePosition.h"



using namespace VizKit;


VisualStagePosition::VisualStagePosition() {
	reset();
}


VisualStagePosition::~VisualStagePosition() {
	// NULL
}


VisualStagePosition::VisualStagePosition(const VisualStagePosition& other) {
	copy(other);
}

			
VisualStagePosition& VisualStagePosition::operator=(const VisualStagePosition& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualStagePosition::copy(const VisualStagePosition& other) {
	horizontalAlignment = other.horizontalAlignment;
	verticalAlignment = other.verticalAlignment;
	depthAlignment = other.depthAlignment;
	
	marginTop = other.marginTop;
	marginLeft = other.marginLeft;
	marginBottom = other.marginBottom;
	marginRight = other.marginRight;

	minMarginTop = other.minMarginTop;
	minMarginLeft = other.minMarginLeft;
	minMarginBottom = other.minMarginBottom;
	minMarginRight = other.minMarginRight;
	
	minWidth = other.minWidth;
	maxWidth = other.maxWidth;
	minHeight = other.minHeight;
	maxHeight = other.maxHeight;
	
	marginTopUnit = other.marginTopUnit;
	marginLeftUnit = other.marginLeftUnit;
	marginRightUnit = other.marginRightUnit;
	marginBottomUnit = other.marginBottomUnit;

	minMarginTopUnit = other.minMarginTopUnit;
	minMarginLeftUnit = other.minMarginLeftUnit;
	minMarginRightUnit = other.minMarginRightUnit;
	minMarginBottomUnit = other.minMarginBottomUnit;
	
	minWidthUnit = other.minWidthUnit;
	maxWidthUnit = other.maxWidthUnit;
	minHeightUnit = other.minHeightUnit;
	maxHeightUnit = other.maxHeightUnit;
}


void VisualStagePosition::reset() {

	this->horizontalAlignment = kCenterAligned;
	this->verticalAlignment = kMiddleAligned;
	this->depthAlignment = kDepthCenterAligned;
	
	this->marginTop = 0.0;
	this->marginLeft = 0.0;
	this->marginBottom = 0.0;
	this->marginRight = 0.0;

	this->minMarginTop = 0.0;
	this->minMarginLeft = 0.0;
	this->minMarginBottom = 0.0;
	this->minMarginRight = 0.0;
	
	this->minWidth = 5.0;
	this->maxWidth = 0.0;
	this->minHeight = 5.0;
	this->maxHeight = 0.0;
	
	this->marginTopUnit = kPixel;
	this->marginLeftUnit = kPixel;
	this->marginRightUnit = kPixel;
	this->marginBottomUnit = kPixel;

	this->minMarginTopUnit = kPixel;
	this->minMarginLeftUnit = kPixel;
	this->minMarginRightUnit = kPixel;
	this->minMarginBottomUnit = kPixel;
	
	this->minWidthUnit = kPixel;
	this->maxWidthUnit = kPixel;
	this->minHeightUnit = kPixel;
	this->maxHeightUnit = kPixel;
	
}
