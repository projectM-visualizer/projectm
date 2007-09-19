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
	(float * rgb) {

  unsigned int i;
  float real, imag;
  
  for (i = 0; i < 24; ++i)
    m_amplitudes[i] = 0.f;
   
  for (i = 0; i < m_numFreqs; ++i)
    {
      /// @bug handle both left and right channels
      real = m_pcm->vdataL[2*i];  imag = m_pcm->vdataL[2*i + 1];
      m_amplitudes[m_barkband_table[i]] += sqrtf (real*real + imag*imag);
    }


  for (i = 0; i < 24; ++i)
    rgb[i/8] += m_amplitudes[i] * m_amplitudes[i];

  rgb[0] = sqrtf (rgb[0]);
  rgb[1] = sqrtf (rgb[1]);
  rgb[2] = sqrtf (rgb[2]);

  /// @bug verify normalized values
  std::cerr << "rgb: " << rgb[0] << "," << "," << rgb[1] << "," << rgb[2] << std::endl;
  for (i = 0; i < 3;i++) {
  	assert(rgb[i] <= 1.0); 
  	assert(rgb[i] >= 0.0); 
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
