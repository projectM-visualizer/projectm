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
 * $Id: PCM.c,v 1.3 2006/03/13 20:35:26 psperl Exp $
 *
 * Takes sound data from wherever and hands it back out.
 * Returns PCM Data or spectrum data, or the derivative of the PCM data
 */
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "Common.hpp"
#include "PCM.hpp"
#include "fftsg.h"
#include "wipemalloc.h"
#include <cassert>


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
auto Pcm::AutoLevel::UpdateLevel(
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
void Pcm::AddPcm(
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
    m_newSamples += count;
    m_level = m_leveler.UpdateLevel(count, sum / 2, max);
}


void Pcm::AddMono(float const* const samples, size_t const count)
{
    AddPcm<0, 0, 1, 1, 0>(samples, count);
}
void Pcm::AddMono(uint8_t const* const samples, size_t const count)
{
    AddPcm<0, 0, 1, 128, 0>(samples, count);
}
void Pcm::AddMono(int16_t const* const samples, size_t const count)
{
    AddPcm<0, 0, 1, 32768, 0>(samples, count);
}


void Pcm::AddStereo(float const* const samples, size_t const count)
{
    AddPcm<0, 1, 2, 1, 0>(samples, count);
}
void Pcm::AddStereo(uint8_t const* const samples, size_t const count)
{
    AddPcm<0, 1, 2, 128, 0>(samples, count);
}
void Pcm::AddStereo(int16_t const* const samples, size_t const count)
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

void Pcm::GetPcm(
    float* const data,
    CHANNEL const channel,
    size_t const samples) const
{
    assert(channel == 0 || channel == 1);

    CopyPcm(data, channel, samples);
}


void Pcm::GetSpectrum(
    float* const data,
    CHANNEL const channel,
    size_t const samples)
{
    assert(channel == 0 || channel == 1);
    UpdateFFT();

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

void Pcm::UpdateFFT()
{
    if (m_newSamples == 0)
    {
        return;
    }

    UpdateFftChannel(0);
    UpdateFftChannel(1);
    m_newSamples = 0;
}

void Pcm::UpdateFftChannel(size_t const channel)
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
void Pcm::CopyPcm(float* const to, size_t const channel, size_t const count) const
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

void Pcm::CopyPcm(double* const to, size_t const channel, size_t const count) const
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


// TESTS


#include "TestRunner.hpp"

#ifndef NDEBUG

#define TEST(cond)                          \
    if (!verify(__FILE__ ": " #cond, cond)) \
    return false
#define TEST2(str, cond)    \
    if (!verify(str, cond)) \
    return false

class PCMTest : public Test
{
public:
    PCMTest()
        : Test("PCMTest")
    {
    }

    static auto Eq(float const a, float const b) -> bool
    {
        return std::abs(a - b) < (std::abs(a) + std::abs(b) + 1) / 1000.0f;
    }

    /* smoke test for each addPCM method */
    auto TestAddpcm() -> bool
    {
        Pcm pcm;

        // mono float
        {
            size_t constexpr samples = 301;
            std::array<float, samples> data{};
            for (size_t i = 0; i < samples; i++)
            {
                data[i] = ((float) i) / (samples - 1);
            }
            for (size_t i = 0; i < 10; i++)
            {
                pcm.AddMono(data.data(), samples);
            }
            std::array<float, samples> copy{};
            pcm.m_level = 1.0;
            pcm.CopyPcm(copy.data(), 0, copy.size());
            for (size_t i = 0; i < samples; i++)
            {
                TEST(Eq(copy[i], ((float) samples - 1 - i) / (samples - 1)));
            }
            pcm.CopyPcm(copy.data(), 1, copy.size());
            for (size_t i = 0; i < samples; i++)
            {
                TEST(Eq(copy[i], ((float) samples - 1 - i) / (samples - 1)));
            }
        }

        // float_2ch
        {
            size_t constexpr samples = 301;
            std::array<float, 2 * samples> data{};
            for (size_t i = 0; i < samples; i++)
            {
                data[i * 2] = ((float) i) / (samples - 1);
                data[i * 2 + 1] = 1.f - data[i * 2];
            }
            for (size_t i = 0; i < 10; i++)
            {
                pcm.AddStereo(data.data(), samples);
            }
            std::array<float, samples> copy0{};
            std::array<float, samples> copy1{};
            pcm.m_level = 1;
            pcm.CopyPcm(copy0.data(), 0, copy0.size());
            pcm.CopyPcm(copy1.data(), 1, copy1.size());
            for (size_t i = 0; i < samples; i++)
            {
                TEST(Eq(1.0, copy0[i] + copy1[i]));
            }
        }

        //        void PCM::addPCM16Data(const short* pcm_data, size_t samples)
        //        void PCM::addPCM16(const short pcmData[2][512])
        //        void PCM::addPCM8(const unsigned char pcmData[2][1024])
        //        void PCM::addPCM8_512(const unsigned char pcmData[2][512])

        return true;
    }

    auto TestFft() -> bool
    {
        Pcm pcm;

        // low frequency
        {
            size_t constexpr samples = 1024;
            std::array<float, 2 * samples> data{};
            for (size_t i = 0; i < samples; i++)
            {
                float const f = static_cast<float>(i) * 2 * 3.141592653589793f / (samples - 1);
                data[i * 2] = std::sin(f);
                data[i * 2 + 1] = std::sin(f + 1.f);// out of phase
            }
            pcm.AddStereo(data.data(), samples);
            pcm.AddStereo(data.data(), samples);
            std::array<float, fftLength> freq0{};
            std::array<float, fftLength> freq1{};
            pcm.m_level = 1.0;
            pcm.GetSpectrum(freq0.data(), CHANNEL_0, fftLength);
            pcm.GetSpectrum(freq1.data(), CHANNEL_1, fftLength);
            // freq0 and freq1 should be equal
            for (size_t i = 0; i < fftLength; i++)
            {
                TEST(Eq(freq0[i], freq1[i]));
            }
            TEST(freq0[0] > 500);
            for (size_t i = 1; i < fftLength; i++)
            {
                TEST(freq0[i] < 0.1);
            }
        }

        // high frequency
        {
            size_t constexpr samples = 2;
            std::array<float, 4> constexpr data{1.0, 0.0, 0.0, 1.0};
            for (size_t i = 0; i < 1024; i++)
            {
                pcm.AddStereo(data.data(), samples);
            }
            std::array<float, fftLength> freq0{};
            std::array<float, fftLength> freq1{};
            pcm.m_level = 1.0;
            pcm.GetSpectrum(freq0.data(), CHANNEL_0, fftLength);
            pcm.GetSpectrum(freq1.data(), CHANNEL_1, fftLength);
            // freq0 and freq1 should be equal
            for (size_t i = 0; i < fftLength; i++)
            {
                TEST(Eq(freq0[i], freq1[i]));
            }
            for (size_t i = 0; i < fftLength - 1; i++)
            {
                TEST(0 == freq0[i]);
            }
            TEST(freq0[fftLength - 1] > 100000);
        }


        return true;
    }

    auto test() -> bool override
    {
        TEST(TestAddpcm());
        TEST(TestFft());
        return true;
    }
};

auto Pcm::MakeTest() -> Test*
{
    return new PCMTest();
}

#else

auto Pcm::MakeTest() -> Test*
{
    return nullptr;
}

#endif
