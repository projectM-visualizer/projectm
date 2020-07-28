/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualStageControl.cpp
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

#include "VisualStageControl.h"
#include "VisualAnimationQueue.h"
#include "VisualErrorHandling.h"
#include "VisualDataStore.h"
#include "VisualNotification.h"
#include "VisualEnsemble.h"
#include "VisualObject.h"
#include "VisualString.h"
#include "VisualStyledString.h"
#include "VisualImage.h"
#include "VisualUpdateManager.h"

#include "VisualActor.h"
#include "BeatlightActor.h"
#include "CoverArtActor.h"
#include "ProcessMonitorActor.h"
#include "TemplateActor.h"
#include "TrackTitleActor.h"
#include "TrackLyricsActor.h"
#include "UpdateServiceActor.h"

#include <algorithm>


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
			theVisualStageControl->init();
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


void VisualStageControl::init() {

	VisualActor* aVisualActor;
	
	this->aVisualEnsemble = new VisualEnsemble;
	if (this->aVisualEnsemble == NULL) {
		writeLog("ERR: this->aVisualEnsemble == NULL");
	}
	
	bool doLoadCoverArtActor = true;
	bool doLoadProcessMonitorActor = true;
	bool doLoadTrackTitleActor = true;
	bool doLoadBeatlightActor = true;
	bool doLoadTrackLyricsActor = true;
	bool doLoadTemplateActor = true;
	bool doLoadUpdateServiceActor = true;

	// CoverArtActor
	if (doLoadCoverArtActor) {
		aVisualActor = new CoverArtActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}

	// ProcessMonitorActor
	if (doLoadProcessMonitorActor) {
		aVisualActor = new ProcessMonitorActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}

	// TrackTitleActor
	if (doLoadTrackTitleActor) {
		aVisualActor = new TrackTitleActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}

	// BeatlightActor
	if (doLoadBeatlightActor) {
		aVisualActor = new BeatlightActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}

	// TrackLyricsActor
	if (doLoadTrackLyricsActor) {
		aVisualActor = new TrackLyricsActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}
	
	// TemplateActor
	if (doLoadTemplateActor) {
		aVisualActor = new TemplateActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}
	
	// UpdateServiceActor
	if (doLoadUpdateServiceActor) {
		aVisualActor = new UpdateServiceActor;
		aVisualActor->init();
		this->aVisualEnsemble->addEnsembleMember(aVisualActor);
	}

}


void VisualStageControl::doEnsembleShow(const VisualPlayerState& visualPlayerState) {
	theVisualStageControl = VisualStageControl::getInstance();
	ProcessMonitorActor* processMonitorActor = dynamic_cast<ProcessMonitorActor*>(VisualStageControl::getVisualActorByName("PROCESSMONITOR"));
	if (processMonitorActor) {
		processMonitorActor->registerProcessMonitorInfoMap(VisualDataStore::getProcessInfoMap());
	}
	VisualAnimationQueue::processAnimations();
    theVisualStageControl->aVisualEnsemble->showEnsemble(visualPlayerState);
}


VisualActor* VisualStageControl::getVisualActorByName(const char* const aVisualActorName) {
	theVisualStageControl = VisualStageControl::getInstance();
	return theVisualStageControl->aVisualEnsemble->getVisualActorByName(aVisualActorName);
}


void VisualStageControl::registerObserverForNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey) {
	theVisualStageControl = VisualStageControl::getInstance();
	theVisualStageControl->aVisualEnsemble->registerObserverForNotification(aVisualActor, aNotificationKey);
}


void VisualStageControl::removeObserverOfNotification(VisualActor* aVisualActor, const VisualNotificationKey aNotificationKey) {
	theVisualStageControl = VisualStageControl::getInstance();
	theVisualStageControl->aVisualEnsemble->removeObserverOfNotification(aVisualActor, aNotificationKey);
}


void VisualStageControl::dispatchNotification(VisualNotification& aNotification) {

	VisualActor* aVisualActor;
	ProcessMonitorActor* aProcessMonitorActor;
	VisualNotificationKey keyOfNotification;

	theVisualStageControl = VisualStageControl::getInstance();

	keyOfNotification = aNotification.getKey();
	
	switch (keyOfNotification) {
		
		case kToggleShowProcessMonitorMsg:
			aVisualActor = theVisualStageControl->aVisualEnsemble->getVisualActorByName("PROCESSMONITOR");
			if (aVisualActor) {
				aProcessMonitorActor = dynamic_cast<ProcessMonitorActor*>(aVisualActor);
				if (aProcessMonitorActor) {
					if (aProcessMonitorActor->getState() == kVisActNoShow) {
						aProcessMonitorActor->setState(kVisActOn);
					} else {
						aProcessMonitorActor->setState(kVisActNoShow);
					}
				}
			}
			break;

		case kToggleProcessMonitorAudioInfoMsg:
		
			aVisualActor = theVisualStageControl->aVisualEnsemble->getVisualActorByName("PROCESSMONITOR");
			if (aVisualActor) {
				aProcessMonitorActor = dynamic_cast<ProcessMonitorActor*>(aVisualActor);
				if (aProcessMonitorActor) {
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
		VisualNotification::post(kAudioPlayReachedFadeOutTimeEvt);
	}
}


void VisualStageControl::handleKeyPressed(const char keyboadVal, const PlayerShowMode showMode) {
	
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
		case 'i':
		case 'I':
			{
				VisualActor* aVisualActor = theVisualStageControl->aVisualEnsemble->getVisualActorByName("UPDATESERVICE");
				if (aVisualActor) {
					UpdateServiceActor* anUpdateServiceActor = dynamic_cast<UpdateServiceActor*>(aVisualActor);
					if (anUpdateServiceActor) {
						VisualActorState actorState = anUpdateServiceActor->getState();
						if (actorState == kVisActOn) {
							VisualUpdateManager::getMoreInfoAboutAvailableUpdate();
						}
					}
				}
			}
			break;
		case 'd':
		case 'D':
			{
				VisualActor* aVisualActor = theVisualStageControl->aVisualEnsemble->getVisualActorByName("UPDATESERVICE");
				if (aVisualActor) {
					UpdateServiceActor* anUpdateServiceActor = dynamic_cast<UpdateServiceActor*>(aVisualActor);
					if (anUpdateServiceActor) {
						VisualActorState actorState = anUpdateServiceActor->getState();
						if (actorState == kVisActOn) {
							VisualUpdateManager::doDownloadAvailableUpdate();
						}
					}
				}
			}
			break;
		default:
			// nothing
			break;
	}

}
