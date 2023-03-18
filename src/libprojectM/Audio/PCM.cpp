/**
 * @file PCM.cpp
 * @brief Takes sound data from wherever and hands it back out.
 *
 * Returns PCM Data or spectrum data, or the derivative of the PCM data
 *
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

#include "PCM.hpp"

#include "fftsg.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>

namespace libprojectM {
namespace Audio {

/*
 * Here is where we try to do auto volume setting.  Doing this here
 * means that none of the code downstream (waveforms, beatdetect, etc) needs
 * to worry about it.
 *
 * 1) Don't over react to level changes within a song
 * 2) Ignore silence/gaps
 *
 * I don't know if it's necessary to have both sum and max, but that makes
 * it easier to experiment...
 */
auto PCM::AutoLevel::UpdateLevel(
    size_t const samples,
    double const sum,
    double const max) -> double
{

    // This is an arbitrary number that helps control
    //   a) how quickly the level can change and
    //   b) keeps code from being affected by how the caller provides data (lot of short buffers, or fewer long buffers)
    size_t constexpr autolevelSegment = 4096;

    if (sum / static_cast<double>(samples) < 0.00001)
    {
        return m_level;
    }
    m_levelSum += sum;
    m_levelax = std::max(m_levelax, max * 1.02);
    m_levelSamples += samples;
    if (m_levelSamples >= autolevelSegment || m_l0 <= 0)
    {
        double const maxRecent = std::max(std::max(m_l0, m_l1), std::max(m_l2, m_levelax));
        m_l0 = m_l1;
        m_l1 = m_l2;
        m_l2 = m_levelax;
        m_levelax *= 0.95;
        m_levelSamples = 0;
        m_levelSum = 0;
        m_level = (m_l0 <= 0) ? maxRecent : m_level * 0.96 + maxRecent * 0.04;
        m_level = std::max(m_level, 0.0001);
    }
    return m_level;
}


template<
    size_t lOffset,
    size_t rOffset,
    size_t stride,
    int signalAmplitude,
    int signalOffset,
    class SampleType>
void PCM::AddPcm(
    SampleType const* const samples,
    size_t const count)
{
    float sum = 0;
    float max = 0;
    for (size_t i = 0; i < count; i++)
    {
        size_t const j = (m_start + i) % maxSamples;
        m_pcmL[j] = (samples[lOffset + i * stride] - signalOffset) / float(signalAmplitude);
        m_pcmR[j] = (samples[rOffset + i * stride] - signalOffset) / float(signalAmplitude);
        sum += std::abs(m_pcmL[j]) + std::abs(m_pcmR[j]);
        max = std::max(std::max(max, std::abs(m_pcmL[j])), std::abs(m_pcmR[j]));
    }
    m_start = (m_start + count) % maxSamples;
    m_level = m_leveler.UpdateLevel(count, sum / 2, max);
}


void PCM::AddMono(float const* const samples, size_t const count)
{
    AddPcm<0, 0, 1, 1, 0>(samples, count);
}
void PCM::AddMono(uint8_t const* const samples, size_t const count)
{
    AddPcm<0, 0, 1, 128, 0>(samples, count);
}
void PCM::AddMono(int16_t const* const samples, size_t const count)
{
    AddPcm<0, 0, 1, 32768, 0>(samples, count);
}


void PCM::AddStereo(float const* const samples, size_t const count)
{
    AddPcm<0, 1, 2, 1, 0>(samples, count);
}
void PCM::AddStereo(uint8_t const* const samples, size_t const count)
{
    AddPcm<0, 1, 2, 128, 0>(samples, count);
}
void PCM::AddStereo(int16_t const* const samples, size_t const count)
{
    AddPcm<0, 1, 2, 32768, 0>(samples, count);
}


template<size_t aSize, size_t ipSize, size_t wSize>
void Rdft(
    int const isgn,
    std::array<double, aSize>& a,
    std::array<int, ipSize>& ip,
    std::array<double, wSize>& w)
{
    // per rdft() documentation
    //    length of ip >= 2+sqrt(n/2) and length of w == n/2
    //    n: length of a, power of 2, n >= 2
    // see fftsg.cpp
    static_assert(2 * (ipSize - 2) * (ipSize - 2) >= aSize,
                  "rdft invariant not preserved: length of ip >= 2+sqrt(n/2)");
    static_assert(2 * wSize == aSize,
                  "rdft invariant not preserved: length of w == n/2");
    static_assert(aSize >= 2,
                  "rdft invariant not preserved: n >= 2");
    static_assert((aSize & (aSize - 1)) == 0,
                  "rdft invariant not preserved: n is power of two");
    rdft(aSize, isgn, a.data(), ip.data(), w.data());
}


// puts sound data requested at provided pointer
//
// samples is number of PCM samples to return
// returned values are normalized from -1 to 1

void PCM::GetPcm(
    float* const data,
    CHANNEL const channel,
    size_t const samples) const
{
    assert(channel == 0 || channel == 1);

    CopyPcm(data, channel, samples);
}


void PCM::GetSpectrum(
    float* const data,
    CHANNEL const channel,
    size_t const samples)
{
    assert(channel == 0 || channel == 1);
    UpdateFftChannel(channel);

    auto const& spectrum = channel == 0 ? m_spectrumL : m_spectrumR;
    size_t const count = samples <= fftLength ? samples : fftLength;
    for (size_t i = 0; i < count; i++)
    {
        data[i] = spectrum[i];
    }
    for (size_t i = count; i < samples; i++)
    {
        data[0] = 0;
    }
}

void PCM::ResetAutoLevel()
{
    m_leveler = AutoLevel();
    m_level = 1.0f;
}

void PCM::UpdateFftChannel(size_t const channel)
{
    assert(channel == 0 || channel == 1);

    auto& freq = channel == 0 ? m_freqL : m_freqR;
    CopyPcm(freq.data(), channel, freq.size());
    Rdft(1, freq, m_ip, m_w);

    // compute magnitude data (m^2 actually)
    auto& spectrum = channel == 0 ? m_spectrumL : m_spectrumR;
    for (size_t i = 1; i < fftLength; i++)
    {
        auto const m2 = static_cast<float>(freq[i * 2] * freq[i * 2] + freq[i * 2 + 1] * freq[i * 2 + 1]);
        spectrum[i - 1] = static_cast<float>(i) * m2 / fftLength;
    }
    spectrum[fftLength - 1] = static_cast<float>(freq[1] * freq[1]);
}

// CPP17: std::clamp
auto Clamp(double const x, double const lo, double const hi) -> double
{
    return x > hi ? hi : x < lo ? lo
                                : x;
}

// pull data from circular buffer
void PCM::CopyPcm(float* const to, size_t const channel, size_t const count) const
{
    assert(channel == 0 || channel == 1);
    assert(count < maxSamples);
    auto const& from = channel == 0 ? m_pcmL : m_pcmR;
    const double volume = 1.0 / m_level;
    for (size_t i = 0, pos = m_start; i < count; i++)
    {
        if (pos == 0)
        {
            pos = maxSamples;
        }
        to[i] = static_cast<float>(from[--pos] * volume);
    }
}

void PCM::CopyPcm(double* const to, size_t const channel, size_t const count) const
{
    assert(channel == 0 || channel == 1);
    auto const& from = channel == 0 ? m_pcmL : m_pcmR;
    double const volume = 1.0 / m_level;
    for (size_t i = 0, pos = m_start; i < count; i++)
    {
        if (pos == 0)
        {
            pos = maxSamples;
        }
        to[i] = from[--pos] * volume;
    }
}

} // namespace Audio
} // namespace libprojectM
