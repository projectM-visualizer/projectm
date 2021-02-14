//
// File:       iTunesAPI.c
//
// Abstract:   part of iTunes Visual SDK
//
// Version:    2.0
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ( "Apple" )
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright © 2000-2011 Apple Inc. All Rights Reserved.
//
#include "iTunesAPI.h"
#include "iTunesVisualAPI.h"

#include <string.h>

// SetNumVersion
//
void SetNumVersion(NumVersion *numVersion, UInt8 majorRev, UInt8 minorAndBugRev, UInt8 stage, UInt8 nonRelRev)
{
	numVersion->majorRev		= majorRev;
	numVersion->minorAndBugRev	= minorAndBugRev;
	numVersion->stage			= stage;
	numVersion->nonRelRev		= nonRelRev;
}


// ITCallApplicationInternal
//
static OSStatus ITCallApplicationInternal(void *appCookie, ITAppProcPtr handler, OSType message, UInt32 messageMajorVersion, UInt32 messageMinorVersion, PlayerMessageInfo *messageInfo)
{
	PlayerMessageInfo	localMessageInfo;
	
	if (messageInfo == nil)
	{
		memset(&localMessageInfo, 0, sizeof(localMessageInfo));
		
		messageInfo = &localMessageInfo;
	}
	
	messageInfo->messageMajorVersion = messageMajorVersion;
	messageInfo->messageMinorVersion = messageMinorVersion;
	messageInfo->messageInfoSize	 = sizeof(PlayerMessageInfo);
	
	return handler(appCookie, message, messageInfo);
}

// ITCallApplication
//
OSStatus ITCallApplication(void *appCookie, ITAppProcPtr handler, OSType message, PlayerMessageInfo *messageInfo)
{
	return ITCallApplicationInternal(appCookie, handler, message, kITPluginMajorMessageVersion, kITPluginMinorMessageVersion, messageInfo);
}


// PlayerSetFullScreen
//
OSStatus PlayerSetFullScreen(void *appCookie, ITAppProcPtr appProc, Boolean fullScreen)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.setFullScreenMessage.fullScreen = fullScreen;

	return ITCallApplication(appCookie, appProc, kPlayerSetFullScreenMessage, &messageInfo);
}




// PlayerRequestCurrentTrackCoverArt
//
OSStatus PlayerRequestCurrentTrackCoverArt(void *appCookie, ITAppProcPtr appProc)
{
	OSStatus			status;
	
	status = ITCallApplication(appCookie, appProc, kPlayerRequestCurrentTrackCoverArtMessage, nil);

	return status;
}


// PlayerGetPluginData
//
OSStatus PlayerGetPluginData(void *appCookie, ITAppProcPtr appProc, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize)
{
	OSStatus			status;
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.getPluginDataMessage.dataPtr			= dataPtr;
	messageInfo.u.getPluginDataMessage.dataBufferSize	= dataBufferSize;
	
	status = ITCallApplication(appCookie, appProc, kPlayerGetPluginDataMessage, &messageInfo);
	
	if (dataSize != nil)
		*dataSize = messageInfo.u.getPluginDataMessage.dataSize;
	
	return status;
}


// PlayerSetPluginData
//
OSStatus PlayerSetPluginData(void *appCookie, ITAppProcPtr appProc, void *dataPtr, UInt32 dataSize)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.setPluginDataMessage.dataPtr	= dataPtr;
	messageInfo.u.setPluginDataMessage.dataSize	= dataSize;
	
	return ITCallApplication(appCookie, appProc, kPlayerSetPluginDataMessage, &messageInfo);
}


// PlayerGetPluginNamedData
//
OSStatus PlayerGetPluginNamedData(void *appCookie, ITAppProcPtr appProc, ConstStringPtr dataName, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize)
{
	OSStatus			status;
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.getPluginNamedDataMessage.dataName		= dataName;
	messageInfo.u.getPluginNamedDataMessage.dataPtr			= dataPtr;
	messageInfo.u.getPluginNamedDataMessage.dataBufferSize	= dataBufferSize;
	
	status = ITCallApplication(appCookie, appProc, kPlayerGetPluginNamedDataMessage, &messageInfo);
	
	if (dataSize != nil)
		*dataSize = messageInfo.u.getPluginNamedDataMessage.dataSize;
	
	return status;
}


// PlayerSetPluginNamedData
//
OSStatus PlayerSetPluginNamedData(void *appCookie, ITAppProcPtr appProc, ConstStringPtr dataName, void *dataPtr, UInt32 dataSize)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.setPluginNamedDataMessage.dataName	= dataName;
	messageInfo.u.setPluginNamedDataMessage.dataPtr		= dataPtr;
	messageInfo.u.setPluginNamedDataMessage.dataSize	= dataSize;
	
	return ITCallApplication(appCookie, appProc, kPlayerSetPluginNamedDataMessage, &messageInfo);
}


// PlayerIdle
//
OSStatus PlayerIdle(void *appCookie, ITAppProcPtr appProc)
{
	return ITCallApplication(appCookie, appProc, kPlayerIdleMessage, nil);
}


// PlayerShowAbout
//
void PlayerShowAbout(void *appCookie, ITAppProcPtr appProc)
{
	ITCallApplication(appCookie, appProc, kPlayerShowAboutMessage, nil);
}


// PlayerOpenURL
//
void PlayerOpenURL(void *appCookie, ITAppProcPtr appProc, SInt8 *string, UInt32 length)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.openURLMessage.url	= string;
	messageInfo.u.openURLMessage.length	= length;

	ITCallApplication(appCookie, appProc, kPlayerOpenURLMessage, &messageInfo);
}

// PlayerUnregisterPlugin
//
OSStatus PlayerUnregisterPlugin(void *appCookie, ITAppProcPtr appProc, PlayerMessageInfo *messageInfo)
{
	return ITCallApplication(appCookie, appProc, kPlayerUnregisterPluginMessage, messageInfo);
}


// PlayerRegisterVisualPlugin
//
OSStatus PlayerRegisterVisualPlugin(void *appCookie, ITAppProcPtr appProc, PlayerMessageInfo *messageInfo)
{
	return ITCallApplicationInternal(appCookie, appProc, kPlayerRegisterVisualPluginMessage, kITVisualPluginMajorMessageVersion, kITVisualPluginMinorMessageVersion, messageInfo);
}

// PlayerGetPluginITFileSpec
//
OSStatus PlayerGetPluginITFileSpec(void *appCookie, ITAppProcPtr appProc, ITFileSpec *pluginFileSpec)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.getPluginITFileSpecMessage.fileSpec = pluginFileSpec;
	
	return ITCallApplication(appCookie, appProc, kPlayerGetPluginITFileSpecMessage, &messageInfo);
}

// PlayerGetFileTrackInfo
//
OSStatus PlayerGetFileTrackInfo(void *appCookie, ITAppProcPtr appProc, const ITFileSpec *fileSpec, ITTrackInfo *trackInfo)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.getFileTrackInfoMessage.fileSpec 	= fileSpec;
	messageInfo.u.getFileTrackInfoMessage.trackInfo = trackInfo;
	
	return ITCallApplication(appCookie, appProc, kPlayerGetFileTrackInfoMessage, &messageInfo);
}

// PlayerSetFileTrackInfo
//
OSStatus PlayerSetFileTrackInfo(void *appCookie, ITAppProcPtr appProc, const ITFileSpec *fileSpec, const ITTrackInfo *trackInfo)
{
	PlayerMessageInfo	messageInfo;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	messageInfo.u.setFileTrackInfoMessage.fileSpec 	= fileSpec;
	messageInfo.u.setFileTrackInfoMessage.trackInfo = trackInfo;
	
	return ITCallApplication(appCookie, appProc, kPlayerSetFileTrackInfoMessage, &messageInfo);
}

// PlayerGetITTrackInfoSize
//
OSStatus PlayerGetITTrackInfoSize(void *appCookie, ITAppProcPtr appProc, UInt32 appPluginMajorVersion, UInt32 appPluginMinorVersion, UInt32 *itTrackInfoSize)
{
	PlayerMessageInfo	messageInfo;
	OSStatus			status;
	
	/*
		Note: appPluginMajorVersion and appPluginMinorVersion are the versions given to the plugin by iTunes in the plugin's init message.
			  These versions are *not* the version of the API used when the plugin was compiled.
	*/
	
	*itTrackInfoSize = 0;
	
	memset(&messageInfo, 0, sizeof(messageInfo));
	
	status = ITCallApplication(appCookie, appProc, kPlayerGetITTrackInfoSizeMessage, &messageInfo);
	if( status == noErr )
	{
		*itTrackInfoSize = messageInfo.u.getITTrackInfoSizeMessage.itTrackInfoSize;
	}
	else if( appPluginMajorVersion == 10 && appPluginMinorVersion == 2 )
	{
		// iTunes 2.0.x
 		
		*itTrackInfoSize = ((UInt32)(uintptr_t) &((ITTrackInfo *) 0)->composer);
		
		status = noErr;
	}
	else if( appPluginMajorVersion == 10 && appPluginMinorVersion == 3 )
	{
		// iTunes 3.0.x
		
		*itTrackInfoSize = ((UInt32)(uintptr_t) &((ITTrackInfo *) 0)->beatsPerMinute);
		
		status = noErr;
	}
	else
	{
		// iTunes 4.0 and later implement the kPlayerGetITTrackInfoSizeMessage message. If you got here
		// then the appPluginMajorVersion or appPluginMinorVersion are incorrect.
		
		status = paramErr;
	}
	
	if( status == noErr && (*itTrackInfoSize) > sizeof(ITTrackInfo) )
	{
		// iTunes is using a larger ITTrackInfo than the one when this plugin was compiled. Pin *itTrackInfoSize to the plugin's known size
		
		*itTrackInfoSize = sizeof(ITTrackInfo);
	}
	
	return status;
}
