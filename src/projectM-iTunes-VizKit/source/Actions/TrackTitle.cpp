/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: TrackTitle.cpp
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

#include "TrackTitle.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualString.h"
#include "VisualStringStyle.h"
#include "VisualFile.h"
#include "VisualTimeline.h"
#include "VisualAnimation.h"
#include "VisualConvolutionFilter.h"



using namespace VizKit;


TrackTitle::TrackTitle() {
	trackInfoAsset.setOpacityValue(kFrontVertexChain, 0.0f);
}


TrackTitle::~TrackTitle() {
}


void TrackTitle::show() {
	this->trackInfoAsset.draw(kFrontVertexChain);
}


OSStatus TrackTitle::makeTextureOfTrackTitle(const VisualString& trackName) {

	OSStatus status = noErr;
	VisualStringStyle stringStyle;
	
	VisualDataStore::getPreferenceValueChar(VisualConfiguration::kTrackInfoFont, stringStyle.fontNameStr);
	
	stringStyle.fontSize = (float)(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kTrackInfoFontSize));
	
	stringStyle.fontColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorRedFloat);
	stringStyle.fontColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorGreenFloat);
	stringStyle.fontColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoFontColorBlueFloat);
	
	stringStyle.horizontalAlignment = kLeftAligned;
	
	this->clear();
	
	VisualImage trackInfoImage;
	status = trackInfoImage.initWithString(trackName, stringStyle);

	if (status == noErr) {

		VisualConvolutionFilter aConvolutionFilter(VisualConvolutionFilter::kBlur);
		trackInfoImage.applyConvolutionFilter(aConvolutionFilter);

/*
		VisualFile outputFile;
		outputFile.initWithUserDesktopDirectory();
		VisualString aFileName = VisualString("vizKitTrackTitleImage.png");
		outputFile.appendFileName(aFileName);

		status = trackInfoImage.writeToFile(outputFile);
		if (status != noErr) {
			printf("err %ld in TrackTitle::makeTextureOfTrackTitle()\n", status);
		}
*/

		this->trackInfoAsset.setImage(trackInfoImage);

		VisualStageBox* trackInfoAssetBox = this->trackInfoAsset.getBox();
		trackInfoAssetBox->setContentPixelWidth(trackInfoImage.getWidth());
		trackInfoAssetBox->setContentPixelHeight(trackInfoImage.getHeight());

	}

	return status;
}


UInt16 TrackTitle::getTrackInfoTextureHeightInPixels() {
	UInt16 heightOfTrackInfoTextureInPixels = 0;
	VisualStageBox* trackInfoAssetBox = this->trackInfoAsset.getBox();
	heightOfTrackInfoTextureInPixels = VisualActorGraphics::yCoordToPixel(trackInfoAssetBox->getTopCoord()) - VisualActorGraphics::yCoordToPixel(trackInfoAssetBox->getBottomCoord());
	return heightOfTrackInfoTextureInPixels;
}


void TrackTitle::calcPositionOnScreen() {

	VisualStageBox* trackInfoAssetBox = this->trackInfoAsset.getBox();
	trackInfoAssetBox->setScalingBehaviour(kPreserveAspectRatio);

	VisualStagePosition trackInfoAssetPosition = this->trackInfoAsset.getPosition();
	
	trackInfoAssetPosition.reset();
	
	trackInfoAssetPosition.horizontalAlignment = kCenterAligned;
	trackInfoAssetPosition.verticalAlignment = kBottomAligned;

	trackInfoAssetPosition.minMarginBottom = 10;
	trackInfoAssetPosition.minMarginBottomUnit = kPixel;	

	this->trackInfoAsset.setPosition(trackInfoAssetPosition);
	
	VertexColor aVertexColor;
	for (UInt8 i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopLeftAlpha);
				this->trackInfoAsset.setTopLeftFrontVertexColor(aVertexColor);
				break;
			case 1:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomLeftAlpha);
				this->trackInfoAsset.setBottomLeftFrontVertexColor(aVertexColor);
				break;
			case 2:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorBottomRightAlpha);
				this->trackInfoAsset.setBottomRightFrontVertexColor(aVertexColor);
				break;
			case 3:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackInfoTextureColorTopRightAlpha);
				this->trackInfoAsset.setTopRightFrontVertexColor(aVertexColor);
				break;
			default:
				writeLog("ERR: switch case unknown");
		}
	}
	
	this->trackInfoAsset.generateVertexChain(kFrontVertexChain);

	char trackLayoutPos[128];
	sprintf(trackLayoutPos, "top: %f, left: %f, bottom: %f, right: %f", trackInfoAssetBox->getTopCoord(), trackInfoAssetBox->getLeftCoord(), trackInfoAssetBox->getBottomCoord(), trackInfoAssetBox->getRightCoord());
	setProcessInfo("TrackLayout", trackLayoutPos);

}


void TrackTitle::fadeIn(UInt32 durationInMilliseconds) {
	VisualAnimation fadeInAnimation(kAnimatedOpacity);
	fadeInAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->trackInfoAsset.addAnimation(fadeInAnimation);
}


void TrackTitle::fadeOut(UInt32 durationInMilliseconds) {
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setDurationInMilliseconds(durationInMilliseconds);
	fadeOutAnimation.setStartValue(1.0);
	fadeOutAnimation.setEndValue(0.0);
	VisualTimeline* fadeOutTimeline = fadeOutAnimation.getTimeline();
	fadeOutTimeline->setInterpolationType(kSinusoidalInterpolation);
	this->trackInfoAsset.addAnimation(fadeOutAnimation);
}


void TrackTitle::pulsate() {
	VisualAnimation pulsateAnimation(kAnimatedOpacity);
	UInt32 durationInMilliseconds = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeOutTimeOnPauseInMS);
	pulsateAnimation.setDurationInMilliseconds(durationInMilliseconds);
	pulsateAnimation.setLoopMode(kMirroredLoop, kInfiniteRepetition);
	this->trackInfoAsset.addAnimation(pulsateAnimation);
}


void TrackTitle::clear() {

	VisualStageBox* trackInfoAssetBox = this->trackInfoAsset.getBox();
	
	trackInfoAssetBox->setContentPixelWidth(0);
	trackInfoAssetBox->setContentPixelHeight(0);

	VisualStagePosition trackInfoAssetPosition = this->trackInfoAsset.getPosition();
	trackInfoAssetPosition.reset();
	this->trackInfoAsset.setPosition(trackInfoAssetPosition);
	
	this->trackInfoAsset.removeAnimations();
	
	this->trackInfoAsset.setOpacityValue(kFrontVertexChain, 0.0f);
}
