/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: CoverArt.cpp
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

#include "CoverArt.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualPreferences.h"
#include "VisualAnimation.h"
#include "VisualTimeline.h"
#include "VisualConvolutionFilter.h"
#include "VisualFile.h"
#include "VisualString.h"
#include "VisualCamera.h"



using namespace VizKit;


CoverArt::CoverArt() {
	coverArtAsset.setOpacityValue(0.0);
}


CoverArt::~CoverArt() {
	// null
}


void CoverArt::prepareCoverAction() {
	VisualActorGraphics::enableDepthTest();
	double distance = this->coverArtAsset.getCamera().getMaxFarPos() - this->coverArtAsset.getCamera().getMaxNearPos();
	double distanceCenter = distance / 2.0 * -1.0;
	VisualStageBox* coverArtAssetBox = this->coverArtAsset.getBox();
	double stageBoxDepth = coverArtAssetBox->getCoordDepth();
	VisualActorGraphics::translateMatrix(0.0, 0.0, distanceCenter - this->coverArtAsset.getCamera().getMaxNearPos() - (stageBoxDepth / 2.0));
}


void CoverArt::showCover() {
	this->coverArtAsset.draw(this->vertexChainId);
}


void CoverArt::finishCoverAction() {
	VisualActorGraphics::disableDepthTest();
}


void CoverArt::setAlbumCoverArtWorkImage(VisualImage& albumCoverArtworkImage) {
	this->coverArtAsset.setImage(albumCoverArtworkImage);
/*
	VisualFile aFile;
	aFile.initWithUserDesktopDirectory();
	VisualString fileName("testCoverArt.png");
	bool success = false;
	success = aFile.appendFileName(fileName);
	albumCoverArtworkImage.writeToPNGFile(aFile);
*/
}


void CoverArt::reshape() {

	VisualCamera aCamera;
	aCamera.setPerspectiveProjection(3.0);
	this->coverArtAsset.setCamera(aCamera);

	VisualStagePosition coverArtAssetPosition = this->coverArtAsset.getPosition();

	coverArtAssetPosition.reset();
	
	VisualStageBox* coverArtAssetBox = this->coverArtAsset.getBox();
	
	double coordDepth = this->calcCoordDepth();
		
	coverArtAssetPosition.depthAlignment = kDepthCenterAligned;
	coverArtAssetBox->setCoordDepth(coordDepth);

	coverArtAssetPosition.horizontalAlignment = kCenterAligned;
	coverArtAssetPosition.verticalAlignment = kMiddleAligned;

	coverArtAssetPosition.minMarginBottom = (double)VisualDataStore::getValueInt(VisualDataStore::kTrackInfoTextureHeight) + 5.0;
	coverArtAssetPosition.minMarginBottomUnit = kPixel;

	coverArtAssetPosition.minMarginTop = 5;
	coverArtAssetPosition.minMarginTopUnit = kPixel;

	coverArtAssetPosition.minMarginLeft = 5;
	coverArtAssetPosition.minMarginLeftUnit = kPixel;

	coverArtAssetPosition.minMarginRight = 5;
	coverArtAssetPosition.minMarginRightUnit = kPixel;
	
	this->coverArtAsset.setPosition(coverArtAssetPosition);
	
	coverArtAssetBox->update(); // VisualStageBox only updates automatically on value changed, after canvas reshape event we have to update manually (before calculating scaleFactor) 
	coverArtAssetBox->setScalingBehaviour(kPreserveAspectRatio);
	
	VisualVertex* aVertex = NULL;

	coverArtAssetBox->initializeVertexChain(this->vertexChainId);
	
	aVertex = coverArtAssetBox->createVertex(0.0, 1.0, 0.5, 0.0, 1.0);
	coverArtAssetBox->addVertexToChain(this->vertexChainId, aVertex);
	
	aVertex = coverArtAssetBox->createVertex(0.0, 0.0, 0.5, 0.0, 0.0);
	coverArtAssetBox->addVertexToChain(this->vertexChainId, aVertex);
	
	aVertex = coverArtAssetBox->createVertex(1.0, 0.0, 0.5, 1.0, 0.0);
	coverArtAssetBox->addVertexToChain(this->vertexChainId, aVertex);

	aVertex = coverArtAssetBox->createVertex(1.0, 1.0, 0.5, 1.0, 1.0);
	coverArtAssetBox->addVertexToChain(this->vertexChainId, aVertex);

}


double CoverArt::calcCoordDepth() {
	VisualStageBox* coverArtAssetBox = this->coverArtAsset.getBox();
	double ratio = 0.0;
	VisualCamera aCamera;
	aCamera.setPerspectiveProjection(3.0);
	CoordSize3D size = aCamera.getSize();
	if (size.width > size.height) {
		ratio = coverArtAssetBox->getUnscaledCoordWidth() / size.width;
	} else {
		ratio = coverArtAssetBox->getUnscaledCoordHeight() / size.height;
	}
	double zCoordAmount = size.depth;
	return zCoordAmount * ratio;
}


void CoverArt::fadeIn(uint32 durationInMilliseconds) {
	VisualAnimation fadeInAnimation(kAnimatedOpacity);
	fadeInAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->coverArtAsset.addAnimation(fadeInAnimation);
}


void CoverArt::fadeOut(uint32 durationInMilliseconds, double stopValue) {
	double distance = 1.0;
	AnimationSpeed animationSpeed = VisualAnimation::calcSpeed(distance, durationInMilliseconds);
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setSpeed(animationSpeed);
	fadeOutAnimation.setStopValue(stopValue);
	this->coverArtAsset.addAnimation(fadeOutAnimation);
}


void CoverArt::rotate() {
	VisualAnimation rotateAnimation(kAnimatedRotation);
	uint32 durationInMilliseconds = 9000;
	rotateAnimation.setDurationInMilliseconds(durationInMilliseconds);
	rotateAnimation.setLoopMode(kLoop, kInfiniteRepetition);
	this->coverArtAsset.addAnimation(rotateAnimation);
}


void CoverArt::scaleSize(uint32 durationInMilliseconds, double from, double to) {
	VisualAnimation sizeAnimation(kAnimatedSize);
	sizeAnimation.setStartValue(from);
	sizeAnimation.setStopValue(to);
	sizeAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->coverArtAsset.addAnimation(sizeAnimation);
}


void CoverArt::clear() {

	VisualStageBox* coverArtAssetBox = this->coverArtAsset.getBox();
	
	coverArtAssetBox->setContentPixelWidth(0);
	coverArtAssetBox->setContentPixelHeight(0);

	VisualStagePosition coverArtAssetPosition = this->coverArtAsset.getPosition();
	coverArtAssetPosition.reset();
	this->coverArtAsset.setPosition(coverArtAssetPosition);
	
	this->coverArtAsset.removeAnimations();
	
	this->coverArtAsset.setOpacityValue(0.0);
}
