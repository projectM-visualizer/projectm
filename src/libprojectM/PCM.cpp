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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
double PCM::AutoLevel::updateLevel(size_t samples, double sum, double max)
{

    // This is an arbitrary number that helps control
    //   a) how quickly the level can change and
    //   b) keeps code from being affected by how the caller provides data (lot of short buffers, or fewer long buffers)
    size_t constexpr AUTOLEVEL_SEGMENT = 4096;

    if (sum / samples < 0.00001)
        return level;
    level_sum += sum;
    level_max = std::max(level_max, max * 1.02);
    level_samples += samples;
    if (level_samples >= AUTOLEVEL_SEGMENT || l0 <= 0)
    {
        double max_recent = std::max(std::max(l0, l1), std::max(l2, level_max));
        l0 = l1;
        l1 = l2;
        l2 = level_max;
        level_max *= 0.95;
        level_sum = level_samples = 0;
        level = (l0 <= 0) ? max_recent : level * 0.96 + max_recent * 0.04;
        level = std::max(level, 0.0001);
    }
    return level;
}


void PCM::addPCMfloat(const float* PCMdata, size_t samples)
{
    float a, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + start) % maxSamples;
        a = pcmL[j] = PCMdata[i];
        pcmR[j] = PCMdata[i];
        sum += std::abs(a);
        max = std::max(max, a);
    }
    start = (start + samples) % maxSamples;
    newSamples += samples;
    level = leveler.updateLevel(samples, sum, max);
}


/* NOTE: this method expects total samples, not samples per channel */
void PCM::addPCMfloat_2ch(const float* PCMdata, size_t count)
{
    size_t samples = count / 2;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (start + i) % maxSamples;
        a = pcmL[j] = PCMdata[i * 2];
        b = pcmR[j] = PCMdata[i * 2 + 1];
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, std::abs(a)), std::abs(b));
    }
    start = (start + samples) % maxSamples;
    newSamples += samples;
    level = leveler.updateLevel(samples, sum / 2, max);
}


void PCM::addPCM16Data(const short* pcm_data, size_t samples)
{
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; ++i)
    {
        size_t j = (i + start) % maxSamples;
        a = pcmL[j] = (pcm_data[i * 2 + 0] / 16384.0);
        b = pcmR[j] = (pcm_data[i * 2 + 1] / 16384.0);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    start = (start + samples) % maxSamples;
    newSamples += samples;
    level = leveler.updateLevel(samples, sum / 2, max);
}


void PCM::addPCM16(const short PCMdata[2][512])
{
    const int samples = 512;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + start) % maxSamples;
        a = pcmL[j] = (PCMdata[0][i] / 16384.0);
        b = pcmR[j] = (PCMdata[1][i] / 16384.0);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    start = (start + samples) % maxSamples;
    newSamples += samples;
    level = leveler.updateLevel(samples, sum / 2, max);
}


void PCM::addPCM8(const unsigned char PCMdata[2][1024])
{
    const int samples = 1024;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + start) % maxSamples;
        a = pcmL[j] = (((float) PCMdata[0][i] - 128.0) / 64);
        b = pcmR[j] = (((float) PCMdata[1][i] - 128.0) / 64);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    start = (start + samples) % maxSamples;
    newSamples += samples;
    level = leveler.updateLevel(samples, sum / 2, max);
}


void PCM::addPCM8_512(const unsigned char PCMdata[2][512])
{
    const size_t samples = 512;
    float a, b, sum = 0, max = 0;
    for (size_t i = 0; i < samples; i++)
    {
        size_t j = (i + start) % maxSamples;
        a = pcmL[j] = (((float) PCMdata[0][i] - 128.0) / 64);
        b = pcmR[j] = (((float) PCMdata[1][i] - 128.0) / 64);
        sum += std::abs(a) + std::abs(b);
        max = std::max(std::max(max, a), b);
    }
    start = (start + samples) % maxSamples;
    newSamples += samples;
    level = leveler.updateLevel(samples, sum / 2, max);
}


template<size_t aSize, size_t ipSize, size_t wSize>
void rdft(
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
// smoothing is the smoothing coefficient
// returned values are normalized from -1 to 1

void PCM::getPCM(float* data, CHANNEL channel, size_t samples, float smoothing)
{
    assert(channel == 0 || channel == 1);

    if (0 == smoothing)
    {
        _copyPCM(data, channel, samples);
        return;
    }

    // since we've already got the freq data laying around, let's use that for smoothing
    _updateFFT();

    // copy
    std::array<double, 2 * FFT_LENGTH> freq;
    auto const& from = channel == 0 ? freqL : freqR;
    for (size_t i = 0; i < FFT_LENGTH * 2; i++)
        freq[i] = from[i];

    // The visible effects ramp up as you smoothing value gets close to 1.0 (consistent with milkdrop2)
    if (1 == 0)// gaussian
    {
        // precompute constant:
        double k = -1.0 / ((1 - smoothing) * (1 - smoothing) * FFT_LENGTH * FFT_LENGTH);
        for (size_t i = 1; i < FFT_LENGTH; i++)
        {
            float g = pow(2.718281828459045, i * i * k);
            freq[i * 2] *= g;
            freq[i * 2 + 1] *= g;
        }
        freq[1] *= pow(2.718281828459045, FFT_LENGTH * FFT_LENGTH * k);
    }
    else
    {
        // butterworth
        // this might be slightly faster to compute. pow() is expensive
        double k = 1.0 / ((1 - smoothing) * (1 - smoothing) * FFT_LENGTH * FFT_LENGTH);
        for (size_t i = 1; i < FFT_LENGTH; i++)
        {
            float b = 1.0 / (1.0 + (i * i * k));
            freq[i * 2] *= b;
            freq[i * 2 + 1] *= b;
        }
        freq[1] *= 1.0 / (1.0 + (FFT_LENGTH * FFT_LENGTH * k));
    }

    // inverse fft
    rdft(-1, freq, ip, w);
    for (size_t j = 0; j < FFT_LENGTH * 2; j++)
        freq[j] *= 1.0 / FFT_LENGTH;

    // copy out with zero-padding if necessary
    size_t count = samples < FFT_LENGTH ? samples : FFT_LENGTH;
    for (size_t i = 0; i < count; i++)
        data[i] = freq[i % (FFT_LENGTH * 2)];
    for (size_t i = count; i < samples; i++)
        data[i] = 0;
}


/* NOTE: Still don't have real support for smoothing parameter, but this gets close to the milkdrop2 default look */
void PCM::getSpectrum(float* data, CHANNEL channel, size_t samples, float smoothing)
{
    assert(channel == 0 || channel == 1);
    _updateFFT();

    auto const& spectrum = channel == 0 ? spectrumL : spectrumR;
    if (smoothing == 0)
    {
        size_t count = samples <= FFT_LENGTH ? samples : FFT_LENGTH;
        for (size_t i = 0; i < count; i++)
            data[i] = spectrum[i];
        for (size_t i = count; i < samples; i++)
            data[0] = 0;
    }
    else
    {
        float l2 = 0, l1 = 0, c = 0, r1, r2;
        r1 = spectrum[0];
        r2 = spectrum[0 + 1];
        for (size_t i = 0; i < samples; i++)
        {
            l2 = l1;
            l1 = c;
            c = r1;
            r1 = r2;
            r2 = (i + 2) >= samples ? 0 : spectrum[i + 2];
            data[i] = (l2 + 4 * l1 + 6 * c + 4 * r1 + r2) / 16.0;
        }
    }
}

void PCM::_updateFFT()
{
    if (newSamples == 0)
    {
        return;
    }

    _updateFFTChannel(0);
    _updateFFTChannel(1);
    newSamples = 0;
}

void PCM::_updateFFTChannel(size_t channel)
{
    assert(channel == 0 || channel == 1);

    auto& freq = channel == 0 ? freqL : freqR;
    _copyPCM(freq.data(), channel, FFT_LENGTH * 2);
    rdft(FFT_LENGTH * 2, 1, freq.data(), ip.data(), w.data());

    // compute magnitude data (m^2 actually)
    auto& spectrum = channel == 0 ? spectrumL : spectrumR;
    for (size_t i = 1; i < FFT_LENGTH; i++)
    {
        double m2 = (freq[i * 2] * freq[i * 2] + freq[i * 2 + 1] * freq[i * 2 + 1]);
        spectrum[i - 1] = m2 * ((double) i) / FFT_LENGTH;
    }
    spectrum[FFT_LENGTH - 1] = freq[1] * freq[1];
}

// CPP17: std::clamp
inline double clamp(double x, double lo, double hi)
{
    return x > hi ? hi : x < lo ? lo
                                : x;
}

// pull data from circular buffer
void PCM::_copyPCM(float* to, int channel, size_t count)
{
    assert(channel == 0 || channel == 1);
    assert(count < maxSamples);
    auto const& from = channel == 0 ? pcmL : pcmR;
    const double volume = 1.0 / level;
    for (size_t i = 0, pos = start; i < count; i++)
    {
        if (pos == 0)
            pos = maxSamples;
        to[i] = from[--pos] * volume;
    }
}

void PCM::_copyPCM(double* to, int channel, size_t count)
{
    assert(channel == 0 || channel == 1);
    auto const& from = channel == 0 ? pcmL : pcmR;
    const double volume = 1.0 / level;
    for (size_t i = 0, pos = start; i < count; i++)
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

struct PCMTest : public Test {
    PCMTest()
        : Test("PCMTest")
    {
    }

    bool eq(float a, float b)
    {
        return std::abs(a - b) < (std::abs(a) + std::abs(b) + 1) / 1000.0f;
    }

public:
    /* smoke test for each addPCM method */
    bool test_addpcm()
    {
        PCM pcm;

        // mono float
        {
            constexpr size_t samples = 301;
            std::array<float, samples> data;
            for (size_t i = 0; i < samples; i++)
                data[i] = ((float) i) / (samples - 1);
            for (size_t i = 0; i < 10; i++)
                pcm.addPCMfloat(data.data(), samples);
            std::array<float, samples> copy;
            pcm.level = 1.0;
            pcm._copyPCM(copy.data(), 0, samples);
            for (size_t i = 0; i < samples; i++)
                TEST(eq(copy[i], ((float) samples - 1 - i) / (samples - 1)));
            pcm._copyPCM(copy.data(), 1, samples);
            for (size_t i = 0; i < samples; i++)
                TEST(eq(copy[i], ((float) samples - 1 - i) / (samples - 1)));
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
                pcm.addPCMfloat_2ch(data.data(), samples * 2);
            std::array<float, samples> copy0;
            std::array<float, samples> copy1;
            pcm.level = 1;
            pcm._copyPCM(copy0.data(), 0, samples);
            pcm._copyPCM(copy1.data(), 1, samples);
            for (size_t i = 0; i < samples; i++)
                TEST(eq(1.0, copy0[i] + copy1[i]));
        }

        //        void PCM::addPCM16Data(const short* pcm_data, size_t samples)
        //        void PCM::addPCM16(const short PCMdata[2][512])
        //        void PCM::addPCM8(const unsigned char PCMdata[2][1024])
        //        void PCM::addPCM8_512(const unsigned char PCMdata[2][512])

        return true;
    }

    bool test_fft()
    {
        PCM pcm;

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
            pcm.addPCMfloat_2ch(data.data(), samples * 2);
            pcm.addPCMfloat_2ch(data.data(), samples * 2);
            std::array<float, FFT_LENGTH> freq0;
            std::array<float, FFT_LENGTH> freq1;
            pcm.level = 1.0;
            pcm.getSpectrum(freq0.data(), CHANNEL_0, FFT_LENGTH, 0.0);
            pcm.getSpectrum(freq1.data(), CHANNEL_1, FFT_LENGTH, 0.0);
            // freq0 and freq1 should be equal
            for (size_t i = 0; i < FFT_LENGTH; i++)
                TEST(eq(freq0[i], freq1[i]));
            TEST(freq0[0] > 500);
            for (size_t i = 1; i < FFT_LENGTH; i++)
                TEST(freq0[i] < 0.1);
        }

        // high frequency
        {
            const size_t samples = 2;
            float data[4] = {1.0, 0.0, 0.0, 1.0};
            for (size_t i = 0; i < 1024; i++)
                pcm.addPCMfloat_2ch(data, samples * 2);
            std::array<float, FFT_LENGTH> freq0;
            std::array<float, FFT_LENGTH> freq1;
            pcm.level = 1.0;
            pcm.getSpectrum(freq0.data(), CHANNEL_0, FFT_LENGTH, 0.0);
            pcm.getSpectrum(freq1.data(), CHANNEL_1, FFT_LENGTH, 0.0);
            // freq0 and freq1 should be equal
            for (size_t i = 0; i < FFT_LENGTH; i++)
                TEST(eq(freq0[i], freq1[i]));
            for (size_t i = 0; i < FFT_LENGTH - 1; i++)
                TEST(0 == freq0[i]);
            TEST(freq0[FFT_LENGTH - 1] > 100000);
        }


        return true;
    }

    bool test() override
    {
        TEST(test_addpcm());
        TEST(test_fft());
        return true;
    }
};

Test* PCM::test()
{
    return new PCMTest();
}

#else

Test* PCM::test()
{
    return nullptr;
}

#endif
