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

#include <numeric>
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
}


void BeatDetect::calculateBeatStatistics()
{
    vol_old = vol;

    std::array<float, fftLength> vdataL{};
    std::array<float, fftLength> vdataR{};
    pcm.GetSpectrum(vdataL.data(), CHANNEL_0, fftLength);
    pcm.GetSpectrum(vdataR.data(), CHANNEL_1, fftLength);

    auto const intensityBetween =
        [&vdataL, &vdataR](size_t const from, size_t const to) {
            return std::accumulate(&vdataL[from], &vdataL[to], 0.f) +
                   std::accumulate(&vdataR[from], &vdataR[to], 0.f);
        };

    static_assert(fftLength >= 256, "fftLength too small");
    std::array<size_t, 4> constexpr ranges{0, 3, 23, 255};

    float const bass_instant = intensityBetween(ranges[0], ranges[1]);
    float const mid_instant = intensityBetween(ranges[1], ranges[2]);
    float const treb_instant = intensityBetween(ranges[2], ranges[3]);
    float const vol_instant = (bass_instant + mid_instant + treb_instant) / 3.0f;

    bass_history.Update(bass_instant);
    mid_history.Update(mid_instant);
    treb_history.Update(treb_instant);
    vol_history.Update(vol_instant);

    bass = bass_instant / std::max(0.0001f, bass_history.Get());
    mid = mid_instant / std::max(0.0001f, mid_history.Get());
    treb = treb_instant / std::max(0.0001f, treb_history.Get());
    vol = vol_instant / std::max(0.0001f, vol_history.Get());

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
}
