/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualMain.cpp
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

#include "VisualMain.h"
#include "VisualMainAction.h"
#include "VisualSignature.h"
#include "VisualConfiguration.h"

#include <stdio.h>

using namespace VizKit;


void VisualMain::convertVisualPluginMessageToString(const OSType visualPluginMessage, char* outString) {
	const char* messageString;
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


bool VisualMain::storeAudioTrackMetadata(const ITTrackInfo* const trackMetadata, bool isAudioStream) {
	
	const uint16* trackName = trackMetadata->name;
	if (trackName) {
		trackName += 1;
		VisualMainAction::setAudioTrackName((void*)trackName, (int)trackMetadata->name[0]);
	}
	
	const uint16* trackArtist = trackMetadata->artist;
	if (trackArtist) {
		trackArtist += 1;
		VisualMainAction::setAudioTrackArtistName((void*)trackArtist, (int)trackMetadata->artist[0]);
	}
	
	const uint16* trackAlbum = trackMetadata->album;
	if (trackAlbum) {
		trackAlbum += 1;
		VisualMainAction::setAudioTrackAlbumName((void*)trackAlbum, (int)trackMetadata->album[0]);
	}

	const uint16* trackComposer = trackMetadata->composer;
	if (trackComposer) {
		trackComposer += 1;
		VisualMainAction::setAudioTrackComposer((void*)trackComposer, (int)trackMetadata->composer[0]);
	}
	
	if (trackMetadata->year > 0) {
		VisualMainAction::setAudioTrackYear(trackMetadata->year);
	}

	VisualMainAction::setAudioDataIsStream(isAudioStream);

	bool audioTrackDidChange = VisualMainAction::analyzeCurrentlySetMetadata();

	if (!isAudioStream) {
		VisualMainAction::setAudioTrackSizeInBytes(static_cast<int>(trackMetadata->sizeInBytes));
		VisualMainAction::setTotalTimeOfCurrentTrack(trackMetadata->totalTimeInMS);
		VisualMainAction::setStartTimeOfCurrentTrack(trackMetadata->startTimeInMS);
		VisualMainAction::setStopTimeOfCurrentTrack(trackMetadata->stopTimeInMS);
	}
	
	return audioTrackDidChange;
	
}


void VisualMain::storeAudioStreamMetadata(const ITStreamInfo* const streamMetadata) {
	
	const uint16* streamTitle = streamMetadata->streamTitle;
	if (streamTitle) {
		streamTitle += 1;
		VisualMainAction::setAudioStreamTitle(streamTitle, (int)streamMetadata->streamTitle[0]);
	}

	const uint16* streamMessage = streamMetadata->streamMessage;
	if (streamMessage) {
		streamMessage += 1;
		VisualMainAction::setAudioStreamMessage(streamMessage, (int)streamMetadata->streamMessage[0]);
	}

	const uint16* streamURL = streamMetadata->streamURL;
	if (streamURL) {
		streamURL += 1;
		VisualMainAction::setAudioStreamURL(streamURL, (int)streamMetadata->streamURL[0]);
	}

	VisualMainAction::setTotalTimeOfCurrentTrack(0);
	
}


void VisualMain::storeAudioStreamMetadata(const ITStreamInfoV1* const streamMetadata) {
	
	const char* streamTitle = reinterpret_cast<const char*>(streamMetadata->streamTitle);
	if (streamTitle) {
		streamTitle += 1;
		VisualMainAction::setAudioStreamTitleV1(streamTitle, (int)streamMetadata->streamTitle[0]);
	}

	const char* streamMessage = reinterpret_cast<const char*>(streamMetadata->streamMessage);
	if (streamMessage) {
		streamMessage += 1;
		VisualMainAction::setAudioStreamMessageV1(streamMessage, (int)streamMetadata->streamMessage[0]);
	}

	const char* streamURL = reinterpret_cast<const char*>(streamMetadata->streamURL);
	if (streamURL) {
		streamURL += 1;
		VisualMainAction::setAudioStreamURLV1(streamURL, (int)streamMetadata->streamURL[0]);
	}

	VisualMainAction::setTotalTimeOfCurrentTrack(0);

}


OSStatus VisualMain::VisualPluginHandler(OSType message, VisualPluginMessageInfo* visualPluginMessageInfo, void* refCon) {

	bool debug = false;

    OSStatus status;
	
    UInt32 majorVersionNum;
    UInt32 minorVersionNum;

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
		if (message != messageWriteLogHistory[curr]) {
			// do not log alternating messages (like render/idle/update messages during render loop)
			if (message == messageWriteLogHistory[prev] && prevMessage == messageWriteLogHistory[curr]) doWriteLog = false;
			if (message == messageWriteLogHistory[oneBeforePrev] && prevMessage == messageWriteLogHistory[curr] && oneBeforePrevMessage == messageWriteLogHistory[prev]) doWriteLog = false;
			if (message == messageWriteLogHistory[prev] && prevMessage == messageWriteLogHistory[oneBeforePrev] && oneBeforePrevMessage == messageWriteLogHistory[curr]) doWriteLog = false;
			if (doWriteLog == true) {
				char visualPluginMessage[64];
				char logStr[128];
				VisualMain::convertVisualPluginMessageToString(message, visualPluginMessage);
				sprintf(logStr, "VisualPluginHandler: message: %s", visualPluginMessage);
				VisualMainAction::writeLogEntry(logStr);
				messageWriteLogHistory[oneBeforePrev] = messageWriteLogHistory[prev];
				messageWriteLogHistory[prev] = messageWriteLogHistory[curr];
				messageWriteLogHistory[curr] = message;
				oneBeforePrevMessage = prevMessage;
				prevMessage = message;
			}
		}
	}

	VisualMainAction::updateTiming();

    switch (message) {
	
        case kVisualPluginInitMessage:
			// Sent when the visual plugin is registered.
			// The plugin should do minimal memory allocations here. 

			majorVersionNum = visualPluginMessageInfo->u.initMessage.messageMajorVersion;
			minorVersionNum = visualPluginMessageInfo->u.initMessage.messageMinorVersion;

			visualPluginMessageInfo->u.initMessage.options = (kVisualDoesNotNeedResolutionSwitch);

			VisualSignature::setAppCookie(visualPluginMessageInfo->u.initMessage.appCookie);
			VisualSignature::setAppProc(visualPluginMessageInfo->u.initMessage.appProc);

			VisualMainAction::setAppVersionNum(visualPluginMessageInfo->u.initMessage.appVersion.majorRev, visualPluginMessageInfo->u.initMessage.appVersion.minorAndBugRev);
			
			VisualMainAction::handleInitMessage(kVisualMaxDataChannels, kVisualNumWaveformEntries, kVisualNumSpectrumEntries);
			
			//visualPluginMessageInfo->u.initMessage.refCon	= (void*) visualPluginData;
			
            break;

        case kVisualPluginCleanupMessage:
			// Sent when the visual plugin is unloaded
			// This message is sent when iTunes is about to quit. 
			// You should free any resources allocated by your visualizer plug-in at this time.

            VisualMainAction::closeThreads();
			VisualMainAction::closePersistentStore();

            break;
                
        case kVisualPluginEnableMessage:
			// Sent when the visual plugin is enabled. 
			// iTunes currently enables all loaded visual plugins. 
			// The plugin should not do anything here.
			
			break;

        case kVisualPluginDisableMessage:
			
            break;

        case kVisualPluginIdleMessage:
			// Sent if the plugin requests idle messages. 
			// Do this by setting the kVisualWantsIdleMessages option in the RegisterVisualMessage.options field.

			VisualMainAction::handleIdleAndUpdateMessage();
			
            break;

        case kVisualPluginConfigureMessage:
			// Sent if the plugin requests the ability for the user to configure it. 
			// Do this by setting the kVisualWantsConfigure option in the RegisterVisualMessage.options field.

			VisualMainAction::showConfigurationDialog();

			break;
        
        case kVisualPluginShowWindowMessage:
			// Sent when iTunes is going to show the visual plugin in a port. 
			// At this point, the plugin should allocate any large buffers it needs.
			
			{
				VisualMainAction::setGraphicsDevice(visualPluginMessageInfo->u.showWindowMessage.GRAPHICS_DEVICE_NAME);
				
				VisualMainAction::setTotalVisualizerRectVerticalOffsetFromBottom(visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.top, visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.bottom - visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.top);
				VisualMainAction::setTotalVisualizerRect(visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.top, visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.left, visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.bottom, visualPluginMessageInfo->u.showWindowMessage.totalVisualizerRect.right);
				VisualMainAction::setDrawRect(visualPluginMessageInfo->u.showWindowMessage.drawRect.top, visualPluginMessageInfo->u.showWindowMessage.drawRect.left, visualPluginMessageInfo->u.showWindowMessage.drawRect.bottom, visualPluginMessageInfo->u.showWindowMessage.drawRect.right);
				bool isFullscreen = ((visualPluginMessageInfo->u.showWindowMessage.options & kWindowIsFullScreen) > 0);
				VisualMainAction::handleShowWindowMessage(isFullscreen);
			}

            break;

        case kVisualPluginHideWindowMessage:
			// Sent when iTunes is no longer displayed.

			VisualMainAction::handleHideWindowMessage();
			
			VisualMainAction::closeThreads();
            
            break;
		
        case kVisualPluginSetWindowMessage:
			// Sent when iTunes needs to change the port or rectangle of the currently displayed visual.

			{
				VisualMainAction::setTotalVisualizerRectVerticalOffsetFromBottom(visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.top, visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.bottom - visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.top);
				VisualMainAction::setTotalVisualizerRect(visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.top, visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.left, visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.bottom, visualPluginMessageInfo->u.setWindowMessage.totalVisualizerRect.right);
				VisualMainAction::setDrawRect(visualPluginMessageInfo->u.setWindowMessage.drawRect.top, visualPluginMessageInfo->u.setWindowMessage.drawRect.left, visualPluginMessageInfo->u.setWindowMessage.drawRect.bottom, visualPluginMessageInfo->u.setWindowMessage.drawRect.right);
				bool isFullscreen = ((visualPluginMessageInfo->u.setWindowMessage.options & kWindowIsFullScreen) > 0);
				VisualMainAction::handleSetWindowMessage(isFullscreen);
			}

            break;
        
        case kVisualPluginRenderMessage:
			// Sent for the visual plugin to render a frame.

			VisualMainAction::handleRenderMessage(visualPluginMessageInfo->u.renderMessage.currentPositionInMS, visualPluginMessageInfo->u.renderMessage.timeStampID, visualPluginMessageInfo->u.renderMessage.renderData->numWaveformChannels, kVisualNumWaveformEntries, (char*)visualPluginMessageInfo->u.renderMessage.renderData->waveformData, visualPluginMessageInfo->u.renderMessage.renderData->numSpectrumChannels, kVisualNumSpectrumEntries, (char*)visualPluginMessageInfo->u.renderMessage.renderData->spectrumData);
			
            break;

		case kVisualPluginDisplayChangedMessage:
			// Something about display state changed 
			
			break;
	
        case kVisualPluginUpdateMessage:
			// Sent in response to an update event. 
			// The visual plugin should update into its remembered port. 
			// This will only be sent if the plugin has been previously given a ShowWindow message.
			
			VisualMainAction::handleIdleAndUpdateMessage();
			
            break;        

        case kVisualPluginPlayMessage:
			// Sent when the player starts.

			{
			
				VisualMainAction::resetVisualAudioLabDataIfNotPaused();
				VisualMainAction::resetCurrAudioMetaData();
			
				bool isAudioStream = false;
				if (visualPluginMessageInfo->u.playMessage.streamInfoUnicode) {
					isAudioStream = (bool)(visualPluginMessageInfo->u.playMessage.streamInfoUnicode->streamTitle[0] > 0 ||
											visualPluginMessageInfo->u.playMessage.streamInfoUnicode->streamMessage[0] > 0 ||
											visualPluginMessageInfo->u.playMessage.streamInfoUnicode->streamURL[0] > 0);
				} else if (visualPluginMessageInfo->u.playMessage.streamInfo) {
					isAudioStream = (bool)(visualPluginMessageInfo->u.playMessage.streamInfo->streamTitle[0] > 0 ||
											visualPluginMessageInfo->u.playMessage.streamInfo->streamMessage[0] > 0 ||
											visualPluginMessageInfo->u.playMessage.streamInfo->streamURL[0] > 0);
				}

				if (isAudioStream) {
					if (visualPluginMessageInfo->u.playMessage.streamInfoUnicode) {
						VisualMain::storeAudioStreamMetadata(const_cast<const ITStreamInfo*>(visualPluginMessageInfo->u.playMessage.streamInfoUnicode));
					} else if (visualPluginMessageInfo->u.playMessage.streamInfo) {
						VisualMain::storeAudioStreamMetadata(const_cast<const ITStreamInfoV1*>(visualPluginMessageInfo->u.playMessage.streamInfo));
					}
				}
				bool trackDidChange = VisualMain::storeAudioTrackMetadata(visualPluginMessageInfo->u.playMessage.trackInfoUnicode, isAudioStream);
							
				VisualMainAction::handleAudioPlayStartedEvent(trackDidChange);
			    
			}

            break;

        case kVisualPluginChangeTrackMessage:
			// Sent when the player changes the current track information. 
			// This is used when the information about a track changes, 
			// or when the CD moves onto the next track. 
			// The visual plugin should update any displayed information about the currently playing song.
			
			{
				VisualMainAction::resetCurrAudioMetaData();
				
				bool isAudioStream = false;
				if (visualPluginMessageInfo->u.changeTrackMessage.streamInfoUnicode) {
					isAudioStream = (bool)(visualPluginMessageInfo->u.changeTrackMessage.streamInfoUnicode->streamTitle[0] > 0 ||
											visualPluginMessageInfo->u.changeTrackMessage.streamInfoUnicode->streamMessage[0] > 0 ||
											visualPluginMessageInfo->u.changeTrackMessage.streamInfoUnicode->streamURL[0] > 0);
				} else if (visualPluginMessageInfo->u.changeTrackMessage.streamInfo) {
					isAudioStream = (bool)(visualPluginMessageInfo->u.changeTrackMessage.streamInfo->streamTitle[0] > 0 ||
											visualPluginMessageInfo->u.changeTrackMessage.streamInfo->streamMessage[0] > 0 ||
											visualPluginMessageInfo->u.changeTrackMessage.streamInfo->streamURL[0] > 0);
				}

				if (isAudioStream) {
					if (visualPluginMessageInfo->u.changeTrackMessage.streamInfoUnicode) {
						VisualMain::storeAudioStreamMetadata(const_cast<const ITStreamInfo*>(visualPluginMessageInfo->u.changeTrackMessage.streamInfoUnicode));
					} else if (visualPluginMessageInfo->u.changeTrackMessage.streamInfo) {
						VisualMain::storeAudioStreamMetadata(const_cast<const ITStreamInfoV1*>(visualPluginMessageInfo->u.changeTrackMessage.streamInfo));
					}
				}
				bool trackDidChange = VisualMain::storeAudioTrackMetadata(visualPluginMessageInfo->u.changeTrackMessage.trackInfoUnicode, isAudioStream);
				
				if (trackDidChange) {
					VisualMainAction::resetVisualAudioLabDataIfNotPaused();
					VisualMainAction::handleAudioPlayStartedEvent(trackDidChange);
				}
			}

            break;

        case kVisualPluginStopMessage:
			// Sent when the player stops.

            VisualMainAction::handleAudioPlayStoppedEvent();
			
            break;
        
        case kVisualPluginSetPositionMessage:
			// Sent when the player changes position.

			VisualMainAction::setTrackPlayPositionInMS(visualPluginMessageInfo->u.setPositionMessage.positionTimeInMS);
			
            break;

        case kVisualPluginPauseMessage:
			// Sent when the player pauses. 
			// iTunes does not currently use pause or unpause.
			// A pause in iTunes is handled by stopping and remembering the position.
			
			VisualMainAction::handleAudioPlayStoppedEvent();

            break;
                
        case kVisualPluginUnpauseMessage:
			// Sent when the player unpauses. 
			// iTunes does not currently use pause or unpause.
			// A pause in iTunes is handled by stopping and remembering the position.
			
			{
				bool trackDidChange = false;
				VisualMainAction::handleAudioPlayStartedEvent(trackDidChange);
			}
			
            break;        

        case kVisualPluginEventMessage:
			// Sent to the plugin in response to a OS event. 
			// The plugin should return noErr for any event it handles completely, 
			// or an error (unimpErr) if iTunes should handle it.
			
			{
				EventRecord* eventPtr = visualPluginMessageInfo->u.eventMessage.event;
				VisualMainAction::handleOSEvent((void*)eventPtr);
			}

			status = unimpErr; // each event should be passed on to iTunes

            break;

        default:
		
            status = unimpErr;
			
            break;
			
    }
	
    return status;	

}


OSStatus VisualMain::RegisterVisualPlugin(PluginMessageInfo* pluginMessageInfo) {

    PlayerMessageInfo playerMessageInfo;
    
    memset(&playerMessageInfo.u.registerVisualPluginMessage, 0, sizeof(playerMessageInfo.u.registerVisualPluginMessage));
	
	playerMessageInfo.u.registerVisualPluginMessage.options = (kVisualWantsConfigure | kVisualWantsIdleMessages | kVisualProvidesUnicodeName);

    playerMessageInfo.u.registerVisualPluginMessage.name[0] = (UInt8)strlen(VisualConfiguration::visualizerShowName);
    memcpy(&playerMessageInfo.u.registerVisualPluginMessage.name[1], VisualConfiguration::visualizerShowName, strlen(VisualConfiguration::visualizerShowName));
	
	uint32 numberOfCharactersOfVisualizerShowNameUnicode = 0;
	uint16* visualizerShowNameUnicode = VisualConfiguration::getVisualizerShowNameUnicode(numberOfCharactersOfVisualizerShowNameUnicode);
	playerMessageInfo.u.registerVisualPluginMessage.unicodeName[0] = numberOfCharactersOfVisualizerShowNameUnicode;
	for (uint32 i = 0; i < numberOfCharactersOfVisualizerShowNameUnicode; i++) {
		playerMessageInfo.u.registerVisualPluginMessage.unicodeName[i + 1] = visualizerShowNameUnicode[i];
	}
    
	const UInt8 stage = 0x80; // (dev = 0x20, alpha = 0x40, beta = 0x60, final = 0x80)
	SetNumVersion(&playerMessageInfo.u.registerVisualPluginMessage.pluginVersion, VisualConfiguration::getMajorReleaseNumberAsBCD(), VisualConfiguration::getMinorReleaseNumberAsBCD(), stage, VisualConfiguration::getReleaseRevisionNumber());
    
    playerMessageInfo.u.registerVisualPluginMessage.handler = (VisualPluginProcPtr)VisualMain::VisualPluginHandler;

    playerMessageInfo.u.registerVisualPluginMessage.registerRefCon = 0;
	
	OSType visualPluginCreator = 'hook';
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
    
    OSStatus status = PlayerRegisterVisualPlugin(pluginMessageInfo->u.initMessage.appCookie, pluginMessageInfo->u.initMessage.appProc, &playerMessageInfo);
    
    return status;
	
}


#if TARGET_OS_MAC
OSStatus iTunesPluginMainMachO(OSType message, PluginMessageInfo* pluginMessageInfo, void* refCon)
#endif
#if TARGET_OS_WIN
OSStatus iTunesPluginMain(OSType message, PluginMessageInfo* pluginMessageInfo, void* refCon)
#endif
{
    OSStatus status;
    
    switch (message) {
        case kPluginInitMessage:
			pluginMessageInfo->u.initMessage.options = kPluginWantsDisplayNotification;
            status = VisualMain::RegisterVisualPlugin(pluginMessageInfo);
            break;
                
        case kPluginCleanupMessage:
            status = noErr;
            break;

        case kPluginPrepareToQuitMessage:
            status = noErr;
            break;
                
        default:
            status = unimpErr;
            break;
    }
    
    return status;
}

