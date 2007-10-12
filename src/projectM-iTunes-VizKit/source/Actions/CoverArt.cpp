/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: CoverArt.cpp
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

#include "CoverArt.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualAnimation.h"
#include "VisualTimeline.h"
#include "VisualConvolutionFilter.h"
#include "VisualFile.h"
#include "VisualString.h"



using namespace VizKit;


CoverArt::CoverArt() {
	coverArtAsset.setOpacityValue(kFrontVertexChain, 0.0f);
}


CoverArt::~CoverArt() {
	// null
}


void CoverArt::prepareCoverAction() {
	VisualActorGraphics::prepareCoverArtAction();
}


void CoverArt::showCover() {
	this->coverArtAsset.draw(kFrontVertexChain);
}


void CoverArt::finishCoverAction() {
	VisualActorGraphics::finishCoverArtAction();
}


void CoverArt::makeImageOfCover() {
	if (VisualDataStore::getNumberOfCoverArtworksOfCurrentTrack() > 0) {

		VisualImage* coverArtImage = VisualActorGraphics::createCoverArtImage();

/*
		VisualImage* coverArtImage = new VisualImage;
		VisualFile* inputFile = VisualFile::createWithUserDesktopDirectory();
		VisualString inputFileName = VisualString("spot.png");
		inputFile->appendFileName(inputFileName);
		coverArtImage->initWithFile(*inputFile);
		delete(inputFile);
*/
		
		//VisualString url("http://www.imagomat.de/images/coverversion/screen_mac.png");
		//VisualImage* coverArtImage = VisualImage::createWithURL(url);
		
		if (coverArtImage != NULL) {

			//VisualConvolutionFilter aConvolutionFilter(VisualConvolutionFilter::kEmboss);
			//coverArtImage->applyConvolutionFilter(aConvolutionFilter);

/*
			VisualFile outputFile;
			outputFile.initWithUserDesktopDirectory();
			VisualString aFileName = VisualString("vizKitCoverArtImage.png");
			outputFile.appendFileName(aFileName);

			OSStatus status = coverArtImage->writeToFile(outputFile);
			if (status != noErr) {
				printf("err %ld in CoverArt::makeImageOfCover()\n", status);
			}
*/

			this->coverArtAsset.setImage(*coverArtImage);

			VisualStageBox* coverArtAssetBox = this->coverArtAsset.getBox();
			coverArtAssetBox->setContentPixelWidth(coverArtImage->getWidth());
			coverArtAssetBox->setContentPixelHeight(coverArtImage->getHeight());
			VisualActorGraphics::releaseCoverArtImage(&coverArtImage);
		}
	}
}


void CoverArt::calcPositionOnScreen() {

	VisualActorGraphics::prepareCoverArtAction();

	VisualStagePosition coverArtAssetPosition = this->coverArtAsset.getPosition();

	coverArtAssetPosition.reset();
	
	coverArtAssetPosition.horizontalAlignment = kCenterAligned;
	coverArtAssetPosition.verticalAlignment = kMiddleAligned;

	coverArtAssetPosition.minMarginBottom = (double)VisualDataStore::getValueInt(VisualConfiguration::kTrackInfoTextureHeight) + 5.0;
	coverArtAssetPosition.minMarginBottomUnit = kPixel;

	coverArtAssetPosition.minMarginTop = 5;
	coverArtAssetPosition.minMarginTopUnit = kPixel;

	coverArtAssetPosition.minMarginLeft = 5;
	coverArtAssetPosition.minMarginLeftUnit = kPixel;

	coverArtAssetPosition.minMarginRight = 5;
	coverArtAssetPosition.minMarginRightUnit = kPixel;
	
	this->coverArtAsset.setPosition(coverArtAssetPosition);
	
	VisualStageBox* coverArtAssetBox = this->coverArtAsset.getBox();
	coverArtAssetBox->update(); // VisualStageBox only updates automatically on value changed, after canvas reshape event we have to update manually (before calculating scaleFactor) 
	coverArtAssetBox->setScalingBehaviour(kPreserveAspectRatio);
	
	this->coverArtAsset.generateVertexChain(kFrontVertexChain);
	
	VisualActorGraphics::finishCoverArtAction();

}


void CoverArt::fadeIn(UInt32 durationInMilliseconds) {
	VisualAnimation fadeInAnimation(kAnimatedOpacity);
	fadeInAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->coverArtAsset.addAnimation(fadeInAnimation);
}


void CoverArt::fadeOut(UInt32 durationInMilliseconds, double clampValue) {
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setDurationInMilliseconds(durationInMilliseconds);
	fadeOutAnimation.setStartValue(1.0);
	fadeOutAnimation.setEndValue(0.0);
	if (clampValue > 0.0f) {
		fadeOutAnimation.setMinClampValue(clampValue);
	}
	this->coverArtAsset.addAnimation(fadeOutAnimation, kContinueWithCurrentValueWithPossibleDelay);
}


void CoverArt::pulsate() {
	VisualAnimation pulsateAnimation(kAnimatedOpacity);
	UInt32 durationInMilliseconds = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeOnPauseInMS);
	pulsateAnimation.setDurationInMilliseconds(durationInMilliseconds);
	pulsateAnimation.setLoopMode(kMirroredLoop, kInfiniteRepetition);
	this->coverArtAsset.addAnimation(pulsateAnimation);
}


void CoverArt::rotate() {
	VisualAnimation rotateAnimation(kAnimatedRotation);
	UInt32 durationInMilliseconds = 9000;
	rotateAnimation.setDurationInMilliseconds(durationInMilliseconds);
	rotateAnimation.setLoopMode(kLoop, kInfiniteRepetition);
	this->coverArtAsset.addAnimation(rotateAnimation);
}


void CoverArt::scaleSize(UInt32 durationInMilliseconds, double from, double to) {
	VisualAnimation sizeAnimation(kAnimatedSize);
	sizeAnimation.setStartValue(from);
	sizeAnimation.setEndValue(to);
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
	
	this->coverArtAsset.setOpacityValue(kFrontVertexChain, 0.0f);
}
