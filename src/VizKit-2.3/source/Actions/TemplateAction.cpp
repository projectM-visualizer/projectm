/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TemplateAction.cpp
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

#include "TemplateAction.h"
#include "VisualActorGraphics.h"
#include "VisualColorTools.h"
#include "VisualAsset.h"
#include "VisualCamera.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualTextureContainer.h"
#include "VisualAnimationPackage.h"

using namespace VizKit;


TemplateAction::TemplateAction() {

	templateAsset = new VisualAsset;
	testAsset = new VisualAsset;

	uint32 width = 256;
	uint32 height = 256;
	uint32* argbPixels = VisualColorTools::createARGBCheckPixels(width, height);
	
	VisualImage* anImage = VisualImage::createWithARGBPixelData(argbPixels, width, height);
	
	free(argbPixels);
	argbPixels = NULL;

	VisualCamera aCamera;
	aCamera.setPerspectiveProjection(3.0);
	//aCamera.setOrthographicProjection();
	templateAsset->setCamera(aCamera);

	templateAsset->setImage(*anImage);
	delete anImage;
	
	VisualStageBox* templateAssetBox = templateAsset->getBox();
	
	//templateAsset->setDebugMode(true);
	
	
	templateAssetBox->initializeVertexChain(vertexChainId);

	VisualVertex* aVertex = NULL;
	aVertex = templateAssetBox->createVertex(0.0, 0.0, 1.0, 0.0, 1.0);
	templateAssetBox->addVertexToChain(vertexChainId, aVertex);

	aVertex = templateAssetBox->createVertex(0.0, 0.0, 0.0, 0.0, 0.0);
	templateAssetBox->addVertexToChain(vertexChainId, aVertex);

	aVertex = templateAssetBox->createVertex(1.0, 0.0, 0.0, 1.0, 0.0);
	templateAssetBox->addVertexToChain(vertexChainId, aVertex);

	aVertex = templateAssetBox->createVertex(1.0, 0.0, 1.0, 1.0, 1.0);
	templateAssetBox->addVertexToChain(vertexChainId, aVertex);

	VisualStagePosition testAssetPosition = this->testAsset->getPosition();
	testAssetPosition.horizontalAlignment = kRightAligned;
	this->testAsset->setPosition(testAssetPosition);
}


TemplateAction::~TemplateAction() {
	delete templateAsset;
}


void TemplateAction::setImage(const VisualImage& anImage) {
	testAsset->setImage(anImage);
}


void TemplateAction::removeImage() {
	testAsset->removeImage();
}


void TemplateAction::show() {
	//VisualActorGraphics::translateMatrix(0.0, 0.0, -3.0);
	
	templateAsset->draw();
	VisualActorGraphics::drawProjectionMetrics(templateAsset->getCamera());
	if (testAsset->hasImage()) {
		testAsset->draw();
	}
}
