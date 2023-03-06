/**
 * @file BeatDetect.cpp
 * @brief Beat detection class. Takes decompressed sound buffers and returns various characteristics.
 *
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
#pragma once

#include "FrameAudioData.hpp"
#include "PCM.hpp"

#include <array>

namespace libprojectM {
namespace Audio {

class BeatDetect
{
public:
    // We should probably remove pcm from the constructor,
    // and just pass it as an argument to CalculateBeatStatistics.
    explicit BeatDetect(PCM& pcm);

    auto Reset() noexcept -> void;

    /**
     * Calculates and updates information about the beat
     */
    auto CalculateBeatStatistics() -> void;

    // getPCMScale() was added to address https://github.com/projectM-visualizer/projectm/issues/161
    // Returning 1.0 results in using the raw PCM data, which can make the presets look pretty unresponsive
    // if the application volume is low.
    [[nodiscard]]
    auto GetPCMScale() const noexcept -> float;

    /**
     * @brief Returns a filled FrameAudioData structure for the current frame.
     * @return A FrameAudioData structure with waveform, spectrum beat detection data.
     */
    [[nodiscard]]
    auto GetFrameAudioData() const -> FrameAudioData;

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

    PCM& pcm;

private:
    class LowPassFilter
    {
    public:
        auto
        Update(float nextValue) noexcept -> void;

        [[nodiscard]] auto
        Get() const noexcept -> float;

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

} // namespace Audio
} // namespace libprojectM
