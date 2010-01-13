/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStringStyle.cpp
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

#include "VisualStringStyle.h"


using namespace VizKit;


VisualStringStyle::VisualStringStyle() {
	fontNameStr[0] = '\0';
#if TARGET_OS_MAC
	strcpy(fontNameStr, "Lucida Grande");
#endif
#if TARGET_OS_WIN
	strcpy(fontNameStr, "Tahoma");
#endif
	fontSize = 16.0;
	fontColor.red = 1.0;
	fontColor.green = 1.0;
	fontColor.blue = 1.0;
	fontColor.alpha = 1.0;
	fontColor.r = 1.0;
	fontColor.g = 1.0;
	fontColor.b = 1.0;
	fontColor.a = 1.0;
	horizontalAlignment = kLeftAligned;
}


VisualStringStyle::~VisualStringStyle() {
	// NULL
}


VisualStringStyle::VisualStringStyle(const VisualStringStyle& other) : VisualObject(other) {
	copy(other);
}


VisualStringStyle& VisualStringStyle::operator=(const VisualStringStyle& other) {
	
	if (this == &other) return *this;
	
	VisualObject::operator=(other);
	
	this->copy(other);
	
	return *this;
}


VisualStringStyle* VisualStringStyle::clone(void) const {
	return new VisualStringStyle(*this);
}


void VisualStringStyle::copy(const VisualStringStyle& other) {
	strcpy(this->fontNameStr, other.fontNameStr);
	this->fontSize = other.fontSize;
	this->fontColor = other.fontColor;
	this->horizontalAlignment = other.horizontalAlignment;
}
