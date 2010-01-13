/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: UpdateServiceActor.cpp
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

#include "UpdateServiceActor.h"
#include "VisualNotification.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualCamera.h"
#include "VisualString.h"
#include "VisualStyledString.h"
#include "VisualImage.h"
#include "VisualAnimationPackage.h"
#include "VisualStageBox.h"


using namespace VizKit;


UpdateServiceActor::UpdateServiceActor() {
	state = kVisActNoShow;
	strcpy(actorName, "UPDATESERVICE");
	VisualStyledString styledString;
	VisualString aString;
	aString.initWithUTF8String("Update available \nPress [i] on the keyboard for more info. Press [d] for download.");
	styledString.initWithVisualString(aString);
	VisualStringStyle stringStyle;
	stringStyle.fontSize = 30;

	stringStyle.fontColor.red = 1.0;
	stringStyle.fontColor.green = 1.0;
	stringStyle.fontColor.blue = 1.0;
	stringStyle.fontColor.alpha = 1.0;
	stringStyle.fontColor.r = 1.0;
	stringStyle.fontColor.g = 1.0;
	stringStyle.fontColor.b = 1.0;
	stringStyle.fontColor.a = 1.0;

	styledString.setStyle(stringStyle);
	VisualImage image;
	bool success = image.initWithStyledString(styledString);
	if (success) {
		updateServiceActorAsset.setImage(image);
	}

	VisualStageBox* updateServiceActorAssetBox = updateServiceActorAsset.getBox();
	
	updateServiceActorAssetBox->initializeVertexChain(frontVertexChainIdent);

	VertexColor frontVertexColor;
	double frontColor = 1.0;
	frontVertexColor.r = frontColor;
	frontVertexColor.g = frontColor;
	frontVertexColor.b = 0.0;
	frontVertexColor.a = frontColor;
	frontVertexColor.red = frontColor;
	frontVertexColor.green = frontColor;
	frontVertexColor.blue = 0.0;
	frontVertexColor.alpha = frontColor;
	VisualVertex* aVertex = NULL;
	for (uint8 i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				aVertex = updateServiceActorAssetBox->createVertex(0.0, 1.0, 0.0, 0.0, 1.0, frontVertexColor);
				updateServiceActorAssetBox->addVertexToChain(frontVertexChainIdent, aVertex);
				break;
			case 1:
				aVertex = updateServiceActorAssetBox->createVertex(0.0, 0.0, 0.0, 0.0, 0.0, frontVertexColor);
				updateServiceActorAssetBox->addVertexToChain(frontVertexChainIdent, aVertex);
				break;
			case 2:
				aVertex = updateServiceActorAssetBox->createVertex(1.0, 0.0, 0.0, 1.0, 0.0, frontVertexColor);
				updateServiceActorAssetBox->addVertexToChain(frontVertexChainIdent, aVertex);
				break;
			case 3:
				aVertex = updateServiceActorAssetBox->createVertex(1.0, 1.0, 0.0, 1.0, 1.0, frontVertexColor);
				updateServiceActorAssetBox->addVertexToChain(frontVertexChainIdent, aVertex);
				break;
			default:
				writeLog("ERR: switch case unknown");
		}
	}

	updateServiceActorAssetBox->initializeVertexChain(backgroundVertexChainIdent);

	VertexColor backgroundVertexColor;
	double bgColor = 0.3;
	backgroundVertexColor.r = bgColor;
	backgroundVertexColor.g = bgColor;
	backgroundVertexColor.b = bgColor;
	backgroundVertexColor.a = bgColor;
	backgroundVertexColor.red = bgColor;
	backgroundVertexColor.green = bgColor;
	backgroundVertexColor.blue = bgColor;
	backgroundVertexColor.alpha = bgColor;
	
	for (uint8 i = 0; i < 4; i++) {
		switch (i) {
			case 0:
				aVertex = updateServiceActorAssetBox->createVertex(0.0, 1.0, 0.0, 0.0, 1.0, backgroundVertexColor);
				updateServiceActorAssetBox->addVertexToChain(backgroundVertexChainIdent, aVertex);
				break;
			case 1:
				aVertex = updateServiceActorAssetBox->createVertex(0.0, 0.0, 0.0, 0.0, 0.0, backgroundVertexColor);
				updateServiceActorAssetBox->addVertexToChain(backgroundVertexChainIdent, aVertex);
				break;
			case 2:
				aVertex = updateServiceActorAssetBox->createVertex(1.0, 0.0, 0.0, 1.0, 0.0, backgroundVertexColor);
				updateServiceActorAssetBox->addVertexToChain(backgroundVertexChainIdent, aVertex);
				break;
			case 3:
				aVertex = updateServiceActorAssetBox->createVertex(1.0, 1.0, 0.0, 1.0, 1.0, backgroundVertexColor);
				updateServiceActorAssetBox->addVertexToChain(backgroundVertexChainIdent, aVertex);
				break;
			default:
				writeLog("ERR: switch case unknown");
		}
	}

}


UpdateServiceActor::~UpdateServiceActor() {

}


void UpdateServiceActor::fadeOutAnimationEnds(VisualAnimationComponent* animationComponent, void* userData) {
	UpdateServiceActor* updateServiceActor = reinterpret_cast<UpdateServiceActor*>(userData);
	if (updateServiceActor) {
		updateServiceActor->setState(kVisActNoShow);
	}
}


void UpdateServiceActor::setState(VisualActorState actorState) {
	VisualActor::setState(actorState);
	if (actorState == kVisActOn) {
		VisualAnimation fadeInAnimation(kAnimatedOpacity);
		fadeInAnimation.setDurationInMilliseconds(2000);
		fadeInAnimation.setStartValue(0.0);
		fadeInAnimation.setStopValue(1.0);
		
		VisualAnimation fadeOutAnimation(kAnimatedOpacity);
		fadeOutAnimation.setDurationInMilliseconds(2000);
		fadeOutAnimation.setStartValue(1.0);
		fadeOutAnimation.setStopValue(0.0);
		fadeOutAnimation.setStartDelayInMilliseconds(15000);
		fadeOutAnimation.setCallbackToNotifyBeforeDeath(UpdateServiceActor::fadeOutAnimationEnds, this);
		
		VisualAnimationSequence animationSequence;
		animationSequence.add(fadeInAnimation);
		animationSequence.add(fadeOutAnimation);
		
		this->updateServiceActorAsset.addAnimation(animationSequence);

		VisualStagePosition assetPosition = this->updateServiceActorAsset.getPosition();
		assetPosition.horizontalAlignment = kCenterAligned;
		assetPosition.verticalAlignment = kBottomAligned;
		this->updateServiceActorAsset.setPosition(assetPosition);
	}
}


void UpdateServiceActor::show(const VisualPlayerState& visualPlayerState) {
	VisualCamera* aCamera = NULL;
	aCamera = VisualCamera::createDefaultCamera();

	VisualStageBox* updateServiceActorAssetBox = this->updateServiceActorAsset.getBox();
	const VertexChain* const backgroundVertexChain = updateServiceActorAssetBox->getVertexChain(backgroundVertexChainIdent);
	VisualActorGraphics::drawVertexChain(*backgroundVertexChain, kGL_QUADS, kBlend);

	updateServiceActorAsset.draw(frontVertexChainIdent);
}


void UpdateServiceActor::handleNotification(VisualNotification& aNotification) {

	// VisualActor::handleNotification(aNotification); // debug

/*
	VisualNotificationKey notificationKey = aNotification.getKey();
	
	switch (notificationKey) {
	
		default:
			char notificationString[64];
			VisualNotification::convertNotificationKeyToString(notificationKey, notificationString);
			char errLog[256];
			sprintf(errLog, "Unhandled VisualNotificationKey %s in file: %s (line: %d) [%s])", notificationString, __FILE__, __LINE__, __FUNCTION__);
			writeLog(errLog);
			break;
	}
*/
}
