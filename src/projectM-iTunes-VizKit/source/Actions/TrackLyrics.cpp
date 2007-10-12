/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: TrackLyrics.cpp
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

#include "TrackLyrics.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualString.h"
#include "VisualStringStyle.h"
#include "VisualAnimation.h"
#include "VisualTimeline.h"



using namespace VizKit;


TrackLyrics::TrackLyrics() {
	trackLyricsAsset.setOpacityValue(kFrontVertexChain, 0.0f);
}


TrackLyrics::~TrackLyrics() {
	// NULL
}


void TrackLyrics::show() {
	this->trackLyricsAsset.draw(kFrontVertexChain);
}


OSStatus TrackLyrics::makeTextureOfTrackLyrics(const VisualString& trackLyrics) {

	VisualStringStyle stringStyle;
	
	VisualDataStore::getPreferenceValueChar(VisualConfiguration::kTrackLyricsFont, stringStyle.fontNameStr);
	
	stringStyle.fontSize = (float)(VisualDataStore::getPreferenceValueInt(VisualConfiguration::kTrackLyricsFontSize));

	stringStyle.fontColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorRedFloat);
	stringStyle.fontColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorGreenFloat);
	stringStyle.fontColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsFontColorBlueFloat);
	
	stringStyle.horizontalAlignment = kLeftAligned;

	this->clear();

	VisualImage lyricsImage;
	OSStatus status = lyricsImage.initWithString(trackLyrics, stringStyle);
	
	if (status == noErr) {
	
		this->trackLyricsAsset.setImage(lyricsImage);
		
		VisualStageBox* trackLyricsAssetBox = this->trackLyricsAsset.getBox();
		trackLyricsAssetBox->setContentPixelWidth(lyricsImage.getWidth());
		trackLyricsAssetBox->setContentPixelHeight(lyricsImage.getHeight());
		
	}
	
	return status;

}


void TrackLyrics::calcPositionOnScreen() {

	VisualStageBox* trackLyricsAssetBox = this->trackLyricsAsset.getBox();
	trackLyricsAssetBox->setScalingBehaviour(kPreserveAspectRatio);

	VisualStagePosition trackLyricsAssetPosition = this->trackLyricsAsset.getPosition();
	
	trackLyricsAssetPosition.reset();
	
	trackLyricsAssetPosition.horizontalAlignment = kCenterAligned;
	trackLyricsAssetPosition.verticalAlignment = kTopAligned;

	trackLyricsAssetPosition.minMarginTop = 5;
	trackLyricsAssetPosition.minMarginTopUnit = kPixel;

	trackLyricsAssetPosition.minMarginRight = 5;
	trackLyricsAssetPosition.minMarginRightUnit = kPixel;

	trackLyricsAssetPosition.minMarginLeft = 5;
	trackLyricsAssetPosition.minMarginLeftUnit = kPixel;
	
	trackLyricsAssetPosition.minMarginBottom = (double)VisualDataStore::getValueInt(VisualConfiguration::kTrackInfoTextureHeight) + 5.0;
	trackLyricsAssetPosition.minMarginBottomUnit = kPixel;

	this->trackLyricsAsset.setPosition(trackLyricsAssetPosition);
	
	VertexColor aVertexColor;
	for (UInt8 i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopLeftAlpha);
				this->trackLyricsAsset.setTopLeftFrontVertexColor(aVertexColor);
				break;
			case 1:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomLeftAlpha);
				this->trackLyricsAsset.setBottomLeftFrontVertexColor(aVertexColor);
				break;
			case 2:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorBottomRightAlpha);
				this->trackLyricsAsset.setBottomRightFrontVertexColor(aVertexColor);
				break;
			case 3:
				aVertexColor.r = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightRed);
				aVertexColor.g = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightGreen);
				aVertexColor.b = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightBlue);
				aVertexColor.a = VisualDataStore::getPreferenceValueFloat(VisualConfiguration::kTrackLyricsTextureColorTopRightAlpha);
				this->trackLyricsAsset.setTopRightFrontVertexColor(aVertexColor);
				break;
			default:
				writeLog("ERR: switch case unknown");
		}
	}

	this->trackLyricsAsset.generateVertexChain(kFrontVertexChain);

	char trackLyricsLayoutPos[128];
	sprintf(trackLyricsLayoutPos, "top: %f, left: %f, bottom: %f, right: %f", trackLyricsAssetBox->getTopCoord(), trackLyricsAssetBox->getLeftCoord(), trackLyricsAssetBox->getBottomCoord(), trackLyricsAssetBox->getRightCoord());
	setProcessInfo("TrackLyricsLayout", trackLyricsLayoutPos);

}


void TrackLyrics::fadeIn(UInt32 durationInMilliseconds) {
	VisualAnimation fadeInAnimation(kAnimatedOpacity);
	fadeInAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->trackLyricsAsset.addAnimation(fadeInAnimation);
}


void TrackLyrics::fadeOut(UInt32 durationInMilliseconds, float clampValue) {
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setDurationInMilliseconds(durationInMilliseconds);
	fadeOutAnimation.setStartValue(1.0);
	fadeOutAnimation.setEndValue(0.0);
	VisualTimeline* fadeOutTimeline = fadeOutAnimation.getTimeline();
	fadeOutTimeline->setInterpolationType(kSinusoidalInterpolation);
	if (clampValue > 0.0) {
		fadeOutAnimation.setMinClampValue(clampValue);
	}
	this->trackLyricsAsset.addAnimation(fadeOutAnimation);
}


void TrackLyrics::clear() {

	//this->trackLyricsAsset.disposeTextureData();

	VisualStageBox* trackLyricsAssetBox = this->trackLyricsAsset.getBox();
	
	trackLyricsAssetBox->setContentPixelWidth(0);
	trackLyricsAssetBox->setContentPixelHeight(0);

	VisualStagePosition trackLyricsAssetPosition = this->trackLyricsAsset.getPosition();
	trackLyricsAssetPosition.reset();
	this->trackLyricsAsset.setPosition(trackLyricsAssetPosition);
	
	this->trackLyricsAsset.removeAnimations();
	
	this->trackLyricsAsset.setOpacityValue(kFrontVertexChain, 0.0f);
}
