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
 * Encapsulation of raw sound buffer. Used in beat detection
 *
 * $Log$
 */

#ifndef _PCM_H
#define _PCM_H

#include <array>
#include <stdlib.h>


// FFT_LENGTH is number of magnitude values available from getSpectrum().
// Internally this is generated using 2xFFT_LENGTH samples per channel.
size_t constexpr FFT_LENGTH = 512;
class Test;
class AutoLevel;

enum CHANNEL
{
    CHANNEL_L = 0,
    CHANNEL_0 = 0,
    CHANNEL_R = 1,
    CHANNEL_1 = 1
};

class PCM
{
public:
    /* maximum number of sound samples that are actually stored. */
    static constexpr size_t maxsamples=2048;

    PCM();
    ~PCM();

    void addPCMfloat( const float *PCMdata, size_t samples );
    void addPCMfloat_2ch( const float *PCMdata, size_t count );
    void addPCM16( const short [2][512] );
    void addPCM16Data( const short* pcm_data, size_t samples );
    void addPCM8( const unsigned char [2][1024] );
    void addPCM8_512( const unsigned char [2][512] );

    /**
     * PCM data
     * When smoothing=0 is copied directly from PCM buffers. smoothing=1.0 is almost a straight line.
     * The returned data will 'wrap' if more than maxsamples are requested.
     */
    void getPCM(float *data, CHANNEL channel, size_t samples, float smoothing);

    /** Spectrum data
     * Smoothing is not fully implemented, only none (smoothing==0) or a little (smoothing!=0).
     * The returned data will be zero padded if more than FFT_LENGTH values are requested
     */
    void getSpectrum(float *data, CHANNEL channel, size_t samples, float smoothing);

  	static Test* test();

private:
    // mem-usage:
    // pcmd 2x2048*4b    = 16K
    // vdata 2x512x2*8b  = 16K
    // spectrum 2x512*4b = 4k
    // w = 512*8b        = 4k

    // circular PCM buffer
    // adjust "volume" of PCM data as we go, this simplifies everything downstream...
    // normalize to range [-1.0,1.0]
    std::array<float, maxsamples> pcmL{0.f};
    std::array<float, maxsamples> pcmR{0.f};
    int start{0};
    size_t newsamples{0};

    // raw FFT data
    std::array<double, 2*FFT_LENGTH> freqL{0.0};
    std::array<double, 2*FFT_LENGTH> freqR{0.0};
    // magnitude data
    std::array<float, FFT_LENGTH> spectrumL{0.f};
    std::array<float, FFT_LENGTH> spectrumR{0.f};

    std::array<double, FFT_LENGTH> w{0.0};
    std::array<int, 34> ip{0};

    // copy data out of the circular PCM buffer
    void _copyPCM(float *PCMdata, int channel, size_t count);
    void _copyPCM(double *PCMdata, int channel, size_t count);

    // update FFT data if new samples are available.
    void _updateFFT();
    void _updateFFT(size_t channel);

    friend class PCMTest;

    // state for tracking audio level
    double level;
    class AutoLevel *leveler;
};

#endif /** !_PCM_H */
