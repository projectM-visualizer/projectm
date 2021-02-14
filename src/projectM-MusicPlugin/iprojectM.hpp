//
// File:       iTunesPlugIn.h
//
// Abstract:   Visual plug-in for iTunes
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
// Copyright © 2001-2011 Apple Inc. All Rights Reserved.
//

#ifndef ITUNESPLUGIN_H
#define ITUNESPLUGIN_H

#include "iTunesVisualAPI.h"
#include "libprojectM/projectM.hpp"
#include "getConfigFilename.h"
#include <time.h>
#include <OpenGL/gl3.h>

#if TARGET_OS_MAC
#import <Cocoa/Cocoa.h>
#endif

#if TARGET_OS_WIN32
#include <Gdiplus.h>
#endif // TARGET_OS_WIN32

//-------------------------------------------------------------------------------------------------
//	build flags
//-------------------------------------------------------------------------------------------------
#define USE_SUBVIEW						(TARGET_OS_MAC && 1)		// use a custom NSView subview on the Mac

//-------------------------------------------------------------------------------------------------
//	typedefs, structs, enums, etc.
//-------------------------------------------------------------------------------------------------

#define	kTVisualPluginCreator			'hook'

#define	kTVisualPluginMajorVersion		3
#define	kTVisualPluginMinorVersion	    0
#define	kTVisualPluginReleaseStage		betaStage
#define	kTVisualPluginNonFinalRelease	0

struct VisualPluginData;

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

// "namespace" our ObjC classname to avoid load conflicts between multiple visualizer plugins
#define VisualView		iProjectM_VisualView

@class VisualView;


#define kInfoTimeOutInSeconds		10							// draw info/artwork for N seconds when it changes or playback starts
#define kPlayingPulseRateInHz		60							// when iTunes is playing, draw N times a second
#define kStoppedPulseRateInHz		5							// when iTunes is not playing, draw N times a second

struct VisualPluginData
{
	void *				appCookie;
	ITAppProcPtr		appProc;
    
    // projectM stuff
    projectM            *pm;

#if TARGET_OS_MAC
	NSView*		        destView;
	NSRect				destRect;
	#if USE_SUBVIEW
	VisualView*			subview;								// custom subview
	#endif
	NSImage *			currentArtwork;
#else
	HWND				destView;
	RECT				destRect;
	Gdiplus::Bitmap* 	currentArtwork;
	long int			lastDrawTime;
#endif
	OptionBits			destOptions;

	RenderVisualData	renderData;
	UInt32				renderTimeStampID;
	
	ITTrackInfo			trackInfo;
	ITStreamInfo		streamInfo;

	// Plugin-specific data

	Boolean				playing;								// is iTunes currently playing audio?
	Boolean				padding[3];
    
    Boolean             readyToDraw;

	time_t				drawInfoTimeOut;						// when should we stop showing info/artwork?

	UInt8				minLevel[kVisualMaxDataChannels];		// 0-128
	UInt8				maxLevel[kVisualMaxDataChannels];		// 0-128
};
typedef struct VisualPluginData VisualPluginData;

void		GetVisualName( ITUniStr255 name );
OptionBits	GetVisualOptions( void );
OSStatus	RegisterVisualPlugin( PluginMessageInfo * messageInfo );

OSStatus	ActivateVisual( VisualPluginData * visualPluginData, VISUAL_PLATFORM_VIEW destView, OptionBits options );
OSStatus	MoveVisual( VisualPluginData * visualPluginData, OptionBits newOptions );
OSStatus	DeactivateVisual( VisualPluginData * visualPluginData );
OSStatus	ResizeVisual( VisualPluginData * visualPluginData );

void		ProcessRenderData( VisualPluginData * visualPluginData, UInt32 timeStampID, const RenderVisualData * renderData );
void		ResetRenderData( VisualPluginData * visualPluginData );
void		UpdateInfoTimeOut( VisualPluginData * visualPluginData );
void		UpdateTrackInfo( VisualPluginData * visualPluginData, ITTrackInfo * trackInfo, ITStreamInfo * streamInfo );
void		UpdateArtwork( VisualPluginData * visualPluginData, VISUAL_PLATFORM_DATA coverArt, UInt32 coverArtSize, UInt32 coverArtFormat );
void		UpdatePulseRate( VisualPluginData * visualPluginData, UInt32 * ioPulseRate );

void		DrawVisual( VisualPluginData * visualPluginData );
void		PulseVisual( VisualPluginData * visualPluginData, UInt32 timeStampID, const RenderVisualData * renderData, UInt32 * ioPulseRate );
void		InvalidateVisual( VisualPluginData * visualPluginData );

OSStatus	ConfigureVisual( VisualPluginData * visualPluginData );

void        initProjectM( VisualPluginData * visualPluginData, std::string presetPath );
void        renderProjectMTexture( VisualPluginData * visualPluginData );
void        keypressProjectM( VisualPluginData * visualPluginData, projectMEvent event, projectMKeycode keycode, projectMModifier mod );

#endif
