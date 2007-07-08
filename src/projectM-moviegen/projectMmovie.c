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

#include <math.h>
#include "projectM/projectM.hpp"
#include "event/sdltoprojectM.hpp"

#ifdef DEBUG
FILE *debugFile = NULL;
#endif

projectM_t *globalPM = NULL;

int dumpFrame = 0;
int frameNumber = 0;
GLubyte *fbuffer = NULL;

void renderLoop( projectM_t *pm, short pcm_data[2][512] ) {

    int i;
    int x, y;
    int index;

    /** Handle any keys... */
    projectMEvent evt;
    projectMKeycode key;
    projectMModifier mod;

    /** Process SDL events */
    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) {
        /** Translate into projectM codes and process */
        evt = sdl2pmEvent( event );
        key = sdl2pmKeycode( event.key.keysym.sym );
        mod = sdl2pmModifier( event.key.keysym.mod );
        if ( evt == PROJECTM_KEYDOWN ) {
            key_handler( evt, key, mod );
          }
      }

    /** Add the waveform data */
    addPCM16( pcm_data );

    /** Render the new frame */
    renderFrame( pm );

    if ( dumpFrame ) {
        char fname[1024];
        FILE *f;
        sprintf( fname, "projectM_%08d.ppm", frameNumber++ );
        f = fopen( fname, "wb" );
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

    SDL_GL_SwapBuffers();
	
  }
           

int main( int argc, char **argv ) {

    /** Variables */
    int fullscreen = 0;
    int width = 512,
        height = 512;
    SDL_Surface *screen;

#ifdef DEBUG
	int value;
	int rgb_size[3];
#endif

  const SDL_VideoInfo* info = NULL;
  int bpp = 0;
  /* Flags we will pass into SDL_SetVideoMode. */
  int flags = 0;

#ifdef DEBUG
#ifdef WIN32
    /** Init debug */
    debugFile = fopen( "c:\\projectMvis.txt", "wb" );
#else
    debugFile = fopen( "/tmp/projectMvis.txt", "wb" );
#endif /** WIN32 */
#endif /** DEBUG */

    /** Allocate the SDL windows */
  /* Information about the current video settings. */
  /* First, initialize SDL's video subsystem. */
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    /* Failed, exit. */
#ifdef DEBUG
    fprintf( debugFile, "Video initialization failed: %s\n",
             SDL_GetError( ) );
#endif
    //projectM_vtable.disable_plugin (&projectM_vtable);
    return PROJECTM_ERROR;
    
  }

  /* Let's get some video information. */
  info = SDL_GetVideoInfo( );
  if( !info ) {
    /* This should probably never happen. */
#ifdef DEBUG
    fprintf( debugFile, "Video query failed: %s\n",
             SDL_GetError( ) );
#endif
    //    projectM_vtable.disable_plugin (&projectM_vtable);
    return PROJECTM_ERROR;
  }

  bpp = info->vfmt->BitsPerPixel;

//  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
//  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
//  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );

   SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 8 );
   SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  if (fullscreen==0)
     flags = SDL_OPENGL | SDL_HWSURFACE;
  else flags = SDL_OPENGL | SDL_HWSURFACE |SDL_FULLSCREEN;

//  w = 512; h = 512; bpp = 16;
#ifdef DEBUG
fprintf( debugFile, "pre SDL_SetVideoMode()\n" );
#endif
  screen = SDL_SetVideoMode( width, height, bpp, flags ) ;
#ifdef DEBUG
fprintf( debugFile, "post SDL_SetVideoMode()\n" );
#endif


  if(screen == NULL ) {
    /* 
     * This could happen for a variety of reasons,
     * including DISPLAY not being set, the specified
     * resolution not being available, etc.
     */
#ifdef DEBUG
   fprintf( debugFile, "Video mode set failed: %s\n",
	     SDL_GetError( ) );
#endif    

   // projectM_vtable.disable_plugin (&projectM_vtable);
    return PROJECTM_ERROR;
  }

#ifdef DEBUG
	fprintf(debugFile, "Screen BPP: %d\n", SDL_GetVideoSurface()->format->BitsPerPixel);
	fprintf(debugFile, "\n");
	fprintf( debugFile, "Vendor     : %s\n", glGetString( GL_VENDOR ) );
	fprintf( debugFile, "Renderer   : %s\n", glGetString( GL_RENDERER ) );
	fprintf( debugFile, "Version    : %s\n", glGetString( GL_VERSION ) );
	fprintf( debugFile, "Extensions : %s\n", glGetString( GL_EXTENSIONS ) );
	fprintf(debugFile, "\n");

	rgb_size[0] = 8;
	rgb_size[1] = 8;
	rgb_size[2] = 8;
	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value );
	fprintf( debugFile, "SDL_GL_RED_SIZE: requested %d, got %d\n", rgb_size[0],value);
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value );
	fprintf( debugFile, "SDL_GL_GREEN_SIZE: requested %d, got %d\n", rgb_size[1],value);
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value );
	fprintf( debugFile, "SDL_GL_BLUE_SIZE: requested %d, got %d\n", rgb_size[2],value);
	SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value );
	fprintf( debugFile, "SDL_GL_DEPTH_SIZE: requested %d, got %d\n", bpp, value );
	SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value );
	fprintf( debugFile, "SDL_GL_DOUBLEBUFFER: requested 1, got %d\n", value );
#ifdef PANTS
	if ( fsaa ) {
		SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &value );
		printf( "SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value );
		SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &value );
		printf( "SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa, value );
	}
#endif
#endif

    /** Setup some window stuff */
    SDL_WM_SetCaption( PROJECTM_TITLE, NULL );
 
    /** Initialise projectM */
    globalPM = (projectM_t *)malloc( sizeof( projectM_t ) );
    projectM_reset( globalPM );

    globalPM->fullscreen = 0;
    globalPM->renderTarget->texsize = 1024;
//	globalPM->renderTarget->origCcontext = (void *)aglGetCurrentContext();
#ifdef DEBUG
    if ( debugFile != NULL ) {
        fprintf( debugFile, "current context: %X\n",
                 globalPM->renderTarget->origContext );
        fflush( debugFile );
      }
#endif
    
#ifdef MACOS
    globalPM->fontURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( globalPM->fontURL, "../../fonts" );

    globalPM->presetURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( globalPM->presetURL, "../../presets" );
#else
    globalPM->fontURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( globalPM->fontURL, "c:\\tmp\\projectM\\fonts" );
    
    globalPM->presetURL = (char *)malloc( sizeof( char ) * 512 );
    strcpy( globalPM->presetURL, "c:\\tmp\\projectM\\presets_test" );
#endif /** MACOS */

    projectM_init( globalPM );

    projectM_resetGL( globalPM, width, height );

    /** Allocate the buffer for frame dumping, if applicable */
    if ( dumpFrame ) {
        fbuffer = (GLubyte *)malloc( sizeof( GLubyte ) * globalPM->wvw * globalPM->wvh * 3 );
      }

    /** Initialise the thread */
//    renderLoop( globalPM );
    main123( argc, argv );

    return PROJECTM_SUCCESS;
  }
