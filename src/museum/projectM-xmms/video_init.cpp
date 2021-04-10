//video_init.c - SDL/Opengl Windowing Creation/Resizing Functions
//
//by Peter Sperl
//
//Opens an SDL Window and creates an OpenGL session
//also able to handle resizing and fullscreening of windows
//just call init_display again with differant variables

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "video_init.h"
#include <iostream>
extern SDL_Surface *screen;
extern int texsize;

void resize_display(int w, int h, int f) {
  int flags;
  if (f) flags =  SDL_OPENGL|SDL_HWSURFACE|SDL_FULLSCREEN;
  else   flags =  SDL_OPENGL|SDL_HWSURFACE|SDL_RESIZABLE;
//  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  screen = SDL_SetVideoMode( w, h, 0, flags ) ;
  if(screen == 0 ) {
      fprintf( stderr, "Video mode set failed: %s\n", SDL_GetError( ) );
      return;
  }
  
  SDL_ShowCursor(f ? SDL_DISABLE : SDL_ENABLE);
}

//init_display
//
//Sets screen to new width and height (w,h)
//Also switches between fullscreen and windowed
//with the boolean f (fullscreen)
void init_display(int w, int h, int *fvw, int *fvh, int f)
{
  
  /* Information about the current video settings. */
  const SDL_VideoInfo* info = NULL;
  int bpp = 0;
  /* Flags we will pass into SDL_SetVideoMode. */
  int flags = 0;


  /* Let's get some video information. */
  info = SDL_GetVideoInfo( );
  if( !info ) {
    /* This should probably never happen. */
    fprintf( stderr, "Video query failed: %s\n",
             SDL_GetError( ) );
    //    projectM_vtable.disable_plugin (&projectM_vtable);
    return;
  }
  
//  printf("Screen Resolution: %d x %d\n", info->current_w, info->current_h);
  *fvw = info->current_w;
  *fvh = info->current_h;
  bpp = info->vfmt->BitsPerPixel;
  //SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  //SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  //SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );

  // SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 8 );
  //  SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 8 );
  //  SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  if (f==0)
     flags = SDL_OPENGL|SDL_HWSURFACE|SDL_RESIZABLE;
  else flags = SDL_OPENGL|SDL_HWSURFACE|SDL_FULLSCREEN;
 screen= SDL_SetVideoMode( w, h, bpp, flags ) ;
  if(screen == 0 ) {
    /* 
     * This could happen for a variety of reasons,
     * including DISPLAY not being set, the specified
     * resolution not being available, etc.
     */
   fprintf( stderr, "Video mode set failed: %s\n",
	     SDL_GetError( ) );
    
   // projectM_vtable.disable_plugin (&projectM_vtable);
    return;
    
  }
  
  // setup_opengl(w,h);
  //gluOrtho2D(0, w, 0, h);
}


