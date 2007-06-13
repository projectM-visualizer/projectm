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
 * Takes sound data from wherever and returns beat detection values
 * Uses statistical Energy-Based methods. Very simple
 * 
 * Some stuff was taken from Frederic Patin's beat-detection article,
 * you'll find it online
 */

#include <stdlib.h>
#include <stdio.h>

#include "wipemalloc.h"

#include "common.h"
#include "projectM.h"
#include "BeatDetect.h"
#include "PCM.h"

DLLEXPORT BeatDetect::BeatDetect() {
  int x,y; 

  vol_instant=0;
  vol_history=0;

  for (y=0;y<80;y++)
    {
      vol_buffer[y]=0;
    }

  beat_buffer_pos=0;

  beat_val=(float *)wipemalloc(32*sizeof(float));
  beat_att=(float *)wipemalloc(32*sizeof(float));
  beat_variance=(float *)wipemalloc(32*sizeof(float));

  for (x=0;x<32;x++) {
      beat_instant[x]=0;
      beat_history[x]=0;
      beat_val[x]=1.0;
      beat_att[x]=1.0;
      beat_variance[x]=0;
      for (y=0;y<80;y++) {
	    beat_buffer[x][y]=0;
	    }
    }

    this->treb = 0;
    this->mid = 0;
    this->bass = 0;
    this->bass_old = 0;
    this->beat_sensitivity = 8.00;
    this->treb_att = 0;
    this->mid_att = 0;
    this->bass_att = 0;
    this->vol = 0;

    pcm = new PCM();
  }

void BeatDetect::initBeatDetect() {

  } 

void BeatDetect::reset() {
  treb = 0;
  mid = 0;
  bass = 0;
  treb_att = 0;
  mid_att = 0;
  bass_att = 0;
  }

void BeatDetect::detectFromSamples() {
    bass_old = bass;
    bass=0;mid=0;treb=0;

    getBeatVals(pcm->vdataL,pcm->vdataR);
  }

void BeatDetect::getBeatVals( float *vdataL,float *vdataR ) {

  int linear=0;
  int x,y;
  float temp2=0;

  vol_instant=0;

      for ( x=0;x<16;x++)
	{
	  
	  beat_instant[x]=0;
	  for ( y=linear*2;y<(linear+8+x)*2;y++)
	    {
	      beat_instant[x]+=((vdataL[y]*vdataL[y])+(vdataR[y]*vdataR[y]))*(1.0/(8+x)); 
//	      printf( "beat_instant[%d]: %f %f %f\n", x, beat_instant[x], vdataL[y], vdataR[y] );
	      vol_instant+=((vdataL[y]*vdataL[y])+(vdataR[y]*vdataR[y]))*(1.0/512.0);

	    }
	  
	  linear=y/2;
	  beat_history[x]-=(beat_buffer[x][beat_buffer_pos])*.0125;
	  beat_buffer[x][beat_buffer_pos]=beat_instant[x];
	  beat_history[x]+=(beat_instant[x])*.0125;
	  
	  beat_val[x]=(beat_instant[x])/(beat_history[x]);
	  
	  beat_att[x]+=(beat_instant[x])/(beat_history[x]);


 	  
	}
      
      vol_history-=(vol_buffer[beat_buffer_pos])*.0125;
      vol_buffer[beat_buffer_pos]=vol_instant;
      vol_history+=(vol_instant)*.0125;

      mid=0;
      for(x=1;x<10;x++)
	{
	 mid+=(beat_instant[x]);
	  temp2+=(beat_history[x]);
	 
	}

	 mid=mid/(1.5*temp2);
	 temp2=0;
	 treb=0;
 	  for(x=10;x<16;x++)
	    { 
	      treb+=(beat_instant[x]);
	      temp2+=(beat_history[x]);
	    }
	  treb=treb/(1.5*temp2);
//	  *vol=vol_instant/(1.5*vol_history);
	  vol=vol_instant/(1.5*vol_history);

	  bass=(beat_instant[0])/(1.5*beat_history[0]);

	  if ( isnan( treb ) ) {
	    treb = 0.0;
	  }
	  if ( isnan( mid ) ) {
	    mid = 0.0;
	  }
	  if ( isnan( bass ) ) {
	    bass = 0.0;
	  }
	  treb_att=.6 * treb_att + .4 * treb;
	  mid_att=.6 * mid_att + .4 * mid;
	  bass_att=.6 * bass_att + .4 * bass;

    DWRITE( "BeatDetect::getBeatVals: treb: %f\tmid: %f\tbass: %f\n",
            treb, mid, bass );
    DWRITE( "BeatDetect::getBeatVals: treb_att: %f\tmid_att: %f\tbass_att: %f\n",
            treb_att, mid_att, bass_att );

	   // *vol=(beat_instant[3])/(beat_history[3]);
	  beat_buffer_pos++;
	  if( beat_buffer_pos>79)beat_buffer_pos=0;
	
}

void BeatDetect::freeBeatDetect() {

  free(beat_att);
  free(beat_val);
  free(beat_variance);

  beat_att = NULL;
  beat_val = NULL;
  beat_variance = NULL;
}
