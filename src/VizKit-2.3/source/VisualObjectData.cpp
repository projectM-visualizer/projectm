/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualObjectData.cpp
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

#include "VisualObjectData.h"
#include "VisualErrorHandling.h"
#include "VisualNetwork.h"
#include "VisualItemIdentifier.h"

using namespace VizKit;


VisualObjectData::VisualObjectData() {
	data = NULL;
	dataSizeAllocated = 0;
	dataSizeUsed = 0;
	dataSizeDeclared = 0;
	dataSizeDeclaredHasBeenSetBool = false;
	refCount = 1;
	hasObjectBool = true;
	itemIdentifier = new VisualItemIdentifier;
	isDownloadingBool = false;
	visualObjectMutex = new VisualObjectMutex;
}


VisualObjectData::VisualObjectData(const VisualItemIdentifier& anIdentifier) {
	itemIdentifier = new VisualItemIdentifier(anIdentifier);
	data = NULL;
	dataSizeAllocated = 0;
	dataSizeUsed = 0;
	dataSizeDeclared = 0;
	dataSizeDeclaredHasBeenSetBool = false;
	refCount = 1;
	hasObjectBool = true;
	isDownloadingBool = false;
	visualObjectMutex = new VisualObjectMutex;
}


VisualObjectData::~VisualObjectData() {
	if (data != NULL) {
		free(data);
	}
	delete itemIdentifier;
	delete visualObjectMutex;
}


VisualObjectData::VisualObjectData(const VisualObjectData& other) {
	this->copyObject(other);
}


VisualObjectData& VisualObjectData::operator=(const VisualObjectData& other) {
	if (this == &other) return *this;
	
	if (this->data != NULL) {
		free(this->data);
	}
	delete this->itemIdentifier;
	delete this->visualObjectMutex;
	this->visualObjectMutex = NULL;
	
	this->copyObject(other);

	return *this;
}


bool VisualObjectData::operator<(const VisualObjectData& other) const {
	return this->itemIdentifier < other.itemIdentifier;
}


bool VisualObjectData::operator>(const VisualObjectData& other) const {
	return this->itemIdentifier > other.itemIdentifier;
}


bool VisualObjectData::operator==(const VisualObjectData& other) const {
	return this->itemIdentifier == other.itemIdentifier;
}


bool VisualObjectData::operator!=(const VisualObjectData& other) const {
	return this->itemIdentifier != other.itemIdentifier;
}


const VisualItemIdentifier VisualObjectData::getIdentifier() const {
	return *(this->itemIdentifier);
}


bool VisualObjectData::addData(void* dataPtr, uint32 dataSize) {
	bool success = true;
	if (this->dataSizeAllocated < (this->dataSizeUsed + dataSize)) {
		while (this->dataSizeAllocated < (this->dataSizeUsed + dataSize)) {
			if (this->dataSizeAllocated == 0) {
				uint32 initialAllocationSize = dataSize;
				if ((this->dataSizeDeclaredHasBeenSetBool == true) && (this->dataSizeDeclared > dataSize)) {
					initialAllocationSize = this->dataSizeDeclared;
				}
				this->data = (char*)malloc(initialAllocationSize * sizeof(char));
				this->dataSizeAllocated = initialAllocationSize;
			} else {
				if ((this->dataSizeDeclaredHasBeenSetBool == true) && (this->dataSizeDeclared >= this->dataSizeUsed + dataSize)) {
					this->dataSizeAllocated = this->dataSizeDeclared;
				} else {
					this->dataSizeAllocated *= 2;
				}
				this->data = (char*)realloc(this->data, this->dataSizeAllocated);
				if (this->data == NULL) {
					char errLog[256];
					sprintf(errLog, "Err: Realloc failed in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
					writeLog(errLog);
				}
			}
		}
	}
	memcpy(this->data + this->dataSizeUsed, dataPtr, dataSize);
	this->dataSizeUsed += dataSize;
	return success;
}


void VisualObjectData::freeData(void) {
	if (this->data) {
		free(this->data);
		this->data = NULL;
		this->dataSizeAllocated = 0;
		this->dataSizeUsed = 0;
	}
}


size_t VisualObjectData::getDataSizeUsed() {
	return this->dataSizeUsed;
}


size_t VisualObjectData::getDataSizeDeclared() {
	return this->dataSizeDeclared;
}


void VisualObjectData::setDataSizeDeclared(const size_t& aDataSizeDeclared) {
	this->dataSizeDeclared = aDataSizeDeclared;
	this->dataSizeDeclaredHasBeenSetBool = true;
}


bool VisualObjectData::dataSizeDeclaredHasBeenSet(void) {
	return this->dataSizeDeclaredHasBeenSetBool;
}


void VisualObjectData::release() {
	this->refCount--;
}


size_t VisualObjectData::getRefCount(void) {
	return this->refCount;
}


bool VisualObjectData::hasObject(){
	return this->hasObjectBool;
}


bool VisualObjectData::isDownloading(void) {
	return this->isDownloadingBool;
}


void VisualObjectData::setIsDownloading(bool isDownloadingBoolean) {
	this->isDownloadingBool = isDownloadingBoolean;
}


void VisualObjectData::enterCriticalRegion() {
	this->visualObjectMutex->enterCriticalRegion();
}


void VisualObjectData::exitCriticalRegion() {
	this->visualObjectMutex->exitCriticalRegion();
}


void VisualObjectData::copyObject(const VisualObjectData& other) {
	this->itemIdentifier = new VisualItemIdentifier(*(other.itemIdentifier));
	if (other.data != NULL) {
		this->data = (char*)malloc(other.dataSizeAllocated);
		memcpy(this->data, other.data, other.dataSizeUsed);
	} else {
		this->data = NULL;
	}
	this->visualObjectMutex = new VisualObjectMutex(*other.visualObjectMutex);
	this->dataSizeAllocated = other.dataSizeAllocated;
	this->dataSizeUsed = other.dataSizeUsed;
	this->dataSizeDeclared = other.dataSizeDeclared;
	this->dataSizeDeclaredHasBeenSetBool = other.dataSizeDeclaredHasBeenSetBool;
	this->refCount = other.refCount;
	this->hasObjectBool = other.hasObjectBool;
}
