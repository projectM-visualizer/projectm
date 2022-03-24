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

#include <stdio.h>
#include <stdlib.h>

#include "wipemalloc.h"

#include "BeatDetect.hpp"
#include "Common.hpp"
#include "PCM.hpp"

#include <algorithm>
#include <cmath>


BeatDetect::BeatDetect(Pcm& _pcm)
    : pcm(_pcm)
{
}


void BeatDetect::reset()
{
    this->treb = 0;
    this->mid = 0;
    this->bass = 0;
    this->treb_att = 0;
    this->mid_att = 0;
    this->bass_att = 0;
    this->vol_att = 0;
    this->vol_old = 0;
    this->vol_instant = 0;
}


void BeatDetect::calculateBeatStatistics()
{
    size_t constexpr fft_length = fftLength;
    float vdataL[fftLength];
    float vdataR[fftLength];
    pcm.GetSpectrum(vdataL, CHANNEL_0, fftLength);
    pcm.GetSpectrum(vdataR, CHANNEL_1, fftLength);


    static_assert(fft_length >= 256, "fft_length too small");
    unsigned ranges[4] = {0, 3, 23, 255};

    bass_instant = 0;
    for (unsigned i = ranges[0]; i < ranges[1]; i++)
        bass_instant += vdataL[i] + vdataR[i];
    bass_history -= bass_buffer[beat_buffer_pos] * (1.0 / bass_buffer.size());
    bass_buffer[beat_buffer_pos] = bass_instant;
    bass_history += bass_instant * (1.0 / bass_buffer.size());

    mid_instant = 0;
    for (unsigned i = ranges[1]; i < ranges[2]; i++)
        mid_instant += vdataL[i] + vdataR[i];
    mid_history -= mid_buffer[beat_buffer_pos] * (1.0 / mid_buffer.size());
    mid_buffer[beat_buffer_pos] = mid_instant;
    mid_history += mid_instant * (1.0 / mid_buffer.size());

    treb_instant = 0;
    for (unsigned i = ranges[2]; i < ranges[3]; i++)
        treb_instant += vdataL[i] + vdataR[i];
    treb_history -= treb_buffer[beat_buffer_pos] * (1.0 / treb_buffer.size());
    treb_buffer[beat_buffer_pos] = treb_instant;
    treb_history += treb_instant * (1.0 / treb_buffer.size());

    vol_instant = (bass_instant + mid_instant + treb_instant) / 3.0f;
    vol_history -= (vol_buffer[beat_buffer_pos]) * (1.0 / vol_buffer.size());
    vol_buffer[beat_buffer_pos] = vol_instant;
    vol_history += vol_instant * (1.0 / vol_buffer.size());

    //    fprintf(stderr, "%6.3f %6.2f %6.3f\n", bass_history/vol_history, mid_history/vol_history, treb_history/vol_history);
    bass = bass_instant / std::max(0.0001f, bass_history);
    mid = mid_instant / std::max(0.0001f, mid_history);
    treb = treb_instant / std::max(0.0001f, treb_history);
    vol = vol_instant / std::max(0.0001f, vol_history);

    if (std::isnan(treb))
    {
        treb = 0.0;
    }
    if (std::isnan(mid))
    {
        mid = 0.0;
    }
    if (std::isnan(bass))
    {
        bass = 0.0;
    }

    treb_att = .6f * treb_att + .4f * treb;
    mid_att = .6f * mid_att + .4f * mid;
    bass_att = .6f * bass_att + .4f * bass;
    vol_att = .6f * vol_att + .4f * vol;

    bass_att = std::min(bass_att, 100.f);
    bass = std::min(bass, 100.f);
    mid_att = std::min(mid_att, 100.f);
    mid = std::min(mid, 100.f);
    treb_att = std::min(treb_att, 100.f);
    treb = std::min(treb, 100.f);
    vol_att = std::min(vol_att, 100.f);
    vol = std::min(vol, 100.f);

    beat_buffer_pos++;
    if (beat_buffer_pos > 79)
        beat_buffer_pos = 0;
}
