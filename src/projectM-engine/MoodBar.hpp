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

class MoodBar {
public:
  MoodBar(unsigned int numFreqs, int size, int rate) : m_numFreqs(numFreqs), m_size(size), m_rate(rate) {
	calcBarkbandTable();	
  }

  ~MoodBar() { delete(m_barkband_table); }

  /// Calculate rgb mood values. Out should be an array containing
  /// numFreqs pairs of real/complex values.  
  void calculateMood(const float * out, float * rgb);

private:
  unsigned int m_numFreqs;
  int m_size;
  int m_rate;
  /* This calculates a table that caches which bark band slot each
   * incoming band is supposed to go in. */
  void calcBarkbandTable ();

  float m_amplitudes[24];
  static const unsigned int s_bark_bands[24];
  unsigned int * m_barkband_table;

};

#endif
