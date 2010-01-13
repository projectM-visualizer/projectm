/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TrackTitle.cpp
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

#include "TrackTitle.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualDispatch.h"
#include "VisualPreferences.h"
#include "VisualString.h"
#include "VisualStringStyle.h"
#include "VisualStyledString.h"
#include "VisualFile.h"
#include "VisualTimeline.h"
#include "VisualAnimation.h"
#include "VisualCamera.h"
#include "VisualConvolutionFilter.h"
#include "VisualFontManager.h"



using namespace VizKit;


TrackTitle::TrackTitle() {
	trackInfoAsset.setOpacityValue(0.0);
}


TrackTitle::~TrackTitle() {
}


void TrackTitle::show() {
	this->trackInfoAsset.draw(this->vertexChainId);
}


VisualStringStyle TrackTitle::getTrackInfoStringStyle() {

	VisualStringStyle stringStyle;
	
	VisualPreferences::getValue(VisualPreferences::kTrackInfoFont, stringStyle.fontNameStr);
	if (VisualFontManager::fontIsAvailableAsVisualizerResource(stringStyle.fontNameStr) == true) {
		VisualFontActivationResult result = VisualFontManager::activateFont(stringStyle.fontNameStr);
		if (result == fontActivationUnableToActivateFont) {
			VisualPreferences::getValue(VisualPreferences::kFallbackSystemFont, stringStyle.fontNameStr);
		}
	}
	
	stringStyle.fontSize = (float)(VisualPreferences::getValue(VisualPreferences::kTrackInfoFontSize));
	
	stringStyle.fontColor.r = VisualPreferences::getValue(VisualPreferences::kTrackInfoFontColorRedFloat);
	stringStyle.fontColor.g = VisualPreferences::getValue(VisualPreferences::kTrackInfoFontColorGreenFloat);
	stringStyle.fontColor.b = VisualPreferences::getValue(VisualPreferences::kTrackInfoFontColorBlueFloat);
	stringStyle.fontColor.red = VisualPreferences::getValue(VisualPreferences::kTrackInfoFontColorRedFloat);
	stringStyle.fontColor.green = VisualPreferences::getValue(VisualPreferences::kTrackInfoFontColorGreenFloat);
	stringStyle.fontColor.blue = VisualPreferences::getValue(VisualPreferences::kTrackInfoFontColorBlueFloat);
	
	stringStyle.horizontalAlignment = kLeftAligned;
	
	return stringStyle;
}


void TrackTitle::setTrackInfoImage(VisualImage& styledTrackInfoStringImage) {
	this->clear();
	VisualConvolutionFilter aConvolutionFilter(VisualConvolutionFilter::kBlur);
	styledTrackInfoStringImage.applyConvolutionFilter(aConvolutionFilter);
	//styledTrackInfoStringImage.setBlendMode(kReplace);
	this->trackInfoAsset.setImage(styledTrackInfoStringImage);
/*
	VisualFile aFile;
	aFile.initWithUserDesktopDirectory();
	VisualString fileName("testTrackInfo.png");
	bool success = false;
	success = aFile.appendFileName(fileName);
	styledTrackInfoStringImage.writeToPNGFile(aFile);
*/
}


uint16 TrackTitle::getTrackInfoTextureHeightInPixels() {
	uint16 heightOfTrackInfoTextureInPixels = 0;
	VisualStageBox* trackInfoAssetBox = this->trackInfoAsset.getBox();
	VisualCamera& aCamera = this->trackInfoAsset.getCamera();
	heightOfTrackInfoTextureInPixels = VisualActorGraphics::yCoordToPixel(trackInfoAssetBox->getTopCoord(), aCamera) - VisualActorGraphics::yCoordToPixel(trackInfoAssetBox->getBottomCoord(), aCamera);
	return heightOfTrackInfoTextureInPixels;
}


void TrackTitle::reshape() {

	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	this->trackInfoAsset.setCamera(aCamera);

	VisualStageBox* trackInfoAssetBox = this->trackInfoAsset.getBox();
	trackInfoAssetBox->setScalingBehaviour(kPreserveAspectRatio);

	VisualStagePosition trackInfoAssetPosition = this->trackInfoAsset.getPosition();
	
	trackInfoAssetPosition.reset();
	
	trackInfoAssetPosition.horizontalAlignment = kCenterAligned;
	trackInfoAssetPosition.verticalAlignment = kBottomAligned;
	
	this->trackInfoAsset.setPosition(trackInfoAssetPosition);

	VisualVertex* aVertex = NULL;
	trackInfoAssetBox->initializeVertexChain(this->vertexChainId);

	VertexColor aVertexColor;
	for (uint8 i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopLeftRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopLeftGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopLeftBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopLeftAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackInfoAssetBox->createVertex(0.0, 1.0, 0.0, 0.0, 1.0, aVertexColor);
				trackInfoAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			case 1:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomLeftRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomLeftGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomLeftBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomLeftAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackInfoAssetBox->createVertex(0.0, 0.0, 0.0, 0.0, 0.0, aVertexColor);
				trackInfoAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			case 2:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomRightRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomRightGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomRightBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorBottomRightAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackInfoAssetBox->createVertex(1.0, 0.0, 0.0, 1.0, 0.0, aVertexColor);
				trackInfoAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			case 3:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopRightRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopRightGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopRightBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackInfoTextureColorTopRightAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackInfoAssetBox->createVertex(1.0, 1.0, 0.0, 1.0, 1.0, aVertexColor);
				trackInfoAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			default:
				writeLog("ERR: switch case unknown");
		}
	}

	char trackLayoutPos[128];
	sprintf(trackLayoutPos, "top: %f, left: %f, bottom: %f, right: %f", trackInfoAssetBox->getTopCoord(), trackInfoAssetBox->getLeftCoord(), trackInfoAssetBox->getBottomCoord(), trackInfoAssetBox->getRightCoord());
	setProcessInfo("TrackLayout", trackLayoutPos);

}


void TrackTitle::fadeIn(uint32 durationInMilliseconds) {
	VisualAnimation fadeInAnimation(kAnimatedOpacity);
	fadeInAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->trackInfoAsset.addAnimation(fadeInAnimation);
}


void TrackTitle::fadeOut(uint32 durationInMilliseconds) {
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setDurationInMilliseconds(durationInMilliseconds);
	fadeOutAnimation.setStartValue(1.0);
	fadeOutAnimation.setStopValue(0.0);
	this->trackInfoAsset.addAnimation(fadeOutAnimation);
}


void TrackTitle::pulsate() {
	VisualAnimation pulsateAnimation(kAnimatedOpacity);
	uint32 durationInMilliseconds = VisualPreferences::getValue(VisualPreferences::kFadeOutTimeOnPauseInMS);
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
	
	this->trackInfoAsset.setOpacityValue(0.0);
}


void TrackTitle::addMoveAnimation() {
	VisualAnimation moveAnimation(kAnimatedLocation);
	uint32 durationInMilliseconds = 15000;
	moveAnimation.setDurationInMilliseconds(durationInMilliseconds);
	moveAnimation.setLoopMode(kMirroredLoop, kInfiniteRepetition);
	VisualStagePosition startPosition;
	startPosition.horizontalAlignment = kCenterAligned;
	startPosition.verticalAlignment = kBottomAligned;
	VisualStagePosition stopPosition;
	stopPosition.horizontalAlignment = kCenterAligned;
	stopPosition.verticalAlignment = kTopAligned;
	moveAnimation.setStartValue(startPosition);
	moveAnimation.setStopValue(stopPosition);
	this->trackInfoAsset.addAnimation(moveAnimation);
}
