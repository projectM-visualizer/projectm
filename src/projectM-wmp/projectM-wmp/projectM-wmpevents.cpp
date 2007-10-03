/////////////////////////////////////////////////////////////////////////////
//
// projectM-wmpEvents.cpp : Implementation of CProjectMwmp events
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "projectM-wmp.h"

extern projectM* globalPM;

void CProjectMwmp::OpenStateChange( long NewState )
{
    switch (NewState)
    {
    case wmposUndefined:
        break;
	case wmposPlaylistChanging:
        break;
	case wmposPlaylistLocating:
        break;
	case wmposPlaylistConnecting:
        break;
	case wmposPlaylistLoading:
        break;
	case wmposPlaylistOpening:
        break;
	case wmposPlaylistOpenNoMedia:
        break;
	case wmposPlaylistChanged:
        break;
	case wmposMediaChanging:
        break;
	case wmposMediaLocating:
        break;
	case wmposMediaConnecting:
        break;
	case wmposMediaLoading:
        break;
	case wmposMediaOpening:
        break;
	case wmposMediaOpen:
        break;
	case wmposBeginCodecAcquisition:
        break;
	case wmposEndCodecAcquisition:
        break;
	case wmposBeginLicenseAcquisition:
        break;
	case wmposEndLicenseAcquisition:
        break;
	case wmposBeginIndividualization:
        break;
	case wmposEndIndividualization:
        break;
	case wmposMediaWaiting:
        break;
	case wmposOpeningUnknownURL:
        break;
    default:
        break;
    }
}

void CProjectMwmp::PlayStateChange( long NewState )
{
    switch (NewState)
    {
    case wmppsUndefined:
        break;
	case wmppsStopped:
        break;
	case wmppsPaused:
        break;
	case wmppsPlaying:
        break;
	case wmppsScanForward:
        break;
	case wmppsScanReverse:
        break;
	case wmppsBuffering:
        break;
	case wmppsWaiting:
        break;
	case wmppsMediaEnded:
        break;
	case wmppsTransitioning:
        break;
	case wmppsReady:
        break;
	case wmppsReconnecting:
        break;
	case wmppsLast:
        break;
    default:
        break;
    }
}

void CProjectMwmp::AudioLanguageChange( long LangID )
{
}

void CProjectMwmp::StatusChange()
{
}

void CProjectMwmp::ScriptCommand( BSTR scType, BSTR Param )
{
}

void CProjectMwmp::NewStream()
{
}

void CProjectMwmp::Disconnect( long Result )
{
}

void CProjectMwmp::Buffering( VARIANT_BOOL Start )
{
}

void CProjectMwmp::Error()
{
    CComPtr<IWMPError>      spError;
    CComPtr<IWMPErrorItem>  spErrorItem;
    HRESULT                 dwError = S_OK;
    HRESULT                 hr = S_OK;

    if (m_spCore)
    {
        hr = m_spCore->get_error(&spError);

        if (SUCCEEDED(hr))
        {
            hr = spError->get_item(0, &spErrorItem);
        }

        if (SUCCEEDED(hr))
        {
            hr = spErrorItem->get_errorCode( (long *) &dwError );
        }
    }
}

void CProjectMwmp::Warning( long WarningType, long Param, BSTR Description )
{
}

void CProjectMwmp::EndOfStream( long Result )
{
}

void CProjectMwmp::PositionChange( double oldPosition, double newPosition)
{
}

void CProjectMwmp::MarkerHit( long MarkerNum )
{
}

void CProjectMwmp::DurationUnitChange( long NewDurationUnit )
{
}

void CProjectMwmp::CdromMediaChange( long CdromNum )
{
}

void CProjectMwmp::PlaylistChange( IDispatch * Playlist, WMPPlaylistChangeEventType change )
{
    switch (change)
    {
    case wmplcUnknown:
        break;
	case wmplcClear:
        break;
	case wmplcInfoChange:
        break;
	case wmplcMove:
        break;
	case wmplcDelete:
        break;
	case wmplcInsert:
        break;
	case wmplcAppend:
        break;
	case wmplcPrivate:
        break;
	case wmplcNameChange:
        break;
	case wmplcMorph:
        break;
	case wmplcSort:
        break;
	case wmplcLast:
        break;
    default:
        break;
    }
}

void CProjectMwmp::CurrentPlaylistChange( WMPPlaylistChangeEventType change )
{
    switch (change)
    {
    case wmplcUnknown:
        break;
	case wmplcClear:
        break;
	case wmplcInfoChange:
        break;
	case wmplcMove:
        break;
	case wmplcDelete:
        break;
	case wmplcInsert:
        break;
	case wmplcAppend:
        break;
	case wmplcPrivate:
        break;
	case wmplcNameChange:
        break;
	case wmplcMorph:
        break;
	case wmplcSort:
        break;
	case wmplcLast:
        break;
    default:
        break;
    }
}

void CProjectMwmp::CurrentPlaylistItemAvailable( BSTR bstrItemName )
{
}

void CProjectMwmp::MediaChange( IDispatch * Item )
{
}

void CProjectMwmp::CurrentMediaItemAvailable( BSTR bstrItemName )
{
}

void CProjectMwmp::CurrentItemChange( IDispatch *pdispMedia)
{
}

void CProjectMwmp::MediaCollectionChange()
{
}

void CProjectMwmp::MediaCollectionAttributeStringAdded( BSTR bstrAttribName,  BSTR bstrAttribVal )
{
}

void CProjectMwmp::MediaCollectionAttributeStringRemoved( BSTR bstrAttribName,  BSTR bstrAttribVal )
{
}

void CProjectMwmp::MediaCollectionAttributeStringChanged( BSTR bstrAttribName, BSTR bstrOldAttribVal, BSTR bstrNewAttribVal)
{
}

void CProjectMwmp::PlaylistCollectionChange()
{
}

void CProjectMwmp::PlaylistCollectionPlaylistAdded( BSTR bstrPlaylistName)
{
}

void CProjectMwmp::PlaylistCollectionPlaylistRemoved( BSTR bstrPlaylistName)
{
}

void CProjectMwmp::PlaylistCollectionPlaylistSetAsDeleted( BSTR bstrPlaylistName, VARIANT_BOOL varfIsDeleted)
{
}

void CProjectMwmp::ModeChange( BSTR ModeName, VARIANT_BOOL NewValue)
{
}

void CProjectMwmp::MediaError( IDispatch * pMediaObject)
{
}

void CProjectMwmp::OpenPlaylistSwitch( IDispatch *pItem )
{
}

void CProjectMwmp::DomainChange( BSTR strDomain)
{
}

void CProjectMwmp::SwitchedToPlayerApplication()
{
}

void CProjectMwmp::SwitchedToControl()
{
}

void CProjectMwmp::PlayerDockedStateChange()
{
}

void CProjectMwmp::PlayerReconnect()
{
}

void CProjectMwmp::Click( short nButton, short nShiftState, long fX, long fY )
{	
}

void CProjectMwmp::DoubleClick( short nButton, short nShiftState, long fX, long fY )
{
}

void CProjectMwmp::KeyDown( short nKeyCode, short nShiftState )
{	
	
	//if(nKeyCode)nKeyCode = 4;
	globalPM->key_handler(PROJECTM_KEYDOWN,PROJECTM_K_n,PROJECTM_KMOD_RSHIFT);
}
void CProjectMwmp::KeyPress( short nKeyAscii )
{
}

void CProjectMwmp::KeyUp( short nKeyCode, short nShiftState )
{
}

void CProjectMwmp::MouseDown( short nButton, short nShiftState, long fX, long fY )
{
}

void CProjectMwmp::MouseMove( short nButton, short nShiftState, long fX, long fY )
{
}

void CProjectMwmp::MouseUp( short nButton, short nShiftState, long fX, long fY )
{
}
