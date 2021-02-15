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

#include <stdlib.h>
#include "dlldefs.h"


// This is number of magnitude values, float array is 2x for R and I values
#define FFT_LENGTH 512
class Test;
class AutoLevel;

class 
#ifdef WIN32 
DLLEXPORT 
#endif 
PCM
{
public:
    static const size_t maxsamples=2048;

    PCM();
    ~PCM();
    void addPCMfloat( const float *PCMdata, size_t samples );
    void addPCMfloat_2ch( const float *PCMdata, size_t samples );
    void addPCM16( const short [2][512] );
    void addPCM16Data( const short* pcm_data, size_t samples );
    void addPCM8( const unsigned char [2][1024] );
    void addPCM8_512( const unsigned char [2][512] );

    /** PCM data */
    void getPCM(float *data, int channel, size_t samples, float smoothing);
    void getSpectrum(float *data, int channel, size_t samples);

  	static Test* test();

private:
    // pcmd 2x2048*4b    = 16K
    // vdata 2x512x2*8b  = 16K
    // spectrum 2x512*4b = 4k
    // w = 512*8b        = 4k

    // circular PCM buffer
    // adjust "volume" of PCM data as we go, this simplifies everything downstream...
    // normalize to range [-1.0,1.0]
    float pcmL[maxsamples];
    float pcmR[maxsamples];
    int start;
    size_t newsamples;

    // raw FFT data
    double freqL[FFT_LENGTH*2];
    double freqR[FFT_LENGTH*2];
    // magnitude data
    float spectrumL[FFT_LENGTH];
    float spectrumR[FFT_LENGTH];

    // for FFT library
    int *ip;
    double *w;

    void freePCM();

    // get data out of circular PCM buffer
    void _copyPCM(float *PCMdata, int channel, size_t count);
    void _copyPCM(double *PCMdata, int channel, size_t count);
    // compute FFT
    void _updateFFT();
    void _updateFFT(size_t channel);

    friend class PCMTest;

    // state for tracking audio level
    double level;
    class AutoLevel *leveler;
};

#endif /** !_PCM_H */
