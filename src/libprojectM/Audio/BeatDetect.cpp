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

#include "BeatDetect.hpp"
#include "PCM.hpp"

#include <algorithm>
#include <numeric>

namespace libprojectM {
namespace Audio {

BeatDetect::BeatDetect(PCM& _pcm)
    : pcm(_pcm)
{
}


auto BeatDetect::Reset() noexcept -> void
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


auto BeatDetect::GetPCMScale() const noexcept -> float
{
    return beatSensitivity;
}


auto BeatDetect::GetFrameAudioData() const -> FrameAudioData
{
    FrameAudioData data{};

    pcm.GetPcm(data.waveformLeft.data(), CHANNEL_L, WaveformSamples);
    pcm.GetPcm(data.waveformRight.data(), CHANNEL_R, WaveformSamples);
    pcm.GetSpectrum(data.spectrumLeft.data(), CHANNEL_L, SpectrumSamples);
    pcm.GetSpectrum(data.spectrumRight.data(), CHANNEL_R, SpectrumSamples);

    data.vol = vol;
    data.volAtt = volAtt;

    data.bass = bass;
    data.bassAtt = bassAtt;
    data.mid = mid;
    data.midAtt = midAtt;
    data.treb = treb;
    data.trebAtt = trebAtt;

    return data;
}

auto BeatDetect::CalculateBeatStatistics() -> void
{
    volOld = vol;

    std::array<float, fftLength> const freqL =
        [this]() {
            std::array<float, fftLength> freq{};
            pcm.GetSpectrum(freq.data(), CHANNEL_L, fftLength);
            return freq;
        }();
    std::array<float, fftLength> const freqR =
        [this]() {
            std::array<float, fftLength> freq{};
            pcm.GetSpectrum(freq.data(), CHANNEL_R, fftLength);
            return freq;
        }();

    auto const intensityBetween = [&freqL, &freqR](size_t const from, size_t const to) {
        return std::accumulate(&freqL[from], &freqL[to], 0.f) +
               std::accumulate(&freqR[from], &freqR[to], 0.f);
    };

    auto const& updateBand =
        [](float& band, float& bandAtt, LowPassFilter& bandSmoothed, float const bandIntensity) {
            bandSmoothed.Update(bandIntensity);
            band = bandIntensity / std::max(0.0001f, bandSmoothed.Get());
            bandAtt = .6f * bandAtt + .4f * band;
            bandAtt = std::min(bandAtt, 100.f);
            band = std::min(band, 100.f);
        };

    static_assert(fftLength >= 256, "fftLength too small");
    std::array<size_t, 4> constexpr ranges{0, 3, 23, 255};

    float const bassIntensity = intensityBetween(ranges[0], ranges[1]);
    updateBand(bass, bassAtt, bassSmoothed, bassIntensity);

    float const midIntensity = intensityBetween(ranges[1], ranges[2]);
    updateBand(mid, midAtt, midSmoothed, midIntensity);

    float const trebIntensity = intensityBetween(ranges[2], ranges[3]);
    updateBand(treb, trebAtt, trebSmoothed, trebIntensity);

    float const volIntensity = bassIntensity + midIntensity + trebIntensity;
    updateBand(vol, volAtt, volSmoothed, volIntensity);
}



auto BeatDetect::LowPassFilter::Update(float nextValue) noexcept -> void
{
    m_current -= m_buffer[m_bufferPos] / bufferLength;
    m_current += nextValue / bufferLength;
    m_buffer[m_bufferPos] = nextValue;

    ++m_bufferPos;
    m_bufferPos %= bufferLength;
}


auto BeatDetect::LowPassFilter::Get() const noexcept -> float
{
    return m_current;
}

} // namespace Audio
} // namespace libprojectM
