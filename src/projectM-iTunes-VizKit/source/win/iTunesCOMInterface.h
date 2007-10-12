/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Oct 20 13:02:07 2005
 */
/* Compiler settings for iTunesCOMInterface.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __iTunesCOMInterface_h__
#define __iTunesCOMInterface_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IITObject_FWD_DEFINED__
#define __IITObject_FWD_DEFINED__
typedef interface IITObject IITObject;
#endif 	/* __IITObject_FWD_DEFINED__ */


#ifndef __IITSource_FWD_DEFINED__
#define __IITSource_FWD_DEFINED__
typedef interface IITSource IITSource;
#endif 	/* __IITSource_FWD_DEFINED__ */


#ifndef __IITSourceCollection_FWD_DEFINED__
#define __IITSourceCollection_FWD_DEFINED__
typedef interface IITSourceCollection IITSourceCollection;
#endif 	/* __IITSourceCollection_FWD_DEFINED__ */


#ifndef __IITEncoder_FWD_DEFINED__
#define __IITEncoder_FWD_DEFINED__
typedef interface IITEncoder IITEncoder;
#endif 	/* __IITEncoder_FWD_DEFINED__ */


#ifndef __IITEncoderCollection_FWD_DEFINED__
#define __IITEncoderCollection_FWD_DEFINED__
typedef interface IITEncoderCollection IITEncoderCollection;
#endif 	/* __IITEncoderCollection_FWD_DEFINED__ */


#ifndef __IITEQPreset_FWD_DEFINED__
#define __IITEQPreset_FWD_DEFINED__
typedef interface IITEQPreset IITEQPreset;
#endif 	/* __IITEQPreset_FWD_DEFINED__ */


#ifndef __IITEQPresetCollection_FWD_DEFINED__
#define __IITEQPresetCollection_FWD_DEFINED__
typedef interface IITEQPresetCollection IITEQPresetCollection;
#endif 	/* __IITEQPresetCollection_FWD_DEFINED__ */


#ifndef __IITPlaylist_FWD_DEFINED__
#define __IITPlaylist_FWD_DEFINED__
typedef interface IITPlaylist IITPlaylist;
#endif 	/* __IITPlaylist_FWD_DEFINED__ */


#ifndef __IITOperationStatus_FWD_DEFINED__
#define __IITOperationStatus_FWD_DEFINED__
typedef interface IITOperationStatus IITOperationStatus;
#endif 	/* __IITOperationStatus_FWD_DEFINED__ */


#ifndef __IITConvertOperationStatus_FWD_DEFINED__
#define __IITConvertOperationStatus_FWD_DEFINED__
typedef interface IITConvertOperationStatus IITConvertOperationStatus;
#endif 	/* __IITConvertOperationStatus_FWD_DEFINED__ */


#ifndef __IITLibraryPlaylist_FWD_DEFINED__
#define __IITLibraryPlaylist_FWD_DEFINED__
typedef interface IITLibraryPlaylist IITLibraryPlaylist;
#endif 	/* __IITLibraryPlaylist_FWD_DEFINED__ */


#ifndef __IITUserPlaylist_FWD_DEFINED__
#define __IITUserPlaylist_FWD_DEFINED__
typedef interface IITUserPlaylist IITUserPlaylist;
#endif 	/* __IITUserPlaylist_FWD_DEFINED__ */


#ifndef __IITTrack_FWD_DEFINED__
#define __IITTrack_FWD_DEFINED__
typedef interface IITTrack IITTrack;
#endif 	/* __IITTrack_FWD_DEFINED__ */


#ifndef __IITTrackCollection_FWD_DEFINED__
#define __IITTrackCollection_FWD_DEFINED__
typedef interface IITTrackCollection IITTrackCollection;
#endif 	/* __IITTrackCollection_FWD_DEFINED__ */


#ifndef __IITVisual_FWD_DEFINED__
#define __IITVisual_FWD_DEFINED__
typedef interface IITVisual IITVisual;
#endif 	/* __IITVisual_FWD_DEFINED__ */


#ifndef __IITVisualCollection_FWD_DEFINED__
#define __IITVisualCollection_FWD_DEFINED__
typedef interface IITVisualCollection IITVisualCollection;
#endif 	/* __IITVisualCollection_FWD_DEFINED__ */


#ifndef __IITWindow_FWD_DEFINED__
#define __IITWindow_FWD_DEFINED__
typedef interface IITWindow IITWindow;
#endif 	/* __IITWindow_FWD_DEFINED__ */


#ifndef __IITBrowserWindow_FWD_DEFINED__
#define __IITBrowserWindow_FWD_DEFINED__
typedef interface IITBrowserWindow IITBrowserWindow;
#endif 	/* __IITBrowserWindow_FWD_DEFINED__ */


#ifndef __IITWindowCollection_FWD_DEFINED__
#define __IITWindowCollection_FWD_DEFINED__
typedef interface IITWindowCollection IITWindowCollection;
#endif 	/* __IITWindowCollection_FWD_DEFINED__ */


#ifndef __IiTunes_FWD_DEFINED__
#define __IiTunes_FWD_DEFINED__
typedef interface IiTunes IiTunes;
#endif 	/* __IiTunes_FWD_DEFINED__ */


#ifndef ___IiTunesEvents_FWD_DEFINED__
#define ___IiTunesEvents_FWD_DEFINED__
typedef interface _IiTunesEvents _IiTunesEvents;
#endif 	/* ___IiTunesEvents_FWD_DEFINED__ */


#ifndef ___IITConvertOperationStatusEvents_FWD_DEFINED__
#define ___IITConvertOperationStatusEvents_FWD_DEFINED__
typedef interface _IITConvertOperationStatusEvents _IITConvertOperationStatusEvents;
#endif 	/* ___IITConvertOperationStatusEvents_FWD_DEFINED__ */


#ifndef __iTunesApp_FWD_DEFINED__
#define __iTunesApp_FWD_DEFINED__

#ifdef __cplusplus
typedef class iTunesApp iTunesApp;
#else
typedef struct iTunesApp iTunesApp;
#endif /* __cplusplus */

#endif 	/* __iTunesApp_FWD_DEFINED__ */


#ifndef __iTunesConvertOperationStatus_FWD_DEFINED__
#define __iTunesConvertOperationStatus_FWD_DEFINED__

#ifdef __cplusplus
typedef class iTunesConvertOperationStatus iTunesConvertOperationStatus;
#else
typedef struct iTunesConvertOperationStatus iTunesConvertOperationStatus;
#endif /* __cplusplus */

#endif 	/* __iTunesConvertOperationStatus_FWD_DEFINED__ */


#ifndef __IITArtwork_FWD_DEFINED__
#define __IITArtwork_FWD_DEFINED__
typedef interface IITArtwork IITArtwork;
#endif 	/* __IITArtwork_FWD_DEFINED__ */


#ifndef __IITArtworkCollection_FWD_DEFINED__
#define __IITArtworkCollection_FWD_DEFINED__
typedef interface IITArtworkCollection IITArtworkCollection;
#endif 	/* __IITArtworkCollection_FWD_DEFINED__ */


#ifndef __IITURLTrack_FWD_DEFINED__
#define __IITURLTrack_FWD_DEFINED__
typedef interface IITURLTrack IITURLTrack;
#endif 	/* __IITURLTrack_FWD_DEFINED__ */


#ifndef __IITAudioCDPlaylist_FWD_DEFINED__
#define __IITAudioCDPlaylist_FWD_DEFINED__
typedef interface IITAudioCDPlaylist IITAudioCDPlaylist;
#endif 	/* __IITAudioCDPlaylist_FWD_DEFINED__ */


#ifndef __IITPlaylistCollection_FWD_DEFINED__
#define __IITPlaylistCollection_FWD_DEFINED__
typedef interface IITPlaylistCollection IITPlaylistCollection;
#endif 	/* __IITPlaylistCollection_FWD_DEFINED__ */


#ifndef __IITIPodSource_FWD_DEFINED__
#define __IITIPodSource_FWD_DEFINED__
typedef interface IITIPodSource IITIPodSource;
#endif 	/* __IITIPodSource_FWD_DEFINED__ */


#ifndef __IITFileOrCDTrack_FWD_DEFINED__
#define __IITFileOrCDTrack_FWD_DEFINED__
typedef interface IITFileOrCDTrack IITFileOrCDTrack;
#endif 	/* __IITFileOrCDTrack_FWD_DEFINED__ */


#ifndef __IITPlaylistWindow_FWD_DEFINED__
#define __IITPlaylistWindow_FWD_DEFINED__
typedef interface IITPlaylistWindow IITPlaylistWindow;
#endif 	/* __IITPlaylistWindow_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "DispEx.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_iTunesCOMInterface_0000 */
/* [local] */ 

typedef /* [public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0000_0001
    {	kITTypeLibrary_MajorVersion	= 1,
	kITTypeLibrary_MinorVersion	= 7
    }	ITVersion;

typedef /* [public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0000_0002
    {	ITUNES_E_USERCANCEL	= 0xa0040201,
	ITUNES_E_OBJECTDELETED	= 0xa0040202,
	ITUNES_E_OBJECTLOCKED	= 0xa0040203,
	ITUNES_E_CONVERSIONINPROGRESS	= 0xa0040204,
	ITUNES_E_MUSICSTOREDISABLED	= 0xa0040205,
	ITUNES_E_OBJECTEXISTS	= 0xa0040206,
	ITUNES_E_PODCASTSDISABLED	= 0xa0040207
    }	ITErrors;



extern RPC_IF_HANDLE __MIDL_itf_iTunesCOMInterface_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_iTunesCOMInterface_0000_v0_0_s_ifspec;


#ifndef __iTunesLib_LIBRARY_DEFINED__
#define __iTunesLib_LIBRARY_DEFINED__

/* library iTunesLib */
/* [helpstring][uuid][version] */ 



















typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0267_0001
    {	ITPlayerStateStopped	= 0,
	ITPlayerStatePlaying	= ITPlayerStateStopped + 1,
	ITPlayerStateFastForward	= ITPlayerStatePlaying + 1,
	ITPlayerStateRewind	= ITPlayerStateFastForward + 1
    }	ITPlayerState;

typedef /* [public][public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0267_0002
    {	ITVisualSizeSmall	= 0,
	ITVisualSizeMedium	= ITVisualSizeSmall + 1,
	ITVisualSizeLarge	= ITVisualSizeMedium + 1
    }	ITVisualSize;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0267_0003
    {	ITCOMDisabledReasonOther	= 0,
	ITCOMDisabledReasonDialog	= ITCOMDisabledReasonOther + 1,
	ITCOMDisabledReasonQuitting	= ITCOMDisabledReasonDialog + 1
    }	ITCOMDisabledReason;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0267_0004
    {	ITPlayButtonStatePlayDisabled	= 0,
	ITPlayButtonStatePlayEnabled	= ITPlayButtonStatePlayDisabled + 1,
	ITPlayButtonStatePauseEnabled	= ITPlayButtonStatePlayEnabled + 1,
	ITPlayButtonStatePauseDisabled	= ITPlayButtonStatePauseEnabled + 1,
	ITPlayButtonStateStopEnabled	= ITPlayButtonStatePauseDisabled + 1,
	ITPlayButtonStateStopDisabled	= ITPlayButtonStateStopEnabled + 1
    }	ITPlayButtonState;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0267_0005
    {	ITPlayerButtonPrevious	= 0,
	ITPlayerButtonPlay	= ITPlayerButtonPrevious + 1,
	ITPlayerButtonNext	= ITPlayerButtonPlay + 1
    }	ITPlayerButton;

typedef /* [public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0267_0006
    {	ITPlayerButtonModifierKeyNone	= 0,
	ITPlayerButtonModifierKeyShift	= 1,
	ITPlayerButtonModifierKeyControl	= 2,
	ITPlayerButtonModifierKeyAlt	= 4,
	ITPlayerButtonModifierKeyCapsLock	= 8
    }	ITPlayerButtonModifierKey;

typedef /* [public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0270_0001
    {	ITEventDatabaseChanged	= 1,
	ITEventPlayerPlay	= 2,
	ITEventPlayerStop	= 3,
	ITEventPlayerPlayingTrackChanged	= 4,
	ITEventUserInterfaceEnabled	= 5,
	ITEventCOMCallsDisabled	= 6,
	ITEventCOMCallsEnabled	= 7,
	ITEventQuitting	= 8,
	ITEventAboutToPromptUserToQuit	= 9,
	ITEventSoundVolumeChanged	= 10
    }	ITEvent;

typedef /* [public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0271_0001
    {	ITConvertOperationStatusChanged	= 1,
	ITConvertOperationComplete	= 2
    }	ITConvertOperationStatusEvent;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0272_0001
    {	ITArtworkFormatUnknown	= 0,
	ITArtworkFormatJPEG	= ITArtworkFormatUnknown + 1,
	ITArtworkFormatPNG	= ITArtworkFormatJPEG + 1,
	ITArtworkFormatBMP	= ITArtworkFormatPNG + 1
    }	ITArtworkFormat;




typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0278_0001
    {	ITPlaylistKindUnknown	= 0,
	ITPlaylistKindLibrary	= ITPlaylistKindUnknown + 1,
	ITPlaylistKindUser	= ITPlaylistKindLibrary + 1,
	ITPlaylistKindCD	= ITPlaylistKindUser + 1,
	ITPlaylistKindDevice	= ITPlaylistKindCD + 1,
	ITPlaylistKindRadioTuner	= ITPlaylistKindDevice + 1
    }	ITPlaylistKind;

typedef /* [public][public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0278_0002
    {	ITPlaylistRepeatModeOff	= 0,
	ITPlaylistRepeatModeOne	= ITPlaylistRepeatModeOff + 1,
	ITPlaylistRepeatModeAll	= ITPlaylistRepeatModeOne + 1
    }	ITPlaylistRepeatMode;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0278_0003
    {	ITPlaylistPrintKindPlaylist	= 0,
	ITPlaylistPrintKindAlbumlist	= ITPlaylistPrintKindPlaylist + 1,
	ITPlaylistPrintKindInsert	= ITPlaylistPrintKindAlbumlist + 1
    }	ITPlaylistPrintKind;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0278_0004
    {	ITPlaylistSearchFieldAll	= 0,
	ITPlaylistSearchFieldVisible	= ITPlaylistSearchFieldAll + 1,
	ITPlaylistSearchFieldArtists	= ITPlaylistSearchFieldVisible + 1,
	ITPlaylistSearchFieldAlbums	= ITPlaylistSearchFieldArtists + 1,
	ITPlaylistSearchFieldComposers	= ITPlaylistSearchFieldAlbums + 1,
	ITPlaylistSearchFieldSongNames	= ITPlaylistSearchFieldComposers + 1
    }	ITPlaylistSearchField;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0280_0001
    {	ITUserPlaylistSpecialKindNone	= 0,
	ITUserPlaylistSpecialKindPurchasedMusic	= ITUserPlaylistSpecialKindNone + 1,
	ITUserPlaylistSpecialKindPartyShuffle	= ITUserPlaylistSpecialKindPurchasedMusic + 1,
	ITUserPlaylistSpecialKindPodcasts	= ITUserPlaylistSpecialKindPartyShuffle + 1,
	ITUserPlaylistSpecialKindFolder	= ITUserPlaylistSpecialKindPodcasts + 1,
	ITUserPlaylistSpecialKindVideos	= ITUserPlaylistSpecialKindFolder + 1
    }	ITUserPlaylistSpecialKind;


typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0283_0001
    {	ITSourceKindUnknown	= 0,
	ITSourceKindLibrary	= ITSourceKindUnknown + 1,
	ITSourceKindIPod	= ITSourceKindLibrary + 1,
	ITSourceKindAudioCD	= ITSourceKindIPod + 1,
	ITSourceKindMP3CD	= ITSourceKindAudioCD + 1,
	ITSourceKindDevice	= ITSourceKindMP3CD + 1,
	ITSourceKindRadioTuner	= ITSourceKindDevice + 1,
	ITSourceKindSharedLibrary	= ITSourceKindRadioTuner + 1
    }	ITSourceKind;


typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0286_0001
    {	ITTrackKindUnknown	= 0,
	ITTrackKindFile	= ITTrackKindUnknown + 1,
	ITTrackKindCD	= ITTrackKindFile + 1,
	ITTrackKindURL	= ITTrackKindCD + 1,
	ITTrackKindDevice	= ITTrackKindURL + 1,
	ITTrackKindSharedLibrary	= ITTrackKindDevice + 1
    }	ITTrackKind;

typedef /* [public][public][v1_enum][uuid] */ 
enum __MIDL___MIDL_itf_iTunesCOMInterface_0292_0001
    {	ITWindowKindUnknown	= 0,
	ITWindowKindBrowser	= ITWindowKindUnknown + 1,
	ITWindowKindPlaylist	= ITWindowKindBrowser + 1,
	ITWindowKindEQ	= ITWindowKindPlaylist + 1,
	ITWindowKindArtwork	= ITWindowKindEQ + 1,
	ITWindowKindNowPlaying	= ITWindowKindArtwork + 1
    }	ITWindowKind;


EXTERN_C const IID LIBID_iTunesLib;

#ifndef __IITObject_INTERFACE_DEFINED__
#define __IITObject_INTERFACE_DEFINED__

/* interface IITObject */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9FAB0E27-70D7-4e3a-9965-B0C8B8869BB6")
    IITObject : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetITObjectIDs( 
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *name) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR name) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ long __RPC_FAR *index) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SourceID( 
            /* [retval][out] */ long __RPC_FAR *sourceID) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlaylistID( 
            /* [retval][out] */ long __RPC_FAR *playlistID) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TrackID( 
            /* [retval][out] */ long __RPC_FAR *trackID) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TrackDatabaseID( 
            /* [retval][out] */ long __RPC_FAR *databaseID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITObject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITObject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITObject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITObject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITObject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITObject __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITObject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITObject __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITObject __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITObject __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITObject __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITObject __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITObject __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        END_INTERFACE
    } IITObjectVtbl;

    interface IITObject
    {
        CONST_VTBL struct IITObjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITObject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITObject_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITObject_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITObject_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITObject_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITObject_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITObject_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITObject_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITObject_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITObject_GetITObjectIDs_Proxy( 
    IITObject __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *sourceID,
    /* [out] */ long __RPC_FAR *playlistID,
    /* [out] */ long __RPC_FAR *trackID,
    /* [out] */ long __RPC_FAR *databaseID);


void __RPC_STUB IITObject_GetITObjectIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITObject_get_Name_Proxy( 
    IITObject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *name);


void __RPC_STUB IITObject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITObject_put_Name_Proxy( 
    IITObject __RPC_FAR * This,
    /* [in] */ BSTR name);


void __RPC_STUB IITObject_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITObject_get_Index_Proxy( 
    IITObject __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *index);


void __RPC_STUB IITObject_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITObject_get_SourceID_Proxy( 
    IITObject __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *sourceID);


void __RPC_STUB IITObject_get_SourceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITObject_get_PlaylistID_Proxy( 
    IITObject __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *playlistID);


void __RPC_STUB IITObject_get_PlaylistID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITObject_get_TrackID_Proxy( 
    IITObject __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *trackID);


void __RPC_STUB IITObject_get_TrackID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITObject_get_TrackDatabaseID_Proxy( 
    IITObject __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *databaseID);


void __RPC_STUB IITObject_get_TrackDatabaseID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITObject_INTERFACE_DEFINED__ */


#ifndef __IITSource_INTERFACE_DEFINED__
#define __IITSource_INTERFACE_DEFINED__

/* interface IITSource */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AEC1C4D3-AEF1-4255-B892-3E3D13ADFDF9")
    IITSource : public IITObject
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Kind( 
            /* [retval][out] */ ITSourceKind __RPC_FAR *kind) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Capacity( 
            /* [retval][out] */ double __RPC_FAR *capacity) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FreeSpace( 
            /* [retval][out] */ double __RPC_FAR *freespace) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Playlists( 
            /* [retval][out] */ IITPlaylistCollection __RPC_FAR *__RPC_FAR *iPlaylistCollection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITSource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITSource __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITSource __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITSource __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITSource __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITSource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITSource __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITSource __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITSource __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ ITSourceKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Capacity )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *capacity);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FreeSpace )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *freespace);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Playlists )( 
            IITSource __RPC_FAR * This,
            /* [retval][out] */ IITPlaylistCollection __RPC_FAR *__RPC_FAR *iPlaylistCollection);
        
        END_INTERFACE
    } IITSourceVtbl;

    interface IITSource
    {
        CONST_VTBL struct IITSourceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITSource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITSource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITSource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITSource_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITSource_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITSource_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITSource_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITSource_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITSource_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITSource_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITSource_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITSource_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITSource_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITSource_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITSource_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITSource_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITSource_get_Capacity(This,capacity)	\
    (This)->lpVtbl -> get_Capacity(This,capacity)

#define IITSource_get_FreeSpace(This,freespace)	\
    (This)->lpVtbl -> get_FreeSpace(This,freespace)

#define IITSource_get_Playlists(This,iPlaylistCollection)	\
    (This)->lpVtbl -> get_Playlists(This,iPlaylistCollection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITSource_get_Kind_Proxy( 
    IITSource __RPC_FAR * This,
    /* [retval][out] */ ITSourceKind __RPC_FAR *kind);


void __RPC_STUB IITSource_get_Kind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITSource_get_Capacity_Proxy( 
    IITSource __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *capacity);


void __RPC_STUB IITSource_get_Capacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITSource_get_FreeSpace_Proxy( 
    IITSource __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *freespace);


void __RPC_STUB IITSource_get_FreeSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITSource_get_Playlists_Proxy( 
    IITSource __RPC_FAR * This,
    /* [retval][out] */ IITPlaylistCollection __RPC_FAR *__RPC_FAR *iPlaylistCollection);


void __RPC_STUB IITSource_get_Playlists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITSource_INTERFACE_DEFINED__ */


#ifndef __IITSourceCollection_INTERFACE_DEFINED__
#define __IITSourceCollection_INTERFACE_DEFINED__

/* interface IITSourceCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITSourceCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2FF6CE20-FF87-4183-B0B3-F323D047AF41")
    IITSourceCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITSourceCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITSourceCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITSourceCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITSourceCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITSourceCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITSourceCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITSourceCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITSourceCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITSourceCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITSourceCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITSourceCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITSourceCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITSourceCollectionVtbl;

    interface IITSourceCollection
    {
        CONST_VTBL struct IITSourceCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITSourceCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITSourceCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITSourceCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITSourceCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITSourceCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITSourceCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITSourceCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITSourceCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITSourceCollection_get_Item(This,index,iSource)	\
    (This)->lpVtbl -> get_Item(This,index,iSource)

#define IITSourceCollection_get_ItemByName(This,name,iSource)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iSource)

#define IITSourceCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITSourceCollection_get_Count_Proxy( 
    IITSourceCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITSourceCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITSourceCollection_get_Item_Proxy( 
    IITSourceCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);


void __RPC_STUB IITSourceCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITSourceCollection_get_ItemByName_Proxy( 
    IITSourceCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);


void __RPC_STUB IITSourceCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITSourceCollection_get__NewEnum_Proxy( 
    IITSourceCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITSourceCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITSourceCollection_INTERFACE_DEFINED__ */


#ifndef __IITEncoder_INTERFACE_DEFINED__
#define __IITEncoder_INTERFACE_DEFINED__

/* interface IITEncoder */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1CF95A1C-55FE-4f45-A2D3-85AC6C504A73")
    IITEncoder : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *name) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Format( 
            /* [retval][out] */ BSTR __RPC_FAR *format) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITEncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITEncoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITEncoder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITEncoder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITEncoder __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITEncoder __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITEncoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITEncoder __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITEncoder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Format )( 
            IITEncoder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *format);
        
        END_INTERFACE
    } IITEncoderVtbl;

    interface IITEncoder
    {
        CONST_VTBL struct IITEncoderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITEncoder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITEncoder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITEncoder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITEncoder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITEncoder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITEncoder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITEncoder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITEncoder_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITEncoder_get_Format(This,format)	\
    (This)->lpVtbl -> get_Format(This,format)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEncoder_get_Name_Proxy( 
    IITEncoder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *name);


void __RPC_STUB IITEncoder_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEncoder_get_Format_Proxy( 
    IITEncoder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *format);


void __RPC_STUB IITEncoder_get_Format_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITEncoder_INTERFACE_DEFINED__ */


#ifndef __IITEncoderCollection_INTERFACE_DEFINED__
#define __IITEncoderCollection_INTERFACE_DEFINED__

/* interface IITEncoderCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITEncoderCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8862BCA9-168D-4549-A9D5-ADB35E553BA6")
    IITEncoderCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITEncoderCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITEncoderCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITEncoderCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITEncoderCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITEncoderCollectionVtbl;

    interface IITEncoderCollection
    {
        CONST_VTBL struct IITEncoderCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITEncoderCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITEncoderCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITEncoderCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITEncoderCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITEncoderCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITEncoderCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITEncoderCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITEncoderCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITEncoderCollection_get_Item(This,index,iEncoder)	\
    (This)->lpVtbl -> get_Item(This,index,iEncoder)

#define IITEncoderCollection_get_ItemByName(This,name,iEncoder)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iEncoder)

#define IITEncoderCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEncoderCollection_get_Count_Proxy( 
    IITEncoderCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITEncoderCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITEncoderCollection_get_Item_Proxy( 
    IITEncoderCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder);


void __RPC_STUB IITEncoderCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEncoderCollection_get_ItemByName_Proxy( 
    IITEncoderCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder);


void __RPC_STUB IITEncoderCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITEncoderCollection_get__NewEnum_Proxy( 
    IITEncoderCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITEncoderCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITEncoderCollection_INTERFACE_DEFINED__ */


#ifndef __IITEQPreset_INTERFACE_DEFINED__
#define __IITEQPreset_INTERFACE_DEFINED__

/* interface IITEQPreset */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITEQPreset;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5BE75F4F-68FA-4212-ACB7-BE44EA569759")
    IITEQPreset : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *name) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Modifiable( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isModifiable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Preamp( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Preamp( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band1( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band1( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band2( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band2( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band3( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band3( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band4( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band4( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band5( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band5( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band6( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band6( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band7( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band7( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band8( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band8( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band9( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band9( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Band10( 
            /* [retval][out] */ double __RPC_FAR *level) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Band10( 
            /* [in] */ double level) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ VARIANT_BOOL updateAllTracks) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Rename( 
            /* [in] */ BSTR newName,
            /* [in] */ VARIANT_BOOL updateAllTracks) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITEQPresetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITEQPreset __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITEQPreset __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITEQPreset __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Modifiable )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isModifiable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Preamp )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Preamp )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band1 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band1 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band2 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band2 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band3 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band3 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band4 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band4 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band5 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band5 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band6 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band6 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band7 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band7 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band8 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band8 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band9 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band9 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Band10 )( 
            IITEQPreset __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *level);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Band10 )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ double level);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL updateAllTracks);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Rename )( 
            IITEQPreset __RPC_FAR * This,
            /* [in] */ BSTR newName,
            /* [in] */ VARIANT_BOOL updateAllTracks);
        
        END_INTERFACE
    } IITEQPresetVtbl;

    interface IITEQPreset
    {
        CONST_VTBL struct IITEQPresetVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITEQPreset_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITEQPreset_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITEQPreset_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITEQPreset_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITEQPreset_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITEQPreset_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITEQPreset_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITEQPreset_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITEQPreset_get_Modifiable(This,isModifiable)	\
    (This)->lpVtbl -> get_Modifiable(This,isModifiable)

#define IITEQPreset_get_Preamp(This,level)	\
    (This)->lpVtbl -> get_Preamp(This,level)

#define IITEQPreset_put_Preamp(This,level)	\
    (This)->lpVtbl -> put_Preamp(This,level)

#define IITEQPreset_get_Band1(This,level)	\
    (This)->lpVtbl -> get_Band1(This,level)

#define IITEQPreset_put_Band1(This,level)	\
    (This)->lpVtbl -> put_Band1(This,level)

#define IITEQPreset_get_Band2(This,level)	\
    (This)->lpVtbl -> get_Band2(This,level)

#define IITEQPreset_put_Band2(This,level)	\
    (This)->lpVtbl -> put_Band2(This,level)

#define IITEQPreset_get_Band3(This,level)	\
    (This)->lpVtbl -> get_Band3(This,level)

#define IITEQPreset_put_Band3(This,level)	\
    (This)->lpVtbl -> put_Band3(This,level)

#define IITEQPreset_get_Band4(This,level)	\
    (This)->lpVtbl -> get_Band4(This,level)

#define IITEQPreset_put_Band4(This,level)	\
    (This)->lpVtbl -> put_Band4(This,level)

#define IITEQPreset_get_Band5(This,level)	\
    (This)->lpVtbl -> get_Band5(This,level)

#define IITEQPreset_put_Band5(This,level)	\
    (This)->lpVtbl -> put_Band5(This,level)

#define IITEQPreset_get_Band6(This,level)	\
    (This)->lpVtbl -> get_Band6(This,level)

#define IITEQPreset_put_Band6(This,level)	\
    (This)->lpVtbl -> put_Band6(This,level)

#define IITEQPreset_get_Band7(This,level)	\
    (This)->lpVtbl -> get_Band7(This,level)

#define IITEQPreset_put_Band7(This,level)	\
    (This)->lpVtbl -> put_Band7(This,level)

#define IITEQPreset_get_Band8(This,level)	\
    (This)->lpVtbl -> get_Band8(This,level)

#define IITEQPreset_put_Band8(This,level)	\
    (This)->lpVtbl -> put_Band8(This,level)

#define IITEQPreset_get_Band9(This,level)	\
    (This)->lpVtbl -> get_Band9(This,level)

#define IITEQPreset_put_Band9(This,level)	\
    (This)->lpVtbl -> put_Band9(This,level)

#define IITEQPreset_get_Band10(This,level)	\
    (This)->lpVtbl -> get_Band10(This,level)

#define IITEQPreset_put_Band10(This,level)	\
    (This)->lpVtbl -> put_Band10(This,level)

#define IITEQPreset_Delete(This,updateAllTracks)	\
    (This)->lpVtbl -> Delete(This,updateAllTracks)

#define IITEQPreset_Rename(This,newName,updateAllTracks)	\
    (This)->lpVtbl -> Rename(This,newName,updateAllTracks)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Name_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *name);


void __RPC_STUB IITEQPreset_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Modifiable_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isModifiable);


void __RPC_STUB IITEQPreset_get_Modifiable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Preamp_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Preamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Preamp_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Preamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band1_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band1_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band2_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band2_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band3_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band3_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band4_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band4_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band5_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band5_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band5_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band5_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band6_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band6_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band7_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band7_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band8_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band8_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band8_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band8_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band9_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band9_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPreset_get_Band10_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *level);


void __RPC_STUB IITEQPreset_get_Band10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITEQPreset_put_Band10_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ double level);


void __RPC_STUB IITEQPreset_put_Band10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITEQPreset_Delete_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL updateAllTracks);


void __RPC_STUB IITEQPreset_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITEQPreset_Rename_Proxy( 
    IITEQPreset __RPC_FAR * This,
    /* [in] */ BSTR newName,
    /* [in] */ VARIANT_BOOL updateAllTracks);


void __RPC_STUB IITEQPreset_Rename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITEQPreset_INTERFACE_DEFINED__ */


#ifndef __IITEQPresetCollection_INTERFACE_DEFINED__
#define __IITEQPresetCollection_INTERFACE_DEFINED__

/* interface IITEQPresetCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITEQPresetCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AEF4D111-3331-48da-B0C2-B468D5D61D08")
    IITEQPresetCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITEQPresetCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITEQPresetCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITEQPresetCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITEQPresetCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITEQPresetCollectionVtbl;

    interface IITEQPresetCollection
    {
        CONST_VTBL struct IITEQPresetCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITEQPresetCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITEQPresetCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITEQPresetCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITEQPresetCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITEQPresetCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITEQPresetCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITEQPresetCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITEQPresetCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITEQPresetCollection_get_Item(This,index,iEQPreset)	\
    (This)->lpVtbl -> get_Item(This,index,iEQPreset)

#define IITEQPresetCollection_get_ItemByName(This,name,iEQPreset)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iEQPreset)

#define IITEQPresetCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPresetCollection_get_Count_Proxy( 
    IITEQPresetCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITEQPresetCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITEQPresetCollection_get_Item_Proxy( 
    IITEQPresetCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);


void __RPC_STUB IITEQPresetCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITEQPresetCollection_get_ItemByName_Proxy( 
    IITEQPresetCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);


void __RPC_STUB IITEQPresetCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITEQPresetCollection_get__NewEnum_Proxy( 
    IITEQPresetCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITEQPresetCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITEQPresetCollection_INTERFACE_DEFINED__ */


#ifndef __IITPlaylist_INTERFACE_DEFINED__
#define __IITPlaylist_INTERFACE_DEFINED__

/* interface IITPlaylist */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITPlaylist;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3D5E072F-2A77-4b17-9E73-E03B77CCCCA9")
    IITPlaylist : public IITObject
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PlayFirstTrack( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Print( 
            /* [in] */ VARIANT_BOOL showPrintDialog,
            /* [in] */ ITPlaylistPrintKind printKind,
            /* [in] */ BSTR theme) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Search( 
            /* [in] */ BSTR searchText,
            /* [in] */ ITPlaylistSearchField searchFields,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Kind( 
            /* [retval][out] */ ITPlaylistKind __RPC_FAR *kind) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ long __RPC_FAR *duration) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Shuffle( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShuffle) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Shuffle( 
            /* [in] */ VARIANT_BOOL shouldShuffle) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ double __RPC_FAR *size) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SongRepeat( 
            /* [retval][out] */ ITPlaylistRepeatMode __RPC_FAR *repeatMode) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SongRepeat( 
            /* [in] */ ITPlaylistRepeatMode repeatMode) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Time( 
            /* [retval][out] */ BSTR __RPC_FAR *time) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Tracks( 
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITPlaylistVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITPlaylist __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITPlaylist __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITPlaylist __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITPlaylist __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayFirstTrack )( 
            IITPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Print )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL showPrintDialog,
            /* [in] */ ITPlaylistPrintKind printKind,
            /* [in] */ BSTR theme);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Search )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ BSTR searchText,
            /* [in] */ ITPlaylistSearchField searchFields,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Shuffle )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShuffle);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Shuffle )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldShuffle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SongRepeat )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistRepeatMode __RPC_FAR *repeatMode);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SongRepeat )( 
            IITPlaylist __RPC_FAR * This,
            /* [in] */ ITPlaylistRepeatMode repeatMode);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tracks )( 
            IITPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        END_INTERFACE
    } IITPlaylistVtbl;

    interface IITPlaylist
    {
        CONST_VTBL struct IITPlaylistVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITPlaylist_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITPlaylist_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITPlaylist_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITPlaylist_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITPlaylist_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITPlaylist_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITPlaylist_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITPlaylist_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITPlaylist_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITPlaylist_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITPlaylist_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITPlaylist_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITPlaylist_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITPlaylist_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITPlaylist_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITPlaylist_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITPlaylist_PlayFirstTrack(This)	\
    (This)->lpVtbl -> PlayFirstTrack(This)

#define IITPlaylist_Print(This,showPrintDialog,printKind,theme)	\
    (This)->lpVtbl -> Print(This,showPrintDialog,printKind,theme)

#define IITPlaylist_Search(This,searchText,searchFields,iTrackCollection)	\
    (This)->lpVtbl -> Search(This,searchText,searchFields,iTrackCollection)

#define IITPlaylist_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITPlaylist_get_Source(This,iSource)	\
    (This)->lpVtbl -> get_Source(This,iSource)

#define IITPlaylist_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITPlaylist_get_Shuffle(This,isShuffle)	\
    (This)->lpVtbl -> get_Shuffle(This,isShuffle)

#define IITPlaylist_put_Shuffle(This,shouldShuffle)	\
    (This)->lpVtbl -> put_Shuffle(This,shouldShuffle)

#define IITPlaylist_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITPlaylist_get_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> get_SongRepeat(This,repeatMode)

#define IITPlaylist_put_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> put_SongRepeat(This,repeatMode)

#define IITPlaylist_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITPlaylist_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITPlaylist_get_Tracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_Tracks(This,iTrackCollection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITPlaylist_Delete_Proxy( 
    IITPlaylist __RPC_FAR * This);


void __RPC_STUB IITPlaylist_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITPlaylist_PlayFirstTrack_Proxy( 
    IITPlaylist __RPC_FAR * This);


void __RPC_STUB IITPlaylist_PlayFirstTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITPlaylist_Print_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL showPrintDialog,
    /* [in] */ ITPlaylistPrintKind printKind,
    /* [in] */ BSTR theme);


void __RPC_STUB IITPlaylist_Print_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITPlaylist_Search_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [in] */ BSTR searchText,
    /* [in] */ ITPlaylistSearchField searchFields,
    /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);


void __RPC_STUB IITPlaylist_Search_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Kind_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ ITPlaylistKind __RPC_FAR *kind);


void __RPC_STUB IITPlaylist_get_Kind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Source_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);


void __RPC_STUB IITPlaylist_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Duration_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *duration);


void __RPC_STUB IITPlaylist_get_Duration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Shuffle_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShuffle);


void __RPC_STUB IITPlaylist_get_Shuffle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITPlaylist_put_Shuffle_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldShuffle);


void __RPC_STUB IITPlaylist_put_Shuffle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Size_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *size);


void __RPC_STUB IITPlaylist_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_SongRepeat_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ ITPlaylistRepeatMode __RPC_FAR *repeatMode);


void __RPC_STUB IITPlaylist_get_SongRepeat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITPlaylist_put_SongRepeat_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [in] */ ITPlaylistRepeatMode repeatMode);


void __RPC_STUB IITPlaylist_put_SongRepeat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Time_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *time);


void __RPC_STUB IITPlaylist_get_Time_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Visible_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);


void __RPC_STUB IITPlaylist_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylist_get_Tracks_Proxy( 
    IITPlaylist __RPC_FAR * This,
    /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);


void __RPC_STUB IITPlaylist_get_Tracks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITPlaylist_INTERFACE_DEFINED__ */


#ifndef __IITOperationStatus_INTERFACE_DEFINED__
#define __IITOperationStatus_INTERFACE_DEFINED__

/* interface IITOperationStatus */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITOperationStatus;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("206479C9-FE32-4f9b-A18A-475AC939B479")
    IITOperationStatus : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_InProgress( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isInProgress) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Tracks( 
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITOperationStatusVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITOperationStatus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITOperationStatus __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITOperationStatus __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITOperationStatus __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITOperationStatus __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITOperationStatus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITOperationStatus __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InProgress )( 
            IITOperationStatus __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isInProgress);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tracks )( 
            IITOperationStatus __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        END_INTERFACE
    } IITOperationStatusVtbl;

    interface IITOperationStatus
    {
        CONST_VTBL struct IITOperationStatusVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITOperationStatus_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITOperationStatus_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITOperationStatus_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITOperationStatus_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITOperationStatus_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITOperationStatus_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITOperationStatus_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITOperationStatus_get_InProgress(This,isInProgress)	\
    (This)->lpVtbl -> get_InProgress(This,isInProgress)

#define IITOperationStatus_get_Tracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_Tracks(This,iTrackCollection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITOperationStatus_get_InProgress_Proxy( 
    IITOperationStatus __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isInProgress);


void __RPC_STUB IITOperationStatus_get_InProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITOperationStatus_get_Tracks_Proxy( 
    IITOperationStatus __RPC_FAR * This,
    /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);


void __RPC_STUB IITOperationStatus_get_Tracks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITOperationStatus_INTERFACE_DEFINED__ */


#ifndef __IITConvertOperationStatus_INTERFACE_DEFINED__
#define __IITConvertOperationStatus_INTERFACE_DEFINED__

/* interface IITConvertOperationStatus */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITConvertOperationStatus;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7063AAF6-ABA0-493b-B4FC-920A9F105875")
    IITConvertOperationStatus : public IITOperationStatus
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetConversionStatus( 
            /* [out] */ BSTR __RPC_FAR *trackName,
            /* [out] */ long __RPC_FAR *progressValue,
            /* [out] */ long __RPC_FAR *maxProgressValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StopConversion( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TrackName( 
            /* [retval][out] */ BSTR __RPC_FAR *trackName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ProgressValue( 
            /* [retval][out] */ long __RPC_FAR *progressValue) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MaxProgressValue( 
            /* [retval][out] */ long __RPC_FAR *maxProgressValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITConvertOperationStatusVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITConvertOperationStatus __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITConvertOperationStatus __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InProgress )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isInProgress);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tracks )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConversionStatus )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *trackName,
            /* [out] */ long __RPC_FAR *progressValue,
            /* [out] */ long __RPC_FAR *maxProgressValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopConversion )( 
            IITConvertOperationStatus __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackName )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *trackName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProgressValue )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *progressValue);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxProgressValue )( 
            IITConvertOperationStatus __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *maxProgressValue);
        
        END_INTERFACE
    } IITConvertOperationStatusVtbl;

    interface IITConvertOperationStatus
    {
        CONST_VTBL struct IITConvertOperationStatusVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITConvertOperationStatus_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITConvertOperationStatus_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITConvertOperationStatus_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITConvertOperationStatus_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITConvertOperationStatus_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITConvertOperationStatus_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITConvertOperationStatus_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITConvertOperationStatus_get_InProgress(This,isInProgress)	\
    (This)->lpVtbl -> get_InProgress(This,isInProgress)

#define IITConvertOperationStatus_get_Tracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_Tracks(This,iTrackCollection)


#define IITConvertOperationStatus_GetConversionStatus(This,trackName,progressValue,maxProgressValue)	\
    (This)->lpVtbl -> GetConversionStatus(This,trackName,progressValue,maxProgressValue)

#define IITConvertOperationStatus_StopConversion(This)	\
    (This)->lpVtbl -> StopConversion(This)

#define IITConvertOperationStatus_get_TrackName(This,trackName)	\
    (This)->lpVtbl -> get_TrackName(This,trackName)

#define IITConvertOperationStatus_get_ProgressValue(This,progressValue)	\
    (This)->lpVtbl -> get_ProgressValue(This,progressValue)

#define IITConvertOperationStatus_get_MaxProgressValue(This,maxProgressValue)	\
    (This)->lpVtbl -> get_MaxProgressValue(This,maxProgressValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITConvertOperationStatus_GetConversionStatus_Proxy( 
    IITConvertOperationStatus __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *trackName,
    /* [out] */ long __RPC_FAR *progressValue,
    /* [out] */ long __RPC_FAR *maxProgressValue);


void __RPC_STUB IITConvertOperationStatus_GetConversionStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITConvertOperationStatus_StopConversion_Proxy( 
    IITConvertOperationStatus __RPC_FAR * This);


void __RPC_STUB IITConvertOperationStatus_StopConversion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITConvertOperationStatus_get_TrackName_Proxy( 
    IITConvertOperationStatus __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *trackName);


void __RPC_STUB IITConvertOperationStatus_get_TrackName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITConvertOperationStatus_get_ProgressValue_Proxy( 
    IITConvertOperationStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *progressValue);


void __RPC_STUB IITConvertOperationStatus_get_ProgressValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITConvertOperationStatus_get_MaxProgressValue_Proxy( 
    IITConvertOperationStatus __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *maxProgressValue);


void __RPC_STUB IITConvertOperationStatus_get_MaxProgressValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITConvertOperationStatus_INTERFACE_DEFINED__ */


#ifndef __IITLibraryPlaylist_INTERFACE_DEFINED__
#define __IITLibraryPlaylist_INTERFACE_DEFINED__

/* interface IITLibraryPlaylist */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITLibraryPlaylist;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53AE1704-491C-4289-94A0-958815675A3D")
    IITLibraryPlaylist : public IITPlaylist
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddFile( 
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddFiles( 
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddURL( 
            /* [in] */ BSTR url,
            /* [retval][out] */ IITURLTrack __RPC_FAR *__RPC_FAR *iURLTrack) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddTrack( 
            /* [in] */ VARIANT __RPC_FAR *iTrackToAdd,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iAddedTrack) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITLibraryPlaylistVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITLibraryPlaylist __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITLibraryPlaylist __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITLibraryPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayFirstTrack )( 
            IITLibraryPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Print )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL showPrintDialog,
            /* [in] */ ITPlaylistPrintKind printKind,
            /* [in] */ BSTR theme);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Search )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ BSTR searchText,
            /* [in] */ ITPlaylistSearchField searchFields,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Shuffle )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShuffle);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Shuffle )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldShuffle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SongRepeat )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistRepeatMode __RPC_FAR *repeatMode);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SongRepeat )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ ITPlaylistRepeatMode repeatMode);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tracks )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddFile )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddFiles )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddURL )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ BSTR url,
            /* [retval][out] */ IITURLTrack __RPC_FAR *__RPC_FAR *iURLTrack);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddTrack )( 
            IITLibraryPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iTrackToAdd,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iAddedTrack);
        
        END_INTERFACE
    } IITLibraryPlaylistVtbl;

    interface IITLibraryPlaylist
    {
        CONST_VTBL struct IITLibraryPlaylistVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITLibraryPlaylist_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITLibraryPlaylist_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITLibraryPlaylist_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITLibraryPlaylist_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITLibraryPlaylist_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITLibraryPlaylist_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITLibraryPlaylist_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITLibraryPlaylist_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITLibraryPlaylist_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITLibraryPlaylist_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITLibraryPlaylist_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITLibraryPlaylist_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITLibraryPlaylist_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITLibraryPlaylist_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITLibraryPlaylist_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITLibraryPlaylist_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITLibraryPlaylist_PlayFirstTrack(This)	\
    (This)->lpVtbl -> PlayFirstTrack(This)

#define IITLibraryPlaylist_Print(This,showPrintDialog,printKind,theme)	\
    (This)->lpVtbl -> Print(This,showPrintDialog,printKind,theme)

#define IITLibraryPlaylist_Search(This,searchText,searchFields,iTrackCollection)	\
    (This)->lpVtbl -> Search(This,searchText,searchFields,iTrackCollection)

#define IITLibraryPlaylist_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITLibraryPlaylist_get_Source(This,iSource)	\
    (This)->lpVtbl -> get_Source(This,iSource)

#define IITLibraryPlaylist_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITLibraryPlaylist_get_Shuffle(This,isShuffle)	\
    (This)->lpVtbl -> get_Shuffle(This,isShuffle)

#define IITLibraryPlaylist_put_Shuffle(This,shouldShuffle)	\
    (This)->lpVtbl -> put_Shuffle(This,shouldShuffle)

#define IITLibraryPlaylist_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITLibraryPlaylist_get_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> get_SongRepeat(This,repeatMode)

#define IITLibraryPlaylist_put_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> put_SongRepeat(This,repeatMode)

#define IITLibraryPlaylist_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITLibraryPlaylist_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITLibraryPlaylist_get_Tracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_Tracks(This,iTrackCollection)


#define IITLibraryPlaylist_AddFile(This,filePath,iStatus)	\
    (This)->lpVtbl -> AddFile(This,filePath,iStatus)

#define IITLibraryPlaylist_AddFiles(This,filePaths,iStatus)	\
    (This)->lpVtbl -> AddFiles(This,filePaths,iStatus)

#define IITLibraryPlaylist_AddURL(This,url,iURLTrack)	\
    (This)->lpVtbl -> AddURL(This,url,iURLTrack)

#define IITLibraryPlaylist_AddTrack(This,iTrackToAdd,iAddedTrack)	\
    (This)->lpVtbl -> AddTrack(This,iTrackToAdd,iAddedTrack)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITLibraryPlaylist_AddFile_Proxy( 
    IITLibraryPlaylist __RPC_FAR * This,
    /* [in] */ BSTR filePath,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IITLibraryPlaylist_AddFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITLibraryPlaylist_AddFiles_Proxy( 
    IITLibraryPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *filePaths,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IITLibraryPlaylist_AddFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITLibraryPlaylist_AddURL_Proxy( 
    IITLibraryPlaylist __RPC_FAR * This,
    /* [in] */ BSTR url,
    /* [retval][out] */ IITURLTrack __RPC_FAR *__RPC_FAR *iURLTrack);


void __RPC_STUB IITLibraryPlaylist_AddURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITLibraryPlaylist_AddTrack_Proxy( 
    IITLibraryPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iTrackToAdd,
    /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iAddedTrack);


void __RPC_STUB IITLibraryPlaylist_AddTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITLibraryPlaylist_INTERFACE_DEFINED__ */


#ifndef __IITUserPlaylist_INTERFACE_DEFINED__
#define __IITUserPlaylist_INTERFACE_DEFINED__

/* interface IITUserPlaylist */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITUserPlaylist;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0A504DED-A0B5-465a-8A94-50E20D7DF692")
    IITUserPlaylist : public IITPlaylist
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddFile( 
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddFiles( 
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddURL( 
            /* [in] */ BSTR url,
            /* [retval][out] */ IITURLTrack __RPC_FAR *__RPC_FAR *iURLTrack) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddTrack( 
            /* [in] */ VARIANT __RPC_FAR *iTrackToAdd,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iAddedTrack) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Shared( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShared) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Shared( 
            /* [in] */ VARIANT_BOOL shouldBeShared) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Smart( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isSmart) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SpecialKind( 
            /* [retval][out] */ ITUserPlaylistSpecialKind __RPC_FAR *specialKind) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IITUserPlaylist __RPC_FAR *__RPC_FAR *iParentPlayList) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreatePlaylist( 
            /* [in] */ BSTR playlistName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateFolder( 
            /* [in] */ BSTR folderName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Parent( 
            /* [in] */ VARIANT __RPC_FAR *iParent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITUserPlaylistVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITUserPlaylist __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITUserPlaylist __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITUserPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayFirstTrack )( 
            IITUserPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Print )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL showPrintDialog,
            /* [in] */ ITPlaylistPrintKind printKind,
            /* [in] */ BSTR theme);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Search )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ BSTR searchText,
            /* [in] */ ITPlaylistSearchField searchFields,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Shuffle )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShuffle);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Shuffle )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldShuffle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SongRepeat )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistRepeatMode __RPC_FAR *repeatMode);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SongRepeat )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ ITPlaylistRepeatMode repeatMode);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tracks )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddFile )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddFiles )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddURL )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ BSTR url,
            /* [retval][out] */ IITURLTrack __RPC_FAR *__RPC_FAR *iURLTrack);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddTrack )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iTrackToAdd,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iAddedTrack);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Shared )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShared);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Shared )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeShared);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Smart )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isSmart);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SpecialKind )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITUserPlaylistSpecialKind __RPC_FAR *specialKind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITUserPlaylist __RPC_FAR *__RPC_FAR *iParentPlayList);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePlaylist )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ BSTR playlistName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateFolder )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ BSTR folderName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Parent )( 
            IITUserPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iParent);
        
        END_INTERFACE
    } IITUserPlaylistVtbl;

    interface IITUserPlaylist
    {
        CONST_VTBL struct IITUserPlaylistVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITUserPlaylist_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITUserPlaylist_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITUserPlaylist_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITUserPlaylist_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITUserPlaylist_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITUserPlaylist_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITUserPlaylist_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITUserPlaylist_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITUserPlaylist_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITUserPlaylist_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITUserPlaylist_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITUserPlaylist_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITUserPlaylist_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITUserPlaylist_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITUserPlaylist_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITUserPlaylist_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITUserPlaylist_PlayFirstTrack(This)	\
    (This)->lpVtbl -> PlayFirstTrack(This)

#define IITUserPlaylist_Print(This,showPrintDialog,printKind,theme)	\
    (This)->lpVtbl -> Print(This,showPrintDialog,printKind,theme)

#define IITUserPlaylist_Search(This,searchText,searchFields,iTrackCollection)	\
    (This)->lpVtbl -> Search(This,searchText,searchFields,iTrackCollection)

#define IITUserPlaylist_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITUserPlaylist_get_Source(This,iSource)	\
    (This)->lpVtbl -> get_Source(This,iSource)

#define IITUserPlaylist_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITUserPlaylist_get_Shuffle(This,isShuffle)	\
    (This)->lpVtbl -> get_Shuffle(This,isShuffle)

#define IITUserPlaylist_put_Shuffle(This,shouldShuffle)	\
    (This)->lpVtbl -> put_Shuffle(This,shouldShuffle)

#define IITUserPlaylist_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITUserPlaylist_get_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> get_SongRepeat(This,repeatMode)

#define IITUserPlaylist_put_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> put_SongRepeat(This,repeatMode)

#define IITUserPlaylist_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITUserPlaylist_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITUserPlaylist_get_Tracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_Tracks(This,iTrackCollection)


#define IITUserPlaylist_AddFile(This,filePath,iStatus)	\
    (This)->lpVtbl -> AddFile(This,filePath,iStatus)

#define IITUserPlaylist_AddFiles(This,filePaths,iStatus)	\
    (This)->lpVtbl -> AddFiles(This,filePaths,iStatus)

#define IITUserPlaylist_AddURL(This,url,iURLTrack)	\
    (This)->lpVtbl -> AddURL(This,url,iURLTrack)

#define IITUserPlaylist_AddTrack(This,iTrackToAdd,iAddedTrack)	\
    (This)->lpVtbl -> AddTrack(This,iTrackToAdd,iAddedTrack)

#define IITUserPlaylist_get_Shared(This,isShared)	\
    (This)->lpVtbl -> get_Shared(This,isShared)

#define IITUserPlaylist_put_Shared(This,shouldBeShared)	\
    (This)->lpVtbl -> put_Shared(This,shouldBeShared)

#define IITUserPlaylist_get_Smart(This,isSmart)	\
    (This)->lpVtbl -> get_Smart(This,isSmart)

#define IITUserPlaylist_get_SpecialKind(This,specialKind)	\
    (This)->lpVtbl -> get_SpecialKind(This,specialKind)

#define IITUserPlaylist_get_Parent(This,iParentPlayList)	\
    (This)->lpVtbl -> get_Parent(This,iParentPlayList)

#define IITUserPlaylist_CreatePlaylist(This,playlistName,iPlaylist)	\
    (This)->lpVtbl -> CreatePlaylist(This,playlistName,iPlaylist)

#define IITUserPlaylist_CreateFolder(This,folderName,iFolder)	\
    (This)->lpVtbl -> CreateFolder(This,folderName,iFolder)

#define IITUserPlaylist_put_Parent(This,iParent)	\
    (This)->lpVtbl -> put_Parent(This,iParent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_AddFile_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ BSTR filePath,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IITUserPlaylist_AddFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_AddFiles_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *filePaths,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IITUserPlaylist_AddFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_AddURL_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ BSTR url,
    /* [retval][out] */ IITURLTrack __RPC_FAR *__RPC_FAR *iURLTrack);


void __RPC_STUB IITUserPlaylist_AddURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_AddTrack_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iTrackToAdd,
    /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iAddedTrack);


void __RPC_STUB IITUserPlaylist_AddTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_get_Shared_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShared);


void __RPC_STUB IITUserPlaylist_get_Shared_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_put_Shared_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeShared);


void __RPC_STUB IITUserPlaylist_put_Shared_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_get_Smart_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isSmart);


void __RPC_STUB IITUserPlaylist_get_Smart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_get_SpecialKind_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [retval][out] */ ITUserPlaylistSpecialKind __RPC_FAR *specialKind);


void __RPC_STUB IITUserPlaylist_get_SpecialKind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_get_Parent_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [retval][out] */ IITUserPlaylist __RPC_FAR *__RPC_FAR *iParentPlayList);


void __RPC_STUB IITUserPlaylist_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_CreatePlaylist_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ BSTR playlistName,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IITUserPlaylist_CreatePlaylist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_CreateFolder_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ BSTR folderName,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder);


void __RPC_STUB IITUserPlaylist_CreateFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITUserPlaylist_put_Parent_Proxy( 
    IITUserPlaylist __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iParent);


void __RPC_STUB IITUserPlaylist_put_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITUserPlaylist_INTERFACE_DEFINED__ */


#ifndef __IITTrack_INTERFACE_DEFINED__
#define __IITTrack_INTERFACE_DEFINED__

/* interface IITTrack */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITTrack;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CB0915D-1E54-4727-BAF3-CE6CC9A225A1")
    IITTrack : public IITObject
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddArtworkFromFile( 
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Kind( 
            /* [retval][out] */ ITTrackKind __RPC_FAR *kind) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Playlist( 
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Album( 
            /* [retval][out] */ BSTR __RPC_FAR *album) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Album( 
            /* [in] */ BSTR album) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Artist( 
            /* [retval][out] */ BSTR __RPC_FAR *artist) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Artist( 
            /* [in] */ BSTR artist) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BitRate( 
            /* [retval][out] */ long __RPC_FAR *bitrate) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BPM( 
            /* [retval][out] */ long __RPC_FAR *beatsPerMinute) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_BPM( 
            /* [in] */ long beatsPerMinute) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Comment( 
            /* [retval][out] */ BSTR __RPC_FAR *comment) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Comment( 
            /* [in] */ BSTR comment) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Compilation( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompilation) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Compilation( 
            /* [in] */ VARIANT_BOOL shouldBeCompilation) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Composer( 
            /* [retval][out] */ BSTR __RPC_FAR *composer) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Composer( 
            /* [in] */ BSTR composer) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DateAdded( 
            /* [retval][out] */ DATE __RPC_FAR *dateAdded) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DiscCount( 
            /* [retval][out] */ long __RPC_FAR *discCount) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DiscCount( 
            /* [in] */ long discCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DiscNumber( 
            /* [retval][out] */ long __RPC_FAR *discNumber) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_DiscNumber( 
            /* [in] */ long discNumber) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ long __RPC_FAR *duration) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL shouldBeEnabled) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EQ( 
            /* [retval][out] */ BSTR __RPC_FAR *eq) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EQ( 
            /* [in] */ BSTR eq) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Finish( 
            /* [in] */ long finish) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Finish( 
            /* [retval][out] */ long __RPC_FAR *finish) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Genre( 
            /* [retval][out] */ BSTR __RPC_FAR *genre) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Genre( 
            /* [in] */ BSTR genre) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Grouping( 
            /* [retval][out] */ BSTR __RPC_FAR *grouping) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Grouping( 
            /* [in] */ BSTR grouping) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_KindAsString( 
            /* [retval][out] */ BSTR __RPC_FAR *kind) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ModificationDate( 
            /* [retval][out] */ DATE __RPC_FAR *dateModified) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlayedCount( 
            /* [retval][out] */ long __RPC_FAR *playedCount) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PlayedCount( 
            /* [in] */ long playedCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlayedDate( 
            /* [retval][out] */ DATE __RPC_FAR *playedDate) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PlayedDate( 
            /* [in] */ DATE playedDate) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlayOrderIndex( 
            /* [retval][out] */ long __RPC_FAR *index) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Rating( 
            /* [retval][out] */ long __RPC_FAR *rating) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Rating( 
            /* [in] */ long rating) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SampleRate( 
            /* [retval][out] */ long __RPC_FAR *sampleRate) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ long __RPC_FAR *size) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Start( 
            /* [retval][out] */ long __RPC_FAR *start) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Start( 
            /* [in] */ long start) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Time( 
            /* [retval][out] */ BSTR __RPC_FAR *time) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TrackCount( 
            /* [retval][out] */ long __RPC_FAR *trackCount) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_TrackCount( 
            /* [in] */ long trackCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_TrackNumber( 
            /* [retval][out] */ long __RPC_FAR *trackNumber) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_TrackNumber( 
            /* [in] */ long trackNumber) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VolumeAdjustment( 
            /* [retval][out] */ long __RPC_FAR *volumeAdjustment) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_VolumeAdjustment( 
            /* [in] */ long volumeAdjustment) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Year( 
            /* [retval][out] */ long __RPC_FAR *year) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Year( 
            /* [in] */ long year) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Artwork( 
            /* [retval][out] */ IITArtworkCollection __RPC_FAR *__RPC_FAR *iArtworkCollection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITTrackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITTrack __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITTrack __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITTrack __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITTrack __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            IITTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddArtworkFromFile )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ ITTrackKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Playlist )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Album )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *album);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Album )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR album);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artist )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *artist);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Artist )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR artist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bitrate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BPM )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *beatsPerMinute);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BPM )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long beatsPerMinute);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Comment )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *comment);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Comment )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR comment);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Compilation )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompilation);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Compilation )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeCompilation);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Composer )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *composer);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Composer )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR composer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateAdded )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *dateAdded);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscCount )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DiscCount )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long discCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscNumber )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discNumber);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DiscNumber )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long discNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeEnabled);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EQ )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *eq);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EQ )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR eq);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Finish )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long finish);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Finish )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *finish);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Genre )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *genre);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Genre )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR genre);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Grouping )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *grouping);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Grouping )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ BSTR grouping);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KindAsString )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModificationDate )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *dateModified);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayedCount )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playedCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayedCount )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long playedCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayedDate )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *playedDate);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayedDate )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ DATE playedDate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayOrderIndex )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *rating);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rating )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long rating);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SampleRate )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sampleRate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Start )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *start);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Start )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long start);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackCount )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TrackCount )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long trackCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackNumber )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackNumber);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TrackNumber )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long trackNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VolumeAdjustment )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *volumeAdjustment);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VolumeAdjustment )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long volumeAdjustment);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Year )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *year);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Year )( 
            IITTrack __RPC_FAR * This,
            /* [in] */ long year);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artwork )( 
            IITTrack __RPC_FAR * This,
            /* [retval][out] */ IITArtworkCollection __RPC_FAR *__RPC_FAR *iArtworkCollection);
        
        END_INTERFACE
    } IITTrackVtbl;

    interface IITTrack
    {
        CONST_VTBL struct IITTrackVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITTrack_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITTrack_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITTrack_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITTrack_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITTrack_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITTrack_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITTrack_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITTrack_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITTrack_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITTrack_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITTrack_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITTrack_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITTrack_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITTrack_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITTrack_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITTrack_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITTrack_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define IITTrack_AddArtworkFromFile(This,filePath,iArtwork)	\
    (This)->lpVtbl -> AddArtworkFromFile(This,filePath,iArtwork)

#define IITTrack_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITTrack_get_Playlist(This,iPlaylist)	\
    (This)->lpVtbl -> get_Playlist(This,iPlaylist)

#define IITTrack_get_Album(This,album)	\
    (This)->lpVtbl -> get_Album(This,album)

#define IITTrack_put_Album(This,album)	\
    (This)->lpVtbl -> put_Album(This,album)

#define IITTrack_get_Artist(This,artist)	\
    (This)->lpVtbl -> get_Artist(This,artist)

#define IITTrack_put_Artist(This,artist)	\
    (This)->lpVtbl -> put_Artist(This,artist)

#define IITTrack_get_BitRate(This,bitrate)	\
    (This)->lpVtbl -> get_BitRate(This,bitrate)

#define IITTrack_get_BPM(This,beatsPerMinute)	\
    (This)->lpVtbl -> get_BPM(This,beatsPerMinute)

#define IITTrack_put_BPM(This,beatsPerMinute)	\
    (This)->lpVtbl -> put_BPM(This,beatsPerMinute)

#define IITTrack_get_Comment(This,comment)	\
    (This)->lpVtbl -> get_Comment(This,comment)

#define IITTrack_put_Comment(This,comment)	\
    (This)->lpVtbl -> put_Comment(This,comment)

#define IITTrack_get_Compilation(This,isCompilation)	\
    (This)->lpVtbl -> get_Compilation(This,isCompilation)

#define IITTrack_put_Compilation(This,shouldBeCompilation)	\
    (This)->lpVtbl -> put_Compilation(This,shouldBeCompilation)

#define IITTrack_get_Composer(This,composer)	\
    (This)->lpVtbl -> get_Composer(This,composer)

#define IITTrack_put_Composer(This,composer)	\
    (This)->lpVtbl -> put_Composer(This,composer)

#define IITTrack_get_DateAdded(This,dateAdded)	\
    (This)->lpVtbl -> get_DateAdded(This,dateAdded)

#define IITTrack_get_DiscCount(This,discCount)	\
    (This)->lpVtbl -> get_DiscCount(This,discCount)

#define IITTrack_put_DiscCount(This,discCount)	\
    (This)->lpVtbl -> put_DiscCount(This,discCount)

#define IITTrack_get_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> get_DiscNumber(This,discNumber)

#define IITTrack_put_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> put_DiscNumber(This,discNumber)

#define IITTrack_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITTrack_get_Enabled(This,isEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,isEnabled)

#define IITTrack_put_Enabled(This,shouldBeEnabled)	\
    (This)->lpVtbl -> put_Enabled(This,shouldBeEnabled)

#define IITTrack_get_EQ(This,eq)	\
    (This)->lpVtbl -> get_EQ(This,eq)

#define IITTrack_put_EQ(This,eq)	\
    (This)->lpVtbl -> put_EQ(This,eq)

#define IITTrack_put_Finish(This,finish)	\
    (This)->lpVtbl -> put_Finish(This,finish)

#define IITTrack_get_Finish(This,finish)	\
    (This)->lpVtbl -> get_Finish(This,finish)

#define IITTrack_get_Genre(This,genre)	\
    (This)->lpVtbl -> get_Genre(This,genre)

#define IITTrack_put_Genre(This,genre)	\
    (This)->lpVtbl -> put_Genre(This,genre)

#define IITTrack_get_Grouping(This,grouping)	\
    (This)->lpVtbl -> get_Grouping(This,grouping)

#define IITTrack_put_Grouping(This,grouping)	\
    (This)->lpVtbl -> put_Grouping(This,grouping)

#define IITTrack_get_KindAsString(This,kind)	\
    (This)->lpVtbl -> get_KindAsString(This,kind)

#define IITTrack_get_ModificationDate(This,dateModified)	\
    (This)->lpVtbl -> get_ModificationDate(This,dateModified)

#define IITTrack_get_PlayedCount(This,playedCount)	\
    (This)->lpVtbl -> get_PlayedCount(This,playedCount)

#define IITTrack_put_PlayedCount(This,playedCount)	\
    (This)->lpVtbl -> put_PlayedCount(This,playedCount)

#define IITTrack_get_PlayedDate(This,playedDate)	\
    (This)->lpVtbl -> get_PlayedDate(This,playedDate)

#define IITTrack_put_PlayedDate(This,playedDate)	\
    (This)->lpVtbl -> put_PlayedDate(This,playedDate)

#define IITTrack_get_PlayOrderIndex(This,index)	\
    (This)->lpVtbl -> get_PlayOrderIndex(This,index)

#define IITTrack_get_Rating(This,rating)	\
    (This)->lpVtbl -> get_Rating(This,rating)

#define IITTrack_put_Rating(This,rating)	\
    (This)->lpVtbl -> put_Rating(This,rating)

#define IITTrack_get_SampleRate(This,sampleRate)	\
    (This)->lpVtbl -> get_SampleRate(This,sampleRate)

#define IITTrack_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITTrack_get_Start(This,start)	\
    (This)->lpVtbl -> get_Start(This,start)

#define IITTrack_put_Start(This,start)	\
    (This)->lpVtbl -> put_Start(This,start)

#define IITTrack_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITTrack_get_TrackCount(This,trackCount)	\
    (This)->lpVtbl -> get_TrackCount(This,trackCount)

#define IITTrack_put_TrackCount(This,trackCount)	\
    (This)->lpVtbl -> put_TrackCount(This,trackCount)

#define IITTrack_get_TrackNumber(This,trackNumber)	\
    (This)->lpVtbl -> get_TrackNumber(This,trackNumber)

#define IITTrack_put_TrackNumber(This,trackNumber)	\
    (This)->lpVtbl -> put_TrackNumber(This,trackNumber)

#define IITTrack_get_VolumeAdjustment(This,volumeAdjustment)	\
    (This)->lpVtbl -> get_VolumeAdjustment(This,volumeAdjustment)

#define IITTrack_put_VolumeAdjustment(This,volumeAdjustment)	\
    (This)->lpVtbl -> put_VolumeAdjustment(This,volumeAdjustment)

#define IITTrack_get_Year(This,year)	\
    (This)->lpVtbl -> get_Year(This,year)

#define IITTrack_put_Year(This,year)	\
    (This)->lpVtbl -> put_Year(This,year)

#define IITTrack_get_Artwork(This,iArtworkCollection)	\
    (This)->lpVtbl -> get_Artwork(This,iArtworkCollection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITTrack_Delete_Proxy( 
    IITTrack __RPC_FAR * This);


void __RPC_STUB IITTrack_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITTrack_Play_Proxy( 
    IITTrack __RPC_FAR * This);


void __RPC_STUB IITTrack_Play_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITTrack_AddArtworkFromFile_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR filePath,
    /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork);


void __RPC_STUB IITTrack_AddArtworkFromFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Kind_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ ITTrackKind __RPC_FAR *kind);


void __RPC_STUB IITTrack_get_Kind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Playlist_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IITTrack_get_Playlist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Album_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *album);


void __RPC_STUB IITTrack_get_Album_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Album_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR album);


void __RPC_STUB IITTrack_put_Album_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Artist_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *artist);


void __RPC_STUB IITTrack_get_Artist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Artist_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR artist);


void __RPC_STUB IITTrack_put_Artist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_BitRate_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *bitrate);


void __RPC_STUB IITTrack_get_BitRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_BPM_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *beatsPerMinute);


void __RPC_STUB IITTrack_get_BPM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_BPM_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long beatsPerMinute);


void __RPC_STUB IITTrack_put_BPM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Comment_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *comment);


void __RPC_STUB IITTrack_get_Comment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Comment_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR comment);


void __RPC_STUB IITTrack_put_Comment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Compilation_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompilation);


void __RPC_STUB IITTrack_get_Compilation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Compilation_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeCompilation);


void __RPC_STUB IITTrack_put_Compilation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Composer_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *composer);


void __RPC_STUB IITTrack_get_Composer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Composer_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR composer);


void __RPC_STUB IITTrack_put_Composer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_DateAdded_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *dateAdded);


void __RPC_STUB IITTrack_get_DateAdded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_DiscCount_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *discCount);


void __RPC_STUB IITTrack_get_DiscCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_DiscCount_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long discCount);


void __RPC_STUB IITTrack_put_DiscCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_DiscNumber_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *discNumber);


void __RPC_STUB IITTrack_get_DiscNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_DiscNumber_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long discNumber);


void __RPC_STUB IITTrack_put_DiscNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Duration_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *duration);


void __RPC_STUB IITTrack_get_Duration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Enabled_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);


void __RPC_STUB IITTrack_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Enabled_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeEnabled);


void __RPC_STUB IITTrack_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_EQ_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *eq);


void __RPC_STUB IITTrack_get_EQ_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_EQ_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR eq);


void __RPC_STUB IITTrack_put_EQ_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Finish_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long finish);


void __RPC_STUB IITTrack_put_Finish_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Finish_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *finish);


void __RPC_STUB IITTrack_get_Finish_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Genre_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *genre);


void __RPC_STUB IITTrack_get_Genre_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Genre_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR genre);


void __RPC_STUB IITTrack_put_Genre_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Grouping_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *grouping);


void __RPC_STUB IITTrack_get_Grouping_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Grouping_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ BSTR grouping);


void __RPC_STUB IITTrack_put_Grouping_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_KindAsString_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *kind);


void __RPC_STUB IITTrack_get_KindAsString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_ModificationDate_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *dateModified);


void __RPC_STUB IITTrack_get_ModificationDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_PlayedCount_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *playedCount);


void __RPC_STUB IITTrack_get_PlayedCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_PlayedCount_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long playedCount);


void __RPC_STUB IITTrack_put_PlayedCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_PlayedDate_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *playedDate);


void __RPC_STUB IITTrack_get_PlayedDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_PlayedDate_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ DATE playedDate);


void __RPC_STUB IITTrack_put_PlayedDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_PlayOrderIndex_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *index);


void __RPC_STUB IITTrack_get_PlayOrderIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Rating_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *rating);


void __RPC_STUB IITTrack_get_Rating_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Rating_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long rating);


void __RPC_STUB IITTrack_put_Rating_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_SampleRate_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *sampleRate);


void __RPC_STUB IITTrack_get_SampleRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Size_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *size);


void __RPC_STUB IITTrack_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Start_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *start);


void __RPC_STUB IITTrack_get_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Start_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long start);


void __RPC_STUB IITTrack_put_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Time_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *time);


void __RPC_STUB IITTrack_get_Time_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_TrackCount_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *trackCount);


void __RPC_STUB IITTrack_get_TrackCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_TrackCount_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long trackCount);


void __RPC_STUB IITTrack_put_TrackCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_TrackNumber_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *trackNumber);


void __RPC_STUB IITTrack_get_TrackNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_TrackNumber_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long trackNumber);


void __RPC_STUB IITTrack_put_TrackNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_VolumeAdjustment_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *volumeAdjustment);


void __RPC_STUB IITTrack_get_VolumeAdjustment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_VolumeAdjustment_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long volumeAdjustment);


void __RPC_STUB IITTrack_put_VolumeAdjustment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Year_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *year);


void __RPC_STUB IITTrack_get_Year_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITTrack_put_Year_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [in] */ long year);


void __RPC_STUB IITTrack_put_Year_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrack_get_Artwork_Proxy( 
    IITTrack __RPC_FAR * This,
    /* [retval][out] */ IITArtworkCollection __RPC_FAR *__RPC_FAR *iArtworkCollection);


void __RPC_STUB IITTrack_get_Artwork_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITTrack_INTERFACE_DEFINED__ */


#ifndef __IITTrackCollection_INTERFACE_DEFINED__
#define __IITTrackCollection_INTERFACE_DEFINED__

/* interface IITTrackCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITTrackCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("755D76F1-6B85-4ce4-8F5F-F88D9743DCD8")
    IITTrackCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByPlayOrder( 
            /* [in] */ long index,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITTrackCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITTrackCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITTrackCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITTrackCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITTrackCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByPlayOrder )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITTrackCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITTrackCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITTrackCollectionVtbl;

    interface IITTrackCollection
    {
        CONST_VTBL struct IITTrackCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITTrackCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITTrackCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITTrackCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITTrackCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITTrackCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITTrackCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITTrackCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITTrackCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITTrackCollection_get_Item(This,index,iTrack)	\
    (This)->lpVtbl -> get_Item(This,index,iTrack)

#define IITTrackCollection_get_ItemByPlayOrder(This,index,iTrack)	\
    (This)->lpVtbl -> get_ItemByPlayOrder(This,index,iTrack)

#define IITTrackCollection_get_ItemByName(This,name,iTrack)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iTrack)

#define IITTrackCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrackCollection_get_Count_Proxy( 
    IITTrackCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITTrackCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITTrackCollection_get_Item_Proxy( 
    IITTrackCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);


void __RPC_STUB IITTrackCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrackCollection_get_ItemByPlayOrder_Proxy( 
    IITTrackCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);


void __RPC_STUB IITTrackCollection_get_ItemByPlayOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITTrackCollection_get_ItemByName_Proxy( 
    IITTrackCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);


void __RPC_STUB IITTrackCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITTrackCollection_get__NewEnum_Proxy( 
    IITTrackCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITTrackCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITTrackCollection_INTERFACE_DEFINED__ */


#ifndef __IITVisual_INTERFACE_DEFINED__
#define __IITVisual_INTERFACE_DEFINED__

/* interface IITVisual */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITVisual;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("340F3315-ED72-4c09-9ACF-21EB4BDF9931")
    IITVisual : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *name) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITVisualVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITVisual __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITVisual __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITVisual __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITVisual __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITVisual __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITVisual __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITVisual __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITVisual __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        END_INTERFACE
    } IITVisualVtbl;

    interface IITVisual
    {
        CONST_VTBL struct IITVisualVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITVisual_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITVisual_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITVisual_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITVisual_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITVisual_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITVisual_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITVisual_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITVisual_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITVisual_get_Name_Proxy( 
    IITVisual __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *name);


void __RPC_STUB IITVisual_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITVisual_INTERFACE_DEFINED__ */


#ifndef __IITVisualCollection_INTERFACE_DEFINED__
#define __IITVisualCollection_INTERFACE_DEFINED__

/* interface IITVisualCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITVisualCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88A4CCDD-114F-4043-B69B-84D4E6274957")
    IITVisualCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITVisualCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITVisualCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITVisualCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITVisualCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITVisualCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITVisualCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITVisualCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITVisualCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITVisualCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITVisualCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITVisualCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITVisualCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITVisualCollectionVtbl;

    interface IITVisualCollection
    {
        CONST_VTBL struct IITVisualCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITVisualCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITVisualCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITVisualCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITVisualCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITVisualCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITVisualCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITVisualCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITVisualCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITVisualCollection_get_Item(This,index,iVisual)	\
    (This)->lpVtbl -> get_Item(This,index,iVisual)

#define IITVisualCollection_get_ItemByName(This,name,iVisual)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iVisual)

#define IITVisualCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITVisualCollection_get_Count_Proxy( 
    IITVisualCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITVisualCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITVisualCollection_get_Item_Proxy( 
    IITVisualCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual);


void __RPC_STUB IITVisualCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITVisualCollection_get_ItemByName_Proxy( 
    IITVisualCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual);


void __RPC_STUB IITVisualCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITVisualCollection_get__NewEnum_Proxy( 
    IITVisualCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITVisualCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITVisualCollection_INTERFACE_DEFINED__ */


#ifndef __IITWindow_INTERFACE_DEFINED__
#define __IITWindow_INTERFACE_DEFINED__

/* interface IITWindow */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("370D7BE0-3A89-4a42-B902-C75FC138BE09")
    IITWindow : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *name) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Kind( 
            /* [retval][out] */ ITWindowKind __RPC_FAR *kind) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL shouldBeVisible) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Resizable( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isResizable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Minimized( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMinimized) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Minimized( 
            /* [in] */ VARIANT_BOOL shouldBeMinimized) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Maximizable( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximizable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Maximized( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximized) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Maximized( 
            /* [in] */ VARIANT_BOOL shouldBeMaximized) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Zoomable( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Zoomed( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomed) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Zoomed( 
            /* [in] */ VARIANT_BOOL shouldBeZoomed) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Top( 
            /* [retval][out] */ long __RPC_FAR *top) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Top( 
            /* [in] */ long top) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Left( 
            /* [retval][out] */ long __RPC_FAR *left) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Left( 
            /* [in] */ long left) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Bottom( 
            /* [retval][out] */ long __RPC_FAR *bottom) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Bottom( 
            /* [in] */ long bottom) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Right( 
            /* [retval][out] */ long __RPC_FAR *right) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Right( 
            /* [in] */ long right) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long __RPC_FAR *width) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ long width) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long __RPC_FAR *height) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ long height) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITWindow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ ITWindowKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Resizable )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isResizable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Minimized )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMinimized);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Minimized )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMinimized);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Maximizable )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximizable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Maximized )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximized);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Maximized )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMaximized);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoomable )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoomed )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomed);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Zoomed )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeZoomed);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Top )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *top);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Top )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ long top);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Left )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *left);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Left )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ long left);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bottom )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bottom);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bottom )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ long bottom);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Right )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *right);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Right )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ long right);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *width);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ long width);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IITWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *height);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IITWindow __RPC_FAR * This,
            /* [in] */ long height);
        
        END_INTERFACE
    } IITWindowVtbl;

    interface IITWindow
    {
        CONST_VTBL struct IITWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITWindow_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITWindow_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITWindow_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITWindow_put_Visible(This,shouldBeVisible)	\
    (This)->lpVtbl -> put_Visible(This,shouldBeVisible)

#define IITWindow_get_Resizable(This,isResizable)	\
    (This)->lpVtbl -> get_Resizable(This,isResizable)

#define IITWindow_get_Minimized(This,isMinimized)	\
    (This)->lpVtbl -> get_Minimized(This,isMinimized)

#define IITWindow_put_Minimized(This,shouldBeMinimized)	\
    (This)->lpVtbl -> put_Minimized(This,shouldBeMinimized)

#define IITWindow_get_Maximizable(This,isMaximizable)	\
    (This)->lpVtbl -> get_Maximizable(This,isMaximizable)

#define IITWindow_get_Maximized(This,isMaximized)	\
    (This)->lpVtbl -> get_Maximized(This,isMaximized)

#define IITWindow_put_Maximized(This,shouldBeMaximized)	\
    (This)->lpVtbl -> put_Maximized(This,shouldBeMaximized)

#define IITWindow_get_Zoomable(This,isZoomable)	\
    (This)->lpVtbl -> get_Zoomable(This,isZoomable)

#define IITWindow_get_Zoomed(This,isZoomed)	\
    (This)->lpVtbl -> get_Zoomed(This,isZoomed)

#define IITWindow_put_Zoomed(This,shouldBeZoomed)	\
    (This)->lpVtbl -> put_Zoomed(This,shouldBeZoomed)

#define IITWindow_get_Top(This,top)	\
    (This)->lpVtbl -> get_Top(This,top)

#define IITWindow_put_Top(This,top)	\
    (This)->lpVtbl -> put_Top(This,top)

#define IITWindow_get_Left(This,left)	\
    (This)->lpVtbl -> get_Left(This,left)

#define IITWindow_put_Left(This,left)	\
    (This)->lpVtbl -> put_Left(This,left)

#define IITWindow_get_Bottom(This,bottom)	\
    (This)->lpVtbl -> get_Bottom(This,bottom)

#define IITWindow_put_Bottom(This,bottom)	\
    (This)->lpVtbl -> put_Bottom(This,bottom)

#define IITWindow_get_Right(This,right)	\
    (This)->lpVtbl -> get_Right(This,right)

#define IITWindow_put_Right(This,right)	\
    (This)->lpVtbl -> put_Right(This,right)

#define IITWindow_get_Width(This,width)	\
    (This)->lpVtbl -> get_Width(This,width)

#define IITWindow_put_Width(This,width)	\
    (This)->lpVtbl -> put_Width(This,width)

#define IITWindow_get_Height(This,height)	\
    (This)->lpVtbl -> get_Height(This,height)

#define IITWindow_put_Height(This,height)	\
    (This)->lpVtbl -> put_Height(This,height)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Name_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *name);


void __RPC_STUB IITWindow_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Kind_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ ITWindowKind __RPC_FAR *kind);


void __RPC_STUB IITWindow_get_Kind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Visible_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);


void __RPC_STUB IITWindow_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Visible_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeVisible);


void __RPC_STUB IITWindow_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Resizable_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isResizable);


void __RPC_STUB IITWindow_get_Resizable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Minimized_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMinimized);


void __RPC_STUB IITWindow_get_Minimized_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Minimized_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeMinimized);


void __RPC_STUB IITWindow_put_Minimized_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Maximizable_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximizable);


void __RPC_STUB IITWindow_get_Maximizable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Maximized_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximized);


void __RPC_STUB IITWindow_get_Maximized_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Maximized_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeMaximized);


void __RPC_STUB IITWindow_put_Maximized_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Zoomable_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomable);


void __RPC_STUB IITWindow_get_Zoomable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Zoomed_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomed);


void __RPC_STUB IITWindow_get_Zoomed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Zoomed_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeZoomed);


void __RPC_STUB IITWindow_put_Zoomed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Top_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *top);


void __RPC_STUB IITWindow_get_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Top_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ long top);


void __RPC_STUB IITWindow_put_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Left_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *left);


void __RPC_STUB IITWindow_get_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Left_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ long left);


void __RPC_STUB IITWindow_put_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Bottom_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *bottom);


void __RPC_STUB IITWindow_get_Bottom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Bottom_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ long bottom);


void __RPC_STUB IITWindow_put_Bottom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Right_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *right);


void __RPC_STUB IITWindow_get_Right_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Right_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ long right);


void __RPC_STUB IITWindow_put_Right_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Width_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *width);


void __RPC_STUB IITWindow_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Width_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ long width);


void __RPC_STUB IITWindow_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindow_get_Height_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *height);


void __RPC_STUB IITWindow_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITWindow_put_Height_Proxy( 
    IITWindow __RPC_FAR * This,
    /* [in] */ long height);


void __RPC_STUB IITWindow_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITWindow_INTERFACE_DEFINED__ */


#ifndef __IITBrowserWindow_INTERFACE_DEFINED__
#define __IITBrowserWindow_INTERFACE_DEFINED__

/* interface IITBrowserWindow */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITBrowserWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C999F455-C4D5-4aa4-8277-F99753699974")
    IITBrowserWindow : public IITWindow
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_MiniPlayer( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMiniPlayer) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_MiniPlayer( 
            /* [in] */ VARIANT_BOOL shouldBeMiniPlayer) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SelectedTracks( 
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SelectedPlaylist( 
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SelectedPlaylist( 
            /* [in] */ VARIANT __RPC_FAR *iPlaylist) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITBrowserWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITBrowserWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITBrowserWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ ITWindowKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Resizable )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isResizable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Minimized )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMinimized);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Minimized )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMinimized);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Maximizable )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximizable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Maximized )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximized);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Maximized )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMaximized);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoomable )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoomed )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomed);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Zoomed )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeZoomed);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Top )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *top);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Top )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ long top);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Left )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *left);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Left )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ long left);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bottom )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bottom);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bottom )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ long bottom);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Right )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *right);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Right )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ long right);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *width);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ long width);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *height);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ long height);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MiniPlayer )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMiniPlayer);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MiniPlayer )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMiniPlayer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectedTracks )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectedPlaylist )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectedPlaylist )( 
            IITBrowserWindow __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iPlaylist);
        
        END_INTERFACE
    } IITBrowserWindowVtbl;

    interface IITBrowserWindow
    {
        CONST_VTBL struct IITBrowserWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITBrowserWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITBrowserWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITBrowserWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITBrowserWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITBrowserWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITBrowserWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITBrowserWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITBrowserWindow_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITBrowserWindow_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITBrowserWindow_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITBrowserWindow_put_Visible(This,shouldBeVisible)	\
    (This)->lpVtbl -> put_Visible(This,shouldBeVisible)

#define IITBrowserWindow_get_Resizable(This,isResizable)	\
    (This)->lpVtbl -> get_Resizable(This,isResizable)

#define IITBrowserWindow_get_Minimized(This,isMinimized)	\
    (This)->lpVtbl -> get_Minimized(This,isMinimized)

#define IITBrowserWindow_put_Minimized(This,shouldBeMinimized)	\
    (This)->lpVtbl -> put_Minimized(This,shouldBeMinimized)

#define IITBrowserWindow_get_Maximizable(This,isMaximizable)	\
    (This)->lpVtbl -> get_Maximizable(This,isMaximizable)

#define IITBrowserWindow_get_Maximized(This,isMaximized)	\
    (This)->lpVtbl -> get_Maximized(This,isMaximized)

#define IITBrowserWindow_put_Maximized(This,shouldBeMaximized)	\
    (This)->lpVtbl -> put_Maximized(This,shouldBeMaximized)

#define IITBrowserWindow_get_Zoomable(This,isZoomable)	\
    (This)->lpVtbl -> get_Zoomable(This,isZoomable)

#define IITBrowserWindow_get_Zoomed(This,isZoomed)	\
    (This)->lpVtbl -> get_Zoomed(This,isZoomed)

#define IITBrowserWindow_put_Zoomed(This,shouldBeZoomed)	\
    (This)->lpVtbl -> put_Zoomed(This,shouldBeZoomed)

#define IITBrowserWindow_get_Top(This,top)	\
    (This)->lpVtbl -> get_Top(This,top)

#define IITBrowserWindow_put_Top(This,top)	\
    (This)->lpVtbl -> put_Top(This,top)

#define IITBrowserWindow_get_Left(This,left)	\
    (This)->lpVtbl -> get_Left(This,left)

#define IITBrowserWindow_put_Left(This,left)	\
    (This)->lpVtbl -> put_Left(This,left)

#define IITBrowserWindow_get_Bottom(This,bottom)	\
    (This)->lpVtbl -> get_Bottom(This,bottom)

#define IITBrowserWindow_put_Bottom(This,bottom)	\
    (This)->lpVtbl -> put_Bottom(This,bottom)

#define IITBrowserWindow_get_Right(This,right)	\
    (This)->lpVtbl -> get_Right(This,right)

#define IITBrowserWindow_put_Right(This,right)	\
    (This)->lpVtbl -> put_Right(This,right)

#define IITBrowserWindow_get_Width(This,width)	\
    (This)->lpVtbl -> get_Width(This,width)

#define IITBrowserWindow_put_Width(This,width)	\
    (This)->lpVtbl -> put_Width(This,width)

#define IITBrowserWindow_get_Height(This,height)	\
    (This)->lpVtbl -> get_Height(This,height)

#define IITBrowserWindow_put_Height(This,height)	\
    (This)->lpVtbl -> put_Height(This,height)


#define IITBrowserWindow_get_MiniPlayer(This,isMiniPlayer)	\
    (This)->lpVtbl -> get_MiniPlayer(This,isMiniPlayer)

#define IITBrowserWindow_put_MiniPlayer(This,shouldBeMiniPlayer)	\
    (This)->lpVtbl -> put_MiniPlayer(This,shouldBeMiniPlayer)

#define IITBrowserWindow_get_SelectedTracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_SelectedTracks(This,iTrackCollection)

#define IITBrowserWindow_get_SelectedPlaylist(This,iPlaylist)	\
    (This)->lpVtbl -> get_SelectedPlaylist(This,iPlaylist)

#define IITBrowserWindow_put_SelectedPlaylist(This,iPlaylist)	\
    (This)->lpVtbl -> put_SelectedPlaylist(This,iPlaylist)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITBrowserWindow_get_MiniPlayer_Proxy( 
    IITBrowserWindow __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMiniPlayer);


void __RPC_STUB IITBrowserWindow_get_MiniPlayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITBrowserWindow_put_MiniPlayer_Proxy( 
    IITBrowserWindow __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldBeMiniPlayer);


void __RPC_STUB IITBrowserWindow_put_MiniPlayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITBrowserWindow_get_SelectedTracks_Proxy( 
    IITBrowserWindow __RPC_FAR * This,
    /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);


void __RPC_STUB IITBrowserWindow_get_SelectedTracks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITBrowserWindow_get_SelectedPlaylist_Proxy( 
    IITBrowserWindow __RPC_FAR * This,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IITBrowserWindow_get_SelectedPlaylist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITBrowserWindow_put_SelectedPlaylist_Proxy( 
    IITBrowserWindow __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iPlaylist);


void __RPC_STUB IITBrowserWindow_put_SelectedPlaylist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITBrowserWindow_INTERFACE_DEFINED__ */


#ifndef __IITWindowCollection_INTERFACE_DEFINED__
#define __IITWindowCollection_INTERFACE_DEFINED__

/* interface IITWindowCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITWindowCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3D8DE381-6C0E-481f-A865-E2385F59FA43")
    IITWindowCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iWindow) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iWindow) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITWindowCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITWindowCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITWindowCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITWindowCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITWindowCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITWindowCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITWindowCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITWindowCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITWindowCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITWindowCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iWindow);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITWindowCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iWindow);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITWindowCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITWindowCollectionVtbl;

    interface IITWindowCollection
    {
        CONST_VTBL struct IITWindowCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITWindowCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITWindowCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITWindowCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITWindowCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITWindowCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITWindowCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITWindowCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITWindowCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITWindowCollection_get_Item(This,index,iWindow)	\
    (This)->lpVtbl -> get_Item(This,index,iWindow)

#define IITWindowCollection_get_ItemByName(This,name,iWindow)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iWindow)

#define IITWindowCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindowCollection_get_Count_Proxy( 
    IITWindowCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITWindowCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITWindowCollection_get_Item_Proxy( 
    IITWindowCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iWindow);


void __RPC_STUB IITWindowCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITWindowCollection_get_ItemByName_Proxy( 
    IITWindowCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iWindow);


void __RPC_STUB IITWindowCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITWindowCollection_get__NewEnum_Proxy( 
    IITWindowCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITWindowCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITWindowCollection_INTERFACE_DEFINED__ */


#ifndef __IiTunes_INTERFACE_DEFINED__
#define __IiTunes_INTERFACE_DEFINED__

/* interface IiTunes */
/* [hidden][unique][helpstring][dual][uuid][object] */ 




EXTERN_C const IID IID_IiTunes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9DD6680B-3EDC-40db-A771-E6FE4832E34A")
    IiTunes : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE BackTrack( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FastForward( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE NextTrack( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PlayFile( 
            /* [in] */ BSTR filePath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PlayPause( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PreviousTrack( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Rewind( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertFile( 
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertFiles( 
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertTrack( 
            /* [in] */ VARIANT __RPC_FAR *iTrackToConvert,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertTracks( 
            /* [in] */ VARIANT __RPC_FAR *iTracksToConvert,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CheckVersion( 
            /* [in] */ long majorVersion,
            /* [in] */ long minorVersion,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompatible) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetITObjectByID( 
            /* [in] */ long sourceID,
            /* [in] */ long playlistID,
            /* [in] */ long trackID,
            /* [in] */ long databaseID,
            /* [retval][out] */ IITObject __RPC_FAR *__RPC_FAR *iObject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreatePlaylist( 
            /* [in] */ BSTR playlistName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OpenURL( 
            /* [in] */ BSTR url) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GotoMusicStoreHomePage( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UpdateIPod( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Authorize( 
            /* [in] */ long numElems,
            /* [size_is][in] */ VARIANT __RPC_FAR data[  ],
            /* [size_is][in] */ BSTR __RPC_FAR names[  ]) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Quit( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Sources( 
            /* [retval][out] */ IITSourceCollection __RPC_FAR *__RPC_FAR *iSourceCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Encoders( 
            /* [retval][out] */ IITEncoderCollection __RPC_FAR *__RPC_FAR *iEncoderCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EQPresets( 
            /* [retval][out] */ IITEQPresetCollection __RPC_FAR *__RPC_FAR *iEQPresetCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Visuals( 
            /* [retval][out] */ IITVisualCollection __RPC_FAR *__RPC_FAR *iVisualCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Windows( 
            /* [retval][out] */ IITWindowCollection __RPC_FAR *__RPC_FAR *iWindowCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SoundVolume( 
            /* [retval][out] */ long __RPC_FAR *volume) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_SoundVolume( 
            /* [in] */ long volume) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Mute( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMuted) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Mute( 
            /* [in] */ VARIANT_BOOL shouldMute) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlayerState( 
            /* [retval][out] */ ITPlayerState __RPC_FAR *playerState) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PlayerPosition( 
            /* [retval][out] */ long __RPC_FAR *playerPos) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_PlayerPosition( 
            /* [in] */ long playerPos) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentEncoder( 
            /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentEncoder( 
            /* [in] */ IITEncoder __RPC_FAR *iEncoder) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VisualsEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_VisualsEnabled( 
            /* [in] */ VARIANT_BOOL shouldEnable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FullScreenVisuals( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isFullScreen) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FullScreenVisuals( 
            /* [in] */ VARIANT_BOOL shouldUseFullScreen) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VisualSize( 
            /* [retval][out] */ ITVisualSize __RPC_FAR *visualSize) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_VisualSize( 
            /* [in] */ ITVisualSize visualSize) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentVisual( 
            /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentVisual( 
            /* [in] */ IITVisual __RPC_FAR *iVisual) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EQEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_EQEnabled( 
            /* [in] */ VARIANT_BOOL shouldEnable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentEQPreset( 
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentEQPreset( 
            /* [in] */ IITEQPreset __RPC_FAR *iEQPreset) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentStreamTitle( 
            /* [retval][out] */ BSTR __RPC_FAR *streamTitle) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentStreamURL( 
            /* [retval][out] */ BSTR __RPC_FAR *streamURL) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BrowserWindow( 
            /* [retval][out] */ IITBrowserWindow __RPC_FAR *__RPC_FAR *iBrowserWindow) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EQWindow( 
            /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iEQWindow) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LibrarySource( 
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iLibrarySource) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LibraryPlaylist( 
            /* [retval][out] */ IITLibraryPlaylist __RPC_FAR *__RPC_FAR *iLibraryPlaylist) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentTrack( 
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentPlaylist( 
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SelectedTracks( 
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR __RPC_FAR *version) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetOptions( 
            /* [in] */ long options) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertFile2( 
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertFiles2( 
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertTrack2( 
            /* [in] */ VARIANT __RPC_FAR *iTrackToConvert,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ConvertTracks2( 
            /* [in] */ VARIANT __RPC_FAR *iTracksToConvert,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AppCommandMessageProcessingEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AppCommandMessageProcessingEnabled( 
            /* [in] */ VARIANT_BOOL shouldEnable) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ForceToForegroundOnDialog( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *forceToForegroundOnDialog) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ForceToForegroundOnDialog( 
            /* [in] */ VARIANT_BOOL forceToForegroundOnDialog) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateEQPreset( 
            /* [in] */ BSTR eqPresetName,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreatePlaylistInSource( 
            /* [in] */ BSTR playlistName,
            /* [in] */ VARIANT __RPC_FAR *iSource,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPlayerButtonsState( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *previousEnabled,
            /* [out] */ ITPlayButtonState __RPC_FAR *playPauseStopState,
            /* [out] */ VARIANT_BOOL __RPC_FAR *nextEnabled) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PlayerButtonClicked( 
            /* [in] */ ITPlayerButton playerButton,
            /* [in] */ long playerButtonModifierKeys) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CanSetShuffle( 
            /* [in] */ VARIANT __RPC_FAR *iPlaylist,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *canSetShuffle) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CanSetSongRepeat( 
            /* [in] */ VARIANT __RPC_FAR *iPlaylist,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *canSetSongRepeat) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ConvertOperationStatus( 
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SubscribeToPodcast( 
            /* [in] */ BSTR url) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UpdatePodcastFeeds( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateFolder( 
            /* [in] */ BSTR folderName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateFolderInSource( 
            /* [in] */ BSTR folderName,
            /* [in] */ VARIANT __RPC_FAR *iSource,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SoundVolumeControlEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IiTunesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IiTunes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IiTunes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IiTunes __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BackTrack )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FastForward )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NextTrack )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayFile )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR filePath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayPause )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PreviousTrack )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Rewind )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertFile )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertFiles )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertTrack )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iTrackToConvert,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertTracks )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iTracksToConvert,
            /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CheckVersion )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ long majorVersion,
            /* [in] */ long minorVersion,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompatible);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectByID )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ long sourceID,
            /* [in] */ long playlistID,
            /* [in] */ long trackID,
            /* [in] */ long databaseID,
            /* [retval][out] */ IITObject __RPC_FAR *__RPC_FAR *iObject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePlaylist )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR playlistName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenURL )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR url);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GotoMusicStoreHomePage )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateIPod )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Authorize )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ long numElems,
            /* [size_is][in] */ VARIANT __RPC_FAR data[  ],
            /* [size_is][in] */ BSTR __RPC_FAR names[  ]);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Quit )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sources )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITSourceCollection __RPC_FAR *__RPC_FAR *iSourceCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Encoders )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITEncoderCollection __RPC_FAR *__RPC_FAR *iEncoderCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EQPresets )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITEQPresetCollection __RPC_FAR *__RPC_FAR *iEQPresetCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visuals )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITVisualCollection __RPC_FAR *__RPC_FAR *iVisualCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Windows )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITWindowCollection __RPC_FAR *__RPC_FAR *iWindowCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SoundVolume )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *volume);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SoundVolume )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ long volume);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Mute )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMuted);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Mute )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldMute);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayerState )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ ITPlayerState __RPC_FAR *playerState);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayerPosition )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playerPos);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayerPosition )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ long playerPos);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentEncoder )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentEncoder )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ IITEncoder __RPC_FAR *iEncoder);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VisualsEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VisualsEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldEnable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullScreenVisuals )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isFullScreen);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FullScreenVisuals )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldUseFullScreen);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VisualSize )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ ITVisualSize __RPC_FAR *visualSize);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VisualSize )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ ITVisualSize visualSize);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentVisual )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentVisual )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ IITVisual __RPC_FAR *iVisual);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EQEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EQEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldEnable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentEQPreset )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentEQPreset )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ IITEQPreset __RPC_FAR *iEQPreset);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentStreamTitle )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *streamTitle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentStreamURL )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *streamURL);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BrowserWindow )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITBrowserWindow __RPC_FAR *__RPC_FAR *iBrowserWindow);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EQWindow )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iEQWindow);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LibrarySource )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iLibrarySource);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LibraryPlaylist )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITLibraryPlaylist __RPC_FAR *__RPC_FAR *iLibraryPlaylist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentTrack )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPlaylist )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectedTracks )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Version )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *version);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetOptions )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ long options);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertFile2 )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertFiles2 )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *filePaths,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertTrack2 )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iTrackToConvert,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertTracks2 )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iTracksToConvert,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AppCommandMessageProcessingEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AppCommandMessageProcessingEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldEnable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ForceToForegroundOnDialog )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *forceToForegroundOnDialog);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ForceToForegroundOnDialog )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL forceToForegroundOnDialog);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateEQPreset )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR eqPresetName,
            /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePlaylistInSource )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR playlistName,
            /* [in] */ VARIANT __RPC_FAR *iSource,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPlayerButtonsState )( 
            IiTunes __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *previousEnabled,
            /* [out] */ ITPlayButtonState __RPC_FAR *playPauseStopState,
            /* [out] */ VARIANT_BOOL __RPC_FAR *nextEnabled);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayerButtonClicked )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ ITPlayerButton playerButton,
            /* [in] */ long playerButtonModifierKeys);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSetShuffle )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iPlaylist,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *canSetShuffle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CanSetSongRepeat )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *iPlaylist,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *canSetSongRepeat);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConvertOperationStatus )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SubscribeToPodcast )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR url);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdatePodcastFeeds )( 
            IiTunes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateFolder )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR folderName,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateFolderInSource )( 
            IiTunes __RPC_FAR * This,
            /* [in] */ BSTR folderName,
            /* [in] */ VARIANT __RPC_FAR *iSource,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SoundVolumeControlEnabled )( 
            IiTunes __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        END_INTERFACE
    } IiTunesVtbl;

    interface IiTunes
    {
        CONST_VTBL struct IiTunesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IiTunes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IiTunes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IiTunes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IiTunes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IiTunes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IiTunes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IiTunes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IiTunes_BackTrack(This)	\
    (This)->lpVtbl -> BackTrack(This)

#define IiTunes_FastForward(This)	\
    (This)->lpVtbl -> FastForward(This)

#define IiTunes_NextTrack(This)	\
    (This)->lpVtbl -> NextTrack(This)

#define IiTunes_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IiTunes_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define IiTunes_PlayFile(This,filePath)	\
    (This)->lpVtbl -> PlayFile(This,filePath)

#define IiTunes_PlayPause(This)	\
    (This)->lpVtbl -> PlayPause(This)

#define IiTunes_PreviousTrack(This)	\
    (This)->lpVtbl -> PreviousTrack(This)

#define IiTunes_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IiTunes_Rewind(This)	\
    (This)->lpVtbl -> Rewind(This)

#define IiTunes_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IiTunes_ConvertFile(This,filePath,iStatus)	\
    (This)->lpVtbl -> ConvertFile(This,filePath,iStatus)

#define IiTunes_ConvertFiles(This,filePaths,iStatus)	\
    (This)->lpVtbl -> ConvertFiles(This,filePaths,iStatus)

#define IiTunes_ConvertTrack(This,iTrackToConvert,iStatus)	\
    (This)->lpVtbl -> ConvertTrack(This,iTrackToConvert,iStatus)

#define IiTunes_ConvertTracks(This,iTracksToConvert,iStatus)	\
    (This)->lpVtbl -> ConvertTracks(This,iTracksToConvert,iStatus)

#define IiTunes_CheckVersion(This,majorVersion,minorVersion,isCompatible)	\
    (This)->lpVtbl -> CheckVersion(This,majorVersion,minorVersion,isCompatible)

#define IiTunes_GetITObjectByID(This,sourceID,playlistID,trackID,databaseID,iObject)	\
    (This)->lpVtbl -> GetITObjectByID(This,sourceID,playlistID,trackID,databaseID,iObject)

#define IiTunes_CreatePlaylist(This,playlistName,iPlaylist)	\
    (This)->lpVtbl -> CreatePlaylist(This,playlistName,iPlaylist)

#define IiTunes_OpenURL(This,url)	\
    (This)->lpVtbl -> OpenURL(This,url)

#define IiTunes_GotoMusicStoreHomePage(This)	\
    (This)->lpVtbl -> GotoMusicStoreHomePage(This)

#define IiTunes_UpdateIPod(This)	\
    (This)->lpVtbl -> UpdateIPod(This)

#define IiTunes_Authorize(This,numElems,data,names)	\
    (This)->lpVtbl -> Authorize(This,numElems,data,names)

#define IiTunes_Quit(This)	\
    (This)->lpVtbl -> Quit(This)

#define IiTunes_get_Sources(This,iSourceCollection)	\
    (This)->lpVtbl -> get_Sources(This,iSourceCollection)

#define IiTunes_get_Encoders(This,iEncoderCollection)	\
    (This)->lpVtbl -> get_Encoders(This,iEncoderCollection)

#define IiTunes_get_EQPresets(This,iEQPresetCollection)	\
    (This)->lpVtbl -> get_EQPresets(This,iEQPresetCollection)

#define IiTunes_get_Visuals(This,iVisualCollection)	\
    (This)->lpVtbl -> get_Visuals(This,iVisualCollection)

#define IiTunes_get_Windows(This,iWindowCollection)	\
    (This)->lpVtbl -> get_Windows(This,iWindowCollection)

#define IiTunes_get_SoundVolume(This,volume)	\
    (This)->lpVtbl -> get_SoundVolume(This,volume)

#define IiTunes_put_SoundVolume(This,volume)	\
    (This)->lpVtbl -> put_SoundVolume(This,volume)

#define IiTunes_get_Mute(This,isMuted)	\
    (This)->lpVtbl -> get_Mute(This,isMuted)

#define IiTunes_put_Mute(This,shouldMute)	\
    (This)->lpVtbl -> put_Mute(This,shouldMute)

#define IiTunes_get_PlayerState(This,playerState)	\
    (This)->lpVtbl -> get_PlayerState(This,playerState)

#define IiTunes_get_PlayerPosition(This,playerPos)	\
    (This)->lpVtbl -> get_PlayerPosition(This,playerPos)

#define IiTunes_put_PlayerPosition(This,playerPos)	\
    (This)->lpVtbl -> put_PlayerPosition(This,playerPos)

#define IiTunes_get_CurrentEncoder(This,iEncoder)	\
    (This)->lpVtbl -> get_CurrentEncoder(This,iEncoder)

#define IiTunes_put_CurrentEncoder(This,iEncoder)	\
    (This)->lpVtbl -> put_CurrentEncoder(This,iEncoder)

#define IiTunes_get_VisualsEnabled(This,isEnabled)	\
    (This)->lpVtbl -> get_VisualsEnabled(This,isEnabled)

#define IiTunes_put_VisualsEnabled(This,shouldEnable)	\
    (This)->lpVtbl -> put_VisualsEnabled(This,shouldEnable)

#define IiTunes_get_FullScreenVisuals(This,isFullScreen)	\
    (This)->lpVtbl -> get_FullScreenVisuals(This,isFullScreen)

#define IiTunes_put_FullScreenVisuals(This,shouldUseFullScreen)	\
    (This)->lpVtbl -> put_FullScreenVisuals(This,shouldUseFullScreen)

#define IiTunes_get_VisualSize(This,visualSize)	\
    (This)->lpVtbl -> get_VisualSize(This,visualSize)

#define IiTunes_put_VisualSize(This,visualSize)	\
    (This)->lpVtbl -> put_VisualSize(This,visualSize)

#define IiTunes_get_CurrentVisual(This,iVisual)	\
    (This)->lpVtbl -> get_CurrentVisual(This,iVisual)

#define IiTunes_put_CurrentVisual(This,iVisual)	\
    (This)->lpVtbl -> put_CurrentVisual(This,iVisual)

#define IiTunes_get_EQEnabled(This,isEnabled)	\
    (This)->lpVtbl -> get_EQEnabled(This,isEnabled)

#define IiTunes_put_EQEnabled(This,shouldEnable)	\
    (This)->lpVtbl -> put_EQEnabled(This,shouldEnable)

#define IiTunes_get_CurrentEQPreset(This,iEQPreset)	\
    (This)->lpVtbl -> get_CurrentEQPreset(This,iEQPreset)

#define IiTunes_put_CurrentEQPreset(This,iEQPreset)	\
    (This)->lpVtbl -> put_CurrentEQPreset(This,iEQPreset)

#define IiTunes_get_CurrentStreamTitle(This,streamTitle)	\
    (This)->lpVtbl -> get_CurrentStreamTitle(This,streamTitle)

#define IiTunes_get_CurrentStreamURL(This,streamURL)	\
    (This)->lpVtbl -> get_CurrentStreamURL(This,streamURL)

#define IiTunes_get_BrowserWindow(This,iBrowserWindow)	\
    (This)->lpVtbl -> get_BrowserWindow(This,iBrowserWindow)

#define IiTunes_get_EQWindow(This,iEQWindow)	\
    (This)->lpVtbl -> get_EQWindow(This,iEQWindow)

#define IiTunes_get_LibrarySource(This,iLibrarySource)	\
    (This)->lpVtbl -> get_LibrarySource(This,iLibrarySource)

#define IiTunes_get_LibraryPlaylist(This,iLibraryPlaylist)	\
    (This)->lpVtbl -> get_LibraryPlaylist(This,iLibraryPlaylist)

#define IiTunes_get_CurrentTrack(This,iTrack)	\
    (This)->lpVtbl -> get_CurrentTrack(This,iTrack)

#define IiTunes_get_CurrentPlaylist(This,iPlaylist)	\
    (This)->lpVtbl -> get_CurrentPlaylist(This,iPlaylist)

#define IiTunes_get_SelectedTracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_SelectedTracks(This,iTrackCollection)

#define IiTunes_get_Version(This,version)	\
    (This)->lpVtbl -> get_Version(This,version)

#define IiTunes_SetOptions(This,options)	\
    (This)->lpVtbl -> SetOptions(This,options)

#define IiTunes_ConvertFile2(This,filePath,iStatus)	\
    (This)->lpVtbl -> ConvertFile2(This,filePath,iStatus)

#define IiTunes_ConvertFiles2(This,filePaths,iStatus)	\
    (This)->lpVtbl -> ConvertFiles2(This,filePaths,iStatus)

#define IiTunes_ConvertTrack2(This,iTrackToConvert,iStatus)	\
    (This)->lpVtbl -> ConvertTrack2(This,iTrackToConvert,iStatus)

#define IiTunes_ConvertTracks2(This,iTracksToConvert,iStatus)	\
    (This)->lpVtbl -> ConvertTracks2(This,iTracksToConvert,iStatus)

#define IiTunes_get_AppCommandMessageProcessingEnabled(This,isEnabled)	\
    (This)->lpVtbl -> get_AppCommandMessageProcessingEnabled(This,isEnabled)

#define IiTunes_put_AppCommandMessageProcessingEnabled(This,shouldEnable)	\
    (This)->lpVtbl -> put_AppCommandMessageProcessingEnabled(This,shouldEnable)

#define IiTunes_get_ForceToForegroundOnDialog(This,forceToForegroundOnDialog)	\
    (This)->lpVtbl -> get_ForceToForegroundOnDialog(This,forceToForegroundOnDialog)

#define IiTunes_put_ForceToForegroundOnDialog(This,forceToForegroundOnDialog)	\
    (This)->lpVtbl -> put_ForceToForegroundOnDialog(This,forceToForegroundOnDialog)

#define IiTunes_CreateEQPreset(This,eqPresetName,iEQPreset)	\
    (This)->lpVtbl -> CreateEQPreset(This,eqPresetName,iEQPreset)

#define IiTunes_CreatePlaylistInSource(This,playlistName,iSource,iPlaylist)	\
    (This)->lpVtbl -> CreatePlaylistInSource(This,playlistName,iSource,iPlaylist)

#define IiTunes_GetPlayerButtonsState(This,previousEnabled,playPauseStopState,nextEnabled)	\
    (This)->lpVtbl -> GetPlayerButtonsState(This,previousEnabled,playPauseStopState,nextEnabled)

#define IiTunes_PlayerButtonClicked(This,playerButton,playerButtonModifierKeys)	\
    (This)->lpVtbl -> PlayerButtonClicked(This,playerButton,playerButtonModifierKeys)

#define IiTunes_get_CanSetShuffle(This,iPlaylist,canSetShuffle)	\
    (This)->lpVtbl -> get_CanSetShuffle(This,iPlaylist,canSetShuffle)

#define IiTunes_get_CanSetSongRepeat(This,iPlaylist,canSetSongRepeat)	\
    (This)->lpVtbl -> get_CanSetSongRepeat(This,iPlaylist,canSetSongRepeat)

#define IiTunes_get_ConvertOperationStatus(This,iStatus)	\
    (This)->lpVtbl -> get_ConvertOperationStatus(This,iStatus)

#define IiTunes_SubscribeToPodcast(This,url)	\
    (This)->lpVtbl -> SubscribeToPodcast(This,url)

#define IiTunes_UpdatePodcastFeeds(This)	\
    (This)->lpVtbl -> UpdatePodcastFeeds(This)

#define IiTunes_CreateFolder(This,folderName,iFolder)	\
    (This)->lpVtbl -> CreateFolder(This,folderName,iFolder)

#define IiTunes_CreateFolderInSource(This,folderName,iSource,iFolder)	\
    (This)->lpVtbl -> CreateFolderInSource(This,folderName,iSource,iFolder)

#define IiTunes_get_SoundVolumeControlEnabled(This,isEnabled)	\
    (This)->lpVtbl -> get_SoundVolumeControlEnabled(This,isEnabled)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_BackTrack_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_BackTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_FastForward_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_FastForward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_NextTrack_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_NextTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Pause_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Play_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_Play_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_PlayFile_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR filePath);


void __RPC_STUB IiTunes_PlayFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_PlayPause_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_PlayPause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_PreviousTrack_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_PreviousTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Resume_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Rewind_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_Rewind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Stop_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertFile_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR filePath,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertFiles_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *filePaths,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertTrack_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iTrackToConvert,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertTracks_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iTracksToConvert,
    /* [retval][out] */ IITOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertTracks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_CheckVersion_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ long majorVersion,
    /* [in] */ long minorVersion,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompatible);


void __RPC_STUB IiTunes_CheckVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_GetITObjectByID_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ long sourceID,
    /* [in] */ long playlistID,
    /* [in] */ long trackID,
    /* [in] */ long databaseID,
    /* [retval][out] */ IITObject __RPC_FAR *__RPC_FAR *iObject);


void __RPC_STUB IiTunes_GetITObjectByID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_CreatePlaylist_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR playlistName,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IiTunes_CreatePlaylist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_OpenURL_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR url);


void __RPC_STUB IiTunes_OpenURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_GotoMusicStoreHomePage_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_GotoMusicStoreHomePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_UpdateIPod_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_UpdateIPod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Authorize_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ long numElems,
    /* [size_is][in] */ VARIANT __RPC_FAR data[  ],
    /* [size_is][in] */ BSTR __RPC_FAR names[  ]);


void __RPC_STUB IiTunes_Authorize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_Quit_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_Quit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_Sources_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITSourceCollection __RPC_FAR *__RPC_FAR *iSourceCollection);


void __RPC_STUB IiTunes_get_Sources_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_Encoders_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITEncoderCollection __RPC_FAR *__RPC_FAR *iEncoderCollection);


void __RPC_STUB IiTunes_get_Encoders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_EQPresets_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITEQPresetCollection __RPC_FAR *__RPC_FAR *iEQPresetCollection);


void __RPC_STUB IiTunes_get_EQPresets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_Visuals_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITVisualCollection __RPC_FAR *__RPC_FAR *iVisualCollection);


void __RPC_STUB IiTunes_get_Visuals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_Windows_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITWindowCollection __RPC_FAR *__RPC_FAR *iWindowCollection);


void __RPC_STUB IiTunes_get_Windows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_SoundVolume_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *volume);


void __RPC_STUB IiTunes_get_SoundVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_SoundVolume_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ long volume);


void __RPC_STUB IiTunes_put_SoundVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_Mute_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMuted);


void __RPC_STUB IiTunes_get_Mute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_Mute_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldMute);


void __RPC_STUB IiTunes_put_Mute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_PlayerState_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ ITPlayerState __RPC_FAR *playerState);


void __RPC_STUB IiTunes_get_PlayerState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_PlayerPosition_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *playerPos);


void __RPC_STUB IiTunes_get_PlayerPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_PlayerPosition_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ long playerPos);


void __RPC_STUB IiTunes_put_PlayerPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentEncoder_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITEncoder __RPC_FAR *__RPC_FAR *iEncoder);


void __RPC_STUB IiTunes_get_CurrentEncoder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_CurrentEncoder_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ IITEncoder __RPC_FAR *iEncoder);


void __RPC_STUB IiTunes_put_CurrentEncoder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_VisualsEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);


void __RPC_STUB IiTunes_get_VisualsEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_VisualsEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldEnable);


void __RPC_STUB IiTunes_put_VisualsEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_FullScreenVisuals_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isFullScreen);


void __RPC_STUB IiTunes_get_FullScreenVisuals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_FullScreenVisuals_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldUseFullScreen);


void __RPC_STUB IiTunes_put_FullScreenVisuals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_VisualSize_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ ITVisualSize __RPC_FAR *visualSize);


void __RPC_STUB IiTunes_get_VisualSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_VisualSize_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ ITVisualSize visualSize);


void __RPC_STUB IiTunes_put_VisualSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentVisual_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITVisual __RPC_FAR *__RPC_FAR *iVisual);


void __RPC_STUB IiTunes_get_CurrentVisual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_CurrentVisual_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ IITVisual __RPC_FAR *iVisual);


void __RPC_STUB IiTunes_put_CurrentVisual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_EQEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);


void __RPC_STUB IiTunes_get_EQEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_EQEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldEnable);


void __RPC_STUB IiTunes_put_EQEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentEQPreset_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);


void __RPC_STUB IiTunes_get_CurrentEQPreset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_CurrentEQPreset_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ IITEQPreset __RPC_FAR *iEQPreset);


void __RPC_STUB IiTunes_put_CurrentEQPreset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentStreamTitle_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *streamTitle);


void __RPC_STUB IiTunes_get_CurrentStreamTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentStreamURL_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *streamURL);


void __RPC_STUB IiTunes_get_CurrentStreamURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_BrowserWindow_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITBrowserWindow __RPC_FAR *__RPC_FAR *iBrowserWindow);


void __RPC_STUB IiTunes_get_BrowserWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_EQWindow_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITWindow __RPC_FAR *__RPC_FAR *iEQWindow);


void __RPC_STUB IiTunes_get_EQWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_LibrarySource_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iLibrarySource);


void __RPC_STUB IiTunes_get_LibrarySource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_LibraryPlaylist_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITLibraryPlaylist __RPC_FAR *__RPC_FAR *iLibraryPlaylist);


void __RPC_STUB IiTunes_get_LibraryPlaylist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentTrack_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITTrack __RPC_FAR *__RPC_FAR *iTrack);


void __RPC_STUB IiTunes_get_CurrentTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CurrentPlaylist_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IiTunes_get_CurrentPlaylist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_SelectedTracks_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);


void __RPC_STUB IiTunes_get_SelectedTracks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_Version_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *version);


void __RPC_STUB IiTunes_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_SetOptions_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ long options);


void __RPC_STUB IiTunes_SetOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertFile2_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR filePath,
    /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertFile2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertFiles2_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *filePaths,
    /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertFiles2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertTrack2_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iTrackToConvert,
    /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertTrack2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_ConvertTracks2_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iTracksToConvert,
    /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_ConvertTracks2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_AppCommandMessageProcessingEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);


void __RPC_STUB IiTunes_get_AppCommandMessageProcessingEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_AppCommandMessageProcessingEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldEnable);


void __RPC_STUB IiTunes_put_AppCommandMessageProcessingEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_ForceToForegroundOnDialog_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *forceToForegroundOnDialog);


void __RPC_STUB IiTunes_get_ForceToForegroundOnDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IiTunes_put_ForceToForegroundOnDialog_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL forceToForegroundOnDialog);


void __RPC_STUB IiTunes_put_ForceToForegroundOnDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_CreateEQPreset_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR eqPresetName,
    /* [retval][out] */ IITEQPreset __RPC_FAR *__RPC_FAR *iEQPreset);


void __RPC_STUB IiTunes_CreateEQPreset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_CreatePlaylistInSource_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR playlistName,
    /* [in] */ VARIANT __RPC_FAR *iSource,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IiTunes_CreatePlaylistInSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_GetPlayerButtonsState_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *previousEnabled,
    /* [out] */ ITPlayButtonState __RPC_FAR *playPauseStopState,
    /* [out] */ VARIANT_BOOL __RPC_FAR *nextEnabled);


void __RPC_STUB IiTunes_GetPlayerButtonsState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_PlayerButtonClicked_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ ITPlayerButton playerButton,
    /* [in] */ long playerButtonModifierKeys);


void __RPC_STUB IiTunes_PlayerButtonClicked_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CanSetShuffle_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iPlaylist,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *canSetShuffle);


void __RPC_STUB IiTunes_get_CanSetShuffle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_CanSetSongRepeat_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *iPlaylist,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *canSetSongRepeat);


void __RPC_STUB IiTunes_get_CanSetSongRepeat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_ConvertOperationStatus_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ IITConvertOperationStatus __RPC_FAR *__RPC_FAR *iStatus);


void __RPC_STUB IiTunes_get_ConvertOperationStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_SubscribeToPodcast_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR url);


void __RPC_STUB IiTunes_SubscribeToPodcast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_UpdatePodcastFeeds_Proxy( 
    IiTunes __RPC_FAR * This);


void __RPC_STUB IiTunes_UpdatePodcastFeeds_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_CreateFolder_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR folderName,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder);


void __RPC_STUB IiTunes_CreateFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IiTunes_CreateFolderInSource_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [in] */ BSTR folderName,
    /* [in] */ VARIANT __RPC_FAR *iSource,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iFolder);


void __RPC_STUB IiTunes_CreateFolderInSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IiTunes_get_SoundVolumeControlEnabled_Proxy( 
    IiTunes __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);


void __RPC_STUB IiTunes_get_SoundVolumeControlEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IiTunes_INTERFACE_DEFINED__ */


#ifndef ___IiTunesEvents_DISPINTERFACE_DEFINED__
#define ___IiTunesEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IiTunesEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IiTunesEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5846EB78-317E-4b6f-B0C3-11EE8C8FEEF2")
    _IiTunesEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IiTunesEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IiTunesEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IiTunesEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IiTunesEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IiTunesEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IiTunesEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IiTunesEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IiTunesEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IiTunesEventsVtbl;

    interface _IiTunesEvents
    {
        CONST_VTBL struct _IiTunesEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IiTunesEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IiTunesEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IiTunesEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IiTunesEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IiTunesEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IiTunesEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IiTunesEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IiTunesEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___IITConvertOperationStatusEvents_DISPINTERFACE_DEFINED__
#define ___IITConvertOperationStatusEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IITConvertOperationStatusEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IITConvertOperationStatusEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5C47A705-8E8A-45a1-9EED-71C993F0BF60")
    _IITConvertOperationStatusEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IITConvertOperationStatusEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IITConvertOperationStatusEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IITConvertOperationStatusEventsVtbl;

    interface _IITConvertOperationStatusEvents
    {
        CONST_VTBL struct _IITConvertOperationStatusEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IITConvertOperationStatusEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IITConvertOperationStatusEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IITConvertOperationStatusEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IITConvertOperationStatusEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IITConvertOperationStatusEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IITConvertOperationStatusEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IITConvertOperationStatusEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IITConvertOperationStatusEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_iTunesApp;

#ifdef __cplusplus

class DECLSPEC_UUID("DC0C2640-1415-4644-875C-6F4D769839BA")
iTunesApp;
#endif

EXTERN_C const CLSID CLSID_iTunesConvertOperationStatus;

#ifdef __cplusplus

class DECLSPEC_UUID("D06596AD-C900-41b2-BC68-1B486450FC56")
iTunesConvertOperationStatus;
#endif

#ifndef __IITArtwork_INTERFACE_DEFINED__
#define __IITArtwork_INTERFACE_DEFINED__

/* interface IITArtwork */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITArtwork;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D0A6C1F8-BF3D-4cd8-AC47-FE32BDD17257")
    IITArtwork : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetArtworkFromFile( 
            /* [in] */ BSTR filePath) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveArtworkToFile( 
            /* [in] */ BSTR filePath) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Format( 
            /* [retval][out] */ ITArtworkFormat __RPC_FAR *format) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITArtworkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITArtwork __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITArtwork __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITArtwork __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITArtwork __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITArtwork __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITArtwork __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITArtwork __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITArtwork __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetArtworkFromFile )( 
            IITArtwork __RPC_FAR * This,
            /* [in] */ BSTR filePath);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveArtworkToFile )( 
            IITArtwork __RPC_FAR * This,
            /* [in] */ BSTR filePath);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Format )( 
            IITArtwork __RPC_FAR * This,
            /* [retval][out] */ ITArtworkFormat __RPC_FAR *format);
        
        END_INTERFACE
    } IITArtworkVtbl;

    interface IITArtwork
    {
        CONST_VTBL struct IITArtworkVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITArtwork_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITArtwork_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITArtwork_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITArtwork_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITArtwork_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITArtwork_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITArtwork_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITArtwork_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITArtwork_SetArtworkFromFile(This,filePath)	\
    (This)->lpVtbl -> SetArtworkFromFile(This,filePath)

#define IITArtwork_SaveArtworkToFile(This,filePath)	\
    (This)->lpVtbl -> SaveArtworkToFile(This,filePath)

#define IITArtwork_get_Format(This,format)	\
    (This)->lpVtbl -> get_Format(This,format)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITArtwork_Delete_Proxy( 
    IITArtwork __RPC_FAR * This);


void __RPC_STUB IITArtwork_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITArtwork_SetArtworkFromFile_Proxy( 
    IITArtwork __RPC_FAR * This,
    /* [in] */ BSTR filePath);


void __RPC_STUB IITArtwork_SetArtworkFromFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITArtwork_SaveArtworkToFile_Proxy( 
    IITArtwork __RPC_FAR * This,
    /* [in] */ BSTR filePath);


void __RPC_STUB IITArtwork_SaveArtworkToFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITArtwork_get_Format_Proxy( 
    IITArtwork __RPC_FAR * This,
    /* [retval][out] */ ITArtworkFormat __RPC_FAR *format);


void __RPC_STUB IITArtwork_get_Format_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITArtwork_INTERFACE_DEFINED__ */


#ifndef __IITArtworkCollection_INTERFACE_DEFINED__
#define __IITArtworkCollection_INTERFACE_DEFINED__

/* interface IITArtworkCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITArtworkCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BF2742D7-418C-4858-9AF9-2981B062D23E")
    IITArtworkCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITArtworkCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITArtworkCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITArtworkCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITArtworkCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITArtworkCollectionVtbl;

    interface IITArtworkCollection
    {
        CONST_VTBL struct IITArtworkCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITArtworkCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITArtworkCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITArtworkCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITArtworkCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITArtworkCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITArtworkCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITArtworkCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITArtworkCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITArtworkCollection_get_Item(This,index,iArtwork)	\
    (This)->lpVtbl -> get_Item(This,index,iArtwork)

#define IITArtworkCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITArtworkCollection_get_Count_Proxy( 
    IITArtworkCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITArtworkCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITArtworkCollection_get_Item_Proxy( 
    IITArtworkCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork);


void __RPC_STUB IITArtworkCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITArtworkCollection_get__NewEnum_Proxy( 
    IITArtworkCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITArtworkCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITArtworkCollection_INTERFACE_DEFINED__ */


#ifndef __IITURLTrack_INTERFACE_DEFINED__
#define __IITURLTrack_INTERFACE_DEFINED__

/* interface IITURLTrack */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITURLTrack;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1116E3B5-29FD-4393-A7BD-454E5E327900")
    IITURLTrack : public IITTrack
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [retval][out] */ BSTR __RPC_FAR *url) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_URL( 
            /* [in] */ BSTR url) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Podcast( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isPodcast) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UpdatePodcastFeed( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DownloadPodcastEpisode( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Category( 
            /* [retval][out] */ BSTR __RPC_FAR *category) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Category( 
            /* [in] */ BSTR category) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *description) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR description) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *longDescription) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongDescription( 
            /* [in] */ BSTR longDescription) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITURLTrackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITURLTrack __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITURLTrack __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITURLTrack __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITURLTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            IITURLTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddArtworkFromFile )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ ITTrackKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Playlist )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Album )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *album);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Album )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR album);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artist )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *artist);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Artist )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR artist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bitrate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BPM )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *beatsPerMinute);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BPM )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long beatsPerMinute);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Comment )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *comment);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Comment )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR comment);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Compilation )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompilation);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Compilation )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeCompilation);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Composer )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *composer);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Composer )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR composer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateAdded )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *dateAdded);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DiscCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long discCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscNumber )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discNumber);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DiscNumber )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long discNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeEnabled);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EQ )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *eq);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EQ )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR eq);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Finish )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long finish);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Finish )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *finish);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Genre )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *genre);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Genre )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR genre);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Grouping )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *grouping);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Grouping )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR grouping);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KindAsString )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModificationDate )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *dateModified);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayedCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playedCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayedCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long playedCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayedDate )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *playedDate);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayedDate )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ DATE playedDate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayOrderIndex )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *rating);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rating )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long rating);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SampleRate )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sampleRate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Start )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *start);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Start )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long start);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TrackCount )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long trackCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackNumber )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackNumber);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TrackNumber )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long trackNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VolumeAdjustment )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *volumeAdjustment);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VolumeAdjustment )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long volumeAdjustment);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Year )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *year);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Year )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ long year);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artwork )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ IITArtworkCollection __RPC_FAR *__RPC_FAR *iArtworkCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_URL )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *url);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_URL )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR url);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Podcast )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isPodcast);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdatePodcastFeed )( 
            IITURLTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DownloadPodcastEpisode )( 
            IITURLTrack __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Category )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *category);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Category )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR category);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *description);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR description);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LongDescription )( 
            IITURLTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *longDescription);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LongDescription )( 
            IITURLTrack __RPC_FAR * This,
            /* [in] */ BSTR longDescription);
        
        END_INTERFACE
    } IITURLTrackVtbl;

    interface IITURLTrack
    {
        CONST_VTBL struct IITURLTrackVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITURLTrack_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITURLTrack_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITURLTrack_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITURLTrack_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITURLTrack_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITURLTrack_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITURLTrack_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITURLTrack_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITURLTrack_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITURLTrack_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITURLTrack_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITURLTrack_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITURLTrack_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITURLTrack_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITURLTrack_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITURLTrack_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITURLTrack_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define IITURLTrack_AddArtworkFromFile(This,filePath,iArtwork)	\
    (This)->lpVtbl -> AddArtworkFromFile(This,filePath,iArtwork)

#define IITURLTrack_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITURLTrack_get_Playlist(This,iPlaylist)	\
    (This)->lpVtbl -> get_Playlist(This,iPlaylist)

#define IITURLTrack_get_Album(This,album)	\
    (This)->lpVtbl -> get_Album(This,album)

#define IITURLTrack_put_Album(This,album)	\
    (This)->lpVtbl -> put_Album(This,album)

#define IITURLTrack_get_Artist(This,artist)	\
    (This)->lpVtbl -> get_Artist(This,artist)

#define IITURLTrack_put_Artist(This,artist)	\
    (This)->lpVtbl -> put_Artist(This,artist)

#define IITURLTrack_get_BitRate(This,bitrate)	\
    (This)->lpVtbl -> get_BitRate(This,bitrate)

#define IITURLTrack_get_BPM(This,beatsPerMinute)	\
    (This)->lpVtbl -> get_BPM(This,beatsPerMinute)

#define IITURLTrack_put_BPM(This,beatsPerMinute)	\
    (This)->lpVtbl -> put_BPM(This,beatsPerMinute)

#define IITURLTrack_get_Comment(This,comment)	\
    (This)->lpVtbl -> get_Comment(This,comment)

#define IITURLTrack_put_Comment(This,comment)	\
    (This)->lpVtbl -> put_Comment(This,comment)

#define IITURLTrack_get_Compilation(This,isCompilation)	\
    (This)->lpVtbl -> get_Compilation(This,isCompilation)

#define IITURLTrack_put_Compilation(This,shouldBeCompilation)	\
    (This)->lpVtbl -> put_Compilation(This,shouldBeCompilation)

#define IITURLTrack_get_Composer(This,composer)	\
    (This)->lpVtbl -> get_Composer(This,composer)

#define IITURLTrack_put_Composer(This,composer)	\
    (This)->lpVtbl -> put_Composer(This,composer)

#define IITURLTrack_get_DateAdded(This,dateAdded)	\
    (This)->lpVtbl -> get_DateAdded(This,dateAdded)

#define IITURLTrack_get_DiscCount(This,discCount)	\
    (This)->lpVtbl -> get_DiscCount(This,discCount)

#define IITURLTrack_put_DiscCount(This,discCount)	\
    (This)->lpVtbl -> put_DiscCount(This,discCount)

#define IITURLTrack_get_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> get_DiscNumber(This,discNumber)

#define IITURLTrack_put_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> put_DiscNumber(This,discNumber)

#define IITURLTrack_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITURLTrack_get_Enabled(This,isEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,isEnabled)

#define IITURLTrack_put_Enabled(This,shouldBeEnabled)	\
    (This)->lpVtbl -> put_Enabled(This,shouldBeEnabled)

#define IITURLTrack_get_EQ(This,eq)	\
    (This)->lpVtbl -> get_EQ(This,eq)

#define IITURLTrack_put_EQ(This,eq)	\
    (This)->lpVtbl -> put_EQ(This,eq)

#define IITURLTrack_put_Finish(This,finish)	\
    (This)->lpVtbl -> put_Finish(This,finish)

#define IITURLTrack_get_Finish(This,finish)	\
    (This)->lpVtbl -> get_Finish(This,finish)

#define IITURLTrack_get_Genre(This,genre)	\
    (This)->lpVtbl -> get_Genre(This,genre)

#define IITURLTrack_put_Genre(This,genre)	\
    (This)->lpVtbl -> put_Genre(This,genre)

#define IITURLTrack_get_Grouping(This,grouping)	\
    (This)->lpVtbl -> get_Grouping(This,grouping)

#define IITURLTrack_put_Grouping(This,grouping)	\
    (This)->lpVtbl -> put_Grouping(This,grouping)

#define IITURLTrack_get_KindAsString(This,kind)	\
    (This)->lpVtbl -> get_KindAsString(This,kind)

#define IITURLTrack_get_ModificationDate(This,dateModified)	\
    (This)->lpVtbl -> get_ModificationDate(This,dateModified)

#define IITURLTrack_get_PlayedCount(This,playedCount)	\
    (This)->lpVtbl -> get_PlayedCount(This,playedCount)

#define IITURLTrack_put_PlayedCount(This,playedCount)	\
    (This)->lpVtbl -> put_PlayedCount(This,playedCount)

#define IITURLTrack_get_PlayedDate(This,playedDate)	\
    (This)->lpVtbl -> get_PlayedDate(This,playedDate)

#define IITURLTrack_put_PlayedDate(This,playedDate)	\
    (This)->lpVtbl -> put_PlayedDate(This,playedDate)

#define IITURLTrack_get_PlayOrderIndex(This,index)	\
    (This)->lpVtbl -> get_PlayOrderIndex(This,index)

#define IITURLTrack_get_Rating(This,rating)	\
    (This)->lpVtbl -> get_Rating(This,rating)

#define IITURLTrack_put_Rating(This,rating)	\
    (This)->lpVtbl -> put_Rating(This,rating)

#define IITURLTrack_get_SampleRate(This,sampleRate)	\
    (This)->lpVtbl -> get_SampleRate(This,sampleRate)

#define IITURLTrack_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITURLTrack_get_Start(This,start)	\
    (This)->lpVtbl -> get_Start(This,start)

#define IITURLTrack_put_Start(This,start)	\
    (This)->lpVtbl -> put_Start(This,start)

#define IITURLTrack_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITURLTrack_get_TrackCount(This,trackCount)	\
    (This)->lpVtbl -> get_TrackCount(This,trackCount)

#define IITURLTrack_put_TrackCount(This,trackCount)	\
    (This)->lpVtbl -> put_TrackCount(This,trackCount)

#define IITURLTrack_get_TrackNumber(This,trackNumber)	\
    (This)->lpVtbl -> get_TrackNumber(This,trackNumber)

#define IITURLTrack_put_TrackNumber(This,trackNumber)	\
    (This)->lpVtbl -> put_TrackNumber(This,trackNumber)

#define IITURLTrack_get_VolumeAdjustment(This,volumeAdjustment)	\
    (This)->lpVtbl -> get_VolumeAdjustment(This,volumeAdjustment)

#define IITURLTrack_put_VolumeAdjustment(This,volumeAdjustment)	\
    (This)->lpVtbl -> put_VolumeAdjustment(This,volumeAdjustment)

#define IITURLTrack_get_Year(This,year)	\
    (This)->lpVtbl -> get_Year(This,year)

#define IITURLTrack_put_Year(This,year)	\
    (This)->lpVtbl -> put_Year(This,year)

#define IITURLTrack_get_Artwork(This,iArtworkCollection)	\
    (This)->lpVtbl -> get_Artwork(This,iArtworkCollection)


#define IITURLTrack_get_URL(This,url)	\
    (This)->lpVtbl -> get_URL(This,url)

#define IITURLTrack_put_URL(This,url)	\
    (This)->lpVtbl -> put_URL(This,url)

#define IITURLTrack_get_Podcast(This,isPodcast)	\
    (This)->lpVtbl -> get_Podcast(This,isPodcast)

#define IITURLTrack_UpdatePodcastFeed(This)	\
    (This)->lpVtbl -> UpdatePodcastFeed(This)

#define IITURLTrack_DownloadPodcastEpisode(This)	\
    (This)->lpVtbl -> DownloadPodcastEpisode(This)

#define IITURLTrack_get_Category(This,category)	\
    (This)->lpVtbl -> get_Category(This,category)

#define IITURLTrack_put_Category(This,category)	\
    (This)->lpVtbl -> put_Category(This,category)

#define IITURLTrack_get_Description(This,description)	\
    (This)->lpVtbl -> get_Description(This,description)

#define IITURLTrack_put_Description(This,description)	\
    (This)->lpVtbl -> put_Description(This,description)

#define IITURLTrack_get_LongDescription(This,longDescription)	\
    (This)->lpVtbl -> get_LongDescription(This,longDescription)

#define IITURLTrack_put_LongDescription(This,longDescription)	\
    (This)->lpVtbl -> put_LongDescription(This,longDescription)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITURLTrack_get_URL_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *url);


void __RPC_STUB IITURLTrack_get_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITURLTrack_put_URL_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [in] */ BSTR url);


void __RPC_STUB IITURLTrack_put_URL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITURLTrack_get_Podcast_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isPodcast);


void __RPC_STUB IITURLTrack_get_Podcast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITURLTrack_UpdatePodcastFeed_Proxy( 
    IITURLTrack __RPC_FAR * This);


void __RPC_STUB IITURLTrack_UpdatePodcastFeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITURLTrack_DownloadPodcastEpisode_Proxy( 
    IITURLTrack __RPC_FAR * This);


void __RPC_STUB IITURLTrack_DownloadPodcastEpisode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITURLTrack_get_Category_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *category);


void __RPC_STUB IITURLTrack_get_Category_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITURLTrack_put_Category_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [in] */ BSTR category);


void __RPC_STUB IITURLTrack_put_Category_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITURLTrack_get_Description_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *description);


void __RPC_STUB IITURLTrack_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITURLTrack_put_Description_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [in] */ BSTR description);


void __RPC_STUB IITURLTrack_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITURLTrack_get_LongDescription_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *longDescription);


void __RPC_STUB IITURLTrack_get_LongDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITURLTrack_put_LongDescription_Proxy( 
    IITURLTrack __RPC_FAR * This,
    /* [in] */ BSTR longDescription);


void __RPC_STUB IITURLTrack_put_LongDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITURLTrack_INTERFACE_DEFINED__ */


#ifndef __IITAudioCDPlaylist_INTERFACE_DEFINED__
#define __IITAudioCDPlaylist_INTERFACE_DEFINED__

/* interface IITAudioCDPlaylist */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITAudioCDPlaylist;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CF496DF3-0FED-4d7d-9BD8-529B6E8A082E")
    IITAudioCDPlaylist : public IITPlaylist
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Artist( 
            /* [retval][out] */ BSTR __RPC_FAR *artist) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Compilation( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompiliation) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Composer( 
            /* [retval][out] */ BSTR __RPC_FAR *composer) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DiscCount( 
            /* [retval][out] */ long __RPC_FAR *discCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_DiscNumber( 
            /* [retval][out] */ long __RPC_FAR *discNumber) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Genre( 
            /* [retval][out] */ BSTR __RPC_FAR *genre) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Year( 
            /* [retval][out] */ long __RPC_FAR *year) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITAudioCDPlaylistVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITAudioCDPlaylist __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITAudioCDPlaylist __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITAudioCDPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PlayFirstTrack )( 
            IITAudioCDPlaylist __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Print )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL showPrintDialog,
            /* [in] */ ITPlaylistPrintKind printKind,
            /* [in] */ BSTR theme);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Search )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ BSTR searchText,
            /* [in] */ ITPlaylistSearchField searchFields,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITSource __RPC_FAR *__RPC_FAR *iSource);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Shuffle )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isShuffle);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Shuffle )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldShuffle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SongRepeat )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ ITPlaylistRepeatMode __RPC_FAR *repeatMode);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SongRepeat )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [in] */ ITPlaylistRepeatMode repeatMode);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Tracks )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artist )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *artist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Compilation )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompiliation);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Composer )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *composer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscCount )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscNumber )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Genre )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *genre);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Year )( 
            IITAudioCDPlaylist __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *year);
        
        END_INTERFACE
    } IITAudioCDPlaylistVtbl;

    interface IITAudioCDPlaylist
    {
        CONST_VTBL struct IITAudioCDPlaylistVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITAudioCDPlaylist_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITAudioCDPlaylist_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITAudioCDPlaylist_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITAudioCDPlaylist_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITAudioCDPlaylist_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITAudioCDPlaylist_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITAudioCDPlaylist_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITAudioCDPlaylist_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITAudioCDPlaylist_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITAudioCDPlaylist_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITAudioCDPlaylist_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITAudioCDPlaylist_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITAudioCDPlaylist_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITAudioCDPlaylist_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITAudioCDPlaylist_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITAudioCDPlaylist_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITAudioCDPlaylist_PlayFirstTrack(This)	\
    (This)->lpVtbl -> PlayFirstTrack(This)

#define IITAudioCDPlaylist_Print(This,showPrintDialog,printKind,theme)	\
    (This)->lpVtbl -> Print(This,showPrintDialog,printKind,theme)

#define IITAudioCDPlaylist_Search(This,searchText,searchFields,iTrackCollection)	\
    (This)->lpVtbl -> Search(This,searchText,searchFields,iTrackCollection)

#define IITAudioCDPlaylist_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITAudioCDPlaylist_get_Source(This,iSource)	\
    (This)->lpVtbl -> get_Source(This,iSource)

#define IITAudioCDPlaylist_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITAudioCDPlaylist_get_Shuffle(This,isShuffle)	\
    (This)->lpVtbl -> get_Shuffle(This,isShuffle)

#define IITAudioCDPlaylist_put_Shuffle(This,shouldShuffle)	\
    (This)->lpVtbl -> put_Shuffle(This,shouldShuffle)

#define IITAudioCDPlaylist_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITAudioCDPlaylist_get_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> get_SongRepeat(This,repeatMode)

#define IITAudioCDPlaylist_put_SongRepeat(This,repeatMode)	\
    (This)->lpVtbl -> put_SongRepeat(This,repeatMode)

#define IITAudioCDPlaylist_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITAudioCDPlaylist_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITAudioCDPlaylist_get_Tracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_Tracks(This,iTrackCollection)


#define IITAudioCDPlaylist_get_Artist(This,artist)	\
    (This)->lpVtbl -> get_Artist(This,artist)

#define IITAudioCDPlaylist_get_Compilation(This,isCompiliation)	\
    (This)->lpVtbl -> get_Compilation(This,isCompiliation)

#define IITAudioCDPlaylist_get_Composer(This,composer)	\
    (This)->lpVtbl -> get_Composer(This,composer)

#define IITAudioCDPlaylist_get_DiscCount(This,discCount)	\
    (This)->lpVtbl -> get_DiscCount(This,discCount)

#define IITAudioCDPlaylist_get_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> get_DiscNumber(This,discNumber)

#define IITAudioCDPlaylist_get_Genre(This,genre)	\
    (This)->lpVtbl -> get_Genre(This,genre)

#define IITAudioCDPlaylist_get_Year(This,year)	\
    (This)->lpVtbl -> get_Year(This,year)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_Artist_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *artist);


void __RPC_STUB IITAudioCDPlaylist_get_Artist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_Compilation_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompiliation);


void __RPC_STUB IITAudioCDPlaylist_get_Compilation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_Composer_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *composer);


void __RPC_STUB IITAudioCDPlaylist_get_Composer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_DiscCount_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *discCount);


void __RPC_STUB IITAudioCDPlaylist_get_DiscCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_DiscNumber_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *discNumber);


void __RPC_STUB IITAudioCDPlaylist_get_DiscNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_Genre_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *genre);


void __RPC_STUB IITAudioCDPlaylist_get_Genre_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITAudioCDPlaylist_get_Year_Proxy( 
    IITAudioCDPlaylist __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *year);


void __RPC_STUB IITAudioCDPlaylist_get_Year_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITAudioCDPlaylist_INTERFACE_DEFINED__ */


#ifndef __IITPlaylistCollection_INTERFACE_DEFINED__
#define __IITPlaylistCollection_INTERFACE_DEFINED__

/* interface IITPlaylistCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITPlaylistCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FF194254-909D-4437-9C50-3AAC2AE6305C")
    IITPlaylistCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *count) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ItemByName( 
            /* [in] */ BSTR name,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITPlaylistCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITPlaylistCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITPlaylistCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *count);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemByName )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IITPlaylistCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);
        
        END_INTERFACE
    } IITPlaylistCollectionVtbl;

    interface IITPlaylistCollection
    {
        CONST_VTBL struct IITPlaylistCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITPlaylistCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITPlaylistCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITPlaylistCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITPlaylistCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITPlaylistCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITPlaylistCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITPlaylistCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITPlaylistCollection_get_Count(This,count)	\
    (This)->lpVtbl -> get_Count(This,count)

#define IITPlaylistCollection_get_Item(This,index,iPlaylist)	\
    (This)->lpVtbl -> get_Item(This,index,iPlaylist)

#define IITPlaylistCollection_get_ItemByName(This,name,iPlaylist)	\
    (This)->lpVtbl -> get_ItemByName(This,name,iPlaylist)

#define IITPlaylistCollection_get__NewEnum(This,iEnumerator)	\
    (This)->lpVtbl -> get__NewEnum(This,iEnumerator)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylistCollection_get_Count_Proxy( 
    IITPlaylistCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *count);


void __RPC_STUB IITPlaylistCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylistCollection_get_Item_Proxy( 
    IITPlaylistCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IITPlaylistCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylistCollection_get_ItemByName_Proxy( 
    IITPlaylistCollection __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IITPlaylistCollection_get_ItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylistCollection_get__NewEnum_Proxy( 
    IITPlaylistCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *iEnumerator);


void __RPC_STUB IITPlaylistCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITPlaylistCollection_INTERFACE_DEFINED__ */


#ifndef __IITIPodSource_INTERFACE_DEFINED__
#define __IITIPodSource_INTERFACE_DEFINED__

/* interface IITIPodSource */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITIPodSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CF4D8ACE-1720-4fb9-B0AE-9877249E89B0")
    IITIPodSource : public IITSource
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UpdateIPod( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EjectIPod( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SoftwareVersion( 
            /* [retval][out] */ BSTR __RPC_FAR *softwareVersion) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITIPodSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITIPodSource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITIPodSource __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITIPodSource __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITIPodSource __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITIPodSource __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITIPodSource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITIPodSource __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITIPodSource __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITIPodSource __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ ITSourceKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Capacity )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *capacity);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FreeSpace )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *freespace);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Playlists )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ IITPlaylistCollection __RPC_FAR *__RPC_FAR *iPlaylistCollection);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateIPod )( 
            IITIPodSource __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EjectIPod )( 
            IITIPodSource __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SoftwareVersion )( 
            IITIPodSource __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *softwareVersion);
        
        END_INTERFACE
    } IITIPodSourceVtbl;

    interface IITIPodSource
    {
        CONST_VTBL struct IITIPodSourceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITIPodSource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITIPodSource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITIPodSource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITIPodSource_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITIPodSource_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITIPodSource_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITIPodSource_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITIPodSource_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITIPodSource_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITIPodSource_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITIPodSource_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITIPodSource_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITIPodSource_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITIPodSource_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITIPodSource_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITIPodSource_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITIPodSource_get_Capacity(This,capacity)	\
    (This)->lpVtbl -> get_Capacity(This,capacity)

#define IITIPodSource_get_FreeSpace(This,freespace)	\
    (This)->lpVtbl -> get_FreeSpace(This,freespace)

#define IITIPodSource_get_Playlists(This,iPlaylistCollection)	\
    (This)->lpVtbl -> get_Playlists(This,iPlaylistCollection)


#define IITIPodSource_UpdateIPod(This)	\
    (This)->lpVtbl -> UpdateIPod(This)

#define IITIPodSource_EjectIPod(This)	\
    (This)->lpVtbl -> EjectIPod(This)

#define IITIPodSource_get_SoftwareVersion(This,softwareVersion)	\
    (This)->lpVtbl -> get_SoftwareVersion(This,softwareVersion)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITIPodSource_UpdateIPod_Proxy( 
    IITIPodSource __RPC_FAR * This);


void __RPC_STUB IITIPodSource_UpdateIPod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITIPodSource_EjectIPod_Proxy( 
    IITIPodSource __RPC_FAR * This);


void __RPC_STUB IITIPodSource_EjectIPod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITIPodSource_get_SoftwareVersion_Proxy( 
    IITIPodSource __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *softwareVersion);


void __RPC_STUB IITIPodSource_get_SoftwareVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITIPodSource_INTERFACE_DEFINED__ */


#ifndef __IITFileOrCDTrack_INTERFACE_DEFINED__
#define __IITFileOrCDTrack_INTERFACE_DEFINED__

/* interface IITFileOrCDTrack */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITFileOrCDTrack;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00D7FE99-7868-4cc7-AD9E-ACFD70D09566")
    IITFileOrCDTrack : public IITTrack
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Location( 
            /* [retval][out] */ BSTR __RPC_FAR *location) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UpdateInfoFromFile( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Podcast( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isPodcast) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UpdatePodcastFeed( void) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_RememberBookmark( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *rememberBookmark) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_RememberBookmark( 
            /* [in] */ VARIANT_BOOL shouldRememberBookmark) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ExcludeFromShuffle( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *excludeFromShuffle) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ExcludeFromShuffle( 
            /* [in] */ VARIANT_BOOL shouldExcludeFromShuffle) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Lyrics( 
            /* [retval][out] */ BSTR __RPC_FAR *lyrics) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Lyrics( 
            /* [in] */ BSTR lyrics) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Category( 
            /* [retval][out] */ BSTR __RPC_FAR *category) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Category( 
            /* [in] */ BSTR category) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *description) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR description) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_LongDescription( 
            /* [retval][out] */ BSTR __RPC_FAR *longDescription) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_LongDescription( 
            /* [in] */ BSTR longDescription) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_BookmarkTime( 
            /* [retval][out] */ long __RPC_FAR *bookmarkTime) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_BookmarkTime( 
            /* [in] */ long bookmarkTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITFileOrCDTrackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITFileOrCDTrack __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITFileOrCDTrack __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetITObjectIDs )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *sourceID,
            /* [out] */ long __RPC_FAR *playlistID,
            /* [out] */ long __RPC_FAR *trackID,
            /* [out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sourceID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlaylistID )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playlistID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackID )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackDatabaseID )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *databaseID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IITFileOrCDTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Play )( 
            IITFileOrCDTrack __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddArtworkFromFile )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR filePath,
            /* [retval][out] */ IITArtwork __RPC_FAR *__RPC_FAR *iArtwork);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ ITTrackKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Playlist )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Album )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *album);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Album )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR album);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artist )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *artist);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Artist )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR artist);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bitrate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BPM )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *beatsPerMinute);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BPM )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long beatsPerMinute);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Comment )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *comment);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Comment )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR comment);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Compilation )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isCompilation);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Compilation )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeCompilation);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Composer )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *composer);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Composer )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR composer);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DateAdded )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *dateAdded);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DiscCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long discCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DiscNumber )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *discNumber);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DiscNumber )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long discNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *duration);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isEnabled);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeEnabled);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EQ )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *eq);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EQ )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR eq);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Finish )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long finish);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Finish )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *finish);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Genre )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *genre);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Genre )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR genre);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Grouping )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *grouping);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Grouping )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR grouping);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KindAsString )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModificationDate )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *dateModified);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayedCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *playedCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayedCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long playedCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayedDate )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *playedDate);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PlayedDate )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ DATE playedDate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PlayOrderIndex )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *index);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rating )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *rating);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rating )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long rating);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SampleRate )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *sampleRate);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *size);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Start )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *start);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Start )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long start);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *time);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackCount);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TrackCount )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long trackCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TrackNumber )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *trackNumber);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TrackNumber )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long trackNumber);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VolumeAdjustment )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *volumeAdjustment);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VolumeAdjustment )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long volumeAdjustment);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Year )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *year);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Year )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long year);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Artwork )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ IITArtworkCollection __RPC_FAR *__RPC_FAR *iArtworkCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Location )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *location);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateInfoFromFile )( 
            IITFileOrCDTrack __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Podcast )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isPodcast);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdatePodcastFeed )( 
            IITFileOrCDTrack __RPC_FAR * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RememberBookmark )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *rememberBookmark);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RememberBookmark )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldRememberBookmark);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExcludeFromShuffle )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *excludeFromShuffle);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExcludeFromShuffle )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldExcludeFromShuffle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Lyrics )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *lyrics);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Lyrics )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR lyrics);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Category )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *category);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Category )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR category);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *description);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR description);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LongDescription )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *longDescription);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LongDescription )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ BSTR longDescription);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BookmarkTime )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bookmarkTime);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BookmarkTime )( 
            IITFileOrCDTrack __RPC_FAR * This,
            /* [in] */ long bookmarkTime);
        
        END_INTERFACE
    } IITFileOrCDTrackVtbl;

    interface IITFileOrCDTrack
    {
        CONST_VTBL struct IITFileOrCDTrackVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITFileOrCDTrack_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITFileOrCDTrack_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITFileOrCDTrack_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITFileOrCDTrack_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITFileOrCDTrack_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITFileOrCDTrack_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITFileOrCDTrack_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITFileOrCDTrack_GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)	\
    (This)->lpVtbl -> GetITObjectIDs(This,sourceID,playlistID,trackID,databaseID)

#define IITFileOrCDTrack_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITFileOrCDTrack_put_Name(This,name)	\
    (This)->lpVtbl -> put_Name(This,name)

#define IITFileOrCDTrack_get_Index(This,index)	\
    (This)->lpVtbl -> get_Index(This,index)

#define IITFileOrCDTrack_get_SourceID(This,sourceID)	\
    (This)->lpVtbl -> get_SourceID(This,sourceID)

#define IITFileOrCDTrack_get_PlaylistID(This,playlistID)	\
    (This)->lpVtbl -> get_PlaylistID(This,playlistID)

#define IITFileOrCDTrack_get_TrackID(This,trackID)	\
    (This)->lpVtbl -> get_TrackID(This,trackID)

#define IITFileOrCDTrack_get_TrackDatabaseID(This,databaseID)	\
    (This)->lpVtbl -> get_TrackDatabaseID(This,databaseID)


#define IITFileOrCDTrack_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IITFileOrCDTrack_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define IITFileOrCDTrack_AddArtworkFromFile(This,filePath,iArtwork)	\
    (This)->lpVtbl -> AddArtworkFromFile(This,filePath,iArtwork)

#define IITFileOrCDTrack_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITFileOrCDTrack_get_Playlist(This,iPlaylist)	\
    (This)->lpVtbl -> get_Playlist(This,iPlaylist)

#define IITFileOrCDTrack_get_Album(This,album)	\
    (This)->lpVtbl -> get_Album(This,album)

#define IITFileOrCDTrack_put_Album(This,album)	\
    (This)->lpVtbl -> put_Album(This,album)

#define IITFileOrCDTrack_get_Artist(This,artist)	\
    (This)->lpVtbl -> get_Artist(This,artist)

#define IITFileOrCDTrack_put_Artist(This,artist)	\
    (This)->lpVtbl -> put_Artist(This,artist)

#define IITFileOrCDTrack_get_BitRate(This,bitrate)	\
    (This)->lpVtbl -> get_BitRate(This,bitrate)

#define IITFileOrCDTrack_get_BPM(This,beatsPerMinute)	\
    (This)->lpVtbl -> get_BPM(This,beatsPerMinute)

#define IITFileOrCDTrack_put_BPM(This,beatsPerMinute)	\
    (This)->lpVtbl -> put_BPM(This,beatsPerMinute)

#define IITFileOrCDTrack_get_Comment(This,comment)	\
    (This)->lpVtbl -> get_Comment(This,comment)

#define IITFileOrCDTrack_put_Comment(This,comment)	\
    (This)->lpVtbl -> put_Comment(This,comment)

#define IITFileOrCDTrack_get_Compilation(This,isCompilation)	\
    (This)->lpVtbl -> get_Compilation(This,isCompilation)

#define IITFileOrCDTrack_put_Compilation(This,shouldBeCompilation)	\
    (This)->lpVtbl -> put_Compilation(This,shouldBeCompilation)

#define IITFileOrCDTrack_get_Composer(This,composer)	\
    (This)->lpVtbl -> get_Composer(This,composer)

#define IITFileOrCDTrack_put_Composer(This,composer)	\
    (This)->lpVtbl -> put_Composer(This,composer)

#define IITFileOrCDTrack_get_DateAdded(This,dateAdded)	\
    (This)->lpVtbl -> get_DateAdded(This,dateAdded)

#define IITFileOrCDTrack_get_DiscCount(This,discCount)	\
    (This)->lpVtbl -> get_DiscCount(This,discCount)

#define IITFileOrCDTrack_put_DiscCount(This,discCount)	\
    (This)->lpVtbl -> put_DiscCount(This,discCount)

#define IITFileOrCDTrack_get_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> get_DiscNumber(This,discNumber)

#define IITFileOrCDTrack_put_DiscNumber(This,discNumber)	\
    (This)->lpVtbl -> put_DiscNumber(This,discNumber)

#define IITFileOrCDTrack_get_Duration(This,duration)	\
    (This)->lpVtbl -> get_Duration(This,duration)

#define IITFileOrCDTrack_get_Enabled(This,isEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,isEnabled)

#define IITFileOrCDTrack_put_Enabled(This,shouldBeEnabled)	\
    (This)->lpVtbl -> put_Enabled(This,shouldBeEnabled)

#define IITFileOrCDTrack_get_EQ(This,eq)	\
    (This)->lpVtbl -> get_EQ(This,eq)

#define IITFileOrCDTrack_put_EQ(This,eq)	\
    (This)->lpVtbl -> put_EQ(This,eq)

#define IITFileOrCDTrack_put_Finish(This,finish)	\
    (This)->lpVtbl -> put_Finish(This,finish)

#define IITFileOrCDTrack_get_Finish(This,finish)	\
    (This)->lpVtbl -> get_Finish(This,finish)

#define IITFileOrCDTrack_get_Genre(This,genre)	\
    (This)->lpVtbl -> get_Genre(This,genre)

#define IITFileOrCDTrack_put_Genre(This,genre)	\
    (This)->lpVtbl -> put_Genre(This,genre)

#define IITFileOrCDTrack_get_Grouping(This,grouping)	\
    (This)->lpVtbl -> get_Grouping(This,grouping)

#define IITFileOrCDTrack_put_Grouping(This,grouping)	\
    (This)->lpVtbl -> put_Grouping(This,grouping)

#define IITFileOrCDTrack_get_KindAsString(This,kind)	\
    (This)->lpVtbl -> get_KindAsString(This,kind)

#define IITFileOrCDTrack_get_ModificationDate(This,dateModified)	\
    (This)->lpVtbl -> get_ModificationDate(This,dateModified)

#define IITFileOrCDTrack_get_PlayedCount(This,playedCount)	\
    (This)->lpVtbl -> get_PlayedCount(This,playedCount)

#define IITFileOrCDTrack_put_PlayedCount(This,playedCount)	\
    (This)->lpVtbl -> put_PlayedCount(This,playedCount)

#define IITFileOrCDTrack_get_PlayedDate(This,playedDate)	\
    (This)->lpVtbl -> get_PlayedDate(This,playedDate)

#define IITFileOrCDTrack_put_PlayedDate(This,playedDate)	\
    (This)->lpVtbl -> put_PlayedDate(This,playedDate)

#define IITFileOrCDTrack_get_PlayOrderIndex(This,index)	\
    (This)->lpVtbl -> get_PlayOrderIndex(This,index)

#define IITFileOrCDTrack_get_Rating(This,rating)	\
    (This)->lpVtbl -> get_Rating(This,rating)

#define IITFileOrCDTrack_put_Rating(This,rating)	\
    (This)->lpVtbl -> put_Rating(This,rating)

#define IITFileOrCDTrack_get_SampleRate(This,sampleRate)	\
    (This)->lpVtbl -> get_SampleRate(This,sampleRate)

#define IITFileOrCDTrack_get_Size(This,size)	\
    (This)->lpVtbl -> get_Size(This,size)

#define IITFileOrCDTrack_get_Start(This,start)	\
    (This)->lpVtbl -> get_Start(This,start)

#define IITFileOrCDTrack_put_Start(This,start)	\
    (This)->lpVtbl -> put_Start(This,start)

#define IITFileOrCDTrack_get_Time(This,time)	\
    (This)->lpVtbl -> get_Time(This,time)

#define IITFileOrCDTrack_get_TrackCount(This,trackCount)	\
    (This)->lpVtbl -> get_TrackCount(This,trackCount)

#define IITFileOrCDTrack_put_TrackCount(This,trackCount)	\
    (This)->lpVtbl -> put_TrackCount(This,trackCount)

#define IITFileOrCDTrack_get_TrackNumber(This,trackNumber)	\
    (This)->lpVtbl -> get_TrackNumber(This,trackNumber)

#define IITFileOrCDTrack_put_TrackNumber(This,trackNumber)	\
    (This)->lpVtbl -> put_TrackNumber(This,trackNumber)

#define IITFileOrCDTrack_get_VolumeAdjustment(This,volumeAdjustment)	\
    (This)->lpVtbl -> get_VolumeAdjustment(This,volumeAdjustment)

#define IITFileOrCDTrack_put_VolumeAdjustment(This,volumeAdjustment)	\
    (This)->lpVtbl -> put_VolumeAdjustment(This,volumeAdjustment)

#define IITFileOrCDTrack_get_Year(This,year)	\
    (This)->lpVtbl -> get_Year(This,year)

#define IITFileOrCDTrack_put_Year(This,year)	\
    (This)->lpVtbl -> put_Year(This,year)

#define IITFileOrCDTrack_get_Artwork(This,iArtworkCollection)	\
    (This)->lpVtbl -> get_Artwork(This,iArtworkCollection)


#define IITFileOrCDTrack_get_Location(This,location)	\
    (This)->lpVtbl -> get_Location(This,location)

#define IITFileOrCDTrack_UpdateInfoFromFile(This)	\
    (This)->lpVtbl -> UpdateInfoFromFile(This)

#define IITFileOrCDTrack_get_Podcast(This,isPodcast)	\
    (This)->lpVtbl -> get_Podcast(This,isPodcast)

#define IITFileOrCDTrack_UpdatePodcastFeed(This)	\
    (This)->lpVtbl -> UpdatePodcastFeed(This)

#define IITFileOrCDTrack_get_RememberBookmark(This,rememberBookmark)	\
    (This)->lpVtbl -> get_RememberBookmark(This,rememberBookmark)

#define IITFileOrCDTrack_put_RememberBookmark(This,shouldRememberBookmark)	\
    (This)->lpVtbl -> put_RememberBookmark(This,shouldRememberBookmark)

#define IITFileOrCDTrack_get_ExcludeFromShuffle(This,excludeFromShuffle)	\
    (This)->lpVtbl -> get_ExcludeFromShuffle(This,excludeFromShuffle)

#define IITFileOrCDTrack_put_ExcludeFromShuffle(This,shouldExcludeFromShuffle)	\
    (This)->lpVtbl -> put_ExcludeFromShuffle(This,shouldExcludeFromShuffle)

#define IITFileOrCDTrack_get_Lyrics(This,lyrics)	\
    (This)->lpVtbl -> get_Lyrics(This,lyrics)

#define IITFileOrCDTrack_put_Lyrics(This,lyrics)	\
    (This)->lpVtbl -> put_Lyrics(This,lyrics)

#define IITFileOrCDTrack_get_Category(This,category)	\
    (This)->lpVtbl -> get_Category(This,category)

#define IITFileOrCDTrack_put_Category(This,category)	\
    (This)->lpVtbl -> put_Category(This,category)

#define IITFileOrCDTrack_get_Description(This,description)	\
    (This)->lpVtbl -> get_Description(This,description)

#define IITFileOrCDTrack_put_Description(This,description)	\
    (This)->lpVtbl -> put_Description(This,description)

#define IITFileOrCDTrack_get_LongDescription(This,longDescription)	\
    (This)->lpVtbl -> get_LongDescription(This,longDescription)

#define IITFileOrCDTrack_put_LongDescription(This,longDescription)	\
    (This)->lpVtbl -> put_LongDescription(This,longDescription)

#define IITFileOrCDTrack_get_BookmarkTime(This,bookmarkTime)	\
    (This)->lpVtbl -> get_BookmarkTime(This,bookmarkTime)

#define IITFileOrCDTrack_put_BookmarkTime(This,bookmarkTime)	\
    (This)->lpVtbl -> put_BookmarkTime(This,bookmarkTime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_Location_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *location);


void __RPC_STUB IITFileOrCDTrack_get_Location_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_UpdateInfoFromFile_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This);


void __RPC_STUB IITFileOrCDTrack_UpdateInfoFromFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_Podcast_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isPodcast);


void __RPC_STUB IITFileOrCDTrack_get_Podcast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_UpdatePodcastFeed_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This);


void __RPC_STUB IITFileOrCDTrack_UpdatePodcastFeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_RememberBookmark_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *rememberBookmark);


void __RPC_STUB IITFileOrCDTrack_get_RememberBookmark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_RememberBookmark_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldRememberBookmark);


void __RPC_STUB IITFileOrCDTrack_put_RememberBookmark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_ExcludeFromShuffle_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *excludeFromShuffle);


void __RPC_STUB IITFileOrCDTrack_get_ExcludeFromShuffle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_ExcludeFromShuffle_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL shouldExcludeFromShuffle);


void __RPC_STUB IITFileOrCDTrack_put_ExcludeFromShuffle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_Lyrics_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *lyrics);


void __RPC_STUB IITFileOrCDTrack_get_Lyrics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_Lyrics_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ BSTR lyrics);


void __RPC_STUB IITFileOrCDTrack_put_Lyrics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_Category_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *category);


void __RPC_STUB IITFileOrCDTrack_get_Category_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_Category_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ BSTR category);


void __RPC_STUB IITFileOrCDTrack_put_Category_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_Description_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *description);


void __RPC_STUB IITFileOrCDTrack_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_Description_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ BSTR description);


void __RPC_STUB IITFileOrCDTrack_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_LongDescription_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *longDescription);


void __RPC_STUB IITFileOrCDTrack_get_LongDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_LongDescription_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ BSTR longDescription);


void __RPC_STUB IITFileOrCDTrack_put_LongDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_get_BookmarkTime_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *bookmarkTime);


void __RPC_STUB IITFileOrCDTrack_get_BookmarkTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IITFileOrCDTrack_put_BookmarkTime_Proxy( 
    IITFileOrCDTrack __RPC_FAR * This,
    /* [in] */ long bookmarkTime);


void __RPC_STUB IITFileOrCDTrack_put_BookmarkTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITFileOrCDTrack_INTERFACE_DEFINED__ */


#ifndef __IITPlaylistWindow_INTERFACE_DEFINED__
#define __IITPlaylistWindow_INTERFACE_DEFINED__

/* interface IITPlaylistWindow */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IITPlaylistWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("349CBB45-2E5A-4822-8E4A-A75555A186F7")
    IITPlaylistWindow : public IITWindow
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_SelectedTracks( 
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Playlist( 
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IITPlaylistWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IITPlaylistWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IITPlaylistWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *name);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Kind )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ ITWindowKind __RPC_FAR *kind);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isVisible);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Resizable )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isResizable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Minimized )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMinimized);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Minimized )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMinimized);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Maximizable )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximizable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Maximized )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isMaximized);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Maximized )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeMaximized);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoomable )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomable);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Zoomed )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *isZoomed);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Zoomed )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL shouldBeZoomed);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Top )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *top);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Top )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ long top);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Left )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *left);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Left )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ long left);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bottom )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *bottom);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Bottom )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ long bottom);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Right )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *right);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Right )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ long right);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *width);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ long width);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *height);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [in] */ long height);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectedTracks )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Playlist )( 
            IITPlaylistWindow __RPC_FAR * This,
            /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);
        
        END_INTERFACE
    } IITPlaylistWindowVtbl;

    interface IITPlaylistWindow
    {
        CONST_VTBL struct IITPlaylistWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IITPlaylistWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IITPlaylistWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IITPlaylistWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IITPlaylistWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IITPlaylistWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IITPlaylistWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IITPlaylistWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IITPlaylistWindow_get_Name(This,name)	\
    (This)->lpVtbl -> get_Name(This,name)

#define IITPlaylistWindow_get_Kind(This,kind)	\
    (This)->lpVtbl -> get_Kind(This,kind)

#define IITPlaylistWindow_get_Visible(This,isVisible)	\
    (This)->lpVtbl -> get_Visible(This,isVisible)

#define IITPlaylistWindow_put_Visible(This,shouldBeVisible)	\
    (This)->lpVtbl -> put_Visible(This,shouldBeVisible)

#define IITPlaylistWindow_get_Resizable(This,isResizable)	\
    (This)->lpVtbl -> get_Resizable(This,isResizable)

#define IITPlaylistWindow_get_Minimized(This,isMinimized)	\
    (This)->lpVtbl -> get_Minimized(This,isMinimized)

#define IITPlaylistWindow_put_Minimized(This,shouldBeMinimized)	\
    (This)->lpVtbl -> put_Minimized(This,shouldBeMinimized)

#define IITPlaylistWindow_get_Maximizable(This,isMaximizable)	\
    (This)->lpVtbl -> get_Maximizable(This,isMaximizable)

#define IITPlaylistWindow_get_Maximized(This,isMaximized)	\
    (This)->lpVtbl -> get_Maximized(This,isMaximized)

#define IITPlaylistWindow_put_Maximized(This,shouldBeMaximized)	\
    (This)->lpVtbl -> put_Maximized(This,shouldBeMaximized)

#define IITPlaylistWindow_get_Zoomable(This,isZoomable)	\
    (This)->lpVtbl -> get_Zoomable(This,isZoomable)

#define IITPlaylistWindow_get_Zoomed(This,isZoomed)	\
    (This)->lpVtbl -> get_Zoomed(This,isZoomed)

#define IITPlaylistWindow_put_Zoomed(This,shouldBeZoomed)	\
    (This)->lpVtbl -> put_Zoomed(This,shouldBeZoomed)

#define IITPlaylistWindow_get_Top(This,top)	\
    (This)->lpVtbl -> get_Top(This,top)

#define IITPlaylistWindow_put_Top(This,top)	\
    (This)->lpVtbl -> put_Top(This,top)

#define IITPlaylistWindow_get_Left(This,left)	\
    (This)->lpVtbl -> get_Left(This,left)

#define IITPlaylistWindow_put_Left(This,left)	\
    (This)->lpVtbl -> put_Left(This,left)

#define IITPlaylistWindow_get_Bottom(This,bottom)	\
    (This)->lpVtbl -> get_Bottom(This,bottom)

#define IITPlaylistWindow_put_Bottom(This,bottom)	\
    (This)->lpVtbl -> put_Bottom(This,bottom)

#define IITPlaylistWindow_get_Right(This,right)	\
    (This)->lpVtbl -> get_Right(This,right)

#define IITPlaylistWindow_put_Right(This,right)	\
    (This)->lpVtbl -> put_Right(This,right)

#define IITPlaylistWindow_get_Width(This,width)	\
    (This)->lpVtbl -> get_Width(This,width)

#define IITPlaylistWindow_put_Width(This,width)	\
    (This)->lpVtbl -> put_Width(This,width)

#define IITPlaylistWindow_get_Height(This,height)	\
    (This)->lpVtbl -> get_Height(This,height)

#define IITPlaylistWindow_put_Height(This,height)	\
    (This)->lpVtbl -> put_Height(This,height)


#define IITPlaylistWindow_get_SelectedTracks(This,iTrackCollection)	\
    (This)->lpVtbl -> get_SelectedTracks(This,iTrackCollection)

#define IITPlaylistWindow_get_Playlist(This,iPlaylist)	\
    (This)->lpVtbl -> get_Playlist(This,iPlaylist)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylistWindow_get_SelectedTracks_Proxy( 
    IITPlaylistWindow __RPC_FAR * This,
    /* [retval][out] */ IITTrackCollection __RPC_FAR *__RPC_FAR *iTrackCollection);


void __RPC_STUB IITPlaylistWindow_get_SelectedTracks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IITPlaylistWindow_get_Playlist_Proxy( 
    IITPlaylistWindow __RPC_FAR * This,
    /* [retval][out] */ IITPlaylist __RPC_FAR *__RPC_FAR *iPlaylist);


void __RPC_STUB IITPlaylistWindow_get_Playlist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IITPlaylistWindow_INTERFACE_DEFINED__ */

#endif /* __iTunesLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
