/* projectM -- Milkdrop-esque visualisation SDK
   Copyright (C)2003-2007 projectM Team
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

#ifndef _MOODBAR_HPP
#define _MOODBAR_HPP
#include "PCM.hpp"
#include "RingBuffer.hpp"

class MoodBar {
public:


  MoodBar(PCM * pcm) : m_numFreqs(pcm->numsamples/2 + 1), m_size(pcm->numsamples), m_rate(FIXED_SAMPLE_RATE), m_pcm(pcm) {
	calcBarkbandTable();	
	resetBuffer();

  }

  MoodBar(int rate, PCM * pcm) : m_numFreqs(pcm->numsamples/2 + 1), m_size(pcm->numsamples), m_rate(rate), m_pcm(pcm) {
	calcBarkbandTable();	
	resetBuffer();
  }

  ~MoodBar() { delete(m_barkband_table); }

  /// Calculate rgb mood values for both left and right channels. 
  /// Uses the pcm instance's latest assignment into its
  /// pcmL/R data buffers as inputs 
  void calculateMood(float * rgb_left, float * rgb_right, float * rgb_avg);

private:
  void resetBuffer() ;

  /// @bug need to find this elsewhere
  static const int FIXED_SAMPLE_RATE = 44100;

  unsigned int m_numFreqs;
  int m_size;
  int m_rate;
  /* This calculates a table that caches which bark band slot each
   * incoming band is supposed to go in. */
  void calcBarkbandTable ();
  PCM * m_pcm;
  void standardNormalize(float * rgb);
  float m_amplitudes_left[24];
  float m_amplitudes_right[24];
  
  void stretchNormalize (float * colors);
  static const unsigned int s_bark_bands[24];

  RingBuffer<float> m_ringBuffers[3];

  unsigned int * m_barkband_table;

};

#endif
