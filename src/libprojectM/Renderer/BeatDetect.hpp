/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
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
 * $Id$
 *
 * Beat detection class. Takes decompressed sound buffers and returns
 * various characteristics
 *
 * $Log$
 *
 */

#ifndef _BEAT_DETECT_H
#define _BEAT_DETECT_H

#include "../PCM.hpp"
#include "../dlldefs.h"
#include <algorithm>
#include <cmath>


// this is the size of the buffer used to determine avg levels of the input audio
// the actual time represented in the history depends on FPS
#define BEAT_HISTORY_LENGTH 80

class DLLEXPORT BeatDetect
{
	public:
		float treb ;
		float mid ;
		float bass ;
		float vol_old ;
		float beat_sensitivity;
		float treb_att ;
		float mid_att ;
		float bass_att ;
		float vol;
        float vol_att ;

		PCM *pcm;

		/** Methods */
		explicit BeatDetect(PCM *pcm);
		~BeatDetect();
		void reset();
		void detectFromSamples();
		void getBeatVals( float samplerate, unsigned fft_length, float *vdataL, float *vdataR );

        // getPCMScale() was added to address https://github.com/projectM-visualizer/projectm/issues/161
        // Returning 1.0 results in using the raw PCM data, which can make the presets look pretty unresponsive
        // if the application volume is low.
		float getPCMScale();

	private:
		int beat_buffer_pos;
        float bass_buffer[BEAT_HISTORY_LENGTH];
		float bass_history;
        float bass_instant;

		float mid_buffer[BEAT_HISTORY_LENGTH];
        float mid_history;
		float mid_instant;

		float treb_buffer[BEAT_HISTORY_LENGTH];
		float treb_history;
		float treb_instant;

        float vol_buffer[BEAT_HISTORY_LENGTH];
        float vol_history;
        float vol_instant;
};

#endif /** !_BEAT_DETECT_H */
