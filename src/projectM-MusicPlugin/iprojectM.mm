// cross-platform iTunes plugin code, from the iTunes Visual SDK
// with additions for projectM support


#include "iprojectM.hpp"
#include <string.h>


// projectM
void initProjectM( VisualPluginData * visualPluginData, std::string presetPath ) {
//    std::string config_filename = getConfigFilename();
    std::string cfg_path = "/usr/local/share/projectM/config.inp";

    // hardcoded settings - disabled
    projectM::Settings settings;
    settings.meshX = 140;
    settings.meshY = 110;
    settings.fps   = 60;
    settings.textureSize = 2048;  // idk?
    settings.windowWidth = 1920;
    settings.windowHeight = 1280;
    settings.smoothPresetDuration = 0.5; // seconds
    settings.aspectCorrection = 1;
    settings.easterEgg = 0; // ???
    settings.presetDuration = 15; // seconds
    settings.beatSensitivity = 3;
    settings.shuffleEnabled = 1;
    settings.softCutRatingsEnabled = 1; // ???

    settings.presetURL = presetPath;
    settings.titleFontURL = "/usr/local/share/projectM/fonts/Vera.ttf";
    settings.menuFontURL = "/usr/local/share/projectM/fonts/VeraMono.ttf";
    projectM *pm = new projectM(settings, 0);
    
    NSLog(@"GL_VERSION: %s", glGetString(GL_VERSION));
    NSLog(@"GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    NSLog(@"GL_VENDOR: %s", glGetString(GL_VENDOR));

    // use config file
//    projectM *pm = new projectM(cfg_path);

    visualPluginData->pm = pm;
    
    pm->selectRandom(true);
    NSLog(@"random selected");

}

void keypressProjectM( VisualPluginData * visualPluginData, projectMEvent event, projectMKeycode keycode, projectMModifier mod ) {
    visualPluginData->pm->key_handler(event, keycode, mod);
}

void renderProjectMTexture( VisualPluginData * visualPluginData ){
    // this needs to be updated for gl3 (see SDL version)
#if 0
    static int textureHandle = visualPluginData->pm->initRenderToTexture();
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 2, 10);
    
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glColor4d(1.0, 1.0, 1.0, 1.0);
    
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1);
    glVertex3d(-0.8, 0.8, 0);
    glTexCoord2d(0, 0);
    glVertex3d(-0.8, -0.8, 0);
    glTexCoord2d(1, 0);
    glVertex3d(0.8, -0.8, 0);
    glTexCoord2d(1, 1);
    glVertex3d(0.8, 0.8, 0);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_DEPTH_TEST);
#endif
}

//-------------------------------------------------------------------------------------------------
// ProcessRenderData
//-------------------------------------------------------------------------------------------------
//
void ProcessRenderData( VisualPluginData * visualPluginData, UInt32 timeStampID, const RenderVisualData * renderData )
{
	SInt16		index;
	SInt32		channel;
    projectM    *pm = visualPluginData->pm;

	visualPluginData->renderTimeStampID	= timeStampID;

	if ( renderData == NULL )
	{
		memset( &visualPluginData->renderData, 0, sizeof(visualPluginData->renderData) );
		return;
	}

	visualPluginData->renderData = *renderData;
	
	for ( channel = 0;channel < renderData->numSpectrumChannels; channel++ )
	{
		visualPluginData->minLevel[channel] = 
			visualPluginData->maxLevel[channel] = 
			renderData->spectrumData[channel][0];

		for ( index = 1; index < kVisualNumSpectrumEntries; index++ )
		{
			UInt8		value;
			
			value = renderData->spectrumData[channel][index];

			if ( value < visualPluginData->minLevel[channel] )
				visualPluginData->minLevel[channel] = value;
			else if ( value > visualPluginData->maxLevel[channel] )
				visualPluginData->maxLevel[channel] = value;
		}
	}
    
    if (pm != NULL) {
        // pass waveform data to projectM
        pm->pcm()->addPCM8_512(renderData->waveformData);
    }
}

//-------------------------------------------------------------------------------------------------
//	ResetRenderData
//-------------------------------------------------------------------------------------------------
//
void ResetRenderData( VisualPluginData * visualPluginData )
{
	memset( &visualPluginData->renderData, 0, sizeof(visualPluginData->renderData) );
	memset( visualPluginData->minLevel, 0, sizeof(visualPluginData->minLevel) );
}

//-------------------------------------------------------------------------------------------------
//	UpdateInfoTimeOut
//-------------------------------------------------------------------------------------------------
//
void UpdateInfoTimeOut( VisualPluginData * visualPluginData )
{
	// reset the timeout value we will use to show the info/artwork if we have it during DrawVisual()
	visualPluginData->drawInfoTimeOut = time( NULL ) + kInfoTimeOutInSeconds;
}

//-------------------------------------------------------------------------------------------------
//	UpdatePulseRate
//-------------------------------------------------------------------------------------------------
//
void UpdatePulseRate( VisualPluginData * visualPluginData, UInt32 * ioPulseRate )
{
	// vary the pulse rate based on whether or not iTunes is currently playing
	if ( visualPluginData->playing )
		*ioPulseRate = kPlayingPulseRateInHz;
	else
		*ioPulseRate = kStoppedPulseRateInHz;
}

//-------------------------------------------------------------------------------------------------
//	UpdateTrackInfo
//-------------------------------------------------------------------------------------------------
//
void UpdateTrackInfo( VisualPluginData * visualPluginData, ITTrackInfo * trackInfo, ITStreamInfo * streamInfo )
{
	if ( trackInfo != NULL )
		visualPluginData->trackInfo = *trackInfo;
	else
		memset( &visualPluginData->trackInfo, 0, sizeof(visualPluginData->trackInfo) );

	if ( streamInfo != NULL )
		visualPluginData->streamInfo = *streamInfo;
	else
		memset( &visualPluginData->streamInfo, 0, sizeof(visualPluginData->streamInfo) );

	UpdateInfoTimeOut( visualPluginData );
}

//-------------------------------------------------------------------------------------------------
//	RequestArtwork
//-------------------------------------------------------------------------------------------------
//
static void RequestArtwork( VisualPluginData * visualPluginData )
{
	// only request artwork if this plugin is active
	if ( visualPluginData->destView != NULL )
	{
		OSStatus		status;

		status = PlayerRequestCurrentTrackCoverArt( visualPluginData->appCookie, visualPluginData->appProc );
	}
}

//-------------------------------------------------------------------------------------------------
//	PulseVisual
//-------------------------------------------------------------------------------------------------
//
void PulseVisual( VisualPluginData * visualPluginData, UInt32 timeStampID, const RenderVisualData * renderData, UInt32 * ioPulseRate )
{
	// update internal state
	ProcessRenderData( visualPluginData, timeStampID, renderData );

	// if desired, adjust the pulse rate
	UpdatePulseRate( visualPluginData, ioPulseRate );
}

//-------------------------------------------------------------------------------------------------
//	VisualPluginHandler
//-------------------------------------------------------------------------------------------------
//
static OSStatus VisualPluginHandler(OSType message,VisualPluginMessageInfo *messageInfo, void *refCon)
{
	OSStatus			status;
	VisualPluginData *	visualPluginData;

	visualPluginData = (VisualPluginData*) refCon;
	
	status = noErr;

	switch ( message )
	{
		/*
			Sent when the visual plugin is registered.  The plugin should do minimal
			memory allocations here.
		*/		
		case kVisualPluginInitMessage:
		{
			visualPluginData = (VisualPluginData *)calloc( 1, sizeof(VisualPluginData) );
			if ( visualPluginData == NULL )
			{
				status = memFullErr;
				break;
			}

			visualPluginData->appCookie	= messageInfo->u.initMessage.appCookie;
			visualPluginData->appProc	= messageInfo->u.initMessage.appProc;

			messageInfo->u.initMessage.refCon = (void *)visualPluginData;
			break;
		}
		/*
			Sent when the visual plugin is unloaded.
		*/		
		case kVisualPluginCleanupMessage:
		{
			if ( visualPluginData != NULL ) {
                if (visualPluginData->pm) {
                    delete(visualPluginData->pm);
                    visualPluginData->pm = NULL;
                }
				free( visualPluginData );
            }
			break;
		}
		/*
			Sent when the visual plugin is enabled/disabled.  iTunes currently enables all
			loaded visual plugins at launch.  The plugin should not do anything here.
		*/
		case kVisualPluginEnableMessage:
		case kVisualPluginDisableMessage:
		{
			break;
		}
		/*
			Sent if the plugin requests idle messages.  Do this by setting the kVisualWantsIdleMessages
			option in the RegisterVisualMessage.options field.
			
			DO NOT DRAW in this routine.  It is for updating internal state only.
		*/
		case kVisualPluginIdleMessage:
		{
			break;
		}			
		/*
			Sent if the plugin requests the ability for the user to configure it.  Do this by setting
			the kVisualWantsConfigure option in the RegisterVisualMessage.options field.
		*/
		case kVisualPluginConfigureMessage:
		{
			status = ConfigureVisual( visualPluginData );
			break;
		}
		/*
			Sent when iTunes is going to show the visual plugin.  At this
			point, the plugin should allocate any large buffers it needs.
		*/
		case kVisualPluginActivateMessage:
		{
			status = ActivateVisual( visualPluginData, messageInfo->u.activateMessage.view, messageInfo->u.activateMessage.options );

			// note: do not draw here if you can avoid it, a draw message will be sent as soon as possible
			
			if ( status == noErr )
				RequestArtwork( visualPluginData );
			break;
		}	
		/*
			Sent when this visual is no longer displayed.
		*/
		case kVisualPluginDeactivateMessage:
		{
			UpdateTrackInfo( visualPluginData, NULL, NULL );

			status = DeactivateVisual( visualPluginData );
			break;
		}
		/*
			Sent when iTunes is moving the destination view to a new parent window (e.g. to/from fullscreen).
		*/
		case kVisualPluginWindowChangedMessage:
		{
			status = MoveVisual( visualPluginData, messageInfo->u.windowChangedMessage.options );
			break;
		}
		/*
			Sent when iTunes has changed the rectangle of the currently displayed visual.
			
			Note: for custom NSView subviews, the subview's frame is automatically resized.
		*/
		case kVisualPluginFrameChangedMessage:
		{
			status = ResizeVisual( visualPluginData );
			break;
		}
		/*
			Sent for the visual plugin to update its internal animation state.
			Plugins are allowed to draw at this time but it is more efficient if they
			wait until the kVisualPluginDrawMessage is sent OR they simply invalidate
			their own subview.  The pulse message can be sent faster than the system
			will allow drawing to support spectral analysis-type plugins but drawing
			will be limited to the system refresh rate.
		*/
		case kVisualPluginPulseMessage:
		{
			PulseVisual( visualPluginData,
						 messageInfo->u.pulseMessage.timeStampID,
						 messageInfo->u.pulseMessage.renderData,
						 &messageInfo->u.pulseMessage.newPulseRateInHz );

            // Invalidate visual seems to lag a few frames behind, so let's draw as soon as possible
            DrawVisual( visualPluginData );
//            InvalidateVisual( visualPluginData );
			break;
		}
		/*
			It's time for the plugin to draw a new frame.
			
			For plugins using custom subviews, you should ignore this message and just
			draw in your view's draw method.  It will never be called if your subview 
			is set up properly.
		*/
		case kVisualPluginDrawMessage:
		{
#if !USE_SUBVIEW
            // Now drawing in kVisualPluginPulseMessage  -revmischa 09/14
            DrawVisual( visualPluginData );
#endif
			break;
		}
		/*
			Sent when the player starts.
		*/
		case kVisualPluginPlayMessage:
		{
			visualPluginData->playing = true;
			
			UpdateTrackInfo( visualPluginData, messageInfo->u.playMessage.trackInfo, messageInfo->u.playMessage.streamInfo );
		
			RequestArtwork( visualPluginData );
			
			InvalidateVisual( visualPluginData );
			break;
		}
		/*
			Sent when the player changes the current track information.  This
			is used when the information about a track changes.
		*/
		case kVisualPluginChangeTrackMessage:
		{
			UpdateTrackInfo( visualPluginData, messageInfo->u.changeTrackMessage.trackInfo, messageInfo->u.changeTrackMessage.streamInfo );

			RequestArtwork( visualPluginData );
				
			InvalidateVisual( visualPluginData );
			break;
		}
		/*
			Artwork for the currently playing song is being delivered per a previous request.
			
			Note that NULL for messageInfo->u.coverArtMessage.coverArt means the currently playing song has no artwork.
		*/
		case kVisualPluginCoverArtMessage:
		{
			UpdateArtwork(	visualPluginData,
							messageInfo->u.coverArtMessage.coverArt,
							messageInfo->u.coverArtMessage.coverArtSize,
							messageInfo->u.coverArtMessage.coverArtFormat );
			
			InvalidateVisual( visualPluginData );
			break;
		}
		/*
			Sent when the player stops or pauses.
		*/
		case kVisualPluginStopMessage:
		{
			visualPluginData->playing = false;
			
			ResetRenderData( visualPluginData );

			InvalidateVisual( visualPluginData );
			break;
		}
		/*
			Sent when the player changes the playback position.
		*/
		case kVisualPluginSetPositionMessage:
		{
			break;
		}
		default:
		{
			status = unimpErr;
			break;
		}
	}

	return status;	
}

//-------------------------------------------------------------------------------------------------
//	RegisterVisualPlugin
//-------------------------------------------------------------------------------------------------
//
OSStatus RegisterVisualPlugin( PluginMessageInfo * messageInfo )
{
	PlayerMessageInfo	playerMessageInfo;
	OSStatus			status;
		
	memset( &playerMessageInfo.u.registerVisualPluginMessage, 0, sizeof(playerMessageInfo.u.registerVisualPluginMessage) );

	GetVisualName( playerMessageInfo.u.registerVisualPluginMessage.name );

	SetNumVersion( &playerMessageInfo.u.registerVisualPluginMessage.pluginVersion, kTVisualPluginMajorVersion, kTVisualPluginMinorVersion, kTVisualPluginReleaseStage, kTVisualPluginNonFinalRelease );

	playerMessageInfo.u.registerVisualPluginMessage.options					= GetVisualOptions();
	playerMessageInfo.u.registerVisualPluginMessage.handler					= (VisualPluginProcPtr)VisualPluginHandler;
	playerMessageInfo.u.registerVisualPluginMessage.registerRefCon			= 0;
	playerMessageInfo.u.registerVisualPluginMessage.creator					= kTVisualPluginCreator;
	
	playerMessageInfo.u.registerVisualPluginMessage.pulseRateInHz			= kStoppedPulseRateInHz;	// update my state N times a second
	playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels		= 2;
	playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels		= 2;
	
	playerMessageInfo.u.registerVisualPluginMessage.minWidth				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.minHeight				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.maxWidth				= 0;	// no max width limit
	playerMessageInfo.u.registerVisualPluginMessage.maxHeight				= 0;	// no max height limit
	
	status = PlayerRegisterVisualPlugin( messageInfo->u.initMessage.appCookie, messageInfo->u.initMessage.appProc, &playerMessageInfo );
    
	return status;
}
