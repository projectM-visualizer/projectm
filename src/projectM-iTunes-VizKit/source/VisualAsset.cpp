/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualAsset.cpp
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

#include "VisualAsset.h"
#include "VisualErrorHandling.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualGraphics.h"
#include "VisualTimeline.h"
#include "VisualStageControl.h"
#include "VisualItemIdentifier.h"



using namespace VizKit;


VisualAsset::VisualAsset() {
	image = NULL;
	stageBox = new VisualStageBox;
	assetIdentifier = new VisualItemIdentifier;
	rotationAngle = 0.0f;
	opacityValue = 1.0f;
	vertexChainFront = NULL;
	vertexChainTop = NULL;
	vertexChainLeft = NULL;
	vertexChainRight = NULL;
	vertexChainBack = NULL;
	vertexChainBottom = NULL;
	topLeftFrontVertexColor.r = 1.0f;
	topLeftFrontVertexColor.g = 1.0f;
	topLeftFrontVertexColor.b = 1.0f;
	topLeftFrontVertexColor.a = 1.0f;
	topLeftBackVertexColor.r = 1.0f;
	topLeftBackVertexColor.g = 1.0f;
	topLeftBackVertexColor.b = 1.0f;
	topLeftBackVertexColor.a = 1.0f;
	bottomLeftFrontVertexColor.r = 1.0f;
	bottomLeftFrontVertexColor.g = 1.0f;
	bottomLeftFrontVertexColor.b = 1.0f;
	bottomLeftFrontVertexColor.a = 1.0f;
	bottomLeftBackVertexColor.r = 1.0f;
	bottomLeftBackVertexColor.g = 1.0f;
	bottomLeftBackVertexColor.b = 1.0f;
	bottomLeftBackVertexColor.a = 1.0f;
	bottomRightFrontVertexColor.r = 1.0f;
	bottomRightFrontVertexColor.g = 1.0f;
	bottomRightFrontVertexColor.b = 1.0f;
	bottomRightFrontVertexColor.a = 1.0f;
	bottomRightBackVertexColor.r = 1.0f;
	bottomRightBackVertexColor.g = 1.0f;
	bottomRightBackVertexColor.b = 1.0f;
	bottomRightBackVertexColor.a = 1.0f;
	topRightFrontVertexColor.r = 1.0f;
	topRightFrontVertexColor.g = 1.0f;
	topRightFrontVertexColor.b = 1.0f;
	topRightFrontVertexColor.a = 1.0f;
	topRightBackVertexColor.r = 1.0f;
	topRightBackVertexColor.g = 1.0f;
	topRightBackVertexColor.b = 1.0f;
	topRightBackVertexColor.a = 1.0f;
}


VisualAsset::~VisualAsset() {

	VisualStageControl::removeVisualAnimationsWithOwnerIdentifier(*assetIdentifier);

	delete stageBox;
	delete assetIdentifier;
	if (image != NULL) {
		delete image;
	}
	
	for (VertexChainMapIterator mapIt = this->vertexChainMap.begin(); mapIt != this->vertexChainMap.end(); mapIt++) {
		for (VertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			delete *chainIt;
			*chainIt = NULL;
		}
		mapIt->second->clear();
		mapIt->second = NULL;
	}
	this->vertexChainMap.clear();

}


VisualAsset::VisualAsset(const VisualAsset& other) {
	stageBox = new VisualStageBox;
	assetIdentifier = new VisualItemIdentifier;
	copy(other);
}


VisualAsset& VisualAsset::operator=(const VisualAsset& other) {
	if (this != &other) {
		if (this->image != NULL) {
			delete this->image;
		}
		this->copy(other);
	}
	return *this;
}


void VisualAsset::copy(const VisualAsset& other) {
	if (other.image != NULL) {
		this->image = new VisualImage(*(other.image));
	} else {
		this->image = NULL;
	}
	*(this->stageBox) = *(other.stageBox);
	*(this->assetIdentifier) = *(other.assetIdentifier);
	this->rotationAngle = other.rotationAngle;
	this->opacityValue = other.opacityValue;
	
	this->vertexChainFront = other.vertexChainFront;
	this->vertexChainTop = other.vertexChainTop;
	this->vertexChainLeft = other.vertexChainLeft;
	this->vertexChainRight = other.vertexChainRight;
	this->vertexChainBack = other.vertexChainBack;
	this->vertexChainBottom = other.vertexChainBottom;
	this->topLeftFrontVertexColor.r = other.topLeftFrontVertexColor.r;
	this->topLeftFrontVertexColor.g = other.topLeftFrontVertexColor.g;
	this->topLeftFrontVertexColor.b = other.topLeftFrontVertexColor.b;
	this->topLeftFrontVertexColor.a = other.topLeftFrontVertexColor.a;
	this->topLeftBackVertexColor.r = other.topLeftBackVertexColor.r;
	this->topLeftBackVertexColor.g = other.topLeftBackVertexColor.g;
	this->topLeftBackVertexColor.b = other.topLeftBackVertexColor.b;
	this->topLeftBackVertexColor.a = other.topLeftBackVertexColor.a;
	this->bottomLeftFrontVertexColor.r = other.bottomLeftFrontVertexColor.r;
	this->bottomLeftFrontVertexColor.g = other.bottomLeftFrontVertexColor.g;
	this->bottomLeftFrontVertexColor.b = other.bottomLeftFrontVertexColor.b;
	this->bottomLeftFrontVertexColor.a = other.bottomLeftFrontVertexColor.a;
	this->bottomLeftBackVertexColor.r = other.bottomLeftBackVertexColor.r;
	this->bottomLeftBackVertexColor.g = other.bottomLeftBackVertexColor.g;
	this->bottomLeftBackVertexColor.b = other.bottomLeftBackVertexColor.b;
	this->bottomLeftBackVertexColor.a = other.bottomLeftBackVertexColor.a;
	this->bottomRightFrontVertexColor.r = other.bottomRightFrontVertexColor.r;
	this->bottomRightFrontVertexColor.g = other.bottomRightFrontVertexColor.g;
	this->bottomRightFrontVertexColor.b = other.bottomRightFrontVertexColor.b;
	this->bottomRightFrontVertexColor.a = other.bottomRightFrontVertexColor.a;
	this->bottomRightBackVertexColor.r = other.bottomRightBackVertexColor.r;
	this->bottomRightBackVertexColor.g = other.bottomRightBackVertexColor.g;
	this->bottomRightBackVertexColor.b = other.bottomRightBackVertexColor.b;
	this->bottomRightBackVertexColor.a = other.bottomRightBackVertexColor.a;
	this->topRightFrontVertexColor.r = other.topRightFrontVertexColor.r;
	this->topRightFrontVertexColor.g = other.topRightFrontVertexColor.g;
	this->topRightFrontVertexColor.b = other.topRightFrontVertexColor.b;
	this->topRightFrontVertexColor.a = other.topRightFrontVertexColor.a;
	this->topRightBackVertexColor.r = other.topRightBackVertexColor.r;
	this->topRightBackVertexColor.g = other.topRightBackVertexColor.g;
	this->topRightBackVertexColor.b = other.topRightBackVertexColor.b;
	this->topRightBackVertexColor.a = other.topRightBackVertexColor.a;
}


VisualStageBox* VisualAsset::getBox() {
	return this->stageBox;
}


VisualStagePosition VisualAsset::getPosition() {
	return this->stageBox->getVisualStagePosition();
}


void VisualAsset::setPosition(const VisualStagePosition& aPosition) {
	this->stageBox->setVisualStagePosition(aPosition);
}


void VisualAsset::setImage(VisualImage& anImage) {
	if (this->image != NULL) {
		delete this->image;
	}
	this->image = new VisualImage(anImage);
}


VertexChain* VisualAsset::generateVertexChain(VertexChainName vertexChainName, double scaleFactor) {

	VertexChain* returnedVertexChain = NULL;
	Vertex* aVertex = NULL;

	if (vertexChainName == kFrontVertexChain) {
	
		if (this->vertexChainFront != NULL) {
			for (VertexChainIterator chainIt = this->vertexChainFront->begin(); chainIt != this->vertexChainFront->end(); chainIt++) {
				delete *chainIt;
				*chainIt = NULL;
			}
			delete this->vertexChainFront;
			this->vertexChainFront = NULL;
		}
		
		this->vertexChainFront = new VertexChain;

		aVertex = this->createTopLeftFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainFront->push_back(aVertex);

		aVertex = this->createBottomLeftFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainFront->push_back(aVertex);

		aVertex = this->createBottomRightFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainFront->push_back(aVertex);

		aVertex = this->createTopRightFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainFront->push_back(aVertex);

		this->vertexChainMap[kFrontVertexChain] = this->vertexChainFront;
		
		returnedVertexChain = this->vertexChainFront;
		
	} else if (vertexChainName == kTopVertexChain) {

		if (this->vertexChainTop != NULL) {
			for (VertexChainIterator chainIt = this->vertexChainTop->begin(); chainIt != this->vertexChainTop->end(); chainIt++) {
				delete *chainIt;
				*chainIt = NULL;
			}
			delete this->vertexChainTop;
			this->vertexChainTop = NULL;
		}
		
		this->vertexChainTop = new VertexChain;

		aVertex = this->createTopLeftFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainTop->push_back(aVertex);

		aVertex = this->createTopRightFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainTop->push_back(aVertex);
		
		aVertex = this->createTopRightBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainTop->push_back(aVertex);

		aVertex = this->createTopLeftBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainTop->push_back(aVertex);

		this->vertexChainMap[kTopVertexChain] = this->vertexChainTop;
		
		returnedVertexChain = this->vertexChainTop;

	} else if (vertexChainName == kLeftVertexChain) {

		if (this->vertexChainLeft != NULL) {
			for (VertexChainIterator chainIt = this->vertexChainLeft->begin(); chainIt != this->vertexChainLeft->end(); chainIt++) {
				delete *chainIt;
				*chainIt = NULL;
			}
			delete this->vertexChainLeft;
			this->vertexChainLeft = NULL;
		}
		
		this->vertexChainLeft = new VertexChain;

		aVertex = this->createBottomLeftBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainLeft->push_back(aVertex);

		aVertex = this->createBottomLeftFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainLeft->push_back(aVertex);
		
		aVertex = this->createTopLeftFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainLeft->push_back(aVertex);

		aVertex = this->createTopLeftBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainLeft->push_back(aVertex);

		this->vertexChainMap[kLeftVertexChain] = this->vertexChainLeft;
		
		returnedVertexChain = this->vertexChainLeft;

	} else if (vertexChainName == kRightVertexChain) {

		if (this->vertexChainRight != NULL) {
			for (VertexChainIterator chainIt = this->vertexChainRight->begin(); chainIt != this->vertexChainRight->end(); chainIt++) {
				delete *chainIt;
				*chainIt = NULL;
			}
			delete this->vertexChainRight;
			this->vertexChainRight = NULL;
		}
		
		this->vertexChainRight = new VertexChain;

		aVertex = this->createBottomRightFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainRight->push_back(aVertex);

		aVertex = this->createBottomRightBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainRight->push_back(aVertex);
		
		aVertex = this->createTopRightBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainRight->push_back(aVertex);

		aVertex = this->createTopRightFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainRight->push_back(aVertex);

		this->vertexChainMap[kRightVertexChain] = this->vertexChainRight;
		
		returnedVertexChain = this->vertexChainRight;

	} else if (vertexChainName == kBackVertexChain) {

		if (this->vertexChainBack != NULL) {
			for (VertexChainIterator chainIt = this->vertexChainBack->begin(); chainIt != this->vertexChainBack->end(); chainIt++) {
				delete *chainIt;
				*chainIt = NULL;
			}
			delete this->vertexChainBack;
			this->vertexChainBack = NULL;
		}
		
		this->vertexChainBack = new VertexChain;

		aVertex = this->createBottomRightBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainBack->push_back(aVertex);

		aVertex = this->createBottomLeftBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainBack->push_back(aVertex);
		
		aVertex = this->createTopLeftBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainBack->push_back(aVertex);

		aVertex = this->createTopRightBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainBack->push_back(aVertex);

		this->vertexChainMap[kBackVertexChain] = this->vertexChainBack;
		
		returnedVertexChain = this->vertexChainBack;

	} else if (vertexChainName == kBottomVertexChain) {

		if (this->vertexChainBottom != NULL) {
			for (VertexChainIterator chainIt = this->vertexChainBottom->begin(); chainIt != this->vertexChainBottom->end(); chainIt++) {
				delete *chainIt;
				*chainIt = NULL;
			}
			delete this->vertexChainBottom;
			this->vertexChainBottom = NULL;
		}
		
		this->vertexChainBottom = new VertexChain;

		aVertex = this->createBottomLeftBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainBottom->push_back(aVertex);

		aVertex = this->createBottomRightBackVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = 0.0f;
		this->vertexChainBottom->push_back(aVertex);
		
		aVertex = this->createBottomRightFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = this->image->getLogicalWidth();
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainBottom->push_back(aVertex);

		aVertex = this->createBottomLeftFrontVertex(scaleFactor);
		aVertex->texCoordPosition.sPos = 0.0f;
		aVertex->texCoordPosition.tPos = this->image->getLogicalHeight();
		this->vertexChainBottom->push_back(aVertex);

		this->vertexChainMap[kBottomVertexChain] = this->vertexChainBottom;
		
		returnedVertexChain = this->vertexChainBottom;

	}
	
	return returnedVertexChain;
	
}


VertexChain* VisualAsset::getVertexChain(VertexChainName vertexChainName) {
	VertexChain* vertexChainRef = NULL;
	VertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		vertexChainRef = mapIt->second;
	} else {
		char errLog[64];
		sprintf(errLog, "unable to find vertex chain to return: %d\n", vertexChainName);
		writeLog(errLog);
	}
	return vertexChainRef;
}


bool VisualAsset::hasVertexChain(VertexChainName vertexChainName) {
	bool result = false;
	VertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		result = true;
	}
	return result;
}


void VisualAsset::removeVertexChain(VertexChainName vertexChainName) {
	VertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		for (VertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			delete *chainIt;
			*chainIt = NULL;
		}
		mapIt->second->clear();
		mapIt->second = NULL;
		this->vertexChainMap.erase(mapIt);
	}
}


Vertex* VisualAsset::createTopLeftFrontVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getLeftCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getTopCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getFrontPosition();
	aVertex->vertexColor.r = this->topLeftFrontVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->topLeftFrontVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->topLeftFrontVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->topLeftFrontVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createTopLeftBackVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getLeftCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getTopCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getBackPosition();
	aVertex->vertexColor.r = this->topLeftBackVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->topLeftBackVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->topLeftBackVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->topLeftBackVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createBottomLeftFrontVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getLeftCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getBottomCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getFrontPosition();
	aVertex->vertexColor.r = this->bottomLeftFrontVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->bottomLeftFrontVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->bottomLeftFrontVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->bottomLeftFrontVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createBottomLeftBackVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getLeftCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getBottomCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getBackPosition();
	aVertex->vertexColor.r = this->bottomLeftBackVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->bottomLeftBackVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->bottomLeftBackVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->bottomLeftBackVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createBottomRightFrontVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getRightCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getBottomCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getFrontPosition();
	aVertex->vertexColor.r = this->bottomRightFrontVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->bottomRightFrontVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->bottomRightFrontVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->bottomRightFrontVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createBottomRightBackVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getRightCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getBottomCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getBackPosition();
	aVertex->vertexColor.r = this->bottomRightBackVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->bottomRightBackVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->bottomRightBackVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->bottomRightBackVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createTopRightFrontVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getRightCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getTopCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getFrontPosition();
	aVertex->vertexColor.r = this->topRightFrontVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->topRightFrontVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->topRightFrontVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->topRightFrontVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


Vertex* VisualAsset::createTopRightBackVertex(double scaleFactor) {
	Vertex* aVertex = new Vertex;
	aVertex->vertexPosition.xPos = this->stageBox->getRightCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.yPos = this->stageBox->getTopCoord() * static_cast<float>(scaleFactor);
	aVertex->vertexPosition.zPos = this->stageBox->getBackPosition();
	aVertex->vertexColor.r = this->topRightBackVertexColor.r * this->opacityValue;
	aVertex->vertexColor.g = this->topRightBackVertexColor.g * this->opacityValue;
	aVertex->vertexColor.b = this->topRightBackVertexColor.b * this->opacityValue;
	aVertex->vertexColor.a = this->topRightBackVertexColor.a * this->opacityValue;
	aVertex->texCoordPosition.sPos = 0.0f;
	aVertex->texCoordPosition.tPos = 0.0f;
	return aVertex;
}


void VisualAsset::draw(VertexChainName vertexChainName) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->rotateMatrix(this->rotationAngle, 0.0f, 1.0f, 0.0f);
	if (this->image != NULL) {
		this->image->draw(this->getVertexChain(vertexChainName));
	}
}


void VisualAsset::setTopLeftFrontVertexColor(VertexColor vertexColor) {
	this->topLeftFrontVertexColor = vertexColor;
}


void VisualAsset::setTopLeftBackVertexColor(VertexColor vertexColor) {
	this->topLeftBackVertexColor = vertexColor;
}


void VisualAsset::setBottomLeftFrontVertexColor(VertexColor vertexColor) {
	this->bottomLeftFrontVertexColor = vertexColor;
}


void VisualAsset::setBottomLeftBackVertexColor(VertexColor vertexColor) {
	this->bottomLeftBackVertexColor = vertexColor;
}


void VisualAsset::setBottomRightFrontVertexColor(VertexColor vertexColor) {
	this->bottomRightFrontVertexColor = vertexColor;
}


void VisualAsset::setBottomRightBackVertexColor(VertexColor vertexColor) {
	this->bottomRightBackVertexColor = vertexColor;
}


void VisualAsset::setTopRightFrontVertexColor(VertexColor vertexColor) {
	this->topRightFrontVertexColor = vertexColor;
}


void VisualAsset::setTopRightBackVertexColor(VertexColor vertexColor) {
	this->topRightBackVertexColor = vertexColor;
}


void VisualAsset::setScaleFactor(double aScaleFactor) {
	this->stageBox->setScaleFactor(aScaleFactor);
	this->generateVertexChain(kFrontVertexChain, aScaleFactor);
}


void VisualAsset::setOpacityValue(VertexChainName vertexChainName, float anOpacityValue) {

	this->opacityValue = anOpacityValue;

	UInt16 count = 0;
	VertexChainMapIterator mapIt = this->vertexChainMap.find(vertexChainName);
	if (mapIt != this->vertexChainMap.end()) {
		for (VertexChainIterator chainIt = mapIt->second->begin(); chainIt != mapIt->second->end(); chainIt++) {
			switch (count) {
				case 0:
					(*chainIt)->vertexColor.r = this->topLeftFrontVertexColor.r * this->opacityValue;
					(*chainIt)->vertexColor.g = this->topLeftFrontVertexColor.g * this->opacityValue;
					(*chainIt)->vertexColor.b = this->topLeftFrontVertexColor.b * this->opacityValue;
					(*chainIt)->vertexColor.a = this->topLeftFrontVertexColor.a * this->opacityValue;
					break;
				case 1:
					(*chainIt)->vertexColor.r = this->bottomLeftFrontVertexColor.r * this->opacityValue;
					(*chainIt)->vertexColor.g = this->bottomLeftFrontVertexColor.g * this->opacityValue;
					(*chainIt)->vertexColor.b = this->bottomLeftFrontVertexColor.b * this->opacityValue;
					(*chainIt)->vertexColor.a = this->bottomLeftFrontVertexColor.a * this->opacityValue;
					break;
				case 2:
					(*chainIt)->vertexColor.r = this->bottomRightFrontVertexColor.r * this->opacityValue;
					(*chainIt)->vertexColor.g = this->bottomRightFrontVertexColor.g * this->opacityValue;
					(*chainIt)->vertexColor.b = this->bottomRightFrontVertexColor.b * this->opacityValue;
					(*chainIt)->vertexColor.a = this->bottomRightFrontVertexColor.a * this->opacityValue;
					break;
				case 3:
					(*chainIt)->vertexColor.r = this->topRightFrontVertexColor.r * this->opacityValue;
					(*chainIt)->vertexColor.g = this->topRightFrontVertexColor.g * this->opacityValue;
					(*chainIt)->vertexColor.b = this->topRightFrontVertexColor.b * this->opacityValue;
					(*chainIt)->vertexColor.a = this->topRightFrontVertexColor.a * this->opacityValue;
					break;
				default:
					writeLog("unknown count in VisualAsset::setOpacityValue");
			}
			count++;
		}
	}
}


float VisualAsset::getCurrOpacityValue(VertexChainName vertexChainName) {
	return this->opacityValue;
}


void VisualAsset::setRotationAngleValue(float aRotationAngle) {
	this->rotationAngle = aRotationAngle;
}


float VisualAsset::getCurrRotationAngleValue() {
	return this->rotationAngle;
}


const VisualItemIdentifier* const VisualAsset::addAnimation(VisualAnimation& anAnimation, AnimationAttachmentMode animationAttachmentMode) {

	if (anAnimation.getAnimatedProperty() != kCustomAnimatedProperty) {
		VisualStageControl::removeVisualAnimationsWithOwnerIdentifier(*this->assetIdentifier, anAnimation.getAnimatedProperty());
	}

	switch (animationAttachmentMode) {
		case kContinueWithCurrentValue:
			if (anAnimation.getAnimatedProperty() == kAnimatedOpacity) {
				anAnimation.setCurrentValue(this->opacityValue);
			}
			break;
		case kIgnoreCurrentValue:
			break;
		case kContinueWithCurrentValueWithPossibleDelay:
			if (anAnimation.getAnimatedProperty() == kAnimatedOpacity) {
				anAnimation.setCurrentValue(this->opacityValue);
				
				double currStartValue = anAnimation.getStartValue();
				double currEndValue = anAnimation.getEndValue();
				double distance = 0.0;
				UInt32 startDelayInMilliseconds = 0;
				UInt32 durationInMilliseconds = anAnimation.getDurationInMilliseconds();
				if (currStartValue < currEndValue) {
					distance = currEndValue - currStartValue;
				} else {
					distance = currStartValue - currEndValue;
				}
				VisualTimeline* aTimeline = anAnimation.getTimeline();
				if (distance != 0.0) {
					// prevent divison by zero
					if (aTimeline->getMovingDirection() == kForward) {
						if (currStartValue < currEndValue) {
							startDelayInMilliseconds = (UInt32)((double)durationInMilliseconds * ((this->opacityValue - currStartValue) / distance));
						} else {
							startDelayInMilliseconds = (UInt32)((double)durationInMilliseconds * ((currStartValue - this->opacityValue) / distance));
						}
					} else if (aTimeline->getMovingDirection() == kBackward) {
						if (currStartValue < currEndValue) {
							startDelayInMilliseconds = (UInt32)((double)durationInMilliseconds * ((currEndValue - this->opacityValue) / distance));
						} else {
							startDelayInMilliseconds = (UInt32)((double)durationInMilliseconds * ((this->opacityValue - currEndValue) / distance));
						}
					}
				}

				anAnimation.setStartDelayInMilliseconds(startDelayInMilliseconds);
			}
			break;
		default:
			// null
			break;
	}
	
	if (anAnimation.getAnimatedProperty() == kAnimatedOpacity) {
		anAnimation.setAnimateCallbackFunctionPtr(VisualAsset::animateOpacity, this);
	} else if (anAnimation.getAnimatedProperty() == kAnimatedRotation) {
		anAnimation.setAnimateCallbackFunctionPtr(VisualAsset::animateRotation, this);
	} else if (anAnimation.getAnimatedProperty() == kAnimatedSize) {
		anAnimation.setAnimateCallbackFunctionPtr(VisualAsset::animateSize, this);
	}
	
	return VisualStageControl::addVisualAnimation(anAnimation, *this->assetIdentifier);
	
}


void VisualAsset::removeAnimations(AnimatedProperty anAnimatedProperty) {
	VisualStageControl::removeVisualAnimationsWithOwnerIdentifier(*this->assetIdentifier, anAnimatedProperty);
}


OSStatus VisualAsset::animateOpacity(double currentPosition, void* userData) {
	OSStatus status = noErr;
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(kFrontVertexChain, (float)currentPosition);
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(kBackVertexChain, (float)currentPosition);
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(kTopVertexChain, (float)currentPosition);
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(kBottomVertexChain, (float)currentPosition);
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(kLeftVertexChain, (float)currentPosition);
	reinterpret_cast<VisualAsset*>(userData)->setOpacityValue(kRightVertexChain, (float)currentPosition);
	return status;
}


OSStatus VisualAsset::animateRotation(double currentPosition, void* userData) {
	OSStatus status = noErr;
	float rotationVal = (float)currentPosition * 360.0f;
	reinterpret_cast<VisualAsset*>(userData)->setRotationAngleValue(rotationVal);
	return status;
}


OSStatus VisualAsset::animateSize(double currentPosition, void* userData) {
	OSStatus status = noErr;
	reinterpret_cast<VisualAsset*>(userData)->setScaleFactor(currentPosition);
	return status;
}
