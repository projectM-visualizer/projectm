#include "Renderer.hpp"
#include "wipemalloc.h"
#include "math.h"
#include "common.h"
#include "console_interface.h"
#include "CustomShape.h"
#include "CustomWave.h"

class Preset;

Renderer::Renderer(int width, int height, int gx, int gy, RenderTarget *renderTarget, BeatDetect *beatDetect, char* fontURL)
{
  int x; int y; 
  
  this->gx=gx;
  this->gy=gy;

  this->totalframes = 1;
  this->noSwitch = 0;
  this->showfps = 0;
  this->showtitle = 0;
  this->showpreset = 0;
  this->showhelp = 0;
  this->showstats = 0;
  this->studio = 0;   
this->realfps=0;

  this->title = NULL;
   
    /** Other stuff... */
    this->correction = 1;
    this->aspect=1.33333333;

  this->gridx=(float **)wipemalloc(gx * sizeof(float *));
   for(x = 0; x < gx; x++)
    {
      this->gridx[x] = (float *)wipemalloc(gy * sizeof(float));
    }
  this->gridy=(float **)wipemalloc(gx * sizeof(float *));
 for(x = 0; x < gx; x++)
    {
      this->gridy[x] = (float *)wipemalloc(gy * sizeof(float)); 
    }
 
  this->origx2=(float **)wipemalloc(gx * sizeof(float *));
 for(x = 0; x < gx; x++)
    {
      this->origx2[x] = (float *)wipemalloc(gy * sizeof(float));
    }  
this->origy2=(float **)wipemalloc(gx * sizeof(float *));
 for(x = 0; x < gx; x++)
    {
      this->origy2[x] = (float *)wipemalloc(gy * sizeof(float));
    }

 
  //initialize reference grid values
  for (x=0;x<gx;x++)
    {
      for(y=0;y<gy;y++)
	{	

           float origx=x/(float)(gx-1);
	   float origy=-((y/(float)(gy-1))-1);
	   this->gridx[x][y]=origx*renderTarget->texsize;
	   this->gridy[x][y]=origy*renderTarget->texsize;
	   this->origx2[x][y]=( origx-.5)*2;
	   this->origy2[x][y]=( origy-.5)*2;
	}}

this->renderTarget = renderTarget;
this->beatDetect = beatDetect;
this->fontURL = fontURL;

#ifdef USE_FTGL
    /** Reset fonts */
    title_font = NULL;
    other_font = NULL;
    poly_font = NULL;
#endif /** USE_FTGL */

}

void Renderer::RenderFrame(PresetOutputs *presetOutputs, PresetInputs *presetInputs)
{
totalframes++;
  reset_per_pixel_matrices();

    DWRITE( "start Pass 1 \n" );

      //BEGIN PASS 1
      //
      //This pass is used to render our texture
      //the texture is drawn to a subsection of the framebuffer
      //and then we perform our manipulations on it
      //in pass 2 we will copy the texture into texture memory

     
    renderTarget->lock();
      
      
      //   glPushAttrib( GL_ALL_ATTRIB_BITS ); /* Overkill, but safe */
      
      glViewport( 0, 0, renderTarget->texsize, renderTarget->texsize );
       
      glEnable( GL_TEXTURE_2D );
      if(this->renderTarget->usePbuffers)
	{
	  glBindTexture( GL_TEXTURE_2D, renderTarget->textureID[1] );
	}
      else
	{
	  glBindTexture( GL_TEXTURE_2D, renderTarget->textureID[0] );
	}
      glMatrixMode(GL_TEXTURE);  
      glLoadIdentity();
      
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      
      glMatrixMode( GL_PROJECTION );
      glPushMatrix();
      glLoadIdentity();  
      glOrtho(0.0, 1, 0.0, 1,10,40);
      
      glMatrixMode( GL_MODELVIEW );
      glPushMatrix();
      glLoadIdentity();
      
    DWRITE( "renderFrame: renderTarget->texsize: %d x %d\n", this->renderTarget->texsize, this->renderTarget->texsize );
    
  
     PerPixelMath(presetOutputs, presetInputs);

    if(this->renderTarget->usePbuffers)
      {
    	//draw_motion_vectors();        //draw motion vectors
    	//unlockPBuffer( this->renderTarget);
    	//lockPBuffer( this->renderTarget, PBUFFER_PASS1 );
      }
    PerFrame(presetOutputs);               //apply per-frame effects
    Interpolation(presetOutputs,presetInputs);       //apply per-pixel effects
   
    draw_title_to_texture();      //draw title to texture

//    if(!this->renderTarget->usePbuffers)
      {
	draw_motion_vectors(presetOutputs);        //draw motion vectors
      }
    draw_shapes(presetOutputs);
    draw_custom_waves(presetOutputs);
    draw_waveform(presetOutputs, presetInputs);
    if(presetOutputs->bDarkenCenter)darken_center();
    draw_borders(presetOutputs);               //draw borders

    /** Restore original view state */
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    /** Restore all original attributes */
    //  glPopAttrib();
    glFlush();

        renderTarget->unlock();


#ifdef DEBUG
    GLint msd = 0,
        psd = 0;
    glGetIntegerv( GL_MODELVIEW_STACK_DEPTH, &msd );
    glGetIntegerv( GL_PROJECTION_STACK_DEPTH, &psd );
    DWRITE( "end pass1: modelview matrix depth: %d\tprojection matrix depth: %d\n",
            msd, psd );
    DWRITE( "begin pass2\n" );
#endif

      //BEGIN PASS 2
      //
      //end of texture rendering
      //now we copy the texture from the framebuffer to
      //video texture memory and render fullscreen on a quad surface.

    /** Reset the viewport size */
    DWRITE( "viewport: %d x %d\n", this->vw, this->vh );
    glViewport( 0, 0, this->vw, this->vh );
    glClear( GL_COLOR_BUFFER_BIT );

    if ( this->renderTarget ) {
        glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
      }

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();  
      glOrtho(-0.5, 0.5, -0.5,0.5,10,40);
     	
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

      glLineWidth( this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512.0);
      //if(this->studio%2)render_texture_to_studio();
      //else 
      render_texture_to_screen(presetOutputs);

      // glClear(GL_COLOR_BUFFER_BIT);     
      //render_Studio();

      //preset editing menu
      glMatrixMode(GL_MODELVIEW);
      glTranslated(-0.5,-0.5,-1);  

      // When console refreshes, there is a chance the preset has been changed by the user
      refreshConsole();
      draw_title_to_screen();
      if(this->showhelp%2)draw_help();
      if(this->showtitle%2)draw_title();
      if(this->showfps%2)draw_fps(this->realfps);
      if(this->showpreset%2)draw_preset();
      if(this->showstats%2)draw_stats(presetInputs);
      glTranslatef(0.5 ,0.5,1);

    DWRITE( "end pass2\n" );
}


void Renderer::Interpolation(PresetOutputs *presetOutputs, PresetInputs *presetInputs)
{
   int x,y;  
  
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslated( 0, 0, -1 );
  
    glColor4f(0.0, 0.0, 0.0,presetOutputs->decay);
    
    glEnable(GL_TEXTURE_2D);
    
#ifdef MACOS2
    /** Bind the stashed texture */
    if ( this->renderTarget->pbuffer != NULL ) {
       glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
#ifdef DEBUG
        if ( glGetError() ) {
            DWRITE( "failed to bind texture\n" );
          }
#endif
      }
#endif
  
  for (x=0;x<presetInputs->gx - 1;x++){
    glBegin(GL_TRIANGLE_STRIP);
    for(y=0;y<presetInputs->gy;y++){
      glTexCoord2f(presetInputs->x_mesh[x][y], presetInputs->y_mesh[x][y]); 
      glVertex2f(this->gridx[x][y], this->gridy[x][y]);
      glTexCoord2f(presetInputs->x_mesh[x+1][y], presetInputs->y_mesh[x+1][y]); 
      glVertex2f(this->gridx[x+1][y], this->gridy[x+1][y]);
    }
    glEnd();	
  }

#ifdef MACOS2
    /** Re-bind the pbuffer */
    if ( this->renderTarget->pbuffer != NULL ) {
      glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
      }
#endif

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}


void Renderer::PerFrame(PresetOutputs *presetOutputs)
{
         glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0, 0, -9);

  //Texture wrapping( clamp vs. wrap)
  if (presetOutputs->bTexWrap==0){
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);}
  else{ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);}
      

  //      glRasterPos2i(0,0);
      //      glClear(GL_COLOR_BUFFER_BIT);
      //      glColor4d(0.0, 0.0, 0.0,1.0);     
       
  //      glMatrixMode(GL_TEXTURE);
    //  glLoadIdentity();

      glRasterPos2i(0,0);
      glClear(GL_COLOR_BUFFER_BIT);
      glColor4d(0.0, 0.0, 0.0,1.0);

 glMatrixMode(GL_TEXTURE);
      glLoadIdentity();

      /*
      glTranslatef(presetOutputs->cx,presetOutputs->cy, 0);
     if(this->correction)  glScalef(1,this->vw/(float)this->vh,1);

      if(!isPerPixelEqn(ROT_OP)) {
	//	printf("ROTATING: rot = %f\n", rot);
	glRotatef(presetOutputs->rot*90, 0, 0, 1);
      }
      if(!isPerPixelEqn(SX_OP)) glScalef(1/presetOutputs->sx,1,1);     
      if(!isPerPixelEqn(SY_OP)) glScalef(1,1/presetOutputs->sy,1); 

      if(this->correction)glScalef(1,this->vh/(float)this->vw,1);
            glTranslatef((-presetOutputs->cx) ,(-presetOutputs->cy),0);  
      */

      if(!presetOutputs->dx_is_mesh) glTranslatef(-presetOutputs->dx,0,0);   
      if(!presetOutputs->dy_is_mesh) glTranslatef(0 ,-presetOutputs->dy,0);  
    }


Renderer::~Renderer() {

  int x;

 for(x = 0; x < this->gx; x++)
    {
      
      free(this->gridx[x]);
      free(this->gridy[x]);    
      free(this->origx2[x]);
      free(this->origy2[x]);
     
      
    }

  free(this->origx2);
  free(this->origy2);
  free(this->gridx);
  free(this->gridy);
 

  
  this->origx2 = NULL;
  this->origy2 = NULL;
  this->gridx = NULL;
  this->gridy = NULL;
 
}


void Renderer::PerPixelMath(PresetOutputs *presetOutputs, PresetInputs *presetInputs) {

  int x,y;
  float fZoom2,fZoom2Inv;



  if(!presetOutputs->cx_is_mesh)
       { 
      for (x=0;x<this->gx;x++){
       
	for(y=0;y<this->gy;y++){
	  presetOutputs->cx_mesh[x][y]=presetOutputs->cx;
	}
	
      }
    }

  if(!presetOutputs->cy_is_mesh)
        { 
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  presetOutputs->cy_mesh[x][y]=presetOutputs->cy;
	}}
    }
  
  if(!presetOutputs->sx_is_mesh)
    { 
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  presetOutputs->sx_mesh[x][y]=presetOutputs->sx;
	}}
    }
  
  if(!presetOutputs->sy_is_mesh)
    { 
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  presetOutputs->sy_mesh[x][y]=presetOutputs->sy;
	}}
    }

  if(!presetOutputs->zoom_is_mesh)
    {       
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  presetOutputs->zoom_mesh[x][y]=presetOutputs->zoom;
	}}
    }
 
  if(!presetOutputs->zoomexp_is_mesh)
    {
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  presetOutputs->zoomexp_mesh[x][y]=presetOutputs->zoomexp;
	}}
    }

  if(!presetOutputs->rot_is_mesh)
    {       
      for (x=0;x<this->gx;x++){
	for(y=0;y<this->gy;y++){
	  presetOutputs->rot_mesh[x][y]=presetOutputs->rot;
	}
      }
    }

  /*
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){	  	  
      x_mesh[x][y]=(x_mesh[x][y]-.5)*2; 
    }
  }
 
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){	  	  
      y_mesh[x][y]=(y_mesh[x][y]-.5)*2; 
    }
  }
  */

  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){
      fZoom2 = powf( presetOutputs->zoom_mesh[x][y], powf( presetOutputs->zoomexp_mesh[x][y], presetInputs->rad_mesh[x][y]*2.0f - 1.0f));
      fZoom2Inv = 1.0f/fZoom2;
      presetInputs->x_mesh[x][y]= this->origx2[x][y]*0.5f*fZoom2Inv + 0.5f;
      presetInputs->y_mesh[x][y]= this->origy2[x][y]*0.5f*fZoom2Inv + 0.5f;
    }
  }
	
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){
      presetInputs->x_mesh[x][y]  = ( presetInputs->x_mesh[x][y] - presetOutputs->cx_mesh[x][y])/presetOutputs->sx_mesh[x][y] + presetOutputs->cx_mesh[x][y];
    }
  }
  
  for (x=0;x<this->gx;x++){
    for(y=0;y<this->gy;y++){
      presetInputs->y_mesh[x][y] = ( presetInputs->y_mesh[x][y] - presetOutputs->cy_mesh[x][y])/presetOutputs->sy_mesh[x][y] + presetOutputs->cy_mesh[x][y];
    }
  }	   
	 

 for (x=0;x<this->gx;x++){
   for(y=0;y<this->gy;y++){
     float u2 = presetInputs->x_mesh[x][y] - presetOutputs->cx_mesh[x][y];
     float v2 = presetInputs->y_mesh[x][y] - presetOutputs->cy_mesh[x][y];
     
     float cos_rot = cosf(presetOutputs->rot_mesh[x][y]);
     float sin_rot = sinf(presetOutputs->rot_mesh[x][y]);
     
     presetInputs->x_mesh[x][y] = u2*cos_rot - v2*sin_rot + presetOutputs->cx_mesh[x][y];
     presetInputs->y_mesh[x][y] = u2*sin_rot + v2*cos_rot + presetOutputs->cy_mesh[x][y];

  }
 }	  

 if(presetOutputs->dx_is_mesh)
   {
     for (x=0;x<this->gx;x++){
       for(y=0;y<this->gy;y++){	      
	 presetInputs->x_mesh[x][y] -= presetOutputs->dx_mesh[x][y];
       }
     }
   }
 
 if(presetOutputs->dy_is_mesh)
   {
     for (x=0;x<this->gx;x++){
       for(y=0;y<this->gy;y++){	      
	 presetInputs->y_mesh[x][y] -= presetOutputs->dy_mesh[x][y];
       }
     }
		  	
   }

}



void Renderer::reset(int w, int h)
{
    this->aspect=(float)h / (float)w;
    this -> vw = w;
    this -> vh = h;

//FIXME maybe needs called elsewhere
  //if (!this->renderTarget->usePbuffers) {
   //   renderTarge->createPBuffers(w,h,this->renderTarget);
   //   }

    /* Our shading model--Gouraud (smooth). */
    glShadeModel( GL_SMOOTH);
    /* Culling. */
    //    glCullFace( GL_BACK );
    //    glFrontFace( GL_CCW );
    //    glEnable( GL_CULL_FACE );
    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );
    /* Setup our viewport. */
    glViewport( 0, 0, w, h );
    /*
    * Change to the projection matrix and set
    * our viewing volume.
    */
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    //    gluOrtho2D(0.0, (GLfloat) width, 0.0, (GLfloat) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  

    //    glFrustum(0.0, height, 0.0,width,10,40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDrawBuffer(GL_BACK); 
    glReadBuffer(GL_BACK); 
    glEnable(GL_BLEND); 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glEnable( GL_LINE_SMOOTH );
      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);		
    
    glEnable(GL_POINT_SMOOTH);
  
    // glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,renderTarget->texsize,renderTarget->texsize,0);
    //glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,renderTarget->texsize,renderTarget->texsize);
    glLineStipple(2, 0xAAAA);

    /** (Re)create the offscreen for pass 1 */
   
    //REME: necesary? 
    //rescale_per_pixel_matrices();

    /** Load TTF font **/
   


#ifdef USE_FTGL
    /**f Load the standard fonts */
    if ( title_font == NULL && other_font == NULL ) {
       char path[1024];

        sprintf( path, "%s%cVera.ttf", this->fontURL, PATH_SEPARATOR );
        title_font = new FTGLPixmapFont(path);
	poly_font = new FTGLPolygonFont(path);
	sprintf( path, "%s%cVeraMono.ttf", this->fontURL, PATH_SEPARATOR );
        other_font = new FTGLPixmapFont(path);
   
      }
#endif /** USE_FTGL */      
}





void Renderer::draw_custom_waves(PresetOutputs *presetOutputs) {

  int x;
  CustomWave *wavecode;

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

  glPointSize(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512); 

  /// @bug SPERL: this is a starting point at least  
  for (PresetOutputs::cwave_container::const_iterator pos = presetOutputs->customWaves.begin();
	pos != presetOutputs->customWaves.end(); ++pos) 
    {
     
      if(wavecode->enabled==1)
	{
	
	  if (wavecode->bAdditive==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	  if (wavecode->bDrawThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : 2*this->renderTarget->texsize/512);
	  
	   beatDetect->pcm->getPCM(wavecode->value1,wavecode->samples,0,wavecode->bSpectrum,wavecode->smoothing,0);
	   beatDetect->pcm->getPCM(wavecode->value2,wavecode->samples,1,wavecode->bSpectrum,wavecode->smoothing,0);
	  // printf("%f\n",pcmL[0]);


	  float mult=wavecode->scaling*presetOutputs->fWaveScale*(wavecode->bSpectrum ? 0.015f :1.0f);

	  for(x=0;x<wavecode->samples;x++)
	    {wavecode->value1[x]*=mult;}
	  
	  for(x=0;x<wavecode->samples;x++)
	    {wavecode->value2[x]*=mult;}

	   for(x=0;x<wavecode->samples;x++)
	     {wavecode->sample_mesh[x]=((float)x)/((float)(wavecode->samples-1));}
	  
	  // printf("mid inner loop\n");  
	  wavecode->evalPerPointEqns();
	
	  //put drawing code here
	  if (wavecode->bUseDots==1)   glBegin(GL_POINTS);
	  else   glBegin(GL_LINE_STRIP);
	  
	  for(x=0;x<wavecode->samples;x++)
	    {
	     
	      glColor4f(wavecode->r_mesh[x],wavecode->g_mesh[x],wavecode->b_mesh[x],wavecode->a_mesh[x]);
	      glVertex3f(wavecode->x_mesh[x],-(wavecode->y_mesh[x]-1),-1);
	    }
	  glEnd();
	  glPointSize(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512); 
	  glLineWidth(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512); 
	  glDisable(GL_LINE_STIPPLE); 
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  //  glPopMatrix();
	  
	}
      
    }

    glPopMatrix();
}


void Renderer::draw_shapes(PresetOutputs *presetOutputs) { 

  int i;

  float theta;
  float radius;

  CustomShape *shapecode;
 
  float pi = 3.14159265;
  float start,inc,xval,yval;
  
  float t;

  //  more=isMoreCustomWave();
  // printf("not inner loop\n");

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );
  
/// @bug SPERL: this is a starting point at least  
  for (PresetOutputs::cshape_container::const_iterator pos = presetOutputs->customShapes.begin();
	pos != presetOutputs->customShapes.end(); ++pos) 
    {

      if(shapecode->enabled==1)
	{
	  // printf("drawing shape %f\n",shapecode->ang);
	  shapecode->y=-((shapecode->y)-1);
	  radius=.5;
	  shapecode->radius=shapecode->radius*(.707*.707*.707*1.04);
	  //Additive Drawing or Overwrite
	  if (shapecode->additive==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	  
	  glMatrixMode(GL_MODELVIEW);
	  	   glPushMatrix(); 
		   /* DEPRECATED
		   if(this->correction)
		     {		     
		       glTranslatef(0.5,0.5, 0);
		       glScalef(1.0,this->vw/(float)this->vh,1.0);  
		       glTranslatef(-0.5 ,-0.5,0);   
		     }
		   */
	
	   xval=shapecode->x;
	   yval=shapecode->y;
	  
	  if (shapecode->textured)
	    {
	      glMatrixMode(GL_TEXTURE);
	       glPushMatrix();
	      glLoadIdentity();
	      //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	      //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	      //glTranslatef(.5,.5, 0);
	      //if (this->correction) glScalef(1,this->vw/(float)this->vh,1);
   
	      //glRotatef((shapecode->tex_ang*360/6.280), 0, 0, 1);
	      
	      //glScalef(1/(shapecode->tex_zoom),1/(shapecode->tex_zoom),1); 
	      
	      // glScalef(1,vh/(float)vw,1);
	      //glTranslatef((-.5) ,(-.5),0);  
	      // glScalef(1,this->vw/(float)this->vh,1);
	      glEnable(GL_TEXTURE_2D);
	      	     

	      glBegin(GL_TRIANGLE_FAN);
	      glColor4f(0.0,0.0,0.0,shapecode->a);
	      //glColor4f(shapecode->r,shapecode->g,shapecode->b,shapecode->a);
	   
	      glTexCoord2f(.5,.5);
	      glVertex3f(xval,yval,-1);	 
	      //glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);  
	      glColor4f(0.0,0.0,0.0,shapecode->a2);

	      for ( i=1;i<shapecode->sides+2;i++)
		{
		 
		  //		  theta+=inc;
		  //  glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);
		  //glTexCoord2f(radius*cos(theta)+.5 ,radius*sin(theta)+.5 );
		  //glVertex3f(shapecode->radius*cos(theta)+xval,shapecode->radius*sin(theta)+yval,-1);  
		  t = (i-1)/(float)shapecode->sides;
		  
		  glTexCoord2f(  0.5f + 0.5f*cosf(t*3.1415927f*2 + shapecode->tex_ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)/shapecode->tex_zoom, 0.5f + 0.5f*sinf(t*3.1415927f*2 + shapecode->tex_ang + 3.1415927f*0.25f)/shapecode->tex_zoom);
		   glVertex3f(shapecode->radius*cosf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)+xval, shapecode->radius*sinf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)+yval,-1);   
		}	
	      glEnd();

	    
	      
	    
	      glDisable(GL_TEXTURE_2D);
	       glPopMatrix();
	      glMatrixMode(GL_MODELVIEW);          
	    }
	  else{//Untextured (use color values)
	    //printf("untextured %f %f %f @:%f,%f %f %f\n",shapecode->a2,shapecode->a,shapecode->border_a, shapecode->x,shapecode->y,shapecode->radius,shapecode->ang);
	    //draw first n-1 triangular pieces
	      glBegin(GL_TRIANGLE_FAN);
	      
	      glColor4f(shapecode->r,shapecode->g,shapecode->b,shapecode->a);
	    
	      // glTexCoord2f(.5,.5);
	      glVertex3f(xval,yval,-1);	 
	     glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);

	      for ( i=1;i<shapecode->sides+2;i++)
		{
		  
		  //theta+=inc;
		  //  glColor4f(shapecode->r2,shapecode->g2,shapecode->b2,shapecode->a2);
		  //  glTexCoord2f(radius*cos(theta)+.5 ,radius*sin(theta)+.5 );
		  //glVertex3f(shapecode->radius*cos(theta)+xval,shapecode->radius*sin(theta)+yval,-1);	  

		  t = (i-1)/(float)shapecode->sides;
		   glVertex3f(shapecode->radius*cosf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)+xval, shapecode->radius*sinf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)+yval,-1);   	  
		       
		}	
	      glEnd();

	   	 
	  }
	    if (presetOutputs->bWaveThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : 2*this->renderTarget->texsize/512);
	      glBegin(GL_LINE_LOOP);
	      glColor4f(shapecode->border_r,shapecode->border_g,shapecode->border_b,shapecode->border_a);
	      for ( i=1;i<shapecode->sides+1;i++)
		{

		  t = (i-1)/(float)shapecode->sides;
		   glVertex3f(shapecode->radius*cosf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)*(this->correction ? this->aspect : 1.0)+xval, shapecode->radius*sinf(t*3.1415927f*2 + shapecode->ang + 3.1415927f*0.25f)+yval,-1); 	  
		 
		  //theta+=inc;
		  //glVertex3f(shapecode->radius*cos(theta)+xval,shapecode->radius*sin(theta)+yval,-1);
		}
	      glEnd();
	  if (presetOutputs->bWaveThick==1)  glLineWidth(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512);
	  
	  glPopMatrix();
	}
    }

    glPopMatrix();
}


void Renderer::draw_waveform(PresetOutputs *presetOutputs, PresetInputs *presetInputs) {

  int x;
  
  float r,theta;
 
  float offset,scale,dy2_adj;

  float co;  
  
  float wave_x_temp=0;
  float wave_y_temp=0;
  float dy_adj;
  float xx,yy; 

  float cos_rot;
  float sin_rot;    

    DWRITE( "draw_waveform: %d\n", presetOutputs->nWaveMode );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

  modulate_opacity_by_volume(presetOutputs); 
  maximize_colors();
  
  if(presetOutputs->bWaveDots==1) glEnable(GL_LINE_STIPPLE);
  
  offset=presetOutputs->wave_x-.5;
  scale=505.0/512.0;


  

  //Thick wave drawing
  if (presetOutputs->bWaveThick==1)  glLineWidth( (this->renderTarget->texsize < 512 ) ? 2 : 2*this->renderTarget->texsize/512);

  //Additive wave drawing (vice overwrite)
  if (presetOutputs->bAdditiveWaves==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  else    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
 
      switch(presetOutputs->nWaveMode)
	{
	  
	case 8://monitor

	  glTranslatef(0.5,0.5, 0);
	  glRotated(-presetOutputs->wave_mystery*90,0,0,1);

	     glTranslatef(-0.5,-0.825, 0);
	     glTranslatef( 0, 0, -1 );
	     
	     /*
	     for (x=0;x<16;x++)
	       {
		 glBegin(GL_LINE_STRIP);
		 glColor4f(1.0-(x/15.0),.5,x/15.0,1.0);
		 glVertex3f((this->totalframes%256)*2*scale, -this->beat_val[x]*presetOutputs->fWaveScale+renderTarget->texsize*wave_y,-1);
		 glColor4f(.5,.5,.5,1.0);
		 glVertex3f((this->totalframes%256)*2*scale, this->renderTarget->texsize*presetOutputs->wave_y,-1);   
		 glColor4f(1.0,1.0,0,1.0);
		 //glVertex3f((this->totalframes%256)*scale*2, this->beat_val_att[x]*presetOutputs->fWaveScale+this->renderTarget->texsize*presetOutputs->wave_y,-1);
		 glEnd();
	       
		 glTranslatef(0,this->renderTarget->texsize*(1/36.0), 0);
	       }
	  */
	     
	    glTranslatef(0,(1/18.0), 0);

 
	     glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,1.0,0.5,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->treb_att*5*presetOutputs->fWaveScale+presetOutputs->wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, presetOutputs->wave_y);   
	     glColor4f(1.0,1.0,0,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->treb*-5*presetOutputs->fWaveScale+presetOutputs->wave_y);
	     glEnd();
	       
	       glTranslatef(0,.075, 0);
	     glBegin(GL_LINE_STRIP);
	     glColor4f(0,1.0,0.0,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->mid_att*5*presetOutputs->fWaveScale+presetOutputs->wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, presetOutputs->wave_y);   
	     glColor4f(.5,1.0,.5,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->mid*-5*presetOutputs->fWaveScale+presetOutputs->wave_y);
	     glEnd(); 
	  
	   
	     glTranslatef(0,.075, 0);
	     glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,0,0,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, beatDetect->bass_att*5*presetOutputs->fWaveScale+presetOutputs->wave_y);
	     glColor4f(.2,.2,.2,1.0);
	     glVertex2f((this->totalframes%256)*2*scale, presetOutputs->wave_y);   
	     glColor4f(1.0,.5,.5,1.0);
	     glVertex2f((this->totalframes%256)*scale*2, beatDetect->bass*-5*presetOutputs->fWaveScale+presetOutputs->wave_y);
	     glEnd(); 
	     
	  break;
	  
	case 0://circular waveforms 
	  //  float co;
	  //	  glPushMatrix(); 
	  /*
	  if(this->correction)
	    {
	      glTranslatef(this->renderTarget->texsize*.5,this->renderTarget->texsize*.5, 0);
	      glScalef(1.0,this->vw/(float)this->vh,1.0);
	      glTranslatef((-this->renderTarget->texsize*.5) ,(-this->renderTarget->texsize*.5),0);   
	    }
	  */

    glTranslatef( 0, 0, -1 );

	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);
 
	  glBegin(GL_LINE_STRIP);

    DWRITE( "nsamples: %d\n", beatDetect->pcm->numsamples );
	 
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    { float inv_nverts_minus_one = 1.0f/(float)(beatDetect->pcm->numsamples);
	    //co= -(fabs(x-((beatDetect->pcm->numsamples*.5)-1))/beatDetect->pcm->numsamples)+1;
	      // printf("%d %f\n",x,co);
	      //theta=x*(6.28/beatDetect->pcm->numsamples);
	      //r= ((1+2*presetOutputs->wave_mystery)*(this->renderTarget->texsize/5.0)+
	      //  ( co*beatDetect->pcm->pcmdataL[x]+ (1-co)*beatDetect->pcm->pcmdataL[-(x-(beatDetect->pcm->numsamples-1))])
	      //  *25*presetOutputs->fWaveScale);
	      r=(0.5 + 0.4f*.12*beatDetect->pcm->pcmdataR[x]*presetOutputs->fWaveScale + presetOutputs->wave_mystery)*.5;
	      theta=(x)*inv_nverts_minus_one*6.28f + presetInputs->time*0.2f;
	      /* 
	      if (x < 51)
		{
		  float mix = x/51.0;
		  mix = 0.5f - 0.5f*cosf(mix * 3.1416f);
		  float rad_2 = 0.5f + 0.4f*.12*beatDetect->pcm->pcmdataR[x]*presetOutputs->fWaveScale + presetOutputs->wave_mystery;
		  r = rad_2*(1.0f-mix) + r*(mix);
		}
	      */
  glVertex2f((r*cos(theta)*(this->correction ? this->aspect : 1.0)+presetOutputs->wave_x), (r*sin(theta)+presetOutputs->wave_y));
	     
	    }

	  //	  r= ( (1+2*presetOutputs->wave_mystery)*(this->renderTarget->texsize/5.0)+
	  //     (0.5*beatDetect->pcm->pcmdataL[0]+ 0.5*beatDetect->pcm->pcmdataL[beatDetect->pcm->numsamples-1])
	  //      *20*presetOutputs->fWaveScale);
      
	  //glVertex3f(r*cos(0)+(presetOutputs->wave_x*this->renderTarget->texsize),r*sin(0)+(presetOutputs->wave_y*this->renderTarget->texsize),-1);

	  glEnd();
	  /*
	  glBegin(GL_LINE_LOOP);
	  
	  for ( x=0;x<(512/pcmbreak);x++)
	    {
	      theta=(blockstart+x)*((6.28*pcmbreak)/512.0);
	      r= ((1+2*presetOutputs->wave_mystery)*(this->renderTarget->texsize/5.0)+fdata_buffer[fbuffer][0][blockstart+x]*.0025*presetOutputs->fWaveScale);
	      
	      glVertex3f(r*cos(theta)+(presetOutputs->wave_x*this->renderTarget->texsize),r*sin(theta)+(wave_y*this->renderTarget->texsize),-1);
	    }
	  glEnd();
	  */
	  //glPopMatrix();

	  break;	
	
	case 1://circularly moving waveform
	  //  float co;
	  
	  glTranslatef(.5,.5, 0);
	  glScalef(1.0,this->vw/(float)this->vh,1.0);
	  glTranslatef((-.5) ,(-.5),0);   
	  glTranslatef( 0, 0, -1 );

	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);

	  glBegin(GL_LINE_STRIP);
	  //theta=(frame%512)*(6.28/512.0);

	  for ( x=1;x<(512-32);x++)
	    {
	      //co= -(abs(x-255)/512.0)+1;
	      // printf("%d %f\n",x,co);
	      //theta=((this->frame%256)*(2*6.28/512.0))+beatDetect->pcm->pcmdataL[x]*.2*presetOutputs->fWaveScale;
	      //r= ((1+2*presetOutputs->wave_mystery)*(this->renderTarget->texsize/5.0)+
	      //   (beatDetect->pcm->pcmdataL[x]-beatDetect->pcm->pcmdataL[x-1])*80*presetOutputs->fWaveScale);
	      theta=beatDetect->pcm->pcmdataL[x+32]*0.06*presetOutputs->fWaveScale * 1.57 + presetInputs->time*2.3;
	      r=(0.53 + 0.43*beatDetect->pcm->pcmdataR[x]*0.12*presetOutputs->fWaveScale+ presetOutputs->wave_mystery)*.5;

	     
 glVertex2f((r*cos(theta)*(this->correction ? this->aspect : 1.0)+presetOutputs->wave_x),(r*sin(theta)+presetOutputs->wave_y));
	    }

	  glEnd(); 
	  /*
	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);  
	  wave_x_temp=(presetOutputs->wave_x*.75)+.125;	
	  wave_x_temp=-(wave_x_temp-1); 

	  glBegin(GL_LINE_STRIP);

	    
	  
	  for (x=0; x<512-32; x++)
	    {
	      float rad = (.53 + 0.43*beatDetect->pcm->pcmdataR[x]) + presetOutputs->wave_mystery;
	      float ang = beatDetect->pcm->pcmdataL[x+32] * 1.57f + this->Time*2.3f;
	      glVertex3f((rad*cosf(ang)*.2*scale*presetOutputs->fWaveScale + wave_x_temp)*this->renderTarget->texsize,(rad*sinf(ang)*presetOutputs->fWaveScale*.2*scale + presetOutputs->wave_y)*this->renderTarget->texsize,-1);
	      
	    }
	  glEnd();
	  */

	  break;
	  
	case 2://EXPERIMENTAL

        glTranslatef( 0, 0, -1 );
	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);  


	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512-32; x++)
	    {
	   
  glVertex2f((beatDetect->pcm->pcmdataR[x]*presetOutputs->fWaveScale*0.5*(this->correction ? this->aspect : 1.0) + presetOutputs->wave_x),( (beatDetect->pcm->pcmdataL[x+32]*presetOutputs->fWaveScale*0.5 + presetOutputs->wave_y)));
	    }
	  glEnd();	
	  
	  break;

	case 3://EXPERIMENTAL

	    glTranslatef( 0, 0, -9 );
	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);  
	  //wave_x_temp=(presetOutputs->wave_x*.75)+.125;	
	  //wave_x_temp=-(wave_x_temp-1); 
	  
	 

	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512-32; x++)
	    {
	      
	      glVertex2f((beatDetect->pcm->pcmdataR[x] * presetOutputs->fWaveScale*0.5 + presetOutputs->wave_x),( (beatDetect->pcm->pcmdataL[x+32]*presetOutputs->fWaveScale*0.5 + presetOutputs->wave_y)));
 
	    }
	  glEnd();	
	  
	  break;

	case 4://single x-axis derivative waveform
	  {
	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);	  
	  glTranslatef(.5,.5, 0);
	  glRotated(-presetOutputs->wave_mystery*90,0,0,1);
	  glTranslatef(-.5,-.5, 0);
        glTranslatef( 0, 0, -1 );
	 
	  float w1 = 0.45f + 0.5f*(presetOutputs->wave_mystery*0.5f + 0.5f);	       
	  float w2 = 1.0f - w1;
	  float xx[512],yy[512];
				
	  glBegin(GL_LINE_STRIP);
	  for (int i=0; i<512; i++)
	    {
	     xx[i] = -1.0f + 2.0f*(i/512.0) + presetOutputs->wave_x;
	     yy[i] =0.4* beatDetect->pcm->pcmdataL[i]*0.47f*presetOutputs->fWaveScale + presetOutputs->wave_y;
	     xx[i] += 0.4*beatDetect->pcm->pcmdataR[i]*0.44f*presetOutputs->fWaveScale;				      
	      
	      if (i>1)
		{
		  xx[i] = xx[i]*w2 + w1*(xx[i-1]*2.0f - xx[i-2]);
		  yy[i] = yy[i]*w2 + w1*(yy[i-1]*2.0f - yy[i-2]);
		}
	      glVertex2f(xx[i],yy[i]);
	    }

	  glEnd();

	  /*
	  presetOutputs->wave_x=(presetOutputs->wave_x*.75)+.125;	  
	  presetOutputs->wave_x=-(presetOutputs->wave_x-1); 
	  glBegin(GL_LINE_STRIP);
	 
	  for ( x=1;x<512;x++)
	    {
	      dy_adj=  beatDetect->pcm->pcmdataL[x]*20*presetOutputs->fWaveScale-beatDetect->pcm->pcmdataL[x-1]*20*presetOutputs->fWaveScale;
	      glVertex3f((x*(this->renderTarget->texsize/512))+dy_adj, beatDetect->pcm->pcmdataL[x]*20*presetOutputs->fWaveScale+this->renderTarget->texsize*presetOutputs->wave_x,-1);
	    }
	  glEnd(); 
	  */
	  }
	  break;

	case 5://EXPERIMENTAL
	  	  
        glTranslatef( 0, 0, -5 );	
	  
	  presetOutputs->wave_y=-1*(presetOutputs->wave_y-1.0);  
	 
	  cos_rot = cosf(presetInputs->time*0.3f);
	  sin_rot = sinf(presetInputs->time*0.3f);

	  glBegin(GL_LINE_STRIP);

	  for (x=0; x<512; x++)
	    {	      
	      float x0 = (beatDetect->pcm->pcmdataR[x]*beatDetect->pcm->pcmdataL[x+32] + beatDetect->pcm->pcmdataL[x+32]*beatDetect->pcm->pcmdataR[x]);
	      float y0 = (beatDetect->pcm->pcmdataR[x]*beatDetect->pcm->pcmdataR[x] - beatDetect->pcm->pcmdataL[x+32]*beatDetect->pcm->pcmdataL[x+32]);
	      
          glVertex2f(((x0*cos_rot - y0*sin_rot)*presetOutputs->fWaveScale*0.5*(this->correction ? this->aspect : 1.0) + presetOutputs->wave_x),( (x0*sin_rot + y0*cos_rot)*presetOutputs->fWaveScale*0.5 + presetOutputs->wave_y));
 
	    }
	  glEnd();	
	  
	 
	  break;

	case 6://single waveform


	  //glMatrixMode(GL_MODELVIEW);
	  //	  	  glLoadIdentity();
	  
	  glTranslatef(.5,.5, 0);
	  glRotated(-presetOutputs->wave_mystery*90,0,0,1);
	  glTranslatef(0,0, -1);
	  
	  wave_x_temp=-2*0.4142*(fabs(fabs(presetOutputs->wave_mystery)-.5)-.5);
	  glScalef(1.0+wave_x_temp,1.0,1.0);
	  glTranslatef(-.5,-.5, 0);
	  wave_x_temp=-1*(presetOutputs->wave_x-1.0);

	  glBegin(GL_LINE_STRIP);
	  //	  wave_x_temp=(wave_x*.75)+.125;	  
	  //	  wave_x_temp=-(wave_x_temp-1);
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      //glVertex3f(x*scale, fdata_buffer[fbuffer][0][blockstart+x]*.0012*fWaveScale+renderTarget->texsize*wave_x_temp,-1);
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataR[x]*.04*presetOutputs->fWaveScale+wave_x_temp);

	      //glVertex3f(x*scale, renderTarget->texsize*wave_y_temp,-1);
	    }
	  //	  printf("%f %f\n",renderTarget->texsize*wave_y_temp,wave_y_temp);
	  glEnd(); 
	  break;
	  
	case 7://dual waveforms

	  glTranslatef(.5,.5, 0);
	  glRotated(-presetOutputs->wave_mystery*90,0,0,1);
	  
	  wave_x_temp=-2*0.4142*(fabs(fabs(presetOutputs->wave_mystery)-.5)-.5);
	  glScalef(1.0+wave_x_temp,1.0,1.0);
	     glTranslatef(-.5,-.5, -1);
    glTranslatef( 0, 0, -1 );

         wave_y_temp=-1*(presetOutputs->wave_x-1);

		  glBegin(GL_LINE_STRIP);
	 
	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataL[x]*.04*presetOutputs->fWaveScale+(wave_y_temp+(presetOutputs->wave_y*presetOutputs->wave_y*.5)));
	    }
	  glEnd(); 

	  glBegin(GL_LINE_STRIP);
	 

	  for ( x=0;x<beatDetect->pcm->numsamples;x++)
	    {
     
	      glVertex2f(x/(float)beatDetect->pcm->numsamples, beatDetect->pcm->pcmdataR[x]*.04*presetOutputs->fWaveScale+(wave_y_temp-(presetOutputs->wave_y*presetOutputs->wave_y*.5)));
	    }
	  glEnd(); 
	  glPopMatrix();
     break;
     
	default:  
 glTranslatef( 0, 0, -1 );
 glBegin(GL_LINE_LOOP);
	  
	  for ( x=0;x<512;x++)
	    {
	      theta=(x)*(6.28/512.0);
	      r= (0.2+beatDetect->pcm->pcmdataL[x]*.002);
	      
	      glVertex2f(r*cos(theta)+presetOutputs->wave_x,r*sin(theta)+presetOutputs->wave_y);
	    }
	  glEnd();

glBegin(GL_LINE_STRIP);
	
	  for ( x=0;x<512;x++)
	    {
	      glVertex3f(x*scale, beatDetect->pcm->pcmdataL[x]*.04*presetOutputs->fWaveScale+((presetOutputs->wave_x+.1)),-1);
	    }
	  glEnd();
	  
	  glBegin(GL_LINE_STRIP);
	  
	 for ( x=0;x<512;x++)
	    {
	      glVertex3f(x*scale, beatDetect->pcm->pcmdataR[x]*.04*presetOutputs->fWaveScale+((presetOutputs->wave_x-.1)),-1);
	      
	    }
	  glEnd();
     break;
         if (presetOutputs->bWaveThick==1)  glLineWidth( (this->renderTarget->texsize < 512) ? 1 : 2*this->renderTarget->texsize/512); 
}	
      glLineWidth( this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512);
      glDisable(GL_LINE_STIPPLE);

    glPopMatrix();
}

void Renderer::maximize_colors(PresetOutputs *presetOutputs) {

 float wave_r_switch=0,wave_g_switch=0,wave_b_switch=0;
 //wave color brightening
      //
      //forces max color value to 1.0 and scales
      // the rest accordingly
 if(presetOutputs->nWaveMode==2 || presetOutputs->nWaveMode==5)
   {
	switch(this->renderTarget->texsize)
			{
			case 256:  presetOutputs->wave_o *= 0.07f; break;
			case 512:  presetOutputs->wave_o *= 0.09f; break;
			case 1024: presetOutputs->wave_o *= 0.11f; break;
			case 2048: presetOutputs->wave_o *= 0.13f; break;
			}
   }

 else if(presetOutputs->nWaveMode==3)
   {
	switch(this->renderTarget->texsize)
			{
			case 256:  presetOutputs->wave_o *= 0.075f; break;
			case 512:  presetOutputs->wave_o *= 0.15f; break;
			case 1024: presetOutputs->wave_o *= 0.22f; break;
			case 2048: presetOutputs->wave_o *= 0.33f; break;
			}
	presetOutputs->wave_o*=1.3f;
	presetOutputs->wave_o*=powf(beatDetect->treb ,2.0f);
   }

      if (presetOutputs->bMaximizeWaveColor==1)  
	{
	  if(presetOutputs->wave_r>=presetOutputs->wave_g && presetOutputs->wave_r>=presetOutputs->wave_b)   //red brightest
	    {
	      wave_b_switch=presetOutputs->wave_b*(1/presetOutputs->wave_r);
	      wave_g_switch=presetOutputs->wave_g*(1/presetOutputs->wave_r);
	      wave_r_switch=1.0;
	    }
	  else if   (presetOutputs->wave_b>=presetOutputs->wave_g && presetOutputs->wave_b>=presetOutputs->wave_r)         //blue brightest
	    {  
	      wave_r_switch=presetOutputs->wave_r*(1/presetOutputs->wave_b);
	      wave_g_switch=presetOutputs->wave_g*(1/presetOutputs->wave_b);
	      wave_b_switch=1.0;
	      
	    }	
	
	  else  if (presetOutputs->wave_g>=presetOutputs->wave_b && presetOutputs->wave_g>=presetOutputs->wave_r)         //green brightest
	    {
	      wave_b_switch=presetOutputs->wave_b*(1/presetOutputs->wave_g);
	      wave_r_switch=presetOutputs->wave_r*(1/presetOutputs->wave_g);
	      wave_g_switch=1.0;
	    }
 
	
	  glColor4f(wave_r_switch, wave_g_switch, wave_b_switch, presetOutputs->wave_o);
	}
      else
	{ 
	  glColor4f(presetOutputs->wave_r, presetOutputs->wave_g, presetOutputs->wave_b, presetOutputs->wave_o);
	}
      
}

void Renderer::darken_center() {

  float unit=0.05f;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix(); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  glTranslatef(0.5,0.5, 0);

  glBegin(GL_TRIANGLE_FAN);
  glColor4f(0,0,0,3.0f/32.0f);
  glVertex3f(0,0,-1);
  glColor4f(0,0,0,-1);
  glVertex3f(-unit,0,-1);
  glVertex3f(0,-unit,-1);
  glVertex3f(unit,0,-1);
  glVertex3f(0,unit,-1);
  glVertex3f(-unit,0,-1);
  glEnd();

  glPopMatrix();
}


void Renderer::modulate_opacity_by_volume(PresetOutputs *presetOutputs) {

 //modulate volume by opacity
      //
      //set an upper and lower bound and linearly
      //calculate the opacity from 0=lower to 1=upper
      //based on current volume


      if (presetOutputs->bModWaveAlphaByVolume==1)
	{if (beatDetect->vol<=presetOutputs->fModWaveAlphaStart)  presetOutputs->wave_o=0.0;       
	else if (beatDetect->vol>=presetOutputs->fModWaveAlphaEnd) presetOutputs->wave_o=presetOutputs->fWaveAlpha;
	else presetOutputs->wave_o=presetOutputs->fWaveAlpha*((beatDetect->vol-presetOutputs->fModWaveAlphaStart)/(presetOutputs->fModWaveAlphaEnd-presetOutputs->fModWaveAlphaStart));}
      else presetOutputs->wave_o=presetOutputs->fWaveAlpha;
}

void Renderer::draw_motion_vectors(PresetOutputs *presetOutputs) {

    int x,y;

    float offsetx=presetOutputs->mv_dx, intervalx=1.0/(float)presetOutputs->mv_x;
    float offsety=presetOutputs->mv_dy, intervaly=1.0/(float)presetOutputs->mv_y;
    

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glPointSize(presetOutputs->mv_l);
    glColor4f(presetOutputs->mv_r, presetOutputs->mv_g, presetOutputs->mv_b, presetOutputs->mv_a);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

    glBegin(GL_POINTS);
    for (x=0;x<presetOutputs->mv_x;x++){
        for(y=0;y<presetOutputs->mv_y;y++){
            float lx, ly, lz;
            lx = offsetx+x*intervalx;
            ly = offsety+y*intervaly;
            lz = -1;
            glVertex2f(lx,ly);	  
          }
      }
  
    glEnd();

    glPopMatrix();    
}


void Renderer::draw_borders(PresetOutputs *presetOutputs) {

    //Draw Borders
    float of=presetOutputs->ob_size*.5;
    float iff=presetOutputs->ib_size*.5;
    float texof=1.0-of;

    //no additive drawing for borders
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  
    glColor4d(presetOutputs->ob_r,presetOutputs->ob_g,presetOutputs->ob_b,presetOutputs->ob_a);
  
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glTranslatef( 0, 0, -1 );

    glRectd(0,0,of,1);
    glRectd(of,0,texof,of);
    glRectd(texof,0,1,1);
    glRectd(of,1,texof,texof);
    glColor4d(presetOutputs->ib_r,presetOutputs->ib_g,presetOutputs->ib_b,presetOutputs->ib_a);
    glRectd(of,of,of+iff,texof);
    glRectd(of+iff,of,texof-iff,of+iff);
    glRectd(texof-iff,of,texof,texof);
    glRectd(of+iff,texof,texof-iff,texof-iff);

    glPopMatrix();
}



void Renderer::draw_title_to_texture() {
  
#ifdef USE_FTGL 
    if (this->drawtitle>80) 
      //    if(1)
      {
      glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
      glColor4f(1.0,1.0,1.0,1.0);
      glPushMatrix();
     
      glTranslatef(0,0.5, -1);
    
      glScalef(0.0025,-0.0025,30*.0025);
      //glTranslatef(0,0, 1.0);
      poly_font->FaceSize( 22);
    
      glRasterPos2f(0.0, 0.0);

   if ( this->title != NULL ) {
      poly_font->Render(this->title );
      } else {
	poly_font->Render("Unknown" );
      }
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glPopMatrix();
      this->drawtitle=0;
    }
#endif /** USE_FTGL */
}

void Renderer::draw_title_to_screen() {

#ifdef USE_FTGL
  if(this->drawtitle>0)
    { 
      float easein = ((80-this->drawtitle)*.0125);
      float easein2 = easein * easein;
      float easein3 = .0025/((-easein2)+1.0);

      glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
      glColor4f(1.0,1.0,1.0,1.0);
      glPushMatrix();


      //glTranslatef(this->vw*.5,this->vh*.5 , -1.0);
      glTranslatef(0,0.5 , -1.0);

      glScalef(easein3,easein3,30*.0025);

      glRotatef(easein2*360,1,0,0);


      //glTranslatef(-.5*this->vw,0, 0.0);
      
      //poly_font->Depth(1.0);  
      poly_font->FaceSize(22);

      glRasterPos2f(0.0, 0.0);
      if ( this->title != NULL ) {
	poly_font->Render(this->title );
      } else {
	poly_font->Render("Unknown" );
      }
      // poly_font->Depth(0.0);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glPopMatrix();	
      
      this->drawtitle++;

    }
#endif /** USE_FTGL */
}

void Renderer::draw_title() {
#ifdef USE_FTGL
  //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0,1.0,1.0,1.0);
  //  glPushMatrix();
  //  glTranslatef(this->vw*.001,this->vh*.03, -1);
  //  glScalef(this->vw*.015,this->vh*.025,0);

      glRasterPos2f(0.01, 0.05);
      title_font->FaceSize( (unsigned)(20*(this->vh/512.0)));
       
      if ( this->title != NULL ) {
       	 title_font->Render(this->title );
      } else {
       	 title_font->Render("Unknown" );
      }
      //  glPopMatrix();
      //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
     
#endif /** USE_FTGL */
}
void Renderer::draw_preset() { 
#ifdef USE_FTGL
  //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
    
  glColor4f(1.0,1.0,1.0,1.0);
      //      glPushMatrix();
      //glTranslatef(this->vw*.001,this->vh*-.01, -1);
      //glScalef(this->vw*.003,this->vh*.004,0);

   
        glRasterPos2f(0.01, 0.01);

	title_font->FaceSize((unsigned)(12*(this->vh/512.0)));
	if(this->noSwitch) title_font->Render("[LOCKED]  " );
	title_font->FaceSize((unsigned)(20*(this->vh/512.0)));
        title_font->Render(this->presetName );

                 
        
	//glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif /** USE_FTGL */    
}

void Renderer::draw_help( ) { 

#ifdef USE_FTGL
//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
  DWRITE("pre-help");
      glColor4f(1.0,1.0,1.0,1.0);
      glPushMatrix();
       glTranslatef(0,1, 0);
      //glScalef(this->vw*.02,this->vh*.02 ,0);

     
       title_font->FaceSize((unsigned)( 18*(this->vh/512.0)));

      glRasterPos2f(0.01, -0.05);
       title_font->Render("Help");  
      
      glRasterPos2f(0.01, -0.09);     
       title_font->Render("----------------------------");  
      
      glRasterPos2f(0.01, -0.13); 
       title_font->Render("F1: This help menu");
  
      glRasterPos2f(0.01, -0.17);
       title_font->Render("F2: Show song title");
      
      glRasterPos2f(0.01, -0.21);
       title_font->Render("F3: Show preset name");
 
       glRasterPos2f(0.01, -0.25);
       title_font->Render("F4: Show Rendering Settings");
 
      glRasterPos2f(0.01, -0.29);
       title_font->Render("F5: Show FPS");

      glRasterPos2f(0.01, -0.35);
       title_font->Render("F: Fullscreen");

      glRasterPos2f(0.01, -0.39);
       title_font->Render("L: Lock/Unlock Preset");

      glRasterPos2f(0.01, -0.43);
       title_font->Render("M: Show Menu");
      
      glRasterPos2f(0.01, -0.49);
       title_font->Render("R: Random preset");
      glRasterPos2f(0.01, -0.53);
       title_font->Render("N: Next preset");
 
      glRasterPos2f(0.01, -0.57);
       title_font->Render("P: Previous preset");

       glPopMatrix();
      //         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
       DWRITE("post-help");
#endif /** USE_FTGL */
}

void Renderer::draw_stats(PresetInputs *presetInputs) {

#ifdef USE_FTGL
 char buffer[128];  
  float offset= (this->showfps%2 ? -0.05 : 0.0);
  // glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
 
  glColor4f(1.0,1.0,1.0,1.0);
  glPushMatrix();
  glTranslatef(0.01,1, 0);
 glRasterPos2f(0, -.05+offset);  
  other_font->Render(this->correction ? "  aspect: corrected" : "  aspect: stretched");  
sprintf( buffer, " (%f)", this->aspect);
    other_font->Render(buffer);



  glRasterPos2f(0, -.09+offset);
  other_font->FaceSize((unsigned)(18*(this->vh/512.0)));

  sprintf( buffer, " texsize: %d", this->renderTarget->texsize);
  other_font->Render(buffer);

  glRasterPos2f(0, -.13+offset);
  sprintf( buffer, "viewport: %d x %d", this->vw, this->vh);
  other_font->Render(buffer);
  /* REME: FIX
  glRasterPos2f(0, -.17+offset);  
  other_font->Render((this->renderer->renderTarget->usePbuffers ? "     FBO: on" : "     FBO: off"));
  */
  glRasterPos2f(0, -.21+offset); 
  sprintf( buffer, "    mesh: %d x %d", presetInputs->gx,presetInputs->gy);
  other_font->Render(buffer);


  glPopMatrix();
  // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
 
    
#endif /** USE_FTGL */    
}
void Renderer::draw_fps( float realfps ) {
#ifdef USE_FTGL
  char bufferfps[20];  
  sprintf( bufferfps, "%.1f fps", realfps);
  // glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
 
  glColor4f(1.0,1.0,1.0,1.0);
  glPushMatrix();
  glTranslatef(0.01,1, 0);
  glRasterPos2f(0, -0.05);
  title_font->FaceSize((unsigned)(20*(this->vh/512.0)));
   title_font->Render(bufferfps);
  
  glPopMatrix();
  // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
#endif /** USE_FTGL */    
}


//Actually draws the texture to the screen
//
//The Video Echo effect is also applied here
void Renderer::render_texture_to_screen(PresetOutputs *presetOutputs) { 

      int flipx=1,flipy=1;
      //glBindTexture( GL_TEXTURE_2D,this->renderTarget->textureID[0] );
     glMatrixMode(GL_TEXTURE);  
     glLoadIdentity();

    glClear( GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(0, 0, -15);  
     
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      
      //       glClear(GL_ACCUM_BUFFER_BIT);
      glColor4d(0.0, 0.0, 0.0,1.0f);

    DWRITE( "rendering texture to screen\n" );

   glBegin(GL_QUADS);
    glVertex3d( 0, 0, -1 );
     glVertex4d(-0.5,-0.5,-1,1);
     glVertex4d(-0.5,  0.5,-1,1);
     glVertex4d(0.5,  0.5,-1,1);
     glVertex4d(0.5, -0.5,-1,1);
      glEnd();
     
      
      glEnable(GL_TEXTURE_2D); 
      //glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );
//      glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID );

      // glAccum(GL_LOAD,0);
      // if (bDarken==1)  glBlendFunc(GL_SRC_COLOR,GL_ZERO); 
	
      //Draw giant rectangle and texture it with our texture!
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4d(-0.5,-0.5,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4d(-0.5,  0.5,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4d(0.5,  0.5,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4d(0.5, -0.5,-1,1);
      glEnd();
       
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //  if (bDarken==1)  glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_ALPHA); 

  // if (bDarken==1) { glAccum(GL_ACCUM,1-fVideoEchoAlpha); glBlendFunc(GL_SRC_COLOR,GL_ZERO); }

       glMatrixMode(GL_TEXTURE);

      //draw video echo
      glColor4f(0.0, 0.0, 0.0,presetOutputs->fVideoEchoAlpha);
      glTranslatef(.5,.5,0);
      glScalef(1.0/presetOutputs->fVideoEchoZoom,1.0/presetOutputs->fVideoEchoZoom,1);
       glTranslatef(-.5,-.5,0);    

      switch (((int)presetOutputs->nVideoEchoOrientation))
	{
	case 0: flipx=1;flipy=1;break;
	case 1: flipx=-1;flipy=1;break;
  	case 2: flipx=1;flipy=-1;break;
	case 3: flipx=-1;flipy=-1;break;
	default: flipx=1;flipy=1; break;
	}
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
      glEnd();

    
      glDisable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


      if (presetOutputs->bBrighten==1)
	{ 
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_ZERO, GL_DST_COLOR);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();

	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} 

      if (presetOutputs->bDarken==1)
	{ 
	  
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ZERO,GL_DST_COLOR);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  


	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} 
    

      if (presetOutputs->bSolarize)
	{ 
       
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ZERO,GL_ONE_MINUS_DST_COLOR);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_DST_COLOR,GL_ONE);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();


	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	} 

      if (presetOutputs->bInvert)
	{ 
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	} 
}
/*
void Renderer::render_texture_to_studio() { 

      int x,y;
      int flipx=1,flipy=1;
 
     glMatrixMode(GL_TEXTURE);  
     glLoadIdentity();

    glClear( GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(0, 0, -15);  
     
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
      
      //       glClear(GL_ACCUM_BUFFER_BIT);
      glColor4f(0.0, 0.0, 0.0,0.04);
      

   glBegin(GL_QUADS);
     glVertex4d(-0.5,-0.5,-1,1);
     glVertex4d(-0.5,  0.5,-1,1);
     glVertex4d(0.5,  0.5,-1,1);
     glVertex4d(0.5, -0.5,-1,1);
      glEnd();


      glColor4f(0.0, 0.0, 0.0,1.0);
      
      glBegin(GL_QUADS);
      glVertex4d(-0.5,0,-1,1);
      glVertex4d(-0.5,  0.5,-1,1);
      glVertex4d(0.5,  0.5,-1,1);
      glVertex4d(0.5, 0,-1,1);
      glEnd();
     
     glBegin(GL_QUADS);
     glVertex4d(0,-0.5,-1,1);
     glVertex4d(0,  0.5,-1,1);
     glVertex4d(0.5,  0.5,-1,1);
     glVertex4d(0.5, -0.5,-1,1);
     glEnd();

     glPushMatrix();
     glTranslatef(.25, .25, 0);
     glScalef(.5,.5,1);
     
     glEnable(GL_TEXTURE_2D);
    

      //Draw giant rectangle and texture it with our texture!
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4d(-0.5,-0.5,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4d(-0.5,  0.5,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4d(0.5,  0.5,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4d(0.5, -0.5,-1,1);
      glEnd();
       
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 

      glMatrixMode(GL_TEXTURE);

      //draw video echo
      glColor4f(0.0, 0.0, 0.0,presetOutputs->fVideoEchoAlpha);
      glTranslated(.5,.5,0);
      glScaled(1/presetOutputs->fVideoEchoZoom,1/presetOutputs->fVideoEchoZoom,1);
      glTranslated(-.5,-.5,0);    

      switch (((int)presetOutputs->nVideoEchoOrientation))
	{
	case 0: flipx=1;flipy=1;break;
	case 1: flipx=-1;flipy=1;break;
  	case 2: flipx=1;flipy=-1;break;
	case 3: flipx=-1;flipy=-1;break;
	default: flipx=1;flipy=1; break;
	}
      glBegin(GL_QUADS);
      glTexCoord4d(0, 1,0,1); glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
      glTexCoord4d(0, 0,0,1); glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 0,0,1); glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
      glTexCoord4d(1, 1,0,1); glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
      glEnd();

    
      //glDisable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

 // if (bDarken==1) { glAccum(GL_ACCUM,fVideoEchoAlpha); glAccum(GL_RETURN,1);}


      if (presetOutputs->bInvert)
	{ 
	  glColor4f(1.0, 1.0, 1.0,1.0);
	  glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
	  glBegin(GL_QUADS);
	  glVertex4f(-0.5*flipx,-0.5*flipy,-1,1);
	  glVertex4f(-0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx,  0.5*flipy,-1,1);
	  glVertex4f(0.5*flipx, -0.5*flipy,-1,1);
	  glEnd();
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	} 

      //  glTranslated(.5,.5,0);
  //  glScaled(1/fVideoEchoZoom,1/fVideoEchoZoom,1);
      //   glTranslated(-.5,-.5,0);    
      //glTranslatef(0,.5*vh,0);

     
      //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);
       
      glDisable(GL_TEXTURE_2D);
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glPushMatrix();
      glTranslatef(.25, -.25, 0);
      glScalef(.5,.5,1);
       glColor4f(1.0,1.0,1.0,1.0);

       for (x=0;x<presetInputs->gx;x++){
	 glBegin(GL_LINE_STRIP);
	 for(y=0;y<presetInputs->gy;y++){
	   glVertex4f((presetInputs->x_mesh[x][y]-.5), (presetInputs->y_mesh[x][y]-.5),-1,1);
	   //glVertex4f((origx[x+1][y]-.5) * vw, (origy[x+1][y]-.5) *vh ,-1,1);
	 }
	 glEnd();	
       }    
       
       for (y=0;y<presetInputs->gy;y++){
	 glBegin(GL_LINE_STRIP);
	 for(x=0;x<presetInputs->gx;x++){
	   glVertex4f((presetInputs->x_mesh[x][y]-.5), (presetInputs->y_mesh[x][y]-.5),-1,1);
	   //glVertex4f((origx[x+1][y]-.5) * vw, (origy[x+1][y]-.5) *vh ,-1,1);
	 }
	 glEnd();	
       }    

        glEnable( GL_TEXTURE_2D );
      
       
 // glTranslated(-.5,-.5,0);     glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 

    // Waveform display -- bottom-left 
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
       glMatrixMode(GL_MODELVIEW);
       glPushMatrix();
   glTranslatef(-.5,0, 0);

    glTranslatef(0,-0.10, 0);
   glBegin(GL_LINE_STRIP);
	     glColor4f(0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->treb_att*-7,-1);
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(.5,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->treb*7,-1);
	     glEnd(); 	
	       
	       glTranslatef(0,-0.13, 0);
 glBegin(GL_LINE_STRIP);
	      glColor4f(0,1.0,0.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->mid_att*-7,-1);
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(.5,1.0,0.0,0.5);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->mid*7,-1);
	     glEnd();
	  
	   
	     glTranslatef(0,-0.13, 0);
 glBegin(GL_LINE_STRIP);
	     glColor4f(1.0,0.0,0.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->bass_att*-7,-1);
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(.7,0.2,0.2,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->bass*7,-1);
	     glEnd();

 glTranslatef(0,-0.13, 0);
 glBegin(GL_LINES);
	     
	     glColor4f(1.0,1.0,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)),0 ,-1);   
	     glColor4f(1.0,0.6,1.0,1.0);
	     glVertex3f((((this->totalframes%256)/551.0)), beatDetect->vol*7,-1);
	     glEnd();

	     glPopMatrix();

   glDisable(GL_TEXTURE_2D);
}

*/

