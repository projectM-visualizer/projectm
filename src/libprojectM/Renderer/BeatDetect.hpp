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
#include <array>

class BeatDetect
{
public:
    explicit BeatDetect(Pcm& pcm);
    void reset();

    /**
     * Calculates and updates information about the beat
     */
    void calculateBeatStatistics();

    // getPCMScale() was added to address https://github.com/projectM-visualizer/projectm/issues/161
    // Returning 1.0 results in using the raw PCM data, which can make the presets look pretty unresponsive
    // if the application volume is low.
    float getPCMScale()
    {
        return beatSensitivity;
    }

    float beatSensitivity{1.f};

    float treb{0.f};
    float mid{0.f};
    float bass{0.f};
    float vol_old{0.f};

    float treb_att{0.f};
    float mid_att{0.f};
    float bass_att{0.f};
    float vol{0.f};
    float vol_att{0.f};

    Pcm& pcm;

private:
    // this is the size of the buffer used to determine avg levels of the input audio
    // the actual time represented in the history depends on FPS
    static size_t constexpr BEAT_HISTORY_LENGTH{80};

    size_t beat_buffer_pos{0};

    std::array<float, BEAT_HISTORY_LENGTH> bass_buffer{0.f};
    float bass_history{0.f};
    float bass_instant{0.f};

    std::array<float, BEAT_HISTORY_LENGTH> mid_buffer{0.f};
    float mid_history{0.f};
    float mid_instant{0.f};

    std::array<float, BEAT_HISTORY_LENGTH> treb_buffer{0.f};
    float treb_history{0.f};
    float treb_instant{0.f};

    std::array<float, BEAT_HISTORY_LENGTH> vol_buffer{0.f};
    float vol_history{0.f};
    float vol_instant{0.f};
};

#endif /** !_BEAT_DETECT_H */
