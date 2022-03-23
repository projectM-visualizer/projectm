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
auto Pcm::AutoLevel::UpdateLevel(size_t samples, double sum, double max) -> double
{

    // This is an arbitrary number that helps control
    //   a) how quickly the level can change and
    //   b) keeps code from being affected by how the caller provides data (lot of short buffers, or fewer long buffers)
    size_t constexpr autolevelSegment = 4096;

    if (sum / samples < 0.00001)
        return m_level;
    m_levelSum += sum;
    m_levelax = std::max(m_levelax, max * 1.02);
    m_levelSamples += samples;
    if (m_levelSamples >= autolevelSegment || m_l0 <= 0)
    {
        double maxRecent = std::max(std::max(m_l0, m_l1), std::max(m_l2, m_levelax));
        m_l0 = m_l1;
        m_l1 = m_l2;
        m_l2 = m_levelax;
        m_levelax *= 0.95;
        m_levelSum = m_levelSamples = 0;
        m_level = (m_l0 <= 0) ? maxRecent : m_level * 0.96 + maxRecent * 0.04;
        m_level = std::max(m_level, 0.0001);
    }
    return m_level;
}


void Pcm::AddPcmFloat(const float* pcmData, size_t samples)
{
    float a, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + m_start) % maxSamples;
        a = m_pcmL[j] = pcmData[i];
        m_pcmR[j] = pcmData[i];
        sum += std::abs(a);
        max = std::max(max, a);
    }
    m_start = (m_start + samples) % maxSamples;
    m_newSamples += samples;
    m_level = m_leveler.UpdateLevel(samples, sum, max);
}


/* NOTE: this method expects total samples, not samples per channel */
void Pcm::AddPcmFloat2Ch(const float* pcmData, size_t count)
{
    size_t samples = count / 2;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (m_start + i) % maxSamples;
        a = m_pcmL[j] = pcmData[i * 2];
        b = m_pcmR[j] = pcmData[i * 2 + 1];
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, std::abs(a)), std::abs(b));
    }
    m_start = (m_start + samples) % maxSamples;
    m_newSamples += samples;
    m_level = m_leveler.UpdateLevel(samples, sum / 2, max);
}


void Pcm::AddPcm16Data(const int16_t* pcm_data, size_t samples)
{
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; ++i)
    {
        size_t j = (i + m_start) % maxSamples;
        a = m_pcmL[j] = (pcm_data[i * 2 + 0] / 16384.0);
        b = m_pcmR[j] = (pcm_data[i * 2 + 1] / 16384.0);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    m_start = (m_start + samples) % maxSamples;
    m_newSamples += samples;
    m_level = m_leveler.UpdateLevel(samples, sum / 2, max);
}


void Pcm::AddPcm16(const int16_t pcmData[2][512])
{
    const int samples = 512;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + m_start) % maxSamples;
        a = m_pcmL[j] = (pcmData[0][i] / 16384.0);
        b = m_pcmR[j] = (pcmData[1][i] / 16384.0);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    m_start = (m_start + samples) % maxSamples;
    m_newSamples += samples;
    m_level = m_leveler.UpdateLevel(samples, sum / 2, max);
}


void Pcm::AddPcm8(const uint8_t pcmData[2][1024])
{
    const int samples = 1024;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + m_start) % maxSamples;
        a = m_pcmL[j] = (((float) pcmData[0][i] - 128.0) / 64);
        b = m_pcmR[j] = (((float) pcmData[1][i] - 128.0) / 64);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    m_start = (m_start + samples) % maxSamples;
    m_newSamples += samples;
    m_level = m_leveler.UpdateLevel(samples, sum / 2, max);
}


void Pcm::AddPcm8(const uint8_t pcmData[2][512])
{
    const size_t samples = 512;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + m_start) % maxSamples;
        a = m_pcmL[j] = (((float) pcmData[0][i] - 128.0) / 64);
        b = m_pcmR[j] = (((float) pcmData[1][i] - 128.0) / 64);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    m_start = (m_start + samples) % maxSamples;
    m_newSamples += samples;
    m_level = m_leveler.UpdateLevel(samples, sum / 2, max);
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
// smoothing does nothing
// returned values are normalized from -1 to 1

void Pcm::GetPcm(float* data, CHANNEL channel, size_t samples, float smoothing)
{
    assert(channel == 0 || channel == 1);

    CopyPcm(data, channel, samples);
    return;
}


/* NOTE: smoothing does nothing */
void Pcm::GetSpectrum(float* data, CHANNEL channel, size_t samples, float smoothing)
{
    assert(channel == 0 || channel == 1);
    UpdateFFT();

    auto const& spectrum = channel == 0 ? m_spectrumL : m_spectrumR;
    size_t count = samples <= fftLength ? samples : fftLength;
    for (size_t i = 0; i < count; i++)
        data[i] = spectrum[i];
    for (size_t i = count; i < samples; i++)
        data[0] = 0;
}

void Pcm::UpdateFFT()
{
    if (m_newSamples == 0)
    {
        return;
    }

    UpdateFFTChannel(0);
    UpdateFFTChannel(1);
    m_newSamples = 0;
}

void Pcm::UpdateFFTChannel(size_t channel)
{
    assert(channel == 0 || channel == 1);

    auto& freq = channel == 0 ? m_freqL : m_freqR;
    CopyPcm(freq.data(), channel, freq.size());
    Rdft(1, freq, m_ip, m_w);

    // compute magnitude data (m^2 actually)
    auto& spectrum = channel == 0 ? m_spectrumL : m_spectrumR;
    for (size_t i = 1; i < fftLength; i++)
    {
        double m2 = (freq[i * 2] * freq[i * 2] + freq[i * 2 + 1] * freq[i * 2 + 1]);
        spectrum[i - 1] = m2 * ((double) i) / fftLength;
    }
    spectrum[fftLength - 1] = freq[1] * freq[1];
}

// CPP17: std::clamp
inline double Clamp(double x, double lo, double hi)
{
    return x > hi ? hi : x < lo ? lo
                                : x;
}

// pull data from circular buffer
void Pcm::CopyPcm(float* to, int channel, size_t count)
{
    assert(channel == 0 || channel == 1);
    assert(count < maxSamples);
    auto const& from = channel == 0 ? m_pcmL : m_pcmR;
    const double volume = 1.0 / m_level;
    for (size_t i = 0, pos = m_start; i < count; i++)
    {
        if (pos == 0)
            pos = maxSamples;
        to[i] = from[--pos] * volume;
    }
}

void Pcm::CopyPcm(double* to, int channel, size_t count)
{
    assert(channel == 0 || channel == 1);
    auto const& from = channel == 0 ? m_pcmL : m_pcmR;
    const double volume = 1.0 / m_level;
    for (size_t i = 0, pos = m_start; i < count; i++)
    {
        if (pos == 0)
            pos = maxSamples;
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

class PCMTest : public Test {
public:
    PCMTest()
        : Test("PCMTest")
    {
    }

    bool Eq(float a, float b)
    {
        return std::abs(a - b) < (std::abs(a) + std::abs(b) + 1) / 1000.0f;
    }

    /* smoke test for each addPCM method */
    auto TestAddpcm() -> bool
    {
        Pcm pcm;

        // mono float
        {
            constexpr size_t samples = 301;
            std::array<float, samples> data;
            for (size_t i = 0; i < samples; i++)
                data[i] = ((float) i) / (samples - 1);
            for (size_t i = 0; i < 10; i++)
                pcm.AddPcmFloat(data.data(), samples);
            std::array<float, samples> copy;
            pcm.m_level = 1.0;
            pcm.CopyPcm(copy.data(), 0, copy.size());
            for (size_t i = 0; i < samples; i++)
                TEST(Eq(copy[i], ((float) samples - 1 - i) / (samples - 1)));
            pcm.CopyPcm(copy.data(), 1, copy.size());
            for (size_t i = 0; i < samples; i++)
                TEST(Eq(copy[i], ((float) samples - 1 - i) / (samples - 1)));
        }

        // float_2ch
        {
            const size_t samples = 301;
            std::array<float, 2 * samples> data;
            for (size_t i = 0; i < samples; i++)
            {
                data[i * 2] = ((float) i) / (samples - 1);
                data[i * 2 + 1] = 1.0 - data[i * 2];
            }
            for (size_t i = 0; i < 10; i++)
                pcm.AddPcmFloat2Ch(data.data(), samples * 2);
            std::array<float, samples> copy0;
            std::array<float, samples> copy1;
            pcm.m_level = 1;
            pcm.CopyPcm(copy0.data(), 0, copy0.size());
            pcm.CopyPcm(copy1.data(), 1, copy1.size());
            for (size_t i = 0; i < samples; i++)
                TEST(Eq(1.0, copy0[i] + copy1[i]));
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
            const size_t samples = 1024;
            std::array<float, 2 * samples> data;
            for (size_t i = 0; i < samples; i++)
            {
                float f = 2 * 3.141592653589793 * ((double) i) / (samples - 1);
                data[i * 2] = sin(f);
                data[i * 2 + 1] = sin(f + 1.0);// out of phase
            }
            pcm.AddPcmFloat2Ch(data.data(), samples * 2);
            pcm.AddPcmFloat2Ch(data.data(), samples * 2);
            std::array<float, fftLength> freq0;
            std::array<float, fftLength> freq1;
            pcm.m_level = 1.0;
            pcm.GetSpectrum(freq0.data(), CHANNEL_0, fftLength, 0.0);
            pcm.GetSpectrum(freq1.data(), CHANNEL_1, fftLength, 0.0);
            // freq0 and freq1 should be equal
            for (size_t i = 0; i < fftLength; i++)
                TEST(Eq(freq0[i], freq1[i]));
            TEST(freq0[0] > 500);
            for (size_t i = 1; i < fftLength; i++)
                TEST(freq0[i] < 0.1);
        }

        // high frequency
        {
            const size_t samples = 2;
            float data[4] = {1.0, 0.0, 0.0, 1.0};
            for (size_t i = 0; i < 1024; i++)
                pcm.AddPcmFloat2Ch(data, samples * 2);
            std::array<float, fftLength> freq0;
            std::array<float, fftLength> freq1;
            pcm.m_level = 1.0;
            pcm.GetSpectrum(freq0.data(), CHANNEL_0, fftLength, 0.0);
            pcm.GetSpectrum(freq1.data(), CHANNEL_1, fftLength, 0.0);
            // freq0 and freq1 should be equal
            for (size_t i = 0; i < fftLength; i++)
                TEST(Eq(freq0[i], freq1[i]));
            for (size_t i = 0; i < fftLength - 1; i++)
                TEST(0 == freq0[i]);
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

Test* PCM::test()
{
    return nullptr;
}

#endif
