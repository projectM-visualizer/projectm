/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: Beatlight.cpp
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

#include "Beatlight.h"
#include "VisualErrorHandling.h"
#include "VisualActorGraphics.h"
#include "VisualTiming.h"
#include "VisualAudioLab.h"
#include "VisualFile.h"
#include "VisualString.h"
#include "VisualImage.h"
#include "VisualStageBox.h"
#include "VisualConfiguration.h"



using namespace VizKit;


Beatlight::Beatlight() {
	addSpotlightImage();
}


void Beatlight::prepareBeatlightShow() {
}


void Beatlight::showBeatlight() {

	this->beatlightAsset.draw(kFrontVertexChain);
	
	/*
    if (theVisualAudioLab->getBeatImpulse() == 1) {
        ageLimit = maxLifetimeMilliSec;
        //pTiming->clearMyTimeStore(timeStoreIndex);
        VisualTiming::resetTimestamp("Beatlight");
        timeToLive = ageLimit;
    } else {
        if ((ageLimit-elapsedMilliseconds) > 0) {
            ageLimit -= elapsedMilliseconds;
            timeToLive = ageLimit;
        } else {
            timeToLive = 0;
        }
    }
	*/
	
}


void Beatlight::finishBeatlightShow() {
}

 
void Beatlight::addSpotlightImage() {

	VisualImage spotlightImage;
	spotlightImage.initWithResource(SPOTPNG);

/*
	VisualFile* inputFile = VisualFile::createWithUserDesktopDirectory();
	VisualString inputFileName = VisualString("spot.png");
	inputFile->appendFileName(inputFileName);
	//spotlightImage.initWithFile(*inputFile);
	spotlightImage.initWithContentsOfFile(*inputFile);
	delete(inputFile);
*/

	//VisualString url("http://www.imagomat.de/images/coverversion/screen_mac.png");
	//VisualImage* spotlightImage = VisualImage::createWithURL(url);
	
	if (!spotlightImage.isEmpty()) {
		
		this->beatlightAsset.setImage(spotlightImage);

		VisualStageBox* beatlightAssetBox = this->beatlightAsset.getBox();
		beatlightAssetBox->setContentPixelWidth(spotlightImage.getWidth());
		beatlightAssetBox->setContentPixelHeight(spotlightImage.getHeight());

/*
		VisualFile outputFile;
		outputFile.initWithUserDesktopDirectory();
		VisualString aFileName = VisualString("vizKitBeatlightImage.png");
		outputFile.appendFileName(aFileName);

		OSStatus status = spotlightImage.writeToFile(outputFile);
		if (status != noErr) {
			printf("err %ld in Beatlight::addSpotlightImage()\n", status);
		}
*/

		this->calcPositionOnScreen();

		VisualAnimation pulsateAnimation(kAnimatedOpacity);
		UInt32 durationInMilliseconds = 550;
		pulsateAnimation.setDurationInMilliseconds(durationInMilliseconds);
		pulsateAnimation.setLoopMode(kMirroredLoop, kInfiniteRepetition);
		this->beatlightAsset.addAnimation(pulsateAnimation);

	}
	
}


void Beatlight::calcPositionOnScreen() {

	VisualStagePosition beatlightAssetPosition = this->beatlightAsset.getPosition();

	beatlightAssetPosition.reset();
	
	beatlightAssetPosition.horizontalAlignment = kRightAligned;
	beatlightAssetPosition.verticalAlignment = kTopAligned;

	beatlightAssetPosition.minMarginBottom = 5;
	beatlightAssetPosition.minMarginBottomUnit = kPixel;

	beatlightAssetPosition.minMarginTop = 20;
	beatlightAssetPosition.minMarginTopUnit = kPixel;

	beatlightAssetPosition.minMarginLeft = 5;
	beatlightAssetPosition.minMarginLeftUnit = kPixel;

	beatlightAssetPosition.minMarginRight = 20;
	beatlightAssetPosition.minMarginRightUnit = kPixel;
	
	this->beatlightAsset.setPosition(beatlightAssetPosition);
	
	VisualStageBox* beatlightAssetBox = this->beatlightAsset.getBox();
	beatlightAssetBox->update(); // VisualStageBox only updates automatically on value changed, after canvas reshape event we have to update manually (before calculating scaleFactor) 
	beatlightAssetBox->setScalingBehaviour(kPreserveAspectRatio);
	
	this->beatlightAsset.generateVertexChain(kFrontVertexChain);

}
