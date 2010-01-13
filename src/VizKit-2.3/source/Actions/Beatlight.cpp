/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: Beatlight.cpp
 *
 */

/*************************************************************************

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

 *************************************************************************/


#include "Beatlight.h"

#include "VisualConfiguration.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualCamera.h"
#include "VisualActorGraphics.h"
#include "VisualTiming.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualAnimation.h"
#include "../AudioLab/VisualAudioLab.h"
#include "VisualFile.h"


using namespace VizKit;


Beatlight::Beatlight() {
	VisualImage* spotLightImage = VisualImage::createWithResource(SPOTPNG);
	/*
	VisualFile testFile;
	bool success = testFile.initWithUserDesktopDirectory();
	VisualString fileName("test.png");
	success = testFile.appendFileName(fileName);
	VisualImage* spotLightImage = VisualImage::createWithFile(testFile);
	*/
	beatlightAsset.setImage(*spotLightImage);
	delete spotLightImage;
	reshape();
	
}


void Beatlight::showBeatlight(const bool audioIsPlaying) {
	beatlightAsset.draw(this->vertexChainId);
}


void Beatlight::applyBehavior() {
	VisualAnimation fadeOutAnimation(kAnimatedOpacity);
	fadeOutAnimation.setDurationInMilliseconds(500);
	fadeOutAnimation.setStartValue(1.0);
	fadeOutAnimation.setStopValue(0.0);
	this->beatlightAsset.addAnimation(fadeOutAnimation);
}


void Beatlight::showBeatMeterLight(const bool audioIsPlaying) {
    uint32 elapsedMilliseconds;
    uint32 accuElapsedMilliseconds;
    static uint32 ageLimit = 500;
    uint32 timeToLive;
    float lifetimeIntensity;
    char info[32];
    VisualAudioLab* theVisualAudioLab;
    uint16 maxLifetimeMilliSec = 500;

	theVisualAudioLab = VisualAudioLab::getInstance();
    
    //timeStoreIndex = pTiming->storeMyTime("Beatlight");
    //elapsedMilliseconds = pTiming->getMyElapsedMilliseconds(timeStoreIndex);
    //accuElapsedMilliseconds += elapsedMilliseconds;
    accuElapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceReset("Meterlight");
    
    //elapsedMilliseconds = accuElapsedMilliseconds - prevAccuElapsedMilliseconds;
    elapsedMilliseconds = VisualTiming::getElapsedMilliSecsSinceLastCall("Meterlight");
    

    if (theVisualAudioLab->getBeatMeter() == 1) {
        ageLimit = maxLifetimeMilliSec;
        VisualTiming::resetTimestamp("Meterlight");
        timeToLive = ageLimit;
    } else {
        if ((ageLimit-elapsedMilliseconds) > 0) {
            ageLimit -= elapsedMilliseconds;
            timeToLive = ageLimit;
        } else {
            timeToLive = 0;
        }
    }

    sprintf(info, "%d", timeToLive);
    //setProcessInfo ("beatlightTimeToLive", info);
    lifetimeIntensity = ((float)timeToLive / (float)maxLifetimeMilliSec);
    if (lifetimeIntensity < 0.5) {
        // Minimum an lifetimeIntensity ist 0.5
        // so bleibt das Licht immer etwas an
        lifetimeIntensity = 0.5f;
    }

	VisualCamera aCamera;
	/*
	beatlightSpot->xPos = aCamera.getMaxRightCoord() - 0.3f;
	beatlightSpot->yPos= aCamera.getMaxTopCoord() - 0.3f;
    beatlightSpot->redVal=1.0;
    beatlightSpot->greenVal=0.0;
    beatlightSpot->blueVal=0.0;
    beatlightSpot->ageLimit=1000;
    beatlightSpot->age=200;
    beatlightSpot->isDead=0;
    beatlightSpot->tailSize=0;
    beatlightSpot->waveformIntensityVal=80; // 0-100
    beatlightSpot->lifetimeIntensity=lifetimeIntensity;
    
	VisualActorGraphics::drawSpot(beatlightSpot->xPos, beatlightSpot->yPos, beatlightSpot->redVal, beatlightSpot->greenVal, beatlightSpot->blueVal, beatlightSpot->waveformIntensityVal, beatlightSpot->lifetimeIntensity, beatlightSpot->tailSize);
    */
}


void Beatlight::reshape() {

	VisualCamera aCamera;
	aCamera.setOrthographicProjection();
	this->beatlightAsset.setCamera(aCamera);

	VisualStagePosition beatlightAssetPosition = this->beatlightAsset.getPosition();

	beatlightAssetPosition.reset();
	
	VisualStageBox* beatlightAssetBox = this->beatlightAsset.getBox();
	
	//double coordDepth = this->calcCoordDepth();
		
	//coverArtAssetPosition.depthAlignment = kDepthCenterAligned;
	//coverArtAssetBox->setCoordDepth(coordDepth);

	beatlightAssetPosition.horizontalAlignment = kLeftAligned;
	beatlightAssetPosition.verticalAlignment = kBottomAligned;
	beatlightAssetPosition.verticalCoordOffset = VisualActorGraphics::yPixelToCoord(220, aCamera);
	
	this->beatlightAsset.setPosition(beatlightAssetPosition);
	
	beatlightAssetBox->update(); // VisualStageBox only updates automatically on value changed, after canvas reshape event we have to update manually (before calculating scaleFactor) 
	beatlightAssetBox->setScalingBehaviour(kPreserveAspectRatio);
	
	VisualVertex* aVertex = NULL;

	beatlightAssetBox->initializeVertexChain(this->vertexChainId);
	
	aVertex = beatlightAssetBox->createVertex(0.0, 1.0, 0.5, 0.0, 1.0);
	beatlightAssetBox->addVertexToChain(this->vertexChainId, aVertex);
	
	aVertex = beatlightAssetBox->createVertex(0.0, 0.0, 0.5, 0.0, 0.0);
	beatlightAssetBox->addVertexToChain(this->vertexChainId, aVertex);
	
	aVertex = beatlightAssetBox->createVertex(1.0, 0.0, 0.5, 1.0, 0.0);
	beatlightAssetBox->addVertexToChain(this->vertexChainId, aVertex);

	aVertex = beatlightAssetBox->createVertex(1.0, 1.0, 0.5, 1.0, 1.0);
	beatlightAssetBox->addVertexToChain(this->vertexChainId, aVertex);

}
