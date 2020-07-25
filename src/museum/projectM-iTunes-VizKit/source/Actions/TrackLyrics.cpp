/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: TrackLyrics.cpp
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

#include "TrackLyrics.h"
#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualDispatch.h"
#include "VisualDataStore.h"
#include "VisualPreferences.h"
#include "VisualStyledString.h"
#include "VisualAnimation.h"
#include "VisualTimeline.h"

#include "VisualFile.h" // test


using namespace VizKit;


TrackLyrics::TrackLyrics() {
	trackLyricsAsset.setOpacityValue(0.0);
}


TrackLyrics::~TrackLyrics() {
	// NULL
}


void TrackLyrics::show() {
	this->trackLyricsAsset.draw(this->vertexChainId);
}


VisualStringStyle TrackLyrics::getTrackLyricsStringStyle() {

	VisualStringStyle stringStyle;
	
	VisualPreferences::getValue(VisualPreferences::kTrackLyricsFont, stringStyle.fontNameStr);
	
	stringStyle.fontSize = (float)(VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontSize));

	stringStyle.fontColor.r = VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontColorRedFloat);
	stringStyle.fontColor.g = VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontColorGreenFloat);
	stringStyle.fontColor.b = VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontColorBlueFloat);
	stringStyle.fontColor.red = VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontColorRedFloat);
	stringStyle.fontColor.green = VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontColorGreenFloat);
	stringStyle.fontColor.blue = VisualPreferences::getValue(VisualPreferences::kTrackLyricsFontColorBlueFloat);
	
	stringStyle.horizontalAlignment = kLeftAligned;
	
	return stringStyle;
}


void TrackLyrics::setTrackLyricsImage(VisualImage& styledTrackLyricsStringImage) {

	// test
	/*
	VisualFile testFile;
	bool success = testFile.initWithUserDesktopDirectory();
	VisualString fileName("testDaten.txt");
	success = testFile.appendFileName(fileName);
	VisualString testString;
	testString.initWithContentsOfFile(testFile);
	bool success = lyricsImage.initWithString(testString, stringStyle);
	*/
	// test

	this->clear();
	this->trackLyricsAsset.setImage(styledTrackLyricsStringImage);

/*
	VisualFile aFile;
	aFile.initWithUserDesktopDirectory();
	VisualString fileName("testLyrics.png");
	bool success = false;
	success = aFile.appendFileName(fileName);
	styledTrackLyricsStringImage.writeToPNGFile(aFile);
*/
}


void TrackLyrics::reshape() {

	VisualStageBox* trackLyricsAssetBox = this->trackLyricsAsset.getBox();
	trackLyricsAssetBox->setScalingBehaviour(kPreserveAspectRatio);

	VisualStagePosition trackLyricsAssetPosition = this->trackLyricsAsset.getPosition();
	
	trackLyricsAssetPosition.reset();
	
	trackLyricsAssetPosition.horizontalAlignment = kCenterAligned;
	trackLyricsAssetPosition.verticalAlignment = kTopAligned;
	
	trackLyricsAssetPosition.minMarginBottom = (double)VisualDataStore::getValueInt(VisualDataStore::kTrackInfoTextureHeight) + 5.0;
	trackLyricsAssetPosition.minMarginBottomUnit = kPixel;

	trackLyricsAssetPosition.minMarginTop = 5;
	trackLyricsAssetPosition.minMarginTopUnit = kPixel;

	trackLyricsAssetPosition.minMarginLeft = 5;
	trackLyricsAssetPosition.minMarginLeftUnit = kPixel;

	trackLyricsAssetPosition.minMarginRight = 5;
	trackLyricsAssetPosition.minMarginRightUnit = kPixel;

	this->trackLyricsAsset.setPosition(trackLyricsAssetPosition);

	VisualVertex* aVertex = NULL;
	trackLyricsAssetBox->initializeVertexChain(this->vertexChainId);

	VertexColor aVertexColor;
	for (uint8 i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopLeftRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopLeftGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopLeftBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopLeftAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackLyricsAssetBox->createVertex(0.0, 1.0, 0.0, 0.0, 1.0, aVertexColor);
				trackLyricsAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			case 1:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomLeftRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomLeftGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomLeftBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomLeftAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackLyricsAssetBox->createVertex(0.0, 0.0, 0.0, 0.0, 0.0, aVertexColor);
				trackLyricsAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			case 2:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomRightRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomRightGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomRightBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorBottomRightAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackLyricsAssetBox->createVertex(1.0, 0.0, 0.0, 1.0, 0.0, aVertexColor);
				trackLyricsAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			case 3:
				aVertexColor.r = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopRightRed);
				aVertexColor.g = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopRightGreen);
				aVertexColor.b = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopRightBlue);
				aVertexColor.a = VisualPreferences::getValue(VisualPreferences::kTrackLyricsTextureColorTopRightAlpha);
				aVertexColor.red = aVertexColor.r;
				aVertexColor.green = aVertexColor.g;
				aVertexColor.blue = aVertexColor.b;
				aVertexColor.alpha = aVertexColor.a;
				aVertex = trackLyricsAssetBox->createVertex(1.0, 1.0, 0.0, 1.0, 1.0, aVertexColor);
				trackLyricsAssetBox->addVertexToChain(this->vertexChainId, aVertex);
				break;
			default:
				writeLog("ERR: switch case unknown");
		}
	}

	char trackLyricsLayoutPos[128];
	sprintf(trackLyricsLayoutPos, "trackLyrics top: %f, left: %f, bottom: %f, right: %f", trackLyricsAssetBox->getTopCoord(), trackLyricsAssetBox->getLeftCoord(), trackLyricsAssetBox->getBottomCoord(), trackLyricsAssetBox->getRightCoord());
	setProcessInfo("TrackLyricsLayout", trackLyricsLayoutPos);

}


void TrackLyrics::fadeIn(uint32 durationInMilliseconds) {
	VisualAnimation fadeInAnimation(kAnimatedOpacity);
	fadeInAnimation.setDurationInMilliseconds(durationInMilliseconds);
	this->trackLyricsAsset.addAnimation(fadeInAnimation);
}


void TrackLyrics::fadeOut(uint32 durationInMilliseconds, float clampValue) {
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setDurationInMilliseconds(durationInMilliseconds);
	fadeOutAnimation.setStartValue(1.0);
	fadeOutAnimation.setStopValue(0.0);
	//VisualTimeline* fadeOutTimeline = fadeOutAnimation.getTimeline();
	//fadeOutTimeline->setInterpolationType(kSinusoidalInterpolation);
	if (clampValue > 0.0) {
		//fadeOutAnimation.setMinClampValue(clampValue);
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
	
	this->trackLyricsAsset.setOpacityValue(0.0);
}
