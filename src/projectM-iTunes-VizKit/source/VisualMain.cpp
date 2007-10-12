/*
 * Project: VizKit
 * Version: 1.9
 
 * Date: 20070503
 * File: VisualMain.cpp
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

#include "VisualMain.h"

#include <time.h> // for srand()

#include "VisualConfiguration.h"
#include "VisualConfigurationDialog.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualSignature.h"
#include "VisualDataStore.h"
#include "VisualQuickTime.h"
#include "VisualGraphics.h"
//#include "VisualAudioLab.h"
#include "VisualTiming.h"
#include "VisualStageControl.h"
#include "VisualNotification.h"
#include "VisualNotificationQueue.h"

#include <string>
#include "projectM.hpp"
#include "carbontoprojectm.hpp"

#if TARGET_OS_WIN
#include <QT/macmemory.h> // HLock
#endif



using namespace VizKit;

projectM *globalPM;

OSStatus VisualMain::prepareRenderAction() {

	VisualQuickTime::initialize();
	long QTVersion = 0;
	OSErr osErr = noErr;
	osErr = ::Gestalt(gestaltQuickTimeVersion, &QTVersion); 

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	
	bool success = theVisualGraphics->setupContext();
	if (!success) {
		return 1001;
	}
	
	//theVisualGraphics->setOrthographicProjection();

	theVisualGraphics->gatherOpenGLCapabilities();

#if TARGET_OS_WIN
	//theVisualGraphics->buildFont(); 
#endif

	//theVisualGraphics->resetModelViewMatrix();

	if (VisualDataStore::audioMetaDataHasBeenSentToEnsemble() == false) {
		VisualNotification::post(kAudioMetadataIsAvailableMsg);
		VisualDataStore::setAudioMetaDataHasBeenSentToEnsemble(true);
	}

	if(!globalPM)
	{
	globalPM = new projectM("C:\\Program\ Files\\projectM\\config.inp");		
	}
	return noErr;
}


OSStatus VisualMain::renderAction() {

	bool debug = false;
	size_t notificationCount = 0;
	UInt16 currNotificationIdx = 1;

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	
	VisualNotification aNotification;
	VisualNotificationKey aKey;
	
	OSStatus status;

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setCurrentContext();
	
	//theVisualGraphics->clearBackground();

	//VisualStageControl::checkForFadeOutEvent();

	if (debug == true) {
		notificationCount = VisualNotificationQueue::size();
	}
	
	while (VisualNotificationQueue::size() > 0) {

		char* keyPressed = NULL;
	
		aNotification = VisualNotificationQueue::pop();
		aKey = aNotification.getKey();

		if (debug == true) {
			char notificationStr[64];
			char logStr[64];
			VisualNotification::convertNotificationKeyToString(aKey, notificationStr);
			sprintf(logStr, "Notification (%d/%d): %s", currNotificationIdx, (UInt16)notificationCount, notificationStr);
			writeLog(logStr);
			currNotificationIdx++;
		}

		switch(aKey) {
			case kAudioPlayStartedEvt:
			case kAudioPlayStoppedEvt:
			case kAudioPlayPausedEvt:
			case kAudioPlayResumedEvt:
			case kAudioPlayReachedFadeOutTimeBeforeEndOfTrackEvt:
			case kAudioMetadataIsAvailableMsg:
			case kLyricsAreAvailableMsg:
			case kCoverTextureIsAvailableMsg:
			case kLyricsTextureIsAvailableMsg:
			case kTrackInfoTextureIsAvailableMsg:
			case kTrackInfoTextureChangedMsg:
			case kCanvasReshapeEvt:		
								
					//delete (globalPM);
					//globalPM = new projectM("C:\\Program\ Files\\projectM\\config.inp");	
				globalPM->projectM_resetTextures();
				break;
			case kKeyPressedEvt:
				UInt32 numberOfBytes;
				
				keyPressed = (char*)aNotification.getValue(numberOfBytes);
				status = VisualStageControl::handleKeyPressed(*keyPressed, theVisualPlayerState->getPlayerShowMode());
				break;
			default:
				char notificationString[32];
				VisualNotification::convertNotificationKeyToString(aKey, notificationString);
				char errLog[256];
				sprintf(errLog, "unhandled VisualNotificationKey %s in file: %s (line: %d) [%s])", notificationString, __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
		}

	}

	AudioPlayState playState = theVisualPlayerState->getAudioPlayState();
	if (playState == kAudioPlayStarted) {
		UInt32 fadeInTimeOnPlayInMS = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnPlayInMS);
		if (theVisualPlayerState->getElapsedAudioPlayStartTime() > fadeInTimeOnPlayInMS) {
			theVisualPlayerState->setAudioPlayState(kAudioIsPlaying);
		}
	} else if (playState == kAudioPlayResumed) {
		UInt32 fadeInTimeOnResumeInMS = VisualDataStore::getPreferenceValueInt(VisualConfiguration::kFadeInTimeOnResumeInMS);
		if (theVisualPlayerState->getElapsedAudioPlayStartTime() > fadeInTimeOnResumeInMS) {
			theVisualPlayerState->setAudioPlayState(kAudioIsPlaying);
		}
	}

	//monitorRenderMessageProcess();

	// show action
	//VisualStageControl::doEnsembleShow(*theVisualPlayerState);
	
	globalPM->projectM_resetGL(theVisualGraphics->getCanvasPixelWidth(),theVisualGraphics->getCanvasPixelHeight());				
	
	globalPM->renderFrame();
	
	theVisualGraphics->finishGLDrawing();

	return noErr;
	
}


OSStatus VisualMain::finishRenderAction() {
	
//	VisualStageControl::dispose();
	VisualNotificationQueue::dispose();
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
    theVisualGraphics->disposeContext();
	VisualGraphics::dispose();
	VisualQuickTime::terminate();	

	return noErr;
}


void VisualMain::handleAudioPlayStoppedEvent() {

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	
	VisualTiming::resetTimestamp("trackPlayStop");
	
	if (theVisualPlayerState->remainingAudioTimeIsKnown() == true && theVisualPlayerState->getRemainingAudioTime() > 2000) {
		// if more than 2 secs. of audio is left to be played,
		// we assume that playing paused
		theVisualPlayerState->setAudioPlayState(kAudioPlayPaused);
		if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
			VisualNotification::post(kAudioPlayPausedEvt);
		}
	} else {
		theVisualPlayerState->setAudioPlayState(kAudioIsNotPlaying);
		theVisualPlayerState->setTrackPlayPositionInMS(0);
		if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
			VisualNotification::post(kAudioPlayStoppedEvt);
		}
	}
}


void VisualMain::handleAudioPlayStartedEvent(bool trackDidChange) {

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	AudioPlayState prevAudioPlayState = theVisualPlayerState->getAudioPlayState();
	
	VisualTiming::resetTimestamp("trackPlayStart");
	
	if (prevAudioPlayState == kAudioIsNotPlaying) {
		theVisualPlayerState->setAudioPlayState(kAudioPlayStarted);
	} else {
		if (trackDidChange == true) {
			theVisualPlayerState->setTrackPlayPositionInMS(0);
			theVisualPlayerState->setAudioPlayState(kAudioPlayStarted);
			prevAudioPlayState = kAudioIsNotPlaying;
		} else {
			theVisualPlayerState->setAudioPlayState(kAudioPlayResumed);
		}
	}

	VisualNotification aVisualNotification;

	if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
		if (theVisualPlayerState->getAudioPlayState() == kAudioPlayResumed) {
			VisualNotification::post(kAudioPlayResumedEvt);
		} else {
			VisualNotification::post(kAudioPlayStartedEvt);
		}
		if (trackDidChange == true) {
			VisualNotification::post(kAudioMetadataIsAvailableMsg);
			VisualDataStore::setAudioMetaDataHasBeenSentToEnsemble(true);
		}
	}

}


void VisualMain::handleShowShouldStartMessage(const bool isFullScreen) {
    
    srand((unsigned int)time(NULL)); // Seed random number generator

	OSStatus osStatus = VisualMain::prepareRenderAction();
	if (osStatus != noErr) {
		VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
		theVisualPlayerState->setPlayerShowMode(kErrorState);
	}

}


void VisualMain::handleCleanupMessage() {
//	VisualAudioLab::dispose();
	VisualTiming::dispose();
	VisualDataStore::dispose();
	VisualPlayerState::dispose();

	if (globalPM) delete(globalPM);

#if TARGET_OS_MAC
	VisualConfigurationDialog::dispose();
#endif
}


void VisualMain::handleCanvasReshape() {
	//VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	//theVisualGraphics->setOrthographicProjection();
	VisualNotification::post(kCanvasReshapeEvt);	
}


void VisualMain::convertVisualPluginMessageToString(const OSType visualPluginMessage, char* outString) {
	char* messageString;
	switch (visualPluginMessage) {
		case kVisualPluginInitMessage:
			messageString = "kVisualPluginInitMessage";
			break;
		case kVisualPluginCleanupMessage:
			messageString = "kVisualPluginCleanupMessage";
			break;
		case kVisualPluginEnableMessage:
			messageString = "kVisualPluginEnableMessage";
			break;
		case kVisualPluginDisableMessage:
			messageString = "kVisualPluginDisableMessage";
			break;
		case kVisualPluginIdleMessage:
			messageString = "kVisualPluginIdleMessage";
			break;
		case kVisualPluginConfigureMessage:
			messageString = "kVisualPluginConfigureMessage";
			break;
		case kVisualPluginShowWindowMessage:
			messageString = "kVisualPluginShowWindowMessage";
			break;
		case kVisualPluginHideWindowMessage:
			messageString = "kVisualPluginHideWindowMessage";
			break;
		case kVisualPluginSetWindowMessage:
			messageString = "kVisualPluginSetWindowMessage";
			break;
		case kVisualPluginRenderMessage:
			messageString = "kVisualPluginRenderMessage";
			break;
		case kVisualPluginUpdateMessage:
			messageString = "kVisualPluginUpdateMessage";
			break;
		case kVisualPluginPlayMessage:
			messageString = "kVisualPluginPlayMessage";
			break;
		case kVisualPluginChangeTrackMessage:
			messageString = "kVisualPluginChangeTrackMessage";
			break;
		case kVisualPluginStopMessage:
			messageString = "kVisualPluginStopMessage";
			break;
		case kVisualPluginSetPositionMessage:
			messageString = "kVisualPluginSetPositionMessage";
			break;
		case kVisualPluginPauseMessage:
			messageString = "kVisualPluginPauseMessage";
			break;
		case kVisualPluginUnpauseMessage:
			messageString = "kVisualPluginUnpauseMessage";
			break;
		case kVisualPluginEventMessage:
			messageString = "kVisualPluginEventMessage";
			break;
		case kVisualPluginDisplayChangedMessage:
			messageString = "kVisualPluginDisplayChangedMessage";
			break;
		default:
			messageString = "unknownVisualPluginMessage";
	}
	strcpy(outString, messageString);
}


OSStatus VisualMain::VisualPluginHandler(OSType message, VisualPluginMessageInfo* messageInfo, void* refCon) {

	bool debug = false;

    OSStatus status;
    EventRecord* eventPtr;
	std::string thissucks;
	
    static UInt32 majorVersionNum;
    static UInt32 minorVersionNum;
    
    //ITFileSpec pluginFileSpec;
	VisualGraphics* theVisualGraphics = NULL;
//	VisualAudioLab* theVisualAudioLab = NULL;
	bool trackDidChange = false;

    //visualPluginData = (VisualPluginData*) refCon;
    
    status = noErr;

	if (debug == true) {
		static bool firstTimeRun = true;
		static OSType oneBeforePrevMessage;
		static OSType prevMessage;
		static OSType messageWriteLogHistory[3];
		size_t curr = 2;
		size_t prev = 1;
		size_t oneBeforePrev = 0;
		if (firstTimeRun == true) {
			oneBeforePrevMessage = '    ';
			prevMessage = '    ';
			messageWriteLogHistory[oneBeforePrev] = '    ';
			messageWriteLogHistory[prev] = '    ';
			messageWriteLogHistory[curr] = '    ';
			firstTimeRun = false;
		}
		bool doWriteLog = true;
		char visualPluginMessage[64];
		char logStr[128];
		if (message != messageWriteLogHistory[curr]) {
			// don't log alternating messages (like render/idle/update messages during render loop)
			if (message == messageWriteLogHistory[prev] && prevMessage == messageWriteLogHistory[curr]) doWriteLog = false;
			if (message == messageWriteLogHistory[oneBeforePrev] && prevMessage == messageWriteLogHistory[curr] && oneBeforePrevMessage == messageWriteLogHistory[prev]) doWriteLog = false;
			if (message == messageWriteLogHistory[prev] && prevMessage == messageWriteLogHistory[oneBeforePrev] && oneBeforePrevMessage == messageWriteLogHistory[curr]) doWriteLog = false;
			if (doWriteLog == true) {
				VisualMain::convertVisualPluginMessageToString(message, visualPluginMessage);
				sprintf(logStr, "VisualPluginMessage: %s", visualPluginMessage);
				writeLog(logStr);
				messageWriteLogHistory[oneBeforePrev] = messageWriteLogHistory[prev];
				messageWriteLogHistory[prev] = messageWriteLogHistory[curr];
				messageWriteLogHistory[curr] = message;
				oneBeforePrevMessage = prevMessage;
				prevMessage = message;
			}
		}
	}

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	VisualTiming::setTimestampOfPlayerCall();

    switch (message) {
	
        case kVisualPluginInitMessage:
			/*
			Sent when the visual plugin is registered.
			The plugin should do minimal memory allocations here. 
			*/
			
			VisualDataStore::setAppVersionNum(messageInfo->u.initMessage.appVersion.majorRev, messageInfo->u.initMessage.appVersion.minorAndBugRev);
			
			VisualSignature::setAppCookie(messageInfo->u.initMessage.appCookie);
			VisualSignature::setAppProc(messageInfo->u.initMessage.appProc);
			
//			VisualAudioLab::setMaxNumberOfAudioDataChannels(kVisualMaxDataChannels);
//			VisualAudioLab::setNumberOfAudioWaveformDataEntries(kVisualNumWaveformEntries);
//			VisualAudioLab::setNumberOfAudioSpectrumDataEntries(kVisualNumSpectrumEntries);

			VisualConfiguration::setDefaultPreferences();

            majorVersionNum = messageInfo->u.initMessage.messageMajorVersion;
            minorVersionNum = messageInfo->u.initMessage.messageMinorVersion;

            //messageInfo->u.initMessage.refCon	= (void*) visualPluginData;
			
            break;

        case kVisualPluginCleanupMessage:
			/*
			Sent when the visual plugin is unloaded
			This message is sent when iTunes is about to quit. 
			You should free any resources allocated by your visualizer plug-in at this time.
			*/

            VisualMain::handleCleanupMessage();

            break;
                
        case kVisualPluginEnableMessage:
			/*
			Sent when the visual plugin is enabled. 
			iTunes currently enables all loaded visual plugins. 
			The plugin should not do anything here.
			*/
        case kVisualPluginDisableMessage:
			
            break;

        case kVisualPluginIdleMessage:
			/*
			Sent if the plugin requests idle messages. 
			Do this by setting the kVisualWantsIdleMessages option in the RegisterVisualMessage.options field.
			*/

			if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
				VisualMain::renderAction();
			}
			
            break;

        case kVisualPluginConfigureMessage:
			/*
			Sent if the plugin requests the ability for the user to configure it. 
			Do this by setting the kVisualWantsConfigure option in the RegisterVisualMessage.options field.
			*/

			VisualConfigurationDialog::show();

			break;
        
        case kVisualPluginShowWindowMessage:			
			/*
			Sent when iTunes is going to show the visual plugin in a port. 
			At this point, the plugin should allocate any large buffers it needs.
			*/

			theVisualGraphics = VisualGraphics::getInstance();
			{
				DisplayResolution thePreferredDisplayResolution = theVisualGraphics->returnPreferredDisplayResolution();
				if (thePreferredDisplayResolution.horizontalPixels > 0) {
					UInt16 minBitsPerPixel = 24;
					UInt16 maxBitsPerPixel = 32;
					VisualDataStore::setPreferredDisplayResolution(minBitsPerPixel, maxBitsPerPixel, thePreferredDisplayResolution.bitsPerPixel, thePreferredDisplayResolution.horizontalPixels, thePreferredDisplayResolution.verticalPixels);
				}
			}

            if ((messageInfo->u.showWindowMessage.options & kWindowIsFullScreen) == kWindowIsFullScreen) {
				theVisualPlayerState->setPlayerShowMode(kIsShowingInFullScreen);
            } else {
				theVisualPlayerState->setPlayerShowMode(kIsShowingInWindow);
            }
            
			theVisualGraphics = VisualGraphics::getInstance();
			theVisualGraphics->isSetupForFullScreenMode((bool)((messageInfo->u.showWindowMessage.options & kWindowIsFullScreen) == kWindowIsFullScreen));
#if TARGET_OS_MAC
			theVisualGraphics->setGraphicsDevicePort(messageInfo->u.showWindowMessage.port);
#endif
#if TARGET_OS_WIN
			theVisualGraphics->setGraphicsDevicePort(messageInfo->u.showWindowMessage.window);
#endif
			theVisualGraphics->setCanvasRect(&messageInfo->u.showWindowMessage.drawRect);
            VisualMain::handleShowShouldStartMessage((bool)((messageInfo->u.showWindowMessage.options & kWindowIsFullScreen) == kWindowIsFullScreen));
			
			if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
				VisualMain::renderAction();
			}
			
			
            break;

        case kVisualPluginHideWindowMessage:
			/*
			Sent when iTunes is no longer displayed.
			*/
                        
            theVisualPlayerState->setPlayerShowMode(kIsNotShowing);
			
			VisualMain::finishRenderAction();
			
			VisualDataStore::setAudioMetaDataHasBeenSentToEnsemble(false);
            
            break;
		
        case kVisualPluginSetWindowMessage:
			/*
			Sent when iTunes needs to change the port or rectangle of the currently displayed visual.
			*/
			
            theVisualGraphics = VisualGraphics::getInstance();
			theVisualGraphics->isSetupForFullScreenMode((bool)((messageInfo->u.setWindowMessage.options & kWindowIsFullScreen) == kWindowIsFullScreen));
			theVisualGraphics->setCanvasRect(&messageInfo->u.setWindowMessage.drawRect);
			VisualMain::handleCanvasReshape();									

			if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
				VisualMain::renderAction();
			}
			
            break;
        
        case kVisualPluginRenderMessage:
			/*
			Sent for the visual plugin to render a frame.
			*/
			
			//theVisualPlayerState->setTrackPlayPositionInMS(messageInfo->u.renderMessage.currentPositionInMS);
			
			/*
			theVisualAudioLab = VisualAudioLab::getInstance();
			if (messageInfo->u.renderMessage.timeStampID != theVisualAudioLab->getCurrTimestampIDOfRenderData()) {
				theVisualAudioLab->processAudioData(
									messageInfo->u.renderMessage.renderData->numWaveformChannels, 
									kVisualNumWaveformEntries, 
									(UInt8*)messageInfo->u.renderMessage.renderData->waveformData, 
									messageInfo->u.renderMessage.renderData->numSpectrumChannels, 
									kVisualNumSpectrumEntries, 
									(UInt8*)messageInfo->u.renderMessage.renderData->spectrumData
								);
				theVisualAudioLab->setCurrTimestampIDOfRenderData(messageInfo->u.renderMessage.timeStampID);
			}
			*/

			globalPM->pcm->addPCM8_512(messageInfo->u.renderMessage.renderData->waveformData);

			if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
				VisualMain::renderAction();
            }
			
            break;

		case kVisualPluginDisplayChangedMessage:
			/* 
			Something about display state changed 
			*/
			
			break;
	
        case kVisualPluginUpdateMessage:
			/*
			Sent in response to an update event. 
			The visual plugin should update into its remembered port. 
			This will only be sent if the plugin has been previously given a ShowWindow message.
			*/
			
			if ((theVisualPlayerState->getPlayerShowMode() & kIsShowing) == kIsShowing) {
				VisualMain::renderAction();
			}
			
            break;        

        case kVisualPluginPlayMessage:
			/*
			Sent when the player starts.
			*/
/*
			if ((theVisualPlayerState->getAudioPlayState() & kAudioPlayPaused) != kAudioPlayPaused) {
				theVisualAudioLab = VisualAudioLab::getInstance();
				theVisualAudioLab->resetVisualAudioLabData();
			}
*/
			if (messageInfo->u.playMessage.streamInfoUnicode->streamTitle[0] > 0 ||
				messageInfo->u.playMessage.streamInfoUnicode->streamMessage[0] > 0 ||
				messageInfo->u.playMessage.streamInfoUnicode->streamURL[0] > 0) {
				trackDidChange = VisualDataStore::storeAudioStreamMetadata(messageInfo->u.playMessage.streamInfoUnicode);
			} else {
				trackDidChange = VisualDataStore::storeAudioTrackMetadata(messageInfo->u.playMessage.trackInfoUnicode);
			}
						
			VisualMain::handleAudioPlayStartedEvent(trackDidChange);
            
			/*
            struct SoundComponentData {
                long                flags;
                OSType              format;
                short               numChannels;
                short               sampleSize;
                UnsignedFixed       sampleRate;
                long                sampleCount;
                Byte *              buffer;
                long                reserved;
            };
			*/
            
            break;

        case kVisualPluginChangeTrackMessage:
			/*
			Sent when the player changes the current track information. 
			This is used when the information about a track changes, 
			or when the CD moves onto the next track. 
			The visual plugin should update any displayed information about the currently playing song.
			*/
	/*		
			if ((theVisualPlayerState->getAudioPlayState() & kAudioPlayPaused) != kAudioPlayPaused) {
				theVisualAudioLab = VisualAudioLab::getInstance();
				theVisualAudioLab->resetVisualAudioLabData();
			}
			*/

			char *title;
			

			if (messageInfo->u.changeTrackMessage.streamInfoUnicode->streamTitle[0] > 0 ||
				messageInfo->u.changeTrackMessage.streamInfoUnicode->streamMessage[0] > 0 ||
				messageInfo->u.changeTrackMessage.streamInfoUnicode->streamURL[0] > 0) {
				trackDidChange = VisualDataStore::storeAudioStreamMetadata(messageInfo->u.changeTrackMessage.streamInfoUnicode);
			
				//globalPM->projectM_setTitle(messageInfo->u.changeTrackMessage.streamInfo->streamTitle);
				
				//CopyPascalStringToC(messageInfo->u.changeTrackMessage.streamInfo->streamTitle, title);


			} else {
				trackDidChange = VisualDataStore::storeAudioTrackMetadata(messageInfo->u.changeTrackMessage.trackInfoUnicode);
					//CopyPascalStringToC(messageInfo->u.changeTrackMessage.trackInfo->name, title);
			}
			
			
			//thissucks = new std::string(title);

			//globalPM->projectM_setTitle(thissucks);

			VisualMain::handleAudioPlayStartedEvent(trackDidChange);

			
            break;

        case kVisualPluginStopMessage:
			/*
			Sent when the player stops.
			*/

            VisualMain::handleAudioPlayStoppedEvent();
			
            break;
        
        case kVisualPluginSetPositionMessage:
			/*
			Sent when the player changes position.
			*/

			theVisualPlayerState->setTrackPlayPositionInMS(messageInfo->u.setPositionMessage.positionTimeInMS);
			
            break;

        case kVisualPluginPauseMessage:
			/*
			Sent when the player pauses. 
			iTunes does not currently use pause or unpause.
			A pause in iTunes is handled by stopping and remembering the position.
			*/
			
			VisualMain::handleAudioPlayStoppedEvent();

            break;
                
        case kVisualPluginUnpauseMessage:
			/*
			Sent when the player unpauses. 
			iTunes does not currently use pause or unpause.
			A pause in iTunes is handled by stopping and remembering the position.
			*/
			
            trackDidChange = false;
            VisualMain::handleAudioPlayStartedEvent(trackDidChange);
            
            break;        

        case kVisualPluginEventMessage:
			/*
			Sent to the plugin in response to a OS event. 
			The plugin should return noErr for any event it handles completely, 
			or an error (unimpErr) if iTunes should handle it.
			*/
			
			eventPtr = messageInfo->u.eventMessage.event;
			/*
			if ((eventPtr->what == keyDown) || (eventPtr->what == autoKey)) {
				char theChar = (char)(eventPtr->message & charCodeMask);
			*/

			projectMEvent event;
            projectMKeycode keycode;
            projectMModifier mod;
           
            event = carbon2pmEvent( messageInfo->u.eventMessage.event );
            keycode = carbon2pmKeycode( messageInfo->u.eventMessage.event );

            if ( keycode == ' ' ) {
                status = unimpErr;
              }
            globalPM->key_handler( event, keycode, mod );            				
			status = noErr; // each key is passed on to iTunes

			
            break;

        default:
		
            status = unimpErr;
			
            break;
			
    }
	
    return status;	

}


OSStatus VisualMain::RegisterVisualPlugin(PluginMessageInfo* messageInfo) {

	OSType visualPluginCreator = 'hook';
    OSStatus status;
    PlayerMessageInfo playerMessageInfo;
    
    memset(&playerMessageInfo.u.registerVisualPluginMessage, 0, sizeof(playerMessageInfo.u.registerVisualPluginMessage));

    playerMessageInfo.u.registerVisualPluginMessage.name[0] = (UInt8)strlen(VisualConfiguration::kVisualPluginShowName);
    memcpy(&playerMessageInfo.u.registerVisualPluginMessage.name[1], VisualConfiguration::kVisualPluginShowName, strlen(VisualConfiguration::kVisualPluginShowName));
    
    SetNumVersion(&playerMessageInfo.u.registerVisualPluginMessage.pluginVersion, gVisualPluginMajorVersion, gVisualPluginMinorVersion, gVisualPluginReleaseStage, gVisualPluginNonFinalRelease);
    
	playerMessageInfo.u.registerVisualPluginMessage.options = (kVisualWantsConfigure | kVisualWantsIdleMessages);
    
    playerMessageInfo.u.registerVisualPluginMessage.handler = (VisualPluginProcPtr)VisualMain::VisualPluginHandler;

    playerMessageInfo.u.registerVisualPluginMessage.registerRefCon = 0;
	
	playerMessageInfo.u.registerVisualPluginMessage.creator = visualPluginCreator;
    
    playerMessageInfo.u.registerVisualPluginMessage.timeBetweenDataInMS = 0xFFFFFFFF; // 16 milliseconds = 1 Tick, 0xFFFFFFFF = Often as possible.
    playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels = 2;
    playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels = 2;
    
    playerMessageInfo.u.registerVisualPluginMessage.minWidth = 64;
    playerMessageInfo.u.registerVisualPluginMessage.minHeight = 64;
    playerMessageInfo.u.registerVisualPluginMessage.maxWidth = 32767;
    playerMessageInfo.u.registerVisualPluginMessage.maxHeight = 32767;
    playerMessageInfo.u.registerVisualPluginMessage.minFullScreenBitDepth = 0; /* 0 = Any */
    playerMessageInfo.u.registerVisualPluginMessage.maxFullScreenBitDepth = 0; /* 0 = Any */
    playerMessageInfo.u.registerVisualPluginMessage.windowAlignmentInBytes = 0; /* Reserved (should be zero) */
    
    status = PlayerRegisterVisualPlugin(messageInfo->u.initMessage.appCookie, messageInfo->u.initMessage.appProc, &playerMessageInfo);
    
    return status;
	
}


#if TARGET_OS_MAC
OSStatus iTunesPluginMainMachO(OSType message, PluginMessageInfo* messageInfo, void* refCon)
#endif
#if TARGET_OS_WIN
OSStatus iTunesPluginMain(OSType message, PluginMessageInfo* messageInfo, void* refCon)
#endif
{
    OSStatus status;
    
    (void) refCon;
    
    switch (message) {
        case kPluginInitMessage:
            status = VisualMain::RegisterVisualPlugin(messageInfo);
            break;
                
        case kPluginCleanupMessage:
            status = noErr;
            break;
                
        default:
            status = unimpErr;
            break;
    }
    
    return status;
}

