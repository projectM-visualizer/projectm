/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStagePosition.cpp
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


bool VisualStagePosition::operator==(const VisualStagePosition& other) const {
	if (this->horizontalAlignment != other.horizontalAlignment) return false;
	if (this->verticalAlignment != other.verticalAlignment) return false;
	if (this->depthAlignment != other.depthAlignment) return false;
	if (this->marginTop != other.marginTop) return false;
	if (this->marginTop != other.marginTop) return false;
	if (this->marginLeft != other.marginLeft) return false;
	if (this->marginBottom != other.marginBottom) return false;
	if (this->marginRight != other.marginRight) return false;
	if (this->minMarginTop != other.minMarginTop) return false;
	if (this->minMarginLeft != other.minMarginLeft) return false;
	if (this->minMarginBottom != other.minMarginBottom) return false;
	if (this->minMarginRight != other.minMarginRight) return false;
	if (this->minWidth != other.minWidth) return false;
	if (this->maxWidth != other.maxWidth) return false;
	if (this->minHeight != other.minHeight) return false;
	if (this->maxHeight != other.maxHeight) return false;
	if (this->marginTopUnit != other.marginTopUnit) return false;
	if (this->marginLeftUnit != other.marginLeftUnit) return false;
	if (this->marginRightUnit != other.marginRightUnit) return false;
	if (this->marginBottomUnit != other.marginBottomUnit) return false;
	if (this->minMarginTopUnit != other.minMarginTopUnit) return false;
	if (this->minMarginLeftUnit != other.minMarginLeftUnit) return false;
	if (this->minMarginRightUnit != other.minMarginRightUnit) return false;
	if (this->minMarginBottomUnit != other.minMarginBottomUnit) return false;
	if (this->minWidthUnit != other.minWidthUnit) return false;
	if (this->maxWidthUnit != other.maxWidthUnit) return false;
	if (this->minHeightUnit != other.minHeightUnit) return false;
	if (this->maxHeightUnit != other.maxHeightUnit) return false;
	if (this->horizontalCoordOffset != other.horizontalCoordOffset) return false;
	if (this->verticalCoordOffset != other.verticalCoordOffset) return false;

	return true;
}


bool VisualStagePosition::operator!=(const VisualStagePosition& other) {
	return !(*this == other);
}


void VisualStagePosition::copy(const VisualStagePosition& other) {
	
	this->horizontalAlignment = other.horizontalAlignment;
	this->verticalAlignment = other.verticalAlignment;
	this->depthAlignment = other.depthAlignment;
	
	this->marginTop = other.marginTop;
	this->marginLeft = other.marginLeft;
	this->marginBottom = other.marginBottom;
	this->marginRight = other.marginRight;

	this->minMarginTop = other.minMarginTop;
	this->minMarginLeft = other.minMarginLeft;
	this->minMarginBottom = other.minMarginBottom;
	this->minMarginRight = other.minMarginRight;
	
	this->minWidth = other.minWidth;
	this->maxWidth = other.maxWidth;
	this->minHeight = other.minHeight;
	this->maxHeight = other.maxHeight;
	
	this->marginTopUnit = other.marginTopUnit;
	this->marginLeftUnit = other.marginLeftUnit;
	this->marginRightUnit = other.marginRightUnit;
	this->marginBottomUnit = other.marginBottomUnit;

	this->minMarginTopUnit = other.minMarginTopUnit;
	this->minMarginLeftUnit = other.minMarginLeftUnit;
	this->minMarginRightUnit = other.minMarginRightUnit;
	this->minMarginBottomUnit = other.minMarginBottomUnit;
	
	this->minWidthUnit = other.minWidthUnit;
	this->maxWidthUnit = other.maxWidthUnit;
	this->minHeightUnit = other.minHeightUnit;
	this->maxHeightUnit = other.maxHeightUnit;
	
	this->horizontalCoordOffset = other.horizontalCoordOffset;
	this->verticalCoordOffset = other.verticalCoordOffset;
}


void VisualStagePosition::reset() {

	this->horizontalAlignment = kCenterAligned;
	this->verticalAlignment = kMiddleAligned;
	this->depthAlignment = kFrontAligned;
	
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
	
	this->horizontalCoordOffset = 0.0;
	this->verticalCoordOffset = 0.0;
	
}
