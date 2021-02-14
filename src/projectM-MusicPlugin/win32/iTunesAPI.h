/*
	File:		iTunesAPI.h
	
	Copyright: 	© Copyright 2003 Apple Computer, Inc. All rights reserved.
	
	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple’s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
				
*/

#ifndef ITUNESAPI_H_
#define ITUNESAPI_H_

#include <windows.h>

#if PRAGMA_ONCE
#pragma once
#endif

typedef unsigned long	UInt32;
typedef signed long		SInt32;
typedef unsigned short	UInt16;
typedef signed short	SInt16;
typedef	unsigned char	UInt8;
typedef signed char		SInt8;

typedef UInt8			Str255[256];
typedef UInt8			Str63[64];
typedef const UInt8 *	ConstStringPtr;
typedef UInt32			OptionBits;
typedef UInt32			OSType;

typedef SInt32			OSStatus;

typedef UInt16			EventKind;

typedef UInt16          EventModifiers;

typedef UInt16			UniChar;

typedef unsigned __int64 UInt64;

typedef void **			Handle;

struct NumVersion {
	UInt8			majorRev;
	UInt8			minorAndBugRev;
	UInt8			stage;
	UInt8			nonRelRev;
};
typedef struct NumVersion NumVersion;

struct Point {
    short                           v;
    short                           h;
};
typedef struct Point                    Point;

struct EventRecord {
    EventKind                       what;
    UInt32                          message;
    UInt32                          when;
    Point                           where;
    EventModifiers                  modifiers;
};
typedef struct EventRecord EventRecord;

typedef UInt8			Boolean;

typedef UInt32			UnsignedFixed;

typedef void *			LogicalAddress;

#define	false			0
#define	true			1
#define	nil				NULL

enum {
	noErr		= 0,
	paramErr	= -50,
	memFullErr	= -108,
	unimpErr	= -4
};

struct Rect {
	short			top;
	short			left;
	short			bottom;
	short			right;
};
typedef struct Rect Rect;

struct SoundComponentData {
	long			flags;
	OSType			format;
	short			numChannels;
	short			sampleSize;
	UnsignedFixed	sampleRate;
	long			sampleCount;
	UInt8 *			buffer;
	long			reserved;
};
typedef struct SoundComponentData SoundComponentData;

#define	VOLUME_REFERENCE		SInt16

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
	kITPluginMajorMessageVersion = 10,
	kITPluginMinorMessageVersion = 5
};

enum {
	kTrackSupportsID3Tags		= (1L << 0),
	kTrackHasVariableBitRate	= (1L << 1),
	kTrackIsLocked				= (1L << 3),		/* Device tracks only. Track cannot be deleted or renamed */
	kTrackCanBeDownloaded		= (1L << 4),		/* Device tracks only. Track can be copied from device to desktop. */
	kTrackIsHidden				= (1L << 5)			/* Device tracks only. Track should not be displayed in the device window. */
};
typedef OptionBits	ITTrackAttributes;

enum {
	/*
		These mask values are specified in ITTrackInfo.validFields
		to indicate which fields contain valid data
	*/
	
	kITTIFieldInvalid				= 0,
	kITTINameFieldMask				= (1L << 0),
	kITTIFileNameFieldMask			= (1L << 1),
	kITTIArtistFieldMask			= (1L << 2),
	kITTIAlbumFieldMask				= (1L << 3),
	kITTIGenreFieldMask				= (1L << 4),
	kITTIKindFieldMask				= (1L << 5),
	kITTITrackNumberFieldsMask		= (1L << 6),
	kITTIYearFieldMask				= (1L << 7),
	kITTISoundVolumeFieldMask		= (1L << 8),
	kITTIEQPresetFieldMask			= (1L << 9),
	kITTICommentsFieldMask			= (1L << 10),
	kITTITotalTimeFieldMask			= (1L << 11),			
	kITTIStartTimeFieldMask			= (1L << 12),
	kITTIStopTimeFieldMask			= (1L << 13),
	kITTISizeFieldMask				= (1L << 14),
	kITTIBitRateFieldMask			= (1L << 15),
	kITTISampleRateFieldMask		= (1L << 16),
	kITTIAttributesFieldMask		= (1L << 17),
	kITTIFileTypeFieldMask			= (1L << 18),
	kITTIDateFieldMask				= (1L << 19),
	kITTIFileCreatorFieldMask		= (1L << 20),
	kITTIComposerFieldMask			= (1L << 21),	/* Added in iTunes 3.0 */
	kITTICompilationFieldMask		= (1L << 22),	/* Added in iTunes 3.0 */
	kITTIDiscNumberFieldsMask		= (1L << 23),	/* Added in iTunes 3.0 */
	kITTIUserRatingFieldMask		= (1L << 24),	/* Added in iTunes 3.0 */
	kITTIPlayCountFieldMask 		= (1L << 25),	/* Added in iTunes 3.0 */
	kITTILastPlayDateFieldMask		= (1L << 26),	/* Added in iTunes 3.0 */
	kITTIBeatsPerMinuteFieldMask	= (1L << 27)	/* Added in iTunes 4.0 */
};
typedef OptionBits	ITTIFieldMask;

enum
{
	kITTIUserModifiableFieldsMask = kITTINameFieldMask | kITTIArtistFieldMask | kITTIAlbumFieldMask | kITTIGenreFieldMask |
										kITTITrackNumberFieldsMask | kITTIYearFieldMask | kITTISoundVolumeFieldMask | kITTIEQPresetFieldMask |
										kITTICommentsFieldMask | kITTIStartTimeFieldMask | kITTIStopTimeFieldMask | kITTIComposerFieldMask |
										kITTICompilationFieldMask | kITTIDiscNumberFieldsMask | kITTIUserRatingFieldMask | kITTIBeatsPerMinuteFieldMask
};

typedef UInt32	ITDeviceID;
typedef UInt32	ITDeviceMediaID;
typedef UInt32	ITDeviceTrackID;
typedef UInt32	ITDevicePlaylistID;
typedef UInt32	ITDevicePlaylistFolderID;
typedef UInt32	ITDevicePlaylistItemID;

typedef UniChar 		ITUniStr255[256];				/* Similar to Str255. First element is length of string in characters. */
typedef UniChar *		ITUniStringPtr;
typedef const UniChar *	ConstITUniStringPtr;

#define	kITFileSpecMaxPathLength	(MAX_PATH - 1)

typedef struct ITFileSpec
{
	UInt16	length;		// Length in characters
	UniChar	fullPath[kITFileSpecMaxPathLength];
} ITFileSpec;
	
struct ITTrackInfo {
	ITTIFieldMask		validFields;
	UInt32				recordLength;					/* Size of this structure in bytes */
	
	ITUniStr255			name;	
	ITUniStr255			fileName;
	ITUniStr255			artist;
	ITUniStr255			album;
	ITUniStr255			genre;
	ITUniStr255			kind;
	ITUniStr255			eqPresetName;
	ITUniStr255			comments;
	
	UInt32				trackNumber;
	UInt32				numTracks;
		
	UInt16				year;
	SInt16				soundVolumeAdjustment;			/* Valid range is -255 to +255 */
		
	UInt32				totalTimeInMS;
	UInt32				startTimeInMS;
	UInt32				stopTimeInMS;

	UInt32				date;
	UInt32				sizeInBytes;

	UInt32				bitRate;
	UInt32				sampleRateFixed;

	OSType				fileType;
	OSType				fileCreator;
	
	ITTrackAttributes	attributes;
	ITTrackAttributes	validAttributes;				/* Mask indicating which attributes are applicable */

	ITUniStr255			composer;						/* Added in iTunes 3.0 */
	
	Boolean				isCompilationTrack;				/* Added in iTunes 3.0 */
	Boolean				reservedBoolean;				/* Reserved. Must be zero. */
	
	UInt16				userRating;						/* Added in iTunes 3.0. 0 = unrated, valid values are 20, 40, 60, 80 and 100. */

	UInt16				discNumber;						/* Added in iTunes 3.0 */
	UInt16				numDiscs;						/* Added in iTunes 3.0 */
	
	UInt32				playCount;						/* Added in iTunes 3.0 */
	UInt32				lastPlayDate;					/* Added in iTunes 3.0 */
	
	UInt16				beatsPerMinute;					/* Added in iTunes 4.0 */
	UInt16				reserved;						/* Reserved. Must be zero. */		
};
typedef struct ITTrackInfo ITTrackInfo;

struct ITStreamInfo {
	SInt32				version;
	ITUniStr255			streamTitle;
	ITUniStr255			streamURL;
	ITUniStr255			streamMessage;
};
typedef struct ITStreamInfo ITStreamInfo;

enum {
	/* messages sent to plugin main */
	
	kPluginInitMessage					= 'init',
	kPluginCleanupMessage				= 'clr ',
	kPluginPrepareToQuitMessage			= 'prqt',
	
	kPluginIdleMessage					= 'idle',
	kPluginVolumeMountedMessage			= 'vmnt',
	kPluginVolumeUnmountedMessage		= 'vunm',
	kPluginVolumeRenamedMessage			= 'vnam'
};


enum {
	/* PluginInitMessage.options */
	
	kPluginWantsIdleMessages	= (1L << 1),	/* Send idle messages to plugin main */
	kPluginWantsToBeLeftOpen	= (1L << 2),	/* Don't close this plugin just because it didn't register anyone */
	kPluginWantsVolumeMessages	= (1L << 3)		/* The plugin wants to be notified when volumes are mounted/unmounted/renamed */
};


enum {
	/* iTunes API messages */

	kPlayerRegisterVisualPluginMessage	= 'rvis',	/* Register a visual plugin */
	kPlayerRegisterDevicePluginMessage	= 'rdev',	/* Register a device plugin */
	
	/* Available for all plugins */

	kPlayerUnregisterPluginMessage		= 'unrg',	/* Unregister the plugin this comes from */
	kPlayerIdleMessage					= 'idle',	/* Give iTunes some time */
	
	kPlayerShowAboutMessage				= 'abou',	/* Show the about box. */
	kPlayerOpenURLMessage				= 'url ',	/* Open a URL */

	kPlayerSetPluginDataMessage			= 'sprf',	/* Set plugin preferences */
	kPlayerGetPluginDataMessage			= 'gprf',	/* Get plugin preferences */
	
	kPlayerSetPluginNamedDataMessage	= 'snpr',	/* Set plugin named preferenes */
	kPlayerGetPluginNamedDataMessage	= 'gnpr',	/* Get plugin named preferenes */
	
	kPlayerGetFileTrackInfoMessage		= 'gfti',	/* Query iTunes for information about a file */ 
	kPlayerSetFileTrackInfoMessage		= 'sfti',	/* Ask iTunes to set information about a file */ 
	
	kPlayerGetITTrackInfoSizeMessage	= 'itsz',	/* Query iTunes for the sizeof(ITTrackInfo). This allows newer plugins to correctly workd with older versions of iTunes. */

	kPlayerHandleMacOSEventMessage		= 'evnt',	/* Tell player to handle unhandled event */
	kPlayerGetPluginITFileSpecMessage	= 'itfs',	/* Get the location of the plugin executable (iTunes 4.1 or later) */
	
	/* Available for visual plugins */

	kPlayerSetFullScreenMessage			= 'sful',	/* Set full screen mode */
	kPlayerSetFullScreenOptionsMessage	= 'sfop',	/* Set full screen options */
	kPlayerGetCurrentTrackCoverArtMessage = 'covr',	/* Get current player track cover artwork */
	
	/* Available for device plugins */

	kPlayerSetDeviceSerialNumberMessage = 'dvsn',	/* Set a serial number string for a device. Needed for per-device prefs */
	kPlayerRefreshDeviceMediaMessage	= 'refm',	/* Tell iTunes to refresh (enumerate) the contents of a single media module on a device */
	kPlayerRefreshAllDeviceMediaMessage = 'refa'	/* Tell iTunes to refresh (enumerate) the contents of all media modules on a device */
};

struct PlayerMessageInfo;

typedef OSStatus (*ITAppProcPtr)(void *appCookie, OSType message, struct PlayerMessageInfo *messageInfo);


/*
	Plugin main Messages
*/

struct PluginInitMessage {
	UInt32							majorVersion;			/* Input */
	UInt32							minorVersion;			/* Input */

	void *							appCookie;				/* Input */
	ITAppProcPtr					appProc;				/* Input */
	
	OptionBits						options;				/* Output, see above for values */
	void *							refCon;					/* Output */
};
typedef struct PluginInitMessage PluginInitMessage;

struct PluginVolumeMountedMessage {
	VOLUME_REFERENCE				vRefNum;				/* Input */
	UInt32							volumeID;				/* Input */
	ConstITUniStringPtr				volumeName;				/* Input */			
};
typedef struct PluginVolumeMountedMessage PluginVolumeMountedMessage;

struct PluginVolumeUnmountedMessage {
	UInt32							volumeID;				/* Input */
	ConstITUniStringPtr				volumeName;				/* Input */			
};
typedef struct PluginVolumeUnmountedMessage PluginVolumeUnmountedMessage;

struct PluginVolumeRenamedMessage {
	VOLUME_REFERENCE				vRefNum;				/* Input */
	UInt32							volumeID;				/* Input */
	ConstITUniStringPtr				newVolumeName;			/* Input */			
};
typedef struct PluginVolumeRenamedMessage PluginVolumeRenamedMessage;

struct PluginMessageInfo {
	union {
		PluginInitMessage				initMessage;
		PluginVolumeMountedMessage		volumeMountedMessage;
		PluginVolumeUnmountedMessage	volumeUnmountedMessage;
		PluginVolumeRenamedMessage		volumeRenamedMessage;
	} u;
};
typedef struct PluginMessageInfo PluginMessageInfo;

struct VisualPluginMessageInfo;
struct DevicePluginMessageInfo;

/* Plugin main entry point message handler */
typedef OSStatus (*PluginProcPtr)(OSType message, PluginMessageInfo *messageInfo, void *refCon);

/* Visual plugin message handler */
typedef OSStatus (*VisualPluginProcPtr)(OSType message, struct VisualPluginMessageInfo *messageInfo, void *refCon);

/* Device plugin message handler */
typedef OSStatus (*DevicePluginProcPtr)(OSType message, struct DevicePluginMessageInfo *messageInfo, void *refCon);



/*
	Callbacks to iTunes
*/

enum {
	/* PlayerRegisterVisualPluginMessage.options */
	
	kVisualWantsIdleMessages			= (1L << 3),
	kVisualWantsConfigure				= (1L << 5)
};

struct PlayerRegisterVisualPluginMessage {
	/* Input from plugin */
	
	Str63							name;						/* Displayed in the Visual menu */
	OptionBits						options;					/* See above */
	
	OSType							creator;					/* Identifies the plugin */
	
	NumVersion						pluginVersion;				/* Version number of the plugin */

	VisualPluginProcPtr				handler;					/* Handler for the plugin's messages */
	void *							registerRefCon;				/* RefCon for the plugin's handler */

	UInt32							timeBetweenDataInMS;		/* How often to call the plugin (0xFFFFFFFF = as often as possible) */
	UInt32							numWaveformChannels;		/* 0-2 waveforms requested */
	UInt32							numSpectrumChannels;		/* 0-2 spectrums requested */
	
	SInt16							minWidth;					/* Minimum resizeable width */
	SInt16							minHeight;					/* Minimum resizeable height */
	
	SInt16							maxWidth;					/* Maximum resizeable width */
	SInt16							maxHeight;					/* Maximum resizeable height */
		
	UInt16							minFullScreenBitDepth;		/* 0 = Any */
	UInt16							maxFullScreenBitDepth;		/* 0 = Any */
	
	UInt16							windowAlignmentInBytes;		/* Reserved (should be zero) */
};
typedef struct PlayerRegisterVisualPluginMessage PlayerRegisterVisualPluginMessage;

enum {
	/* PlayerRegisterDevicePluginMessage.options */
	
	kDeviceWantsIdleMessages = (1L << 0)
};

struct PlayerRegisterDevicePluginMessage {
	/* Input from plugin */
	
	Str63							name;						/* Used internally. Not currently displayed */
	OptionBits						options;					/* See above */
	
	NumVersion						pluginVersion;				/* Version number of the plugin */

	DevicePluginProcPtr				handler;					/* Handler for the plugin's messages */
	void *							registerRefCon;				/* RefCon for the plugin's handler */
	
	UInt32							reserved[2];				/* Must be zero */
};
typedef struct PlayerRegisterDevicePluginMessage PlayerRegisterDevicePluginMessage;

struct PlayerSetFullScreenMessage {
	Boolean							fullScreen;
};
typedef struct PlayerSetFullScreenMessage PlayerSetFullScreenMessage;

struct PlayerSetFullScreenOptionsMessage {
	SInt16							minBitDepth;				/* 0 = Any */
	SInt16							maxBitDepth;				/* 0 = Any */
	SInt16							preferredBitDepth;			/* 0 = Current */
	
	SInt16							desiredWidth;				/* Must be within minWidth & maxWidth */
	SInt16							desiredHeight;				/* Must be within minHeight & maxHeight */
};
typedef struct PlayerSetFullScreenOptionsMessage PlayerSetFullScreenOptionsMessage;

struct PlayerGetCurrentTrackCoverArtMessage {
	Handle						coverArt;		/* output - client must dispose */
	OSType						coverArtFormat;	/* output - format of cover art */
};
typedef struct PlayerGetCurrentTrackCoverArtMessage PlayerGetCurrentTrackCoverArtMessage;

struct PlayerOpenURLMessage {
	SInt8 *							url;
	UInt32							length;
};
typedef struct PlayerOpenURLMessage PlayerOpenURLMessage;

struct PlayerSetPluginDataMessage {
	void *							dataPtr;		/* Input */
	UInt32							dataSize;		/* Input */
};
typedef struct PlayerSetPluginDataMessage PlayerSetPluginDataMessage;

struct PlayerGetPluginDataMessage {
	void *							dataPtr;		/* Input */
	UInt32							dataBufferSize;	/* Input */
	
	UInt32							dataSize;		/* Output */
};
typedef struct PlayerGetPluginDataMessage PlayerGetPluginDataMessage;

struct PlayerSetPluginNamedDataMessage {
	ConstStringPtr					dataName;		/* Input */

	void *							dataPtr;		/* Input */
	UInt32							dataSize;		/* Input */
};
typedef struct PlayerSetPluginNamedDataMessage PlayerSetPluginNamedDataMessage;

struct PlayerGetPluginNamedDataMessage {
	ConstStringPtr					dataName;		/* Input */

	void *							dataPtr;		/* Input */
	UInt32							dataBufferSize;	/* Input */
	
	UInt32							dataSize;		/* Output */
};
typedef struct PlayerGetPluginNamedDataMessage PlayerGetPluginNamedDataMessage;

struct PlayerHandleMacOSEventMessage {
	const EventRecord *				theEvent;		/* Input */
	
	Boolean							handled;		/* Output */
};
typedef struct PlayerHandleMacOSEventMessage PlayerHandleMacOSEventMessage;

struct PlayerGetPluginITFileSpecMessage {
	ITFileSpec *					fileSpec;		/* Output */
};
typedef struct PlayerGetPluginITFileSpecMessage PlayerGetPluginITFileSpecMessage;

struct PlayerSetDeviceSerialNumberMessage {
	ConstStringPtr					serialNumber;	/* Input */
};
typedef struct PlayerSetDeviceSerialNumberMessage PlayerSetDeviceSerialNumberMessage;

struct PlayerRefreshDeviceMediaMessage {
	ITDeviceMediaID					mediaID;		/* Input */
};
typedef struct PlayerRefreshDeviceMediaMessage PlayerRefreshDeviceMediaMessage;

struct PlayerGetFileTrackInfoMessage {
	const ITFileSpec *				fileSpec;		/* Input */
	ITTrackInfo *					trackInfo;		/* Output */
};
typedef struct PlayerGetFileTrackInfoMessage PlayerGetFileTrackInfoMessage;

struct PlayerSetFileTrackInfoMessage {
	const ITFileSpec *				fileSpec;		/* Input */
	const ITTrackInfo *				trackInfo;		/* Input */
};
typedef struct PlayerSetFileTrackInfoMessage PlayerSetFileTrackInfoMessage;

struct PlayerGetITTrackInfoSizeMessage {
	UInt32							itTrackInfoSize;	/* Output */
};
typedef struct PlayerGetITTrackInfoSizeMessage PlayerGetITTrackInfoSizeMessage;

struct PlayerMessageInfo {
	UInt32									messageMajorVersion;		/* Should be kITCurrentPluginMajorMessageVersion */
	UInt32									messageMinorVersion;		/* Should be kITCurrentPluginMinorMessageVersion */
	UInt32									messageInfoSize;			/* Should be sizeof(PlayerMessageInfo) */
	
	union {
		PlayerRegisterVisualPluginMessage	registerVisualPluginMessage;
		PlayerRegisterDevicePluginMessage	registerDevicePluginMessage;

		PlayerOpenURLMessage				openURLMessage;
		
		PlayerSetPluginDataMessage			setPluginDataMessage;
		PlayerGetPluginDataMessage			getPluginDataMessage;
		
		PlayerSetPluginNamedDataMessage		setPluginNamedDataMessage;
		PlayerGetPluginNamedDataMessage		getPluginNamedDataMessage;
		
		PlayerSetFullScreenMessage				setFullScreenMessage;
		PlayerSetFullScreenOptionsMessage		setFullScreenOptionsMessage;
		PlayerGetCurrentTrackCoverArtMessage	getCurrentTrackCoverArtMessage;
		
		PlayerSetDeviceSerialNumberMessage	setDeviceSerialNumberMessage;
		PlayerRefreshDeviceMediaMessage		refreshDeviceMediaMessage;
		
		PlayerGetFileTrackInfoMessage		getFileTrackInfoMessage;
		PlayerSetFileTrackInfoMessage		setFileTrackInfoMessage;
		PlayerGetITTrackInfoSizeMessage		getITTrackInfoSizeMessage;

		PlayerHandleMacOSEventMessage		handleMacOSEventMessage;
		PlayerGetPluginITFileSpecMessage	getPluginITFileSpecMessage;
	} u;
};
typedef struct PlayerMessageInfo PlayerMessageInfo;

extern OSStatus ITCallApplication (void *appCookie, ITAppProcPtr appProc, OSType message, PlayerMessageInfo *messageInfo);
extern void		SetNumVersion (NumVersion *numVersion, UInt8 majorRev, UInt8 minorAndBugRev, UInt8 stage, UInt8 nonRelRev);


/* for use from plugin main */

extern OSStatus PlayerRegisterVisualPlugin (void *appCookie, ITAppProcPtr appProc, PlayerMessageInfo *messageInfo);

/* For all plugins */

extern OSStatus	PlayerUnregisterPlugin (void *appCookie, ITAppProcPtr appProc, PlayerMessageInfo *messageInfo);
extern OSStatus PlayerIdle (void *appCookie, ITAppProcPtr appProc);

extern void		PlayerShowAbout (void *appCookie, ITAppProcPtr appProc);
extern void		PlayerOpenURL (void *appCookie, ITAppProcPtr appProc, SInt8 *string, UInt32 length);

extern OSStatus	PlayerGetPluginData (void *appCookie, ITAppProcPtr appProc, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize);
extern OSStatus	PlayerSetPluginData (void *appCookie, ITAppProcPtr appProc, void *dataPtr, UInt32 dataSize);

extern OSStatus	PlayerGetPluginNamedData (void *appCookie, ITAppProcPtr appProc, ConstStringPtr dataName, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize);
extern OSStatus	PlayerSetPluginNamedData (void *appCookie, ITAppProcPtr appProc, ConstStringPtr dataName, void *dataPtr, UInt32 dataSize);

extern OSStatus	PlayerGetFileTrackInfo (void *appCookie, ITAppProcPtr appProc, const ITFileSpec *fileSpec, ITTrackInfo *trackInfo);
extern OSStatus	PlayerSetFileTrackInfo (void *appCookie, ITAppProcPtr appProc, const ITFileSpec *fileSpec, const ITTrackInfo *trackInfo);

extern OSStatus PlayerGetITTrackInfoSize (void *appCookie, ITAppProcPtr appProc, UInt32 appPluginMajorVersion, UInt32 appPluginMinorVersion, UInt32 *itTrackInfoSize);

extern OSStatus PlayerHandleMacOSEvent (void *appCookie, ITAppProcPtr appProc, const EventRecord *theEvent, Boolean *eventHandled);

extern OSStatus	PlayerGetPluginITFileSpec (void *appCookie, ITAppProcPtr appProc, ITFileSpec *pluginFileSpec);

/* For visual plugins */

extern OSStatus PlayerSetFullScreen (void *appCookie, ITAppProcPtr appProc, Boolean fullScreen);
extern OSStatus PlayerSetFullScreenOptions (void *appCookie, ITAppProcPtr appProc, SInt16 minBitDepth, SInt16 maxBitDepth, SInt16 preferredBitDepth, SInt16 desiredWidth, SInt16 desiredHeight);
extern OSStatus PlayerGetCurrentTrackCoverArt (void *appCookie, ITAppProcPtr appProc, Handle *coverArt, OSType *coverArtFormat);

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

#endif /* ITUNESAPI_H_ */
