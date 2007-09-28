//
// C++ Implementation: MoodBar
//
// Description: 
//  Calculates moodbar values from spectrum frequency data. This code is shamelessly stolen
//  from the GPL'ed moodbar project / research by Gavin Wood. The biggest difference between
//  his implementation and mine is the adaptations for "real time" normalization. A ring buffer
//  is used to remember a fixed window of most recently observed rgb values. All such unnormalized
//  values are used to calculate a "stretched" normalized set of color intensities.
// WARNING: CODE IS VERY EXPERIMENTAL
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "MoodBar.hpp"
#include "Common.hpp"

#include <cmath>
extern "C" {
#include <malloc.h>
}
#include <iostream>
#include <cassert>
#include "PCM.hpp"


#define SPECTRUM_BAND_FREQ(band, size, rate) \
  (((float)(band))*((float)(rate))/((float)(size)))


/// Bark band hard coded values stolen directly from moodbar implementation
const unsigned int MoodBar::s_bark_bands[]  = 
{ 100,  200,  300,  400,  510,  630,  770,   920, 
  1080, 1270, 1480, 1720, 2000, 2320, 2700,  3150, 3700, 4400, 
  5300, 6400, 7700, 9500, 12000, 15500 };


void MoodBar::standardNormalize(float * rgb) {
	
	float sum = 0;

	for (int i = 0; i < 3; i++) {
		sum += rgb[i];
	}
	
	if (sum == 0)
		return;
		
	for (int i = 0; i < 3; i++) {		
		rgb[i] /= sum;
		rgb[i]= projectM_fmin(1.0, rgb[i] + sqrtf(rgb[i]));
	}
	
}

void MoodBar::resetBuffer()  {

	for (int c = 0; c < 3; c++)
		for (unsigned int i = 0; i < RingBuffer<float>::RING_BUFFER_SIZE; i++)
			m_ringBuffers[c].append(.0f);
		
}

void MoodBar::calculateMood
	(float * rgb_left, float * rgb_right, float * rgb_avg) {

  unsigned int i;
  float real_left, real_right, real_avg;
  float imag_left, imag_right, imag_avg;


  for (i = 0; i < 24; ++i) {
    m_amplitudes_left[i] = 0.f;
    m_amplitudes_right[i] = 0.f;
    m_amplitudes_avg[i] = 0.f;
  }

  for (i = 0; i < m_numFreqs-2; ++i)
    {
      //std::cerr << "vdataL[2*" << i << "] = " << m_pcm->vdataL[2*i] << std::endl;
      //std::cerr << "vdataL[2*" << i << "+1] = " << m_pcm->vdataL[2*i+1] << std::endl;
 
      real_left = m_pcm->pcmdataL[2*i];  imag_left = m_pcm->pcmdataL[2*i + 1];
      real_right = m_pcm->pcmdataR[2*i];  imag_right = m_pcm->pcmdataR[2*i + 1];
      real_avg = (real_left + real_right) / 2.0;
      imag_avg = (imag_left + imag_right) / 2.0;
     
      m_amplitudes_left[m_barkband_table[i]] += sqrtf (real_left*real_left + imag_left*imag_left);
      m_amplitudes_right[m_barkband_table[i]] += sqrtf (real_right*real_right + imag_right*imag_right);
      m_amplitudes_avg[m_barkband_table[i]] += sqrtf (real_avg*real_avg + imag_avg*imag_avg);

    }

  for (i= 0; i < 3; i++) {
	rgb_left[i] = 0.0;
  	rgb_right[i] = 0.0; 
	rgb_avg[i] = 0.0;
  }

  for (i = 0; i < 24; ++i) {
    rgb_left[i/8] += m_amplitudes_left[i] * m_amplitudes_left[i];
    rgb_right[i/8] += m_amplitudes_right[i] * m_amplitudes_right[i];
    rgb_avg[i/8] += m_amplitudes_avg[i] * m_amplitudes_avg[i];
  }

   for (i = 0; i < 3; i++) {
	
	rgb_left[i] = sqrtf (rgb_left[i]);
	rgb_right[i] = sqrtf (rgb_right[i]);
	rgb_avg[i] = sqrtf (rgb_avg[i]);

   }


  std::cerr << "rgb_avg (pre norm): " << rgb_avg[0] << "," << rgb_avg[1] << "," << rgb_avg[2] << std::endl;

   //stretchNormalize(rgb_left);
   //stretchNormalize(rgb_right);
   //stretchNormalize(rgb_avg);

   standardNormalize(rgb_left);
   standardNormalize(rgb_right);
   standardNormalize(rgb_avg);


#if 1
  std::cerr << "rgb_avg (post norm): " << rgb_avg[0] << "," << rgb_avg[1] << "," << rgb_avg[2] << std::endl;
  for (i = 0; i < 3; i++) {
  	assert(rgb_avg[i] <= 1.0); 
  	assert(rgb_avg[i] >= 0.0); 
  }
#endif

}


/* This calculates a table that caches which bark band slot each
 * incoming band is supposed to go in. */
void
MoodBar::calcBarkbandTable ()
{
  unsigned int i;
  unsigned int barkband = 0;

  /* Avoid divide-by-zero */
  if (!m_size  ||  !m_rate)
    return;

  if (m_barkband_table)
    delete[] (m_barkband_table);

  m_barkband_table = new unsigned int[(m_numFreqs * sizeof (unsigned int))];
  
  for (i = 0; i < m_numFreqs; ++i)
    {
      if (barkband < 23 && 
	  (unsigned int) SPECTRUM_BAND_FREQ (i, m_size, m_rate) 
 	   >= s_bark_bands[barkband])
	barkband++;

      m_barkband_table[i] = barkband;

    }
}



/* Copied and mod'ed from moodbar source code which also says the following:
   The normalization code was copied from Gav Wood's Exscalibar
 * library, normalise.cpp
 */
void MoodBar::stretchNormalize (float * rgb)
{
  float mini, maxi, tu = 0.f, tb = 0.f;
  float avgu = 0.f, avgb = 0.f, delta, avg = 0.f;
  float avguu = 0.f, avgbb = 0.f;
  unsigned int i;
  int t = 0;

  // iterate over r,g,b
  for (int c = 0; c < 3; c++) {

  // Append latest un-normalized value on ring buffer
  m_ringBuffers[c].append(rgb[c]);
//  std::cerr << "current = " << m_ringBuffers[c].current() << std::endl;
 
  const unsigned int numvals = RingBuffer<float>::RING_BUFFER_SIZE;

  if (numvals == 0)
	return;

  //int oldcurrent = m_ringBuffers[c].current();

  mini = maxi = m_ringBuffers[c].back();
  
  // Compute max and min values of the array
  for (i = 1; i < numvals; i++)
    {

      float _tmpval = m_ringBuffers[c].back();
      if (_tmpval > maxi) 
	maxi = _tmpval;
      else if (_tmpval < mini) 
	mini = _tmpval;
    }

   //assert((oldcurrent % RingBuffer<float>::RING_BUFFER_SIZE) == (m_ringBuffers[c].current() % RingBuffer<float>::RING_BUFFER_SIZE));

  // Compute array average excluding the maximum and minimum ranges
  for (i = 0; i < numvals; i++)
    {
  	float _tmpval = m_ringBuffers[c].back();
 
      if(_tmpval != mini && _tmpval != maxi)
	{
	  avg += _tmpval / ((float) numvals); 
	  t++; 
	}
    }
   
  // Now compute average values if we partition the elements into
  // two sets segmented by the previously computed average
  // Again we exclude the max and min elements.
  for (i = 0; i < numvals; i++)
    {
	float _tmpval = m_ringBuffers[c].back();
 
      if (_tmpval != mini && _tmpval != maxi)
	{
	  if (_tmpval > avg) 
	    { 
	      avgu += _tmpval; 
	      tu++; 
	    }
	  else 
	    { 
	      avgb += _tmpval;
	      tb++; 
	    }
	}
    }
   
  // This normalizes the computations in the previous for loop
  // so they represent proper averages of their respective sets
  avgu /= (float) tu;
  avgb /= (float) tb;

  tu = 0.f; 
  tb = 0.f;

  // Computes two averages. One of values that are less than previously computer lower bound and 
  // one of values greater than the previously computed upper bound.
  // As usual, min and max elements are excluded.
  for (i = 0; i < numvals; i++) {

	float _tmpval = m_ringBuffers[c].back();
	
      if (_tmpval != mini && _tmpval != maxi)
	{
	  if (_tmpval > avgu) 
	    { 
	      avguu += _tmpval;
	      tu++; 
	    }

	  else if (_tmpval < avgb) 
	    { 
	      avgbb += _tmpval;
	      tb++; 
	    }
	}
    }
   
  avguu /= (float) tu;
  avgbb /= (float) tb;

  std::cerr << "avguu: " << avguu << ", avgbb: " << avgbb << std::endl;

  // lost from here- what is theory behind this?
  mini = projectM_fmax (avg + (avgb - avg) * 2.f, avgbb);
  maxi = projectM_fmin (avg + (avgu - avg) * 2.f, avguu);
  delta = maxi - mini;

  if (delta == 0.f)
    delta = 1.f;

   rgb[c] = projectM_fmin(1.f, projectM_fmax(0.f, (rgb[c] - mini) / delta));	
  }
}
