//
// C++ Implementation: MoodBar
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "MoodBar.hpp"
#include <cmath>
extern "C" {
#include <malloc.h>
}
#include <iostream>
#include <cassert>
#include "PCM.hpp"

const unsigned int MoodBar::s_bark_bands[]  = 
{ 100,  200,  300,  400,  510,  630,  770,   920, 
  1080, 1270, 1480, 1720, 2000, 2320, 2700,  3150, 3700, 4400, 
  5300, 6400, 7700, 9500, 12000, 15500 };

void MoodBar::calculateMood
	(float * rgb_left, float * rgb_right, float * rgb_avg) {

  unsigned int i;
  float real_left, real_right;
  float imag_left, imag_right;


  for (i = 0; i < 24; ++i) {
    m_amplitudes_left[i] = 0.f;
    m_amplitudes_right[i] = 0.f;

  }
  for (i = 0; i < m_numFreqs; ++i)
    {
      real_left = m_pcm->vdataL[2*i];  imag_left = m_pcm->vdataL[2*i + 1];
      real_right = m_pcm->vdataR[2*i];  imag_right = m_pcm->vdataR[2*i + 1];

      m_amplitudes_left[m_barkband_table[i]] += sqrtf (real_left*real_left + imag_left*imag_left);
      m_amplitudes_right[m_barkband_table[i]] += sqrtf (real_right*real_right + imag_right*imag_right);
    }

  for (i = 0; i < 24; ++i) {
    rgb_left[i/8] += m_amplitudes_left[i] * m_amplitudes_left[i];
    rgb_right[i/8] += m_amplitudes_right[i] * m_amplitudes_right[i];
  }

   for (i = 0; i < 3; i++) {
	rgb_left[i] = sqrtf (rgb_left[i]);
	rgb_right[i] = sqrtf (rgb_right[i]);
	
	rgb_avg[i] = (rgb_left[i] + rgb_right[i]) / 2;
	rgb_avg[i] = sqrtf (rgb_avg[i]); 
   }

  /// @bug verify normalized values
  std::cerr << "rgb_avg: " << rgb_avg[0] << "," << "," << rgb_avg[1] << "," << rgb_avg[2] << std::endl;
  for (i = 0; i < 3; i++) {
  	assert(rgb_avg[i] <= 1.0); 
  	assert(rgb_avg[i] >= 0.0); 
  }

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
    free (m_barkband_table);

  m_barkband_table = new unsigned int[(m_numFreqs * sizeof (unsigned int))];
  
  for (i = 0; i < m_numFreqs; ++i)
    {
      if (barkband < 23 && 1)
	  //(unsigned int) GST_SPECTRUM_BAND_FREQ (i, m_size, m_rate) 
	  //  >= s_bark_bands[barkband])
	barkband++;

      m_barkband_table[i] = barkband;

    }
}
