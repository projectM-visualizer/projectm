/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualObject.cpp
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

#include "VisualObject.h"
#include "VisualObjectData.h"


using namespace VizKit;


VisualObject::VisualObject() {
	visualObjectData = new VisualObjectData;
}


VisualObject::VisualObject(const VisualItemIdentifier& anIdentifier) {
	visualObjectData = new VisualObjectData(anIdentifier);
}


VisualObject::~VisualObject() {
	visualObjectData->enterCriticalRegion();
	visualObjectData->hasObjectBool = false;
	visualObjectData->exitCriticalRegion();
	visualObjectData->refCount--;
	if (visualObjectData->refCount == 0) {
		delete visualObjectData;
		visualObjectData = NULL;
	}
}

VisualObject::VisualObject(const VisualObject& other) {
	this->copyObject(other);
}


VisualObject& VisualObject::operator=(const VisualObject& other) {
	if (this == &other) return *this;
	
	this->visualObjectData->refCount--;
	if (this->visualObjectData->refCount == 0) {
		delete this->visualObjectData;
		this->visualObjectData = NULL;
	}
	
	this->copyObject(other);

	return *this;
}


bool VisualObject::operator<(const VisualObject& other) const {
	return this->visualObjectData->itemIdentifier < other.visualObjectData->itemIdentifier;
}


bool VisualObject::operator>(const VisualObject& other) const {
	return this->visualObjectData->itemIdentifier > other.visualObjectData->itemIdentifier;
}


bool VisualObject::operator==(const VisualObject& other) const {
	return this->visualObjectData->itemIdentifier == other.visualObjectData->itemIdentifier;
}


bool VisualObject::operator!=(const VisualObject& other) const {
	return this->visualObjectData->itemIdentifier != other.visualObjectData->itemIdentifier;
}


const VisualItemIdentifier VisualObject::getIdentifier() const {
	return *(this->visualObjectData->itemIdentifier);
}


bool VisualObject::addData(void* dataPtr, uint32 dataSize) {
	return this->visualObjectData->addData(dataPtr, dataSize);
}


void VisualObject::freeData(void) {
	this->visualObjectData->freeData();
}


size_t VisualObject::getDataSizeUsed() {
	return this->visualObjectData->dataSizeUsed;
}


size_t VisualObject::getDataSizeDeclared() {
	return this->visualObjectData->dataSizeDeclared;
}


void VisualObject::setDataSizeDeclared(const size_t& aDataSizeDeclared) {
	this->visualObjectData->dataSizeDeclared = aDataSizeDeclared;
	this->visualObjectData->dataSizeDeclaredHasBeenSetBool = true;
}


bool VisualObject::dataSizeDeclaredHasBeenSet(void) {
	return this->visualObjectData->dataSizeDeclaredHasBeenSetBool;
}


VisualObjectData* VisualObject::getVisualObjectData(void) {
	this->visualObjectData->refCount++;
	return this->visualObjectData;
}


void VisualObject::dataLoadDidEnd(const VisualItemIdentifier& identifier) {
	// hook method that does not need to be implemented by subclass
}


bool VisualObject::hasData(void) {
	return (this->visualObjectData->data != NULL);
}


char* VisualObject::getData(void) {
	return this->visualObjectData->data;
}


size_t VisualObject::getDataSize(void) {
	return this->visualObjectData->dataSizeUsed;
}


void VisualObject::copyObject(const VisualObject& other) {
	this->visualObjectData = new VisualObjectData(*other.visualObjectData);
}
