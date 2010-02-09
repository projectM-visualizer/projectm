/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id: iprojectM.c,v 1.10 2004/11/07 17:29:04 cvs Exp $
 *
 * iTunes Plugin Wrapper for projectM
 */

#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <libprojectM/carbontoprojectM.hpp>
#include <libprojectM/Common.hpp>
#include <libprojectM/Renderer/BeatDetect.hpp>
#include <libprojectM/Renderer/PCM.hpp>
#include <libprojectM/projectM.hpp>

#ifdef WIN32

#include "win32/iTunesVisualAPI.h"
#else
#ifdef MACOS
#include "macos/iTunesVisualAPI.h"
#include <agl.h>

#endif /** MACOS */
#endif /** WIN32 */

#ifdef WIN32
// window information
HWND windowHandle;
HDC windowDC;
HGLRC windowRC;
#endif /** WIN32 */

#ifdef MACOS
CGrafPtr port = NULL;
AGLContext context = NULL;
int frameCount = 0;
CFDictionaryRef desktopMode = NULL;
CFDictionaryRef fullscreenMode = NULL;
int inFullScreenMode = 0;
int displayCaptured = 0;
boolean_t exactMatch;
int fullscreenWidth, fullscreenHeight;
#endif /** MACOS */

#ifdef MACOS
#define kTVisualPluginName                      "\pprojectM"
#define kTVisualPluginCreator           'hook'
#define kTVisualPluginMajorVersion      1
#define kTVisualPluginMinorVersion      0
#define kTVisualPluginReleaseStage      finalStage
#define kTVisualPluginNonFinalRelease   0
#endif /** MACOS */

/** Port dimensions */
int windowWidth;
int windowHeight;

/** projectM info */
projectM *globalPM = NULL;
#ifdef DEBUG
FILE *debugFile = NULL;
#endif

// iTunes information
RenderVisualData renderData;
int playing;
void *appCookie;
ITAppProcPtr appProc;
#ifdef WIN32
ITTrackInfoV1       trackInfo;
ITStreamInfoV1      streamInfo;
#else
#ifdef MACOS
ITTrackInfo trackInfo;
ITStreamInfo streamInfo;
FSSpec pluginSpec;
#endif /** MACOS */
#endif /** WIN32 */

#ifdef MACOS
void CleanupGL();
#endif

#ifdef MACOS
/** fss2path takes the FSSpec of the plugin and returns the filename */
void fss2path( FSSpec *fss, char *path ) {

    int l;

    for ( l = 0 ; l < (fss->name[0]) ; l++ ) {
        path[l] = fss->name[l + 1];
      }
    path[l] = '\0';

    DWRITE( "fss2path: '%s'\n", path );

    if ( fss->parID != fsRtParID) {
        int i, len;
        CInfoPBRec pb;

        pb.dirInfo.ioNamePtr = fss->name;
        pb.dirInfo.ioVRefNum = fss->vRefNum;
        pb.dirInfo.ioDrParID = fss->parID;

        do {
            pb.dirInfo.ioFDirIndex = -1;
            pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;

            if ( PBGetCatInfoSync( &pb ) != noErr ) {
                break;
              }

            len = fss->name[0] + 1;
            for ( i = l ; i >= 0 ; i-- ) path[i + len] = path[i];
            for ( i = 1 ; i < len ; i++ ) path[i - 1] = fss->name[i];

            path[i - 1] = '/';
            l += len;
          } while ( pb.dirInfo.ioDrDirID != fsRtDirID );

        len = 9;
        for ( i = l ; i >= 0 ; i-- ) path[i + len] = path[i];
        path[0] = '/';
        path[1] = 'V';
        path[2] = 'o';
        path[3] = 'l';
        path[4] = 'u';
        path[5] = 'm';
        path[6] = 'e';
        path[7] = 's';
        path[8] = '/';
      }
  }
#endif

// create OpenGL rendering context for the specified window
#ifdef WIN32
void InitializeGL(HWND hwnd)
{
    int pixelFormat;
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof (PIXELFORMATDESCRIPTOR), /* struct size      */
        1,                              /* Version number   */
        PFD_DRAW_TO_WINDOW      /* Flags, draw to a window, */
          | PFD_DOUBLEBUFFER    /* Requires Doublebuffer hw */
          | PFD_SUPPORT_OPENGL, /* use OpenGL               */
        PFD_TYPE_RGBA,          /* RGBA pixel values        */
        32,                     /* 24-bit color             */
        0, 0, 0,                /* RGB bits & shift sizes.  */
        0, 0, 0,                /* Don't care about them    */
        0, 0,                   /* No alpha buffer info     */
        0, 0, 0, 0, 0,          /* No accumulation buffer   */
        16,                     /* depth buffer             */
        1,                      /* stencil buffer           */
        0,                      /* No auxiliary buffers     */
        PFD_MAIN_PLANE,         /* Layer type               */
        0,                      /* Reserved (must be 0)     */
        0,                      /* No layer mask            */
        0,                      /* No visible mask          */
        0                       /* No damage mask           */
      };

    windowHandle = hwnd;
    windowDC = GetDC(windowHandle);

//    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pixelFormat = ChoosePixelFormat(windowDC, &pfd);
    if (pixelFormat == 0)
        return;

    if (SetPixelFormat(windowDC, pixelFormat, &pfd) == 0)
        return;

    windowRC = wglCreateContext(windowDC);
    if (windowRC == NULL)
        return;

    if (!wglMakeCurrent(windowDC, windowRC))
        return;

#ifdef PANTS
    // enable v-sync if WGL_EXT_swap_control is supported
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT != NULL)
        wglSwapIntervalEXT(1);
#endif

    /** Initialise projectM */
#ifdef DEBUG
    debugFile = fopen( "c:\\iprojectM.txt", "wb" );
#endif
    globalPM = (projectM_t *)malloc( sizeof( projectM_t ) );
    projectM_reset( globalPM );

    globalPM->fullscreen = 0;
    globalPM->usePbuffers = 1;
    globalPM->renderTarget->texsize = 1024;
    globalPM->showfps = 1;

    /** Initialise font and preset paths */
    globalPM->fontURL = (char *)malloc( sizeof( char ) * 1024 );
    strcpy( globalPM->fontURL, "c:\\Program Files\\iTunes\\Plug-ins\\projectM\\fonts" );

    globalPM->presetURL = (char *)malloc( sizeof( char ) * 1024 );
    strcpy( globalPM->presetURL, "c:\\Program Files\\iTunes\\Plug-ins\\projectM\\presets" );

    projectM_init( globalPM );
}
#else
#ifdef MACOS
void InitializeGL( CGrafPtr destPort, int isFullScreen ) {

    AGLPixelFormat pixelFormat;
    GLint attrib[64];

    DWRITE( "InitializeGL(): in: %d\n", isFullScreen );

    /** Stash the target port */
    port = destPort;

#ifdef PANTS
    if ( isFullScreen ) {
        CGCaptureAllDisplays();
        displayCaptured = 1;
        if ( fullscreenMode != NULL ) {
            if ( CGDisplaySwitchToMode( kCGDirectMainDisplay, fullscreenMode ) != CGDisplayNoErr ) {
                DWRITE( "%s\n", "failed to switch display to fullscreen" );
              } else {
                DWRITE( "%s\n", "display switch to fullscreen ok\n" );
              }
          }
      } else {
        /** Switch back to the previous desktop mode */
        if ( desktopMode != NULL ) {
            if ( CGDisplaySwitchToMode( kCGDirectMainDisplay, desktopMode ) != CGDisplayNoErr ) {
                DWRITE( "%s\n", "failed to switch display" );
              } else {
                DWRITE( "%s\n", "display switch OK" );
              }
          }
      }
#endif

    /** Chuck out the old context if one exists */
    if ( context != NULL ) {
        aglDestroyContext( context );
        context = NULL;
      }

    /** Choose the OpenGL pixel format */
#ifdef PANTS
    if ( isFullScreen ) {
        attrib[0] = AGL_RGBA;
        attrib[1] = AGL_DOUBLEBUFFER;
        attrib[2] = AGL_PIXEL_SIZE;
        attrib[3] = 32;
        attrib[4] = AGL_FULLSCREEN;
        attrib[5] = AGL_NO_RECOVERY;
        attrib[6] = AGL_SINGLE_RENDERER;
        attrib[6] = AGL_NONE;
      } else {
#endif
        attrib[0] = AGL_RGBA;
        attrib[1] = AGL_DOUBLEBUFFER;
        attrib[2] = AGL_ACCELERATED;
        attrib[3] = AGL_PIXEL_SIZE;
        attrib[4] = 32;
        attrib[5] = AGL_NO_RECOVERY;
        attrib[6] = AGL_SINGLE_RENDERER;
        attrib[5] = AGL_NONE;
//      }

    GDHandle mainDevice = GetMainDevice();
    pixelFormat = aglChoosePixelFormat( &mainDevice, 1, attrib );
    if ( pixelFormat == 0 ) {
        DWRITE( "failed to select pixel format\n" );
        exit( 1 );
        CleanupGL();
        isFullScreen = 0;
        return;
      } else {
        DWRITE( "selected pixel format OK\n" );
      }

    context = aglCreateContext( pixelFormat, NULL );
    if ( context == NULL ) {
        DWRITE( "failed to create context\n" );
      } else {
        DWRITE( "created context OK\n" );
      }

//    if ( !isFullScreen ) {
        if ( !aglSetDrawable( context, destPort ) ) {
            DWRITE( "failed to set drawable\n" );
          } else {
            DWRITE( "set drawable OK\n" );
          }
//      } else {
//        aglSetDrawable( context, NULL );
//      }

    if ( !aglSetCurrentContext( context ) ) {
        DWRITE( "failed to make context current\n" );
      } else {
        DWRITE( "set current context OK\n" );
      }

//    if ( !isFullScreen ) {
        if ( !aglUpdateContext( context ) ) {
            DWRITE( "failed to update context\n" );
          } else {
            DWRITE( "updated context OK\n" );
          }

        if ( globalPM != NULL ) {
            globalPM->fullscreen = 0;
          }
#ifdef PANTS
      } else {
        aglSetFullScreen( context, 800, 600, 0, 0 );
        if ( globalPM != NULL ) {
            globalPM->fullscreen = 1;
          }
        GLint displayCaps[3];
        aglGetInteger( context, AGL_FULLSCREEN, displayCaps );
        DWRITE( "dcaps: %d\t%d\t%d\n", 
                displayCaps[0], displayCaps[1], displayCaps[2] );
        fullscreenWidth = displayCaps[0];
        fullscreenHeight = displayCaps[1];
      }
#endif

#ifdef WIN32
    // enable v-sync if WGL_EXT_swap_control is supported
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT != NULL)
        wglSwapIntervalEXT(1);
#endif
#ifdef MACOS
    /** Setup VSYNC */
    GLint sync = 1;
    aglSetInteger ( context, AGL_SWAP_INTERVAL, &sync );
#endif

    /** Initialise projectM */
    globalPM->projectM_init();
  }

#endif /** MACOS */
#endif /** WIN32 */

// render visualization data
void RenderGL()
{
#ifdef WIN32
    if (windowDC == NULL || globalPM == NULL ) return;
#else
#ifdef MACOS
    if ( port == NULL || context == NULL ) {
        return;
      }
#endif /** MACOS */
#endif /** WIN32 */

    /** Stuff the PCM data into projectM */
    globalPM->beatDetect->pcm->addPCM8( renderData.waveformData );

    /** Render a frame via projectM */
    globalPM->renderFrame();

#if defined(DEBUG2) && defined(MACOS)
    fprintf( debugFile, "port: %X\tcontext: %X\n", port, context );
    fflush( debugFile );
#endif

#ifdef WIN32
    /** Buffer swap to display the results */
    SwapBuffers( windowDC );
#else
#ifdef MACOS
    aglSwapBuffers( context );
#endif /** MACOS */
#endif /** WIN32 */
  }

// resize rendering viewport
void ResizeGL( CGrafPtr destPort, Rect windowRect, int isFullScreen ) {

#ifdef MACOS
    GLint bufferRect[4];
#endif

    DWRITE( "ResizeGL(): in: %d\n", isFullScreen );

#ifdef PANTS
    if ( isFullScreen ) {
#ifdef MACOS
        aglDisable( context, AGL_BUFFER_RECT );
#endif /** MACOS */

        /** Reset projectM */
        if ( globalPM != NULL ) {
            globalPM->projectM_resetGL( fullscreenWidth, fullscreenHeight );
          }
      } else {
#endif
        windowWidth = windowRect.right - windowRect.left;
        windowHeight = windowRect.bottom - windowRect.top;

        if (windowHeight == 0) windowHeight = 1;

        /** Re-initialise OpenGL */
        if ( globalPM != NULL ) {
#ifdef MACOS
            /** 
             * Grab the dimensions of the main window itself. This is required
             * to accurately locate the port since OpenGL is "upside-down" from
             * Carbon window coordinates...
             */
            WindowRef itWindowRef;
            Rect mwindowRect;
            itWindowRef = GetWindowFromPort(port);
            GetWindowPortBounds(itWindowRef, &mwindowRect);

            DWRITE( "rect: (%d, %d) -> (%d, %d)\n",
                     windowRect.left, windowRect.top,
                     windowRect.right, windowRect.bottom );
            DWRITE( "window-rect: (%d, %d) -> (%d, %d)\n",
                     mwindowRect.left, mwindowRect.top,
                     mwindowRect.right, mwindowRect.bottom );

            /** Update the buffer rect */
            bufferRect[0] = windowRect.left;
            bufferRect[1] = mwindowRect.bottom - windowRect.bottom;
            bufferRect[2] = windowWidth;
            bufferRect[3] = windowHeight;

            aglSetInteger( context, AGL_BUFFER_RECT, bufferRect );
            aglEnable( context, AGL_BUFFER_RECT );

            if ( !aglUpdateContext( context ) ) {
                DWRITE( "failed to update context\n" );
              } else {
                DWRITE( "updated context OK\n" );
              }
#endif
            globalPM->projectM_resetGL( windowWidth, windowHeight );
          }
//      }
  }

// cleanup OpenGL rendering context
void CleanupGL()
{
#ifdef WIN32
    wglMakeCurrent(NULL, NULL);

    if (windowRC != NULL)
    {
        wglDeleteContext(windowRC);
        windowRC = NULL;
    }

    if (windowDC)
    {
        ReleaseDC(windowHandle, windowDC);
        windowDC = NULL;
    }
#else
#ifdef MACOS
    /** Release the held main display */
#ifdef PANTS
    if ( displayCaptured ) {
        DWRITE( "%s\n", "switching to non-fullscreen" );
        CGDisplaySwitchToMode( kCGDirectMainDisplay, desktopMode );

        DWRITE( "%s\n", "releasing displays" );
        CGReleaseAllDisplays();
      }
#endif
    DWRITE( "pre-context destroy\n" );
    if ( context != NULL ) {
        aglSetCurrentContext( NULL );
        aglDestroyContext( context );
        context = NULL;
      }
#endif /** MACOS */
#endif /** WIN32 */
  }

// handle messages sent by iTunes
OSStatus pluginMessageHandler( OSType message, 
                               VisualPluginMessageInfo *messageInfo,
                               void *refCon) {

    OSStatus status = noErr;

    switch (message) {
        // sent when the plugin is loaded/unloaded
        case kVisualPluginInitMessage: {
            DWRITE( "*** %s\n", "kVisualPluginInitMessage" );
            break;
          }
        case kVisualPluginCleanupMessage: {
            DWRITE( "*** %s\n", "kVisualPluginCleanupMessage" );
//            CleanupGL();
            break;
          }
        // sent when plugin is enabled/disabled, plugin should handle these messages and do nothing
        case kVisualPluginEnableMessage: {
            DWRITE( "*** %s\n", "kVisualPluginEnableMessage" );
            break;
          }
        case kVisualPluginDisableMessage: {
            DWRITE( "*** %s\n", "kVisualPluginDisableMessage" );
            break;
          }

        // redraw the screne while idle
        case kVisualPluginIdleMessage: {
            DWRITE( "*** %s\n", "kVisualPluginIdleMessage" );
            if (playing == false) {
                RenderGL();
              }

            break;
          }

        // sent when the visualizer is shown
        case kVisualPluginShowWindowMessage: {
            DWRITE( "*** kVisualPluginShowWindowMessage( %d )\n",
                     messageInfo->u.showWindowMessage.options );
#ifdef WIN32
            InitializeGL(messageInfo->u.showWindowMessage.window,
                         messageInfo->u.showWindowMessage.options );
#else
#ifdef MACOS
            InitializeGL( messageInfo->u.showWindowMessage.port,
                          /* messageInfo->u.showWindowMessage.options */ 0 );
#endif /** MACOS */
#endif /** WIN32 */
            ResizeGL( messageInfo->u.showWindowMessage.port,
                      messageInfo->u.setWindowMessage.drawRect,
                      /* messageInfo->u.setWindowMessage.options */ 0 );
            RenderGL();
        
            break;
          }

        // sent when the visualizer is hidden
        case kVisualPluginHideWindowMessage: {
            DWRITE( "*** %s\n", "kVisualPluginHideWindowMessage" );
            CleanupGL();
            break;
          }

#ifdef PANTS
        // sent when visualizer viewport size is changed
        case kVisualPluginSetWindowMessage: {
            DWRITE( "*** kVisualPluginSetWindowMessage( %d )\n",
                    messageInfo->u.setWindowMessage.options );
            ResizeGL(messageInfo->u.setWindowMessage.port,
                     messageInfo->u.setWindowMessage.drawRect,
                     messageInfo->u.setWindowMessage.options);
            RenderGL();

            break;
          }
#endif

        // sent when visualizer should render a frame
        case kVisualPluginRenderMessage: {
            DWRITE( "*** %s\n", "kVisualPluginRenderMessage" );
            renderData = *messageInfo->u.renderMessage.renderData;
            RenderGL();

            break;
          }

        // sent when visualizer should update itself
        case kVisualPluginUpdateMessage: {
            DWRITE( "*** %s\n", "kVisualPluginUpdateMessage" );
            RenderGL();
            break;
          }

        // sent when player is stopped or paused
        case kVisualPluginStopMessage: {
            DWRITE( "*** %s\n", "kVisualPluginStopMessage" );
            break;
          }
        case kVisualPluginPauseMessage: {
            DWRITE( "*** %s\n", "kVisualPluginPauseMessage" );
            playing = false;
            break;
          }

        // sent when player is started or unpaused
        case kVisualPluginPlayMessage: {
            DWRITE( "*** %s\n", "kVisualPluginPlayMessage" );
            break;
          }
        case kVisualPluginUnpauseMessage: {
            DWRITE( "*** %s\n", "kVisualPluginPauseMessage" );
            /** Grab the track or stream title from iTunes */
            if (messageInfo->u.changeTrackMessage.trackInfo != nil) {
                trackInfo = *messageInfo->u.changeTrackMessage.trackInfo;
              } else {
                memset( &trackInfo, sizeof( trackInfo ) * sizeof( char ), 0 );
              }

            if (messageInfo->u.changeTrackMessage.streamInfo != nil) {
                streamInfo = *messageInfo->u.changeTrackMessage.streamInfo;
              } else {
                memset( &streamInfo, sizeof( streamInfo ) * sizeof( char ), 0 );
              }

            /** Pass this info into projectM */
            if ( globalPM != NULL ) {
                globalPM->drawtitle = 1;
                if ( strlen( (const char *)trackInfo.name ) > 0 ) {
                    globalPM->projectM_setTitle( (char *)trackInfo.name );
                  } else {
                    if ( strlen( (const char *)streamInfo.streamTitle ) > 0 ) {
                        globalPM->projectM_setTitle( (char *)streamInfo.streamTitle );
                      }
                  }
              }

            playing = true;

            break;
          }

        /** Sent when the track changes */
        case kVisualPluginChangeTrackMessage: {
            DWRITE( "*** %s\n", "kVisualPluginChangeTrackMessage" );
            /** Grab the track or stream title from iTunes */
            if (messageInfo->u.changeTrackMessage.trackInfo != nil) {
                trackInfo = *messageInfo->u.changeTrackMessage.trackInfo;
              } else {
                memset( &trackInfo, sizeof( trackInfo ) * sizeof( char ), 0 );
              }

            if (messageInfo->u.changeTrackMessage.streamInfo != nil) {
                streamInfo = *messageInfo->u.changeTrackMessage.streamInfo;
              } else {
                memset( &streamInfo, sizeof( streamInfo ) * sizeof( char ), 0 );
              }

            /** Pass this info into projectM */
            globalPM->drawtitle = 1;
            if ( strlen( (const char *)trackInfo.name ) > 0 ) {
                globalPM->projectM_setTitle( (char *)trackInfo.name );
              } else {
                if ( strlen( (const char *)streamInfo.streamTitle ) > 0 ) {
                    globalPM->projectM_setTitle( (char *)streamInfo.streamTitle );
                  }
              }
            RenderGL();
            break;
          }
        case kVisualPluginEventMessage: {
            projectMEvent event;
            projectMKeycode keycode;
            projectMModifier mod;
            DWRITE( "*** %s\n", "kVisualPluginEventMessage" );
#ifdef MACOS
            event = carbon2pmEvent( messageInfo->u.eventMessage.event );
            keycode = carbon2pmKeycode( messageInfo->u.eventMessage.event );
#else
#ifdef WIN32
#endif /** WIN32 */
#endif /** MACOS */
            DWRITE( "keycode: %d\n", keycode );
            if ( keycode == ' ' ) {
                status = unimpErr;
              }
            globalPM->key_handler( event, keycode, mod );
            break;
          }
        default: {
            status = unimpErr;
            break;
          }
    }

    return status;
}

// register plugin with iTunes
OSStatus registerPlugin(PluginMessageInfo *messageInfo) {

    OSStatus status;

    // plugin constants
#ifdef WIN32
    const char *pluginTitle = "projectM";
    const UInt32 pluginCreator = '\?\?\?\?';
#else
#ifdef MACOS
    const char *pluginTitle = "projectM";
    const UInt32 pluginCreator = 'hook';
#endif /** MACOS */
#endif /** WIN32 */
    const UInt8 pluginMajorVersion = 10;
    const UInt8 pluginMinorVersion = 9;

    PlayerMessageInfo playerMessageInfo;
    memset( &playerMessageInfo.u.registerVisualPluginMessage, 
            0, sizeof(playerMessageInfo.u.registerVisualPluginMessage));

    // copy in name length byte first
    playerMessageInfo.u.registerVisualPluginMessage.name[0] =
        (UInt8)strlen( pluginTitle );

    // now copy in actual name
    memcpy( &playerMessageInfo.u.registerVisualPluginMessage.name[1],
            pluginTitle, strlen( pluginTitle ));

    SetNumVersion( &playerMessageInfo.u.registerVisualPluginMessage.pluginVersion,
                   pluginMajorVersion, pluginMinorVersion, 0x80, 0);

    playerMessageInfo.u.registerVisualPluginMessage.options =
        kVisualWantsIdleMessages;
#ifdef WIN32
    playerMessageInfo.u.registerVisualPluginMessage.handler =
        pluginMessageHandler;
#else
    playerMessageInfo.u.registerVisualPluginMessage.handler =
        (VisualPluginProcPtr)pluginMessageHandler;
#endif
    playerMessageInfo.u.registerVisualPluginMessage.registerRefCon = 0;
    playerMessageInfo.u.registerVisualPluginMessage.creator = pluginCreator;

    playerMessageInfo.u.registerVisualPluginMessage.timeBetweenDataInMS = 0xFFFFFFFF; // 16 milliseconds = 1 Tick, 0xFFFFFFFF = Often as possible.
    playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels = 2;
    playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels = 2;

    playerMessageInfo.u.registerVisualPluginMessage.minWidth = 64;
    playerMessageInfo.u.registerVisualPluginMessage.minHeight = 64;
    playerMessageInfo.u.registerVisualPluginMessage.maxWidth = 32767;
    playerMessageInfo.u.registerVisualPluginMessage.maxHeight = 32767;
    playerMessageInfo.u.registerVisualPluginMessage.minFullScreenBitDepth = 0;
    playerMessageInfo.u.registerVisualPluginMessage.maxFullScreenBitDepth = 0;
    playerMessageInfo.u.registerVisualPluginMessage.windowAlignmentInBytes = 0;

    status = PlayerRegisterVisualPlugin(messageInfo->u.initMessage.appCookie,
                                        messageInfo->u.initMessage.appProc,
                                        &playerMessageInfo);

    appCookie = messageInfo->u.initMessage.appCookie;
    appProc = messageInfo->u.initMessage.appProc;

    return status;
}

#ifdef WIN32
#define IMPEXP __declspec(dllexport)
#define MAIN iTunesPluginMain
#else
#ifdef MACOS
#define IMPEXP 
#define MAIN iTunesPluginMainMachO
#endif /** MACOS */
#endif /** WIN32 */

#ifdef MACOS
extern "C"
#endif
IMPEXP OSStatus MAIN (OSType message, PluginMessageInfo *messageInfo, void *refCon)
{
    OSStatus status = noErr;
    char fname[1024];
    char fontURL[1024];
    char presetURL[1024];

    switch (message) {
        case kPluginInitMessage: {
#ifdef DEBUG
#ifdef MACOS
            debugFile = fopen( "/Users/descarte/iprojectM.txt", "wb" );
            fprintf( debugFile, "registerPlugin: %d\n", status );
            fflush( debugFile );
#endif /** MACOS */
#endif /** DEBUG */

            DWRITE( "%s: %d\t%d\n", 
                    "kPluginInitMessage",
                    messageInfo->u.initMessage.majorVersion, 
                    messageInfo->u.initMessage.minorVersion );
            status = registerPlugin(messageInfo);
            if ( status == noErr ) {
#ifdef MACOS
                desktopMode = CGDisplayCurrentMode( kCGDirectMainDisplay );
                fullscreenMode = 
                    CGDisplayBestModeForParameters( kCGDirectMainDisplay,
                                                    32, 800, 600, NULL );
#ifdef DEBUG
                if ( debugFile != NULL ) {
                    long dwidth = 0, dheight = 0;
                    /** Get the desktop mode */
                    /* Get the height, width, bpp, and refresh rate of this display mode. Print them out. */
                    CFNumberRef number ;
                    number = (CFNumberRef)CFDictionaryGetValue( desktopMode, kCGDisplayWidth ) ;
                    CFNumberGetValue( number, kCFNumberLongType, &dwidth ) ;
                    number = (CFNumberRef)CFDictionaryGetValue( desktopMode, kCGDisplayHeight ) ;
                    CFNumberGetValue( number, kCFNumberLongType, &dheight ) ;

                    DWRITE( "stashing desktop mode: %d x %d -> %d\n",
                            dwidth, dheight, exactMatch );
                  }
#endif
#endif

                /** Initialise projectM */
		if (globalPM != 0)
			delete(globalPM);
                globalPM = new projectM();
                globalPM->projectM_reset();
                /** Set basic rendering options */
                globalPM->renderTarget->usePbuffers = 1;
                globalPM->fullscreen = 0;
                globalPM->renderTarget->texsize = 1024;
                globalPM->showfps = 1;

                /** Get the font and preset locations */
                PlayerGetPluginFileSpec( appCookie, appProc, &pluginSpec );
                fss2path( &pluginSpec, fname );
                DWRITE( "fsspace name: %s\n", fname );
                sprintf( fontURL, "%s/Contents/Resources/fonts", fname );
                globalPM->fontURL = (char *)malloc( sizeof( char ) * 1024 );
                strncpy( globalPM->fontURL, fontURL, strlen( fontURL ) + 1 );
                DWRITE( "fontURL: %s\n", globalPM->fontURL );

                sprintf( presetURL, "%s/Contents/Resources/presets", fname );
                globalPM->presetURL = (char *)malloc( sizeof( char ) * 1024 );
                strncpy( globalPM->presetURL, presetURL, strlen( presetURL ) + 1 );

                DWRITE( "presetURL: %s\n", globalPM->presetURL );
              }

            break;
          }
        case kPluginCleanupMessage: {
            DWRITE( "%s\n", "kPluginCleanupMessage" );
#ifdef DEBUG
            if ( debugFile != NULL ) {
                fclose( debugFile );
              }
#endif
            status = noErr;
            break;
          }
        default: {
            status = unimpErr;
            break;
          }
    }

    DWRITE( "status: %d\n", status );

    return status;
}
