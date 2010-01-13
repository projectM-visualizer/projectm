/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualMainAction.h
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

#include "VisualMainAction.h"

#include <time.h> // for srand()

#include "VisualConfiguration.h"
#include "VisualConfigurationDialog.h"
#include "VisualErrorHandling.h"
#include "VisualDispatch.h"
#include "VisualPreferences.h"
#include "VisualDataStore.h"
#include "VisualGraphics.h"
#include "VisualAudioLab.h"
#include "VisualTiming.h"
#include "VisualStageControl.h"
#include "VisualAnimationQueue.h"
#include "VisualNotification.h"
#include "VisualNotificationQueue.h"
#include "VisualThreading.h"
#include "VisualThreadingManager.h"
#include "VisualStyledString.h"
#include "VisualImage.h"
#include "VisualFontManager.h"
#include "VisualFile.h"
#include "VisualNetwork.h"
#include "VisualQuickTime.h"
#include "VisualUpdateManager.h"
#include "VisualActor.h"


using namespace VizKit;


void VisualMainAction::updateTiming() {
	VisualTiming::update();
}


void VisualMainAction::setDrawRect(int top, int left, int bottom, int right) {
	TopLeftPositionedPixelRect aTopLeftPositionedPixelRect;
	aTopLeftPositionedPixelRect.pixelRect.width = right - left;
	aTopLeftPositionedPixelRect.pixelRect.height = bottom - top;
	aTopLeftPositionedPixelRect.topLeftPixel.x = left;
	aTopLeftPositionedPixelRect.topLeftPixel.y = top;
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setCanvasRect(aTopLeftPositionedPixelRect);
}


void VisualMainAction::setTotalVisualizerRect(int top, int left, int bottom, int right) {
	TopLeftPositionedPixelRect aTopLeftPositionedPixelRect;
	aTopLeftPositionedPixelRect.pixelRect.width = right - left;
	aTopLeftPositionedPixelRect.pixelRect.height = bottom - top;
	aTopLeftPositionedPixelRect.topLeftPixel.x = left;
	aTopLeftPositionedPixelRect.topLeftPixel.y = top;
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setTotalVisualizerRect(aTopLeftPositionedPixelRect);
}


void VisualMainAction::setTotalVisualizerRectVerticalOffsetFromBottom(int top, int height) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setTotalVisualizerRectVerticalOffsetFromBottom(top, height);
}


void VisualMainAction::setAppVersionNum(int majorRev, int minorRev) {
	VisualDataStore::setAppVersionNum(majorRev, minorRev);
}


void VisualMainAction::handleInitMessage(int maxDataChannels, int numberOfWaveformEntries, int numberOfSpectrumEntries) {
	
	VisualAudioLab::setMaxNumberOfAudioDataChannels(maxDataChannels);
	VisualAudioLab::setNumberOfAudioWaveformDataEntries(numberOfWaveformEntries);
	VisualAudioLab::setNumberOfAudioSpectrumDataEntries(numberOfSpectrumEntries);

	VisualPreferences::setDefaultValues();
	
	VisualFontManager::setupFontBaseChangeObserver();
}


void VisualMainAction::setGraphicsDevice(VIZKIT_GRAPHICS_DEVICE device) {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setGraphicsDevicePort(device);
}


void VisualMainAction::handleShowWindowMessage(bool isFullScreen) {

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if (isFullScreen) {
		theVisualPlayerState->setPlayerShowMode(kIsShowingInFullScreen);
    } else {
		theVisualPlayerState->setPlayerShowMode(kIsShowingInWindow);
    }
    
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->isSetupForFullScreenMode(isFullScreen);

	VisualMainAction::handleShowShouldStartMessage(isFullScreen);

	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		VisualMainAction::renderAction();
	}
}


void VisualMainAction::handleSetWindowMessage(bool isFullScreen) {
    VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->isSetupForFullScreenMode(isFullScreen);
	
	VisualMainAction::handleCanvasReshape();
	
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		VisualMainAction::renderAction();
	}
}


void VisualMainAction::handleRenderMessage(int currentPositionInMS, int timeStampID, int numberOfWaveformChannels, int numberOfWaveformEntries, char* waveformData, int numberOfSpectrumChannels, int numberOfSpectrumEntries, char* spectrumData) {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	VisualMainAction::setTrackPlayPositionInMS(currentPositionInMS);
	
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	if (timeStampID != (int)(theVisualAudioLab->getCurrTimestampIDOfRenderData())) {
		theVisualAudioLab->processAudioData(
							numberOfWaveformChannels, 
							numberOfWaveformEntries, 
							(const uint8* const)waveformData, 
							numberOfSpectrumChannels, 
							numberOfSpectrumEntries, 
							(const uint8* const)spectrumData
						);
		theVisualAudioLab->setCurrTimestampIDOfRenderData(timeStampID);
		theVisualAudioLab->checkForBeatImpulse();
	}
	
	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		VisualMainAction::renderAction();
	}
}


void VisualMainAction::resetVisualAudioLabDataIfNotPaused() {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if ((theVisualPlayerState->getAudioPlayState() & kAudioPlayPaused) != kAudioPlayPaused) {
		VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
		theVisualAudioLab->resetData();
	}
}


bool VisualMainAction::prepareRenderAction() {

	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();

#if TARGET_OS_WIN
	theVisualGraphics->initGdiplus();
#endif

	bool success = theVisualGraphics->setupContext();
	if (!success) {
		return success;
	}
	
	theVisualGraphics->setCurrentContext();
	
	theVisualGraphics->setSurfaceViewport();
	theVisualGraphics->clearSurfaceBackground();
	theVisualGraphics->setCanvasViewport();
	theVisualGraphics->clearCanvasBackground();

	theVisualGraphics->gatherOpenGLCapabilities();

#if TARGET_OS_WIN
	theVisualGraphics->buildFont();
#endif

	theVisualGraphics->resetModelViewMatrix();
	
	VisualMainAction::postTrackInfoOfCurrentAudioData();
	VisualMainAction::postAlbumCoverArtworkOfCurrentAudioData();
	success = VisualMainAction::postLyricsOfCurrentAudioData();
	
	return success;

}


void VisualMainAction::renderAction() {

	static bool isRendering = false;
	if (isRendering == true)
	{
		return;
	}
	isRendering = true;
	
	bool debug = false;
	
	VisualThreadingManager::update();

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();

	theVisualGraphics->setCurrentContext();
	theVisualGraphics->clearCanvasBackground();

	VisualStageControl::checkForFadeOutEvent();
	
	VisualNotificationQueue::update();
	
	bool downloadStarted = false;
	downloadStarted = VisualNetwork::startDownloadThreadIfNeeded();
	
	size_t notificationCount = 0;
	if (debug == true) {
		notificationCount = VisualNotificationQueue::size();
	}
	
	uint16 currNotificationIdx = 1;

	VisualNotification aNotification;
	VisualNotificationKey aKey;
	
	while (VisualNotificationQueue::size() > 0) {

		uint32 numberOfBytes;
	
		aNotification = VisualNotificationQueue::pop();
		aKey = aNotification.getKey();

		if (debug == true) {
			char notificationStr[64];
			char logStr[64];
			VisualNotification::convertNotificationKeyToString(aKey, notificationStr);
			sprintf(logStr, "Notification (%d/%ld): %s", currNotificationIdx, notificationCount, notificationStr);
			writeLog(logStr);
			currNotificationIdx++;
		}

		switch (aKey) {
			case kAudioPlayTrackChangedEvt:
			case kAudioPlayStartedEvt:
			case kAudioPlayStoppedEvt:
			case kAudioPlayPausedEvt:
			case kAudioPlayResumedEvt:
			case kAudioPlayReachedFadeOutTimeEvt:
			case kBeatImpulseEvt:
			case kTrackInfoImageMsg:
			case kTrackLyricsImageMsg:
			case kImageWithIdentifierLoadedAndCreatedMsg:
			case kImageWithIdentifierMsg:
			case kImageHistogramCompletedMsg:
			case kTrackInfoTextureChangedMsg:
			case kCanvasReshapeEvt:
				
				VisualStageControl::dispatchNotification(aNotification);
				break;
				
			case kKeyPressedEvt:
				{
					char* keyPressed = (char*)aNotification.getData(numberOfBytes);
					VisualStageControl::handleKeyPressed(*keyPressed, theVisualPlayerState->getPlayerShowMode());
				}
				break;
				
			case kLyricsAreWrittenIntoTempFileMsg:
			{
				const VisualFile* const lyricsFile = dynamic_cast<const VisualFile* const>(aNotification.getObject());
				if (lyricsFile) {
					VisualDataStore::fileWithLyricsStringId = lyricsFile->getIdentifier();
					const VisualFile* const lyricsFileCopy = lyricsFile->clone();
					bool success = VisualThreading::createThread((ThreadingFuncPtr)VisualDataStore::readFileAndRemoveFileAndSendNotificationWithString, (void*)lyricsFileCopy);
					if (!success)
					{
						char errLog[256];
						sprintf(errLog, "Err: Unable to create thread after kLyricsAreWrittenIntoTempFileMsg in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
						writeLog(errLog);
					}
				}
			}
				break;
			case kStringWithIdentifierLoadedAndCreatedMsg:
				if (aNotification.getIdentifier() == VisualDataStore::fileWithLyricsStringId) {
					VisualItemIdentifier currentTrackIdentifier = VisualDataStore::getIdentifierOfCurrentTrack();
					if (currentTrackIdentifier == VisualDataStore::trackIdentifierOfLyricsMetadata) {
						// audio track is not different from the one we initiated the lyrics retrieval for
						const VisualString* const lyricsString = dynamic_cast<const VisualString* const>(aNotification.getObject());
						if (lyricsString) {
							VisualDataStore::setLyricsOfCurrentTrack(*lyricsString);
							VisualNotification::post(kLyricsAreAvailableInMetadataMsg);
						}
					}
				} else if (aNotification.getIdentifier() == VisualDataStore::updateInformationStringId) {
					bool updateIsAvailable = false;
					updateIsAvailable = VisualUpdateManager::evaluateUpdateInformation();
					if (updateIsAvailable) {
						VisualActor* updateServiceActor = VisualStageControl::getVisualActorByName("UPDATESERVICE");
						if (updateServiceActor) {
							updateServiceActor->setState(kVisActOn);
						}
					}
				} else {
					VisualStageControl::dispatchNotification(aNotification);
				}
				break;
			case kLyricsAreAvailableInMetadataMsg:
			{
				const VisualString trackLyrics = VisualDataStore::getLyricsOfCurrentTrack();
				// "missing value" is literally sent with streams that do not convey any lyrics
				const VisualString missingValueString("missing value");
				const VisualString missingValueStringWithLineEndings("missing value\n");
				if (trackLyrics.getNumberOfNonWhitespaceCharacters() > 0 && trackLyrics != missingValueString && trackLyrics != missingValueStringWithLineEndings) {
					
					VisualNotification trackLyricsStringStyleNotification;
					trackLyricsStringStyleNotification.setKey(kGetTrackLyricsStringStyleMsg);
					VisualStageControl::dispatchNotification(trackLyricsStringStyleNotification);
					VisualStyledString styledTrackLyricsString;
					styledTrackLyricsString.initWithVisualString(trackLyrics);
					const VisualStringStyle* const trackLyricsStringStyle = dynamic_cast<const VisualStringStyle* const>(trackLyricsStringStyleNotification.getObject());
					if (trackLyricsStringStyle) {
						styledTrackLyricsString.setStyle(*trackLyricsStringStyle);
						
						VisualNotification styledTrackLyricsStringNotification(VisualDataStore::styledTrackLyricsStringId);
						styledTrackLyricsStringNotification.setKey(kStyledStringMsg);
						styledTrackLyricsStringNotification.setObject(styledTrackLyricsString);
						styledTrackLyricsStringNotification.post();
					}
				}
			}
				break;
			case kStyledStringMsg:
				if (aNotification.getIdentifier() == VisualDataStore::styledTrackInfoStringId) {
					VisualStyledString* styledTrackInfoString = dynamic_cast<VisualStyledString*>(aNotification.getObject());
					if (styledTrackInfoString) {
						VisualImage styledTrackInfoStringImage;
						bool success = styledTrackInfoStringImage.initWithStyledString(*styledTrackInfoString);
						if (success) {
							VisualNotification trackInfoImageNotification;
							trackInfoImageNotification.setKey(kTrackInfoImageMsg);
							trackInfoImageNotification.setObject(styledTrackInfoStringImage);
							trackInfoImageNotification.post();
						}
					}
				} else if (aNotification.getIdentifier() == VisualDataStore::styledTrackLyricsStringId) {
					VisualStyledString* styledTrackLyricsString = dynamic_cast<VisualStyledString*>(aNotification.getObject());
					if (styledTrackLyricsString) {
						VisualImage styledTrackLyricsStringImage;
						bool success = styledTrackLyricsStringImage.initWithStyledString(*styledTrackLyricsString);
						if (success) {
							VisualNotification trackLyricsImageNotification;
							trackLyricsImageNotification.setKey(kTrackLyricsImageMsg);
							trackLyricsImageNotification.setObject(styledTrackLyricsStringImage);
							trackLyricsImageNotification.post();
						}
					}
				}
				break;
			case kLoadingEncodedImageDataCompletedMsg:
			{
				VisualImage* visualImage = reinterpret_cast<VisualImage*>(aNotification.getPointer());
				if (visualImage) {
					bool success = visualImage->initWithLoadedEncodedData();
					if (success) {
						VisualNotification imageCreationCompletedNotification(aNotification.getIdentifier());
						imageCreationCompletedNotification.setKey(kImageWithIdentifierLoadedAndCreatedMsg);
						imageCreationCompletedNotification.post();
					}
				}
			}
				break;
			case kImageWriteToPNGFileMsg:
			case kImageWriteToPNGFileAndDeleteMsg:
			{
				VisualImage* visualImage = reinterpret_cast<VisualImage*>(aNotification.getPointer());
				if (visualImage) {
					bool success = false;
					VisualFile* aFile = dynamic_cast<VisualFile*>(aNotification.getObject());
					success = visualImage->writeToPNGFile(*aFile);
					if (aKey == kImageWriteToPNGFileAndDeleteMsg) {
						delete visualImage;
					}
				}
			}
				break;
			default:
				char notificationString[32];
				VisualNotification::convertNotificationKeyToString(aKey, notificationString);
				char errLog[256];
				sprintf(errLog, "Unhandled VisualNotificationKey %s in file: %s (line: %d) [%s])", notificationString, __FILE__, __LINE__, __FUNCTION__);
				writeLog(errLog);
		}

	}

	AudioPlayState playState = theVisualPlayerState->getAudioPlayState();
	if (playState == kAudioPlayStarted) {
		uint32 fadeInTimeOnPlayInMS = VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnPlayInMS);
		if (theVisualPlayerState->getElapsedAudioPlayStartTime() > fadeInTimeOnPlayInMS) {
			theVisualPlayerState->setAudioPlayState(kAudioIsPlaying);
		}
	} else if (playState == kAudioPlayResumed) {
		uint32 fadeInTimeOnResumeInMS = VisualPreferences::getValue(VisualPreferences::kFadeInTimeOnResumeInMS);
		if (theVisualPlayerState->getElapsedAudioPlayStartTime() > fadeInTimeOnResumeInMS) {
			theVisualPlayerState->setAudioPlayState(kAudioIsPlaying);
		}
	}

	monitorRenderMessageProcess();

	// show action
	VisualStageControl::doEnsembleShow(*theVisualPlayerState);
	
	theVisualGraphics->finishGLDrawing();

#if TARGET_OS_MAC	
	if ((theVisualPlayerState->getPlayerShowMode() & kIsShowingInFullScreen) == kIsShowingInFullScreen) {
		OSErr err = noErr;
		err = UpdateSystemActivity(UsrActivity); // indicate user activity to prevent screen saver or sleep mode from kicking in
	}
#endif
	
	isRendering = false;
	
}


void VisualMainAction::finishRenderAction() {
	VisualStageControl::dispose();
	VisualAnimationQueue::dispose();
	VisualNotificationQueue::dispose();
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
#if TARGET_OS_WIN
	theVisualGraphics->terminateGdiplus();
#endif
    theVisualGraphics->disposeContext();
	VisualGraphics::dispose();
}


void VisualMainAction::handleHideWindowMessage() {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
    theVisualPlayerState->setPlayerShowMode(kIsNotShowing);
	
	VisualMainAction::finishRenderAction();
}


void VisualMainAction::handleIdleAndUpdateMessage() {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		VisualMainAction::renderAction();
	}
}


void VisualMainAction::handleAudioPlayStoppedEvent() {

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	
	VisualTiming::resetTimestamp("trackPlayStop");
	
	if (theVisualPlayerState->remainingAudioTimeIsKnown() == true && theVisualPlayerState->getRemainingAudioTime() > 2000) {
		// if more than 2 secs. of audio is left to be played,
		// we assume that playing paused
		theVisualPlayerState->setAudioPlayState(kAudioPlayPaused);
		if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
			VisualNotification::post(kAudioPlayPausedEvt);
		}
	} else {
		theVisualPlayerState->setAudioPlayState(kAudioIsNotPlaying);
		VisualMainAction::setTrackPlayPositionInMS(0);
		if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
			VisualNotification::post(kAudioPlayStoppedEvt);
		}
	}
}


void VisualMainAction::handleAudioPlayStartedEvent(bool trackDidChange) {

	bool success = false;

	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	AudioPlayState prevAudioPlayState = theVisualPlayerState->getAudioPlayState();
	
	VisualTiming::resetTimestamp("trackPlayStart");
	
	if (prevAudioPlayState == kAudioIsNotPlaying) {
		theVisualPlayerState->setAudioPlayState(kAudioPlayStarted);
	} else {
		if (trackDidChange == true) {
			VisualMainAction::setTrackPlayPositionInMS(0);
			theVisualPlayerState->setAudioPlayState(kAudioPlayStarted);
			prevAudioPlayState = kAudioIsNotPlaying;
		} else {
			theVisualPlayerState->setAudioPlayState(kAudioPlayResumed);
		}
	}

	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		if (theVisualPlayerState->getAudioPlayState() == kAudioPlayResumed) {
			VisualNotification::post(kAudioPlayResumedEvt);
		} else {
			if (trackDidChange) {
				VisualNotification::post(kAudioPlayTrackChangedEvt);
				VisualMainAction::postTrackInfoOfCurrentAudioData();
				VisualMainAction::postAlbumCoverArtworkOfCurrentAudioData();
				success = VisualMainAction::postLyricsOfCurrentAudioData();
			} else {
				VisualNotification::post(kAudioPlayStartedEvt);
			}
		}
	}

}


void VisualMainAction::setTrackPlayPositionInMS(int trackPlayPosition) {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	theVisualPlayerState->setTrackPlayPositionInMS(trackPlayPosition);
}


void VisualMainAction::handleShowShouldStartMessage(bool isFullScreen) {
    
    srand((unsigned int)time(NULL)); // Seed random number generator

	bool success = VisualMainAction::prepareRenderAction();

	if (!success) {
		VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
		theVisualPlayerState->setPlayerShowMode(kErrorState);
	} else {
		VisualUpdateManager::checkForUpdate();
	}
	
}


void VisualMainAction::closeThreads() {
	VisualNetwork::dispose();
	VisualThreadingManager::dispose();
	VisualUpdateManager::dispose();
}


void VisualMainAction::closePersistentStore() {
	VisualDataStore::dispose();
	VisualConfiguration::dispose();
	VisualPreferences::dispose();
	VisualPlayerState::dispose();
	VisualConfigurationDialog::dispose();
	VisualFontManager::dispose();
	VisualTiming::dispose();
	VisualAudioLab::dispose();
}


void VisualMainAction::handleCanvasReshape() {
	VisualGraphics* theVisualGraphics = VisualGraphics::getInstance();
	theVisualGraphics->setCurrentContext();
	theVisualGraphics->setSurfaceViewport();
	theVisualGraphics->clearSurfaceBackground();
	theVisualGraphics->setCanvasViewport();
	theVisualGraphics->clearCanvasBackground();
	VisualNotification::post(kCanvasReshapeEvt);
}


void VisualMainAction::handleOSEvent(void* anEvent) {
	VisualPlayerState* theVisualPlayerState = VisualPlayerState::getInstance();
	if (theVisualPlayerState->getPlayerShowMode() & kIsShowing) {
		char theChar;
		bool success = VisualQuickTime::getPressedKeyOfEvent(anEvent, theChar);
		if (success)
		{
			VisualNotification aVisualNotification;
			aVisualNotification.setKey(kKeyPressedEvt);
			aVisualNotification.setData(&theChar, 1);
			VisualNotificationQueue::push(aVisualNotification);
		}
	}
}


void VisualMainAction::showConfigurationDialog() {
	VisualConfigurationDialog::show();
}


void VisualMainAction::resetCurrAudioMetaData(void) {
	VisualDataStore::resetCurrAudioMetaData();
}


void VisualMainAction::setAudioTrackName(const void* const audioTrackName, int audioTrackNameLength) {
	VisualDataStore::setAudioTrackName((const uint16*)audioTrackName, audioTrackNameLength);
}


void VisualMainAction::setAudioTrackArtistName(const void* const audioTrackArtistName, int audioTrackArtistNameLength) {
	VisualDataStore::setAudioTrackArtistName((const uint16*)audioTrackArtistName, audioTrackArtistNameLength);
}


void VisualMainAction::setAudioTrackAlbumName(const void* const audioTrackAlbumName, int audioTrackAlbumNameLength) {
	VisualDataStore::setAudioTrackAlbumName((const uint16*)audioTrackAlbumName, audioTrackAlbumNameLength);
}


void VisualMainAction::setAudioTrackComposer(const void* const audioTrackComposer, int audioTrackComposerLength) {
	VisualDataStore::setAudioTrackComposer((const uint16*)audioTrackComposer, audioTrackComposerLength);
}


void VisualMainAction::setAudioStreamTitleV1(const char* const audioStreamTitle, int audioStreamTitleLength) {
	VisualDataStore::setAudioStreamTitle(audioStreamTitle, audioStreamTitleLength);
}


void VisualMainAction::setAudioStreamTitle(const void* const audioStreamTitle, int audioStreamTitleLength) {
	VisualDataStore::setAudioStreamTitle((const uint16*)audioStreamTitle, audioStreamTitleLength);
}


void VisualMainAction::setAudioStreamMessageV1(const char* const audioStreamMessage, int audioStreamMessageLength) {
	VisualDataStore::setAudioStreamMessage(audioStreamMessage, audioStreamMessageLength);
}


void VisualMainAction::setAudioStreamMessage(const void* const audioStreamMessage, int audioStreamMessageLength) {
	VisualDataStore::setAudioStreamMessage((const uint16*)audioStreamMessage, audioStreamMessageLength);
}


void VisualMainAction::setAudioStreamURLV1(const char* const audioStreamURL, int audioStreamURLLength) {
	VisualDataStore::setAudioStreamURL(audioStreamURL, audioStreamURLLength);
}


void VisualMainAction::setAudioStreamURL(const void* const audioStreamURL, int audioStreamURLLength) {
	VisualDataStore::setAudioStreamURL((const uint16*)audioStreamURL, audioStreamURLLength);
}


void VisualMainAction::setAudioTrackSizeInBytes(int audioTrackSizeInBytes) {
	VisualDataStore::setAudioTrackSizeInBytes(audioTrackSizeInBytes);
}


void VisualMainAction::setAudioTrackYear(int aYear) {
	VisualDataStore::setAudioTrackYear(aYear);
}


void VisualMainAction::setAudioDataIsStream(bool isStream) {
	VisualDataStore::setAudioDataIsStream(isStream);
}


bool VisualMainAction::analyzeCurrentlySetMetadata() {
	bool audioTrackDidChange = VisualDataStore::analyzeCurrentlySetMetadata();
	return audioTrackDidChange;
}


void VisualMainAction::setTotalTimeOfCurrentTrack(int totalTimeInMS) {
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	theVisualAudioLab->setTotalTimeOfCurrentTrack(totalTimeInMS);
}


void VisualMainAction::setStartTimeOfCurrentTrack(int startTimeInMS) {
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	theVisualAudioLab->setStartTimeOfCurrentTrack(startTimeInMS);
}


void VisualMainAction::setStopTimeOfCurrentTrack(int stopTimeInMS) {
	VisualAudioLab* theVisualAudioLab = VisualAudioLab::getInstance();
	theVisualAudioLab->setStopTimeOfCurrentTrack(stopTimeInMS);
}


void VisualMainAction::postTrackInfoOfCurrentAudioData() {
	const VisualString trackInfo = VisualDataStore::getInfoOfCurrentAudioDataForDisplay();
	if (trackInfo.getNumberOfNonWhitespaceCharacters() > 0) {
	
		VisualNotification trackInfoStringStyleNotification;
		trackInfoStringStyleNotification.setKey(kGetTrackInfoStringStyleMsg);
		VisualStageControl::dispatchNotification(trackInfoStringStyleNotification);
		
		VisualStyledString styledTrackInfoString;
		styledTrackInfoString.initWithVisualString(trackInfo);
		const VisualStringStyle* const trackInfoStringStyle = dynamic_cast<const VisualStringStyle* const>(trackInfoStringStyleNotification.getObject());
		if (trackInfoStringStyle) {
			styledTrackInfoString.setStyle(*trackInfoStringStyle);
		}
		
		VisualNotification styledTrackInfoStringNotification(VisualDataStore::styledTrackInfoStringId);
		styledTrackInfoStringNotification.setKey(kStyledStringMsg);
		styledTrackInfoStringNotification.setObject(styledTrackInfoString);
		styledTrackInfoStringNotification.post();
	}
}


void VisualMainAction::postAlbumCoverArtworkOfCurrentAudioData() {
	VisualImage* coverArtImage = VisualDataStore::createCoverArtImage();
	if (coverArtImage != NULL) {
		VisualNotification albumCoverArtworkImageNotification(VisualDataStore::albumCoverArtworkImageId);
		albumCoverArtworkImageNotification.setKey(kImageWithIdentifierMsg);
		albumCoverArtworkImageNotification.setObject(*coverArtImage);
		albumCoverArtworkImageNotification.post();
		delete coverArtImage;
	}
}


bool VisualMainAction::postLyricsOfCurrentAudioData() {
	bool success = false;
	success = VisualDataStore::createLyricsOfCurrentTrack();
	return success;
}


void VisualMainAction::writeLogEntry(const char* const infoStr) {
	writeLog(infoStr);
}
