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
#include "PCM.hpp"

#include <algorithm>
#include <cmath>


BeatDetect::BeatDetect(Pcm& _pcm)
    : pcm(_pcm)
{
}


void BeatDetect::Reset()
{
    this->treb = 0;
    this->mid = 0;
    this->bass = 0;
    this->trebAtt = 0;
    this->midAtt = 0;
    this->bassAtt = 0;
    this->volAtt = 0;
    this->volOld = 0;
}


void BeatDetect::CalculateBeatStatistics()
{
    volOld = vol;

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

    float const bassInstant = intensityBetween(ranges[0], ranges[1]);
    float const midInstant = intensityBetween(ranges[1], ranges[2]);
    float const trebInstant = intensityBetween(ranges[2], ranges[3]);
    float const volInstant = (bassInstant + midInstant + trebInstant) / 3.0f;

    bassSmoothed.Update(bassInstant);
    midSmoothed.Update(midInstant);
    trebSmoothed.Update(trebInstant);
    volSmoothed.Update(volInstant);

    bass = bassInstant / std::max(0.0001f, bassSmoothed.Get());
    mid = midInstant / std::max(0.0001f, midSmoothed.Get());
    treb = trebInstant / std::max(0.0001f, trebSmoothed.Get());
    vol = volInstant / std::max(0.0001f, volSmoothed.Get());

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

    trebAtt = .6f * trebAtt + .4f * treb;
    midAtt = .6f * midAtt + .4f * mid;
    bassAtt = .6f * bassAtt + .4f * bass;
    volAtt = .6f * volAtt + .4f * vol;

    bassAtt = std::min(bassAtt, 100.f);
    bass = std::min(bass, 100.f);
    midAtt = std::min(midAtt, 100.f);
    mid = std::min(mid, 100.f);
    trebAtt = std::min(trebAtt, 100.f);
    treb = std::min(treb, 100.f);
    volAtt = std::min(volAtt, 100.f);
    vol = std::min(vol, 100.f);
}
