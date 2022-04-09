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
    void Reset();

    /**
     * Calculates and updates information about the beat
     */
    void CalculateBeatStatistics();

    // getPCMScale() was added to address https://github.com/projectM-visualizer/projectm/issues/161
    // Returning 1.0 results in using the raw PCM data, which can make the presets look pretty unresponsive
    // if the application volume is low.
    float GetPCMScale()
    {
        return beatSensitivity;
    }

    float beatSensitivity{1.f};

    float treb{0.f};
    float mid{0.f};
    float bass{0.f};
    float volOld{0.f};

    float trebAtt{0.f};
    float midAtt{0.f};
    float bassAtt{0.f};
    float vol{0.f};
    float volAtt{0.f};

    Pcm& pcm;

private:
    class LowPassFilter
    {
    public:
        auto
        Update(float nextValue) noexcept -> void
        {
            m_current -= m_buffer[m_bufferPos] / bufferLength;
            m_current += nextValue / bufferLength;
            m_buffer[m_bufferPos] = nextValue;

            ++m_bufferPos;
            m_bufferPos %= bufferLength;
        }

        [[nodiscard]] auto
        Get() const noexcept -> float
        {
            return m_current;
        }

    private:
        static size_t constexpr bufferLength{80};
        size_t m_bufferPos{0};
        std::array<float, bufferLength> m_buffer{0.f};
        float m_current{0.f};
    };

    LowPassFilter bassSmoothed;
    LowPassFilter midSmoothed;
    LowPassFilter trebSmoothed;
    LowPassFilter volSmoothed;
};

#endif /** !_BEAT_DETECT_H */
