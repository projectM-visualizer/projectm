//
// File:       iTunesVisualAPI.h
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
#ifndef ITUNESVISUALAPI_H_
#define ITUNESVISUALAPI_H_

#include "iTunesAPI.h"

#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=power
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 4)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(4)
#endif


enum {
	kCurrentITStreamInfoVersion = 1
};

enum {
	kITVisualPluginMajorMessageVersion = 10,
	kITVisualPluginMinorMessageVersion = 7
};

enum {
	/* VisualPlugin messages */
	
	kVisualPluginInitMessage			= 'init',
	kVisualPluginCleanupMessage			= 'clr ',
	
	kVisualPluginIdleMessage			= 'null',

	kVisualPluginConfigureMessage		= 'cnfg',	/* Configure the plugin. */
	
	kVisualPluginEnableMessage			= 'von ',	/* Enable the plugin and make it available to the user (automatic). */
	kVisualPluginDisableMessage			= 'voff',	/* Disable the plugin. */
	
	kVisualPluginActivateMessage		= 'Vact',	/* Visualizer is being shown on screen (allocate large memory here) */
	kVisualPluginWindowChangedMessage	= 'Vmov',	/* The visualizer context was moved to a new window.  A draw message will be sent immediately. */
	kVisualPluginDeactivateMessage		= 'Vdct',	/* Visualizer is being removed from the screen (deallocate large memory here) */

	kVisualPluginPulseMessage			= 'Vpls',	/* Sent at the rate requested during plugin registration. Contains new data if currently playing audio. */
	kVisualPluginDrawMessage			= 'Vdrw',	/* Draw a new frame.  Sent when the OS decides to repaint the backing view. */
	kVisualPluginFrameChangedMessage	= 'Vfrm',	/* The visualizer area resized.  A draw message will be sent immediately. */

	kVisualPluginPlayMessage			= 'Vply',	/* Starting playback. */
	kVisualPluginChangeTrackMessage		= 'Ctrk',	/* Current track changed or info about the current track has changed. */
	kVisualPluginSetPositionMessage		= 'setp',	/* Setting the position of the currently playing track. */
	kVisualPluginStopMessage			= 'vstp',	/* Stopping playback. */
	kVisualPluginCoverArtMessage		= 'Vart',	/* Delivers the current track artwork as requested by the plugin. Plugin must retain/copy it if it wants to keep it. */

	kVisualPluginDisplayChangedMessage	= 'dchn'	/* Something about display state changed. */

};

/*
	VisualPlugin messages
*/

enum {
	kVisualMaxDataChannels		= 2,

	kVisualNumWaveformEntries	= 512,
	kVisualNumSpectrumEntries	= 512
};

enum {
	/* CoverArt format types */
	kVisualCoverArtFormatJPEG	= 13,
	kVisualCoverArtFormatPNG	= 14,
	kVisualCoverArtFormatBMP	= 27
};

enum {
	/* Activate options */
	
	kWindowIsFullScreen = (1u << 0)
};

struct RenderVisualData {
	UInt8							numWaveformChannels;
	UInt8							waveformData[kVisualMaxDataChannels][kVisualNumWaveformEntries];
	
	UInt8							numSpectrumChannels;
	UInt8							spectrumData[kVisualMaxDataChannels][kVisualNumSpectrumEntries];
};
typedef struct RenderVisualData RenderVisualData;

struct VisualPluginInitMessage {
	UInt32							messageMajorVersion;	/* Input */
	UInt32							messageMinorVersion;	/* Input */
	NumVersion						appVersion;				/* Input */

	void *							appCookie;				/* Input */
	ITAppProcPtr					appProc;				/* Input */

	OptionBits						unused;					/* N/A */
	void *							refCon;					/* Output */
};
typedef struct VisualPluginInitMessage VisualPluginInitMessage;

struct VisualPluginActivateMessage {
	VISUAL_PLATFORM_VIEW			view;					/* Input - plugin should draw in entire bounds */
	OptionBits						options;				/* Input */
};
typedef struct VisualPluginActivateMessage VisualPluginActivateMessage;

struct VisualPluginWindowChangedMessage {
	OptionBits						options;				/* Input */
};
typedef struct VisualPluginWindowChangedMessage VisualPluginWindowChangedMessage;

struct VisualPluginPulseMessage {
	RenderVisualData *				renderData;				/* Input */
	UInt32							timeStampID;			/* Input */
	UInt32							currentPositionInMS;	/* Input */
	
	UInt32							newPulseRateInHz;		/* Input/Output - contains current rate on input, modify it to get a new rate. */
};
typedef struct VisualPluginPulseMessage VisualPluginPulseMessage;

struct VisualPluginPlayMessage {
	ITTrackInfo *					trackInfo;				/* Input */
	ITStreamInfo *					streamInfo;				/* Input */

	AudioStreamBasicDescription		audioFormat;			/* Input */
	UInt32							bitRate;				/* Input */
	SInt32							volume;					/* Input */
};
typedef struct VisualPluginPlayMessage VisualPluginPlayMessage;

struct VisualPluginChangeTrackMessage {
	ITTrackInfo *					trackInfo;				/* Input */
	ITStreamInfo *					streamInfo;				/* Input */
};
typedef struct VisualPluginChangeTrackMessage VisualPluginChangeTrackMessage;

struct VisualPluginCoverArtMessage {
	VISUAL_PLATFORM_DATA		coverArt;					/* input - client must retain (mac) or copy (windows) the data if they want to keep it after this message completes.
																	 - note that coverArt will be NULL if the current track has no artwork */
	UInt32						coverArtSize;				/* input - size of the coverArt in bytes */
	UInt32						coverArtFormat;				/* input - format of cover art */
};
typedef struct VisualPluginCoverArtMessage VisualPluginCoverArtMessage;

struct VisualPluginSetPositionMessage {
	UInt32							positionTimeInMS;		/* Input */
};
typedef struct VisualPluginSetPositionMessage VisualPluginSetPositionMessage;

enum {
	kVisualDisplayDepthChanged 	= 1 << 0,					/* the display's depth has changed */
	kVisualDisplayRectChanged	= 1 << 1,					/* the display's location changed */
	kVisualWindowMoved			= 1 << 2,					/* the window has moved location */	
	kVisualDisplayConfigChanged	= 1 << 3,					/* something else about the display changed */
};

struct VisualPluginDisplayChangedMessage {
	UInt32							flags;		/* Input */
};
typedef struct VisualPluginDisplayChangedMessage VisualPluginDisplayChangedMessage;


struct VisualPluginMessageInfo {
	union {
		VisualPluginInitMessage					initMessage;
		VisualPluginActivateMessage				activateMessage;
		VisualPluginWindowChangedMessage		windowChangedMessage;
		VisualPluginPulseMessage				pulseMessage;
		VisualPluginPlayMessage					playMessage;
		VisualPluginChangeTrackMessage			changeTrackMessage;
		VisualPluginSetPositionMessage			setPositionMessage;
		VisualPluginCoverArtMessage				coverArtMessage;
		VisualPluginDisplayChangedMessage		displayChangedMessage;
	} u;
};
typedef struct VisualPluginMessageInfo VisualPluginMessageInfo;

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef __cplusplus
}
#endif

#endif /* ITUNESVISUALAPI_H_ */
