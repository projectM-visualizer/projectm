/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualStageControl.cpp
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

#include "VisualStageControl.h"
#include "VisualErrorHandling.h"
#include "VisualDataStore.h"
#include "VisualNotification.h"
#include "VisualEnsemble.h"
#include "VisualConfiguration.h"

#include "BeatlightActor.h"
#include "CoverArtActor.h"
#include "ProcessMonitorActor.h"
#include "TemplateActor.h"
#include "TrackTitleActor.h"
#include "TrackLyricsActor.h"

#include <algorithm>

#if TARGET_OS_MAC
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacErrors.h> // unimpErr
#endif

#if TARGET_OS_WIN
#include <QT/macerrors.h> // unimpErr
#endif


using namespace VizKit;


VisualStageControl* VisualStageControl::theVisualStageControl = NULL;


VisualStageControl::VisualStageControl() {
    aVisualEnsemble = NULL;
}


VisualStageControl::~VisualStageControl() {
	if (aVisualEnsemble != NULL) {
		delete aVisualEnsemble;
	}
}


VisualStageControl* VisualStageControl::getInstance() {
    if (theVisualStageControl == NULL) {
		theVisualStageControl = new VisualStageControl;
		if (theVisualStageControl != NULL) {
			theVisualStageControl->initVisualStageControl();
		}
    }
	if (theVisualStageControl == NULL) {
		writeLog("ERR: init theVisualStageControl failed");
	}
	return theVisualStageControl;
}


void VisualStageControl::dispose() {
    if (theVisualStageControl != NULL) {
        delete theVisualStageControl;
        theVisualStageControl = NULL;
    }
}


VisualStageControl* VisualStageControl::initVisualStageControl() {

	VisualActor* aVisualActor;

	theVisualStageControl = VisualStageControl::getInstance();
	
	theVisualStageControl->aVisualEnsemble = new VisualEnsemble;
	if (theVisualStageControl->aVisualEnsemble == NULL) {
		writeLog("ERR: theVisualStageControl->aVisualEnsemble == NULL");
	}


	// TemplateActor
	aVisualActor = new TemplateActor;
	theVisualStageControl->aVisualEnsemble->addEnsembleMember(aVisualActor);

	// CoverArtActor
	aVisualActor = new CoverArtActor;
	theVisualStageControl->aVisualEnsemble->addEnsembleMember(aVisualActor);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioMetadataIsAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kCoverTextureIsAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayStartedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayStoppedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayPausedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayResumedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kCanvasReshapeEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kTrackInfoTextureChangedMsg);

	// TrackTitleActor
	aVisualActor = new TrackTitleActor;
	theVisualStageControl->aVisualEnsemble->addEnsembleMember(aVisualActor);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioMetadataIsAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kTrackInfoTextureIsAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayStartedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayStoppedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayPausedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayResumedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kCanvasReshapeEvt);

	// ProcessMonitorActor
	aVisualActor = new ProcessMonitorActor;
	theVisualStageControl->aVisualEnsemble->addEnsembleMember(aVisualActor);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kCanvasReshapeEvt);

	// BeatlightActor
	aVisualActor = new BeatlightActor;
	theVisualStageControl->aVisualEnsemble->addEnsembleMember(aVisualActor);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kCanvasReshapeEvt);

	// TrackLyricsActor
	aVisualActor = new TrackLyricsActor;
	theVisualStageControl->aVisualEnsemble->addEnsembleMember(aVisualActor);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioMetadataIsAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kLyricsAreAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kLyricsTextureIsAvailableMsg);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayStartedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayStoppedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayPausedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayResumedEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kCanvasReshapeEvt);
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, kTrackInfoTextureChangedMsg);

	return theVisualStageControl;
}


void VisualStageControl::processVisualStageControl(const VisualPlayerState& visualPlayerState) {

    VisualActor* aVisualActor;
    const char* aVisualActorName;

	theVisualStageControl = VisualStageControl::getInstance();

	OSStatus status = noErr;
	VisualAnimationsIterator it = theVisualStageControl->visualAnimations.begin();
	while (it != theVisualStageControl->visualAnimations.end()) {
		if (it->second.isRunning() == true) {
			status = it->second.callAnimateCallbackFunction();
			it++;
		} else if (it->second.isDead() == true) {
			theVisualStageControl->visualAnimations.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}

    theVisualStageControl->aVisualEnsemble->resetVisualActorIterIndex();
    while ((aVisualActor = theVisualStageControl->aVisualEnsemble->getNextVisualActor())) {
        aVisualActorName = aVisualActor->getName();
        if (strcmp(aVisualActorName, "PROCESSMONITOR") == 0) {
			dynamic_cast<ProcessMonitorActor*>(aVisualActor)->registerProcessMonitorInfoMap(VisualDataStore::getProcessInfoMap());
		}
    }
}


void VisualStageControl::doEnsembleShow(const VisualPlayerState& visualPlayerState) {
	theVisualStageControl = VisualStageControl::getInstance();
	theVisualStageControl->processVisualStageControl(visualPlayerState);
    theVisualStageControl->aVisualEnsemble->showEnsemble(visualPlayerState);
}


void VisualStageControl::dispatchNotification(const VisualNotification& aNotification) {

	VisualActor* aVisualActor;
	const char* aVisualActorName;
	ProcessMonitorActor* aProcessMonitorActor;
	VisualNotificationKey keyOfNotification;

	theVisualStageControl = VisualStageControl::getInstance();

	keyOfNotification = aNotification.getKey();
	
	switch (keyOfNotification) {
		
		case kToggleShowProcessMonitorMsg:
			aVisualActor = theVisualStageControl->aVisualEnsemble->getVisualActorByName("PROCESSMONITOR");
			if (aVisualActor) {
				aProcessMonitorActor = dynamic_cast<ProcessMonitorActor*>(aVisualActor);
				if (aProcessMonitorActor->getState() == kVisActNoShow) {
					aProcessMonitorActor->setState(kVisActOn);
				} else {
					aProcessMonitorActor->setState(kVisActNoShow);
				}
			}
			break;

		case kToggleProcessMonitorAudioInfoMsg:
			theVisualStageControl->aVisualEnsemble->resetVisualActorIterIndex();
			while ((aVisualActor = theVisualStageControl->aVisualEnsemble->getNextVisualActor())) {
				aVisualActorName = aVisualActor->getName();
				if (strcmp(aVisualActorName, "PROCESSMONITOR") == 0) {
					aProcessMonitorActor = dynamic_cast<ProcessMonitorActor*>(aVisualActor);
					if (aProcessMonitorActor->isAudioInfoShown() == true) {
						aProcessMonitorActor->setShowAudioInfo(false);
					} else {
						aProcessMonitorActor->setShowAudioInfo(true);
					}
				}
			}
			break;

		default:
			// let the whole ensemble know about the notification
			if (theVisualStageControl->aVisualEnsemble) {
				theVisualStageControl->aVisualEnsemble->dispatchNotification(aNotification);
			}
	}
}


void VisualStageControl::checkForFadeOutEvent() {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if (theVisualPlayerState->fadeOutEventShouldBeSent() == true) {
		VisualNotification::post(kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt);
	}
}


const VisualItemIdentifier* const VisualStageControl::addVisualAnimation(VisualAnimation& aVisualAnimation, VisualItemIdentifier& anOwnerIdentifier) {
	theVisualStageControl = VisualStageControl::getInstance();
	VisualAnimationsIterator it = theVisualStageControl->visualAnimations.insert(std::make_pair(anOwnerIdentifier, aVisualAnimation));
	it->second.start();
	return it->second.getIdentifier();
}


void VisualStageControl::removeVisualAnimationsWithOwnerIdentifier(VisualItemIdentifier& anOwnerIdentifier, AnimatedProperty anAnimatedProperty) {
	bool doErase;
	theVisualStageControl = VisualStageControl::getInstance();
	VisualAnimationsIterator it = theVisualStageControl->visualAnimations.lower_bound(anOwnerIdentifier);
	while (it != theVisualStageControl->visualAnimations.upper_bound(anOwnerIdentifier)) {
		doErase = true;
		if (anAnimatedProperty != kUndefinedAnimatedProperty) {
			if (it->second.getAnimatedProperty() != anAnimatedProperty) {
				doErase = false;
			}
		}
		if (doErase == true) {
			theVisualStageControl->visualAnimations.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}

}


void VisualStageControl::removeVisualAnimation(const VisualItemIdentifier* const animationIdentifier) {
	theVisualStageControl = VisualStageControl::getInstance();
	VisualAnimationsIterator it = theVisualStageControl->visualAnimations.begin();
	while (it != theVisualStageControl->visualAnimations.end()) {
		if (it->second.getIdentifier() == animationIdentifier) {
			theVisualStageControl->visualAnimations.erase(it++); // increment iterator, then call erase with old iterator
			// after erase, accessing iterator (for increment) is invalid
		} else {
			it++;
		}
	}
}


void VisualStageControl::resetVisualAnimationsIterIndex(VisualItemIdentifier& aVisualAssetIdentifier) {
	theVisualStageControl = VisualStageControl::getInstance();
	theVisualStageControl->persistentIdentifier = aVisualAssetIdentifier;
	theVisualStageControl->persistentAnimationIterator = theVisualStageControl->visualAnimations.lower_bound(aVisualAssetIdentifier);
}


VisualAnimation* VisualStageControl::getNextVisualAnimation() {
	theVisualStageControl = VisualStageControl::getInstance();
	if (theVisualStageControl->persistentAnimationIterator == theVisualStageControl->visualAnimations.upper_bound(theVisualStageControl->persistentIdentifier)) {
		return NULL;
	} else {
		return &theVisualStageControl->persistentAnimationIterator->second;
	}
}


void VisualStageControl::advanceAnimationIterator() {
	theVisualStageControl = VisualStageControl::getInstance();
	theVisualStageControl->persistentAnimationIterator++;
}


OSStatus VisualStageControl::handleKeyPressed(const char keyboadVal, const PlayerShowMode showMode) {
	OSStatus status = unimpErr;
	
	VisualNotification aNotification;
	
	switch (keyboadVal) {
		case 's':
		case 'S':
			if ((showMode & kIsShowing) == kIsShowing) {
				//VisualMessaging::post(kToggleShowProcessMonitorMsg);
				aNotification.setKey(kToggleShowProcessMonitorMsg);
				VisualStageControl::dispatchNotification(aNotification);
			}
			break;						
		case 'a':
		case 'A':
			if ((showMode & kIsShowing) == kIsShowing) {
				//VisualMessaging::post(kToggleProcessMonitorAudioInfoMsg);
				aNotification.setKey(kToggleProcessMonitorAudioInfoMsg);
				VisualStageControl::dispatchNotification(aNotification);
			}
			break;
		default:
			// nothing
			break;
	}
	return status;
}
