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

#include <stdlib.h>
#include <stdio.h>

#include "wipemalloc.h"

#include "Common.hpp"
#include "PCM.hpp"
#include <cmath>
#include "BeatDetect.hpp"


BeatDetect::BeatDetect(PCM *_pcm)
{
    this->pcm=_pcm;

    this->vol_instant=0;
    this->vol_history=0;
    for (unsigned y=0;y<BEAT_HISTORY_LENGTH;y++)
        this->vol_buffer[y]=0;

    this->beat_buffer_pos=0;

    this->bass_instant = 0;
    this->bass_history = 0;
    for (unsigned y=0;y<BEAT_HISTORY_LENGTH;y++)
        this->bass_buffer[y]=0;

    this->mid_instant = 0;
    this->mid_history = 0;
    for (unsigned y=0;y<BEAT_HISTORY_LENGTH;y++)
        this->mid_buffer[y]=0;

    this->treb_instant = 0;
    this->treb_history = 0;
    for (unsigned y=0;y<BEAT_HISTORY_LENGTH;y++)
        this->treb_buffer[y]=0;

    this->treb = 0;
    this->mid = 0;
    this->bass = 0;
    this->vol_old = 0;
    this->beatSensitivity = 1.00;
    this->treb_att = 0;
    this->mid_att = 0;
    this->bass_att = 0;
    this->vol_att = 0;
    this->vol = 0;
}


BeatDetect::~BeatDetect()
{

}


void BeatDetect::reset()
{
    this->treb = 0;
    this->mid = 0;
    this->bass = 0;
    this->treb_att = 0;
    this->mid_att = 0;
    this->bass_att = 0;
    this->vol_att = 0;
    this->vol_old = 0;
    this->vol_instant=0;
}


void BeatDetect::detectFromSamples()
{
    vol_old = vol;
    bass=0;
    mid=0;
    treb=0;
    vol=0;

    float vdataL[FFT_LENGTH];
    float vdataR[FFT_LENGTH];
    pcm->getSpectrum(vdataL, CHANNEL_0, FFT_LENGTH, 0.0);
    pcm->getSpectrum(vdataR, CHANNEL_1, FFT_LENGTH, 0.0);

    // OK, we're not really using this number 44.1 anywhere
    // This is more of a nod to the fact that if the actually data rate is REALLY different
    // then in theory the bass/mid/treb ranges should be adjusted.
    // In practice, I doubt it would adversely affect the actually display very much
    getBeatVals(44100.0f, FFT_LENGTH, vdataL, vdataR);
}


void BeatDetect::getBeatVals( float samplerate, unsigned fft_length, float *vdataL, float *vdataR )
{
    assert(fft_length >= 256);
    unsigned ranges[4]  = {0, 3, 23, 255};

    bass_instant=0;
    for (unsigned i=ranges[0] ; i<ranges[1] ; i++)
        bass_instant += vdataL[i] + vdataR[i];
    bass_history -= bass_buffer[beat_buffer_pos] * (1.0/BEAT_HISTORY_LENGTH);
    bass_buffer[beat_buffer_pos] = bass_instant;
    bass_history += bass_instant * (1.0/BEAT_HISTORY_LENGTH);

    mid_instant=0;
    for (unsigned i=ranges[1] ; i<ranges[2] ; i++)
        mid_instant += vdataL[i] + vdataR[i];
    mid_history -= mid_buffer[beat_buffer_pos] * (1.0/BEAT_HISTORY_LENGTH);
    mid_buffer[beat_buffer_pos] = mid_instant;
    mid_history += mid_instant * (1.0/BEAT_HISTORY_LENGTH);

    treb_instant = 0;
    for (unsigned i=ranges[2] ; i<ranges[3] ; i++)
        treb_instant += vdataL[i] + vdataR[i];
    treb_history -= treb_buffer[beat_buffer_pos] * (1.0/BEAT_HISTORY_LENGTH);
    treb_buffer[beat_buffer_pos] = treb_instant;
    treb_history += treb_instant * (1.0/BEAT_HISTORY_LENGTH);

    vol_instant  = (bass_instant + mid_instant + treb_instant) / 3.0f;
    vol_history -= (vol_buffer[beat_buffer_pos])* (1.0/BEAT_HISTORY_LENGTH);
    vol_buffer[beat_buffer_pos] = vol_instant;
    vol_history += vol_instant * (1.0/BEAT_HISTORY_LENGTH);

//    fprintf(stderr, "%6.3f %6.2f %6.3f\n", bass_history/vol_history, mid_history/vol_history, treb_history/vol_history);
    bass = bass_instant / fmax(0.0001, bass_history);
    mid  = mid_instant  / fmax(0.0001, mid_history);
    treb = treb_instant / fmax(0.0001, treb_history);
    vol  = vol_instant  / fmax(0.0001, vol_history);

    if ( projectM_isnan( treb ) ) {
        treb = 0.0;
    }
    if ( projectM_isnan( mid ) ) {
        mid = 0.0;
    }
    if ( projectM_isnan( bass ) ) {
        bass = 0.0;
    }

    treb_att = .6f * treb_att + .4f * treb;
    mid_att  = .6f * mid_att + .4f * mid;
    bass_att = .6f * bass_att + .4f * bass;
    vol_att =  .6f * vol_att + .4f * vol;

    if (bass_att>100) bass_att=100;
    if (bass >100) bass=100;
    if (mid_att>100) mid_att=100;
    if (mid >100) mid=100;
    if (treb_att>100) treb_att=100;
    if (treb >100) treb=100;
    if (vol_att>100) vol_att=100;
    if (vol>100) vol=100;

    beat_buffer_pos++;
    if (beat_buffer_pos>79)
        beat_buffer_pos=0;
}


