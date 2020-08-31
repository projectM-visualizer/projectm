/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
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
 * $Id: wxvisCanvas.cpp,v 1.5 2004/11/12 15:47:27 cvs Exp $
 *
 * Handles all the OpenGL drawing in the main wxvis display
 */

#include <wx/wx.h>
#ifdef WIN32
#include <windows.h>
#endif /** WIN32 */
#include <math.h>
#include <projectM-engine/wipemalloc.h>

#include <projectM-engine/BeatDetect.hpp>
#include <projectM-engine/PCM.hpp>
#include <projectM-engine/Preset.hpp>

#include "wxvisApp.h"
#include "wxvisFrame.h"
#include "wxvisCanvas.h"

/** Event handler table */
BEGIN_EVENT_TABLE(wxvisCanvas,wxGLCanvas)
    EVT_SIZE(wxvisCanvas::OnSize)
    EVT_PAINT(wxvisCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(wxvisCanvas::OnEraseBackground)
    EVT_KEY_DOWN(wxvisCanvas::OnKeyPress)
END_EVENT_TABLE()

/** Create the OpenGL canvas */
wxvisCanvas::wxvisCanvas( wxFrame *parent, const wxWindowID id,
                          const wxPoint &pos, const wxSize &dims,
                          long style, const wxString &name ) :
                            wxGLCanvas( parent, (wxGLCanvas *)NULL, id,
                                        pos, dims, style, name ) {

    _parent = (wxvisFrame *)parent;
    _hasInitialised = 0;
    
    pm = NULL;

    /** Create a semaphore to regulate thread access to the refresh function */
#ifdef WIN32
    refreshSemaphore =
        CreateSemaphore( NULL, 1, 1, "Redraw Semaphore" );

    InitializeCriticalSection( &refreshMutex );
#endif
  }

/** Repaint the canvas */
void wxvisCanvas::OnPaint( wxPaintEvent &event ) {

    /** Acquire the semaphore before running... */
#ifdef WIN32
#ifdef DEBUG2
    fprintf( debugFile, "-> OnPaint(): Acquiring mutex\n" );
    fflush( debugFile );
#endif

//    WaitForSingleObject( refreshSemaphore, INFINITE );
    EnterCriticalSection( &refreshMutex );
#ifdef DEBUG2
    fprintf( debugFile, "-> OnPaint(): Semaphore acquired[]\n" );
    fflush( debugFile );
#endif
#endif

    wxSize dims = GetClientSize();

    /** Get the paint device context */
    wxPaintDC dc( this );

    /** Swap in the OpenGL context */
#ifndef __WXMOTIF__
    if ( !GetContext() ) {
        return;
      }
#endif
    SetCurrent();

    /** See if we've initialised */
    if ( !_hasInitialised ) {
        printf( "here\n" );
        initialise();
        printf( "here2: %X\n", pm );
        if ( pm != NULL ) {
            printf( "here3\n" );
            pm->projectM_resetGL( dims.x, dims.y );
            printf( "here4\n" );
            _hasInitialised = 1;
          } else {
            goto unlockAndExit;
          }
      }

    int i;
    short pcm_data[2][512];

    /** Produce some fake PCM data to stuff into projectM */
    if ( pm->count % 5 == 0 ) {
        for ( i = 0 ; i < 512 ; i++ ) {
            pcm_data[0][i] = 0;
            pcm_data[1][i] = 0;
          }
      } else {
        for ( i = 0 ; i < 512 ; i++ ) {
            if ( i % 2 == 0 ) {
                pcm_data[0][i] = (short)( (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i%14) ) ) );
                pcm_data[1][i] = (short)( (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i/2%14) ) ) );
              } else {
                pcm_data[0][i] = (short)( (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i/2%14) ) ) );
                pcm_data[1][i] = (short)( (float)( rand() / ( (float)RAND_MAX ) * (pow(2,i%14) ) ) );
              }
            if ( i % 2 == 1 ) {
                pcm_data[0][i] = -pcm_data[0][i];
                pcm_data[1][i] = -pcm_data[1][i];
              }
        }
    }

    /** Add the waveform data */
    DWRITE( "pm: %X\tbeatDetect: %X\tpcm: %X\tpcm_data: %X\n",
            pm, pm->beatDetect, pm->beatDetect->pcm, pcm_data );
    pm->beatDetect->pcm->addPCM16( pcm_data );

    /** Render the new frame */
    pm->renderFrame();

#ifdef PANTS
    if ( dumpFrame ) {
        char fname[1024];
        FILE *f = fopen( fname, "wb" );
        sprintf( fname, "projectM_%08d.ppm", frameNumber++ );
        fprintf( f, "P3\n#\n%d %d\n255\n", pm->wvw, pm->wvh );
        glReadPixels( 0, 0, pm->wvw, pm->wvh, GL_RGB, GL_UNSIGNED_BYTE, fbuffer );
        index = 0;
        for ( y = 0 ; y < pm->wvh ; y++ ) {
            for ( x = 0 ; x < pm->wvw ; x++ ) {
                fprintf( f, "%d %d %d ", fbuffer[index++], fbuffer[index++], fbuffer[index++] );
              }
            fprintf( f, "\n" );
          }
        fclose( f );
      }
#endif    

    /** Buffer swap in here */
    SwapBuffers();    

unlockAndExit:
#ifdef WIN32
    /** Unlock the mutex */
    LeaveCriticalSection( &refreshMutex );

//    ReleaseSemaphore( refreshSemaphore, 1, NULL );
#ifdef DEBUG2
    fprintf( debugFile, "<- OnPaint(): Released semaphore[]\n" );
    fflush( debugFile );
#endif

//    ExitThread( 0 );
#endif

    return;
  }

/** Handles resize events */
void wxvisCanvas::OnSize( wxSizeEvent &event ) {

    wxGLCanvas::OnSize( event );

    int w, h;
    GetClientSize( &w, &h );

    /** Reset the viewport size */
#ifndef __WXMOTIF__
    if ( GetContext() ) {
#endif
        SetCurrent();
        glViewport( 0, 0, w, h );
#ifndef __WXMOTIF__
      }
#endif

    if ( pm != NULL && pm->hasInit ) {
        pm->projectM_resetGL( w, h );
      }
  }

/** Enables double-buffering, or a reasonable approximation thereof! */
void wxvisCanvas::OnEraseBackground( wxEraseEvent &event ) {
    // Do nothing, to avoid flashing.
  }

/** Initialise the context -- this only happens once */
void wxvisCanvas::initialise() {

#ifdef WIN32
    /** Initialise font bitmap lists */
    SelectObject( (HDC)GetHDC(), GetStockObject( SYSTEM_FONT ) );
    glColor3f( 1, 1, 1 );
    wglUseFontBitmaps( (HDC)GetHDC(), 0, 255, 1000 );
#else
#ifdef __WXMAC__
    /** Initialise font bitmap lists */
    short familyId = 0;
    GetFNum( (const unsigned char *)"systemFont", &familyId );
    aglUseFont( aglGetCurrentContext(), familyId, normal, 12, 0, 255, 1000 );
#endif
#endif

        /** Initialise projectM */
    pm = (projectM *)wipemalloc( sizeof( projectM ) );
    pm->projectM_reset();

    pm->fullscreen = 0;
    pm->renderTarget->texsize = 1024;

#ifndef WIN32
    pm->fontURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( pm->fontURL, "../../fonts" );

    pm->presetURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( pm->presetURL, "/Users/descarte/tmp/projectM-1.00/presets_milkdrop" );
#else
    pm->fontURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( pm->fontURL, "c:\\tmp\\projectM-1.00\\fonts" );

    pm->presetURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( pm->presetURL, "c:\\tmp\\projectM-1.00\\presets_projectM" );
#endif /** MACOS */

    pm->projectM_init();
  }

/** Returns the number of depth bits */
int wxvisCanvas::getDepthBits() {
    int rv[1];
    
    SetCurrent();
    glGetIntegerv( GL_DEPTH_BITS, (GLint *)rv );

    return rv[0];
  }

/** Returns the number of stencil bits */
int wxvisCanvas::getStencilBits() {
    int rv[1];
    
    SetCurrent();
    glGetIntegerv( GL_STENCIL_BITS, (GLint *)rv );

    return rv[0];
  }

/** Handle keypresses */
void wxvisCanvas::OnKeyPress( wxKeyEvent &event ) {

    int i;

    switch ( event.GetKeyCode() ) {
        case 'R': {
            pm->switchPreset( RANDOM_NEXT, HARD_CUT );
            break;
          }
        case 'N': {
            pm->switchPreset( ALPHA_NEXT, HARD_CUT );
            break;
          }
        case 'P': {
            pm->switchPreset( ALPHA_PREVIOUS, HARD_CUT );
            break;
          }
        case WXK_ESCAPE: {
            _parent->app->shutdown();
            break;
          }
      }

    Refresh( FALSE );
  }

