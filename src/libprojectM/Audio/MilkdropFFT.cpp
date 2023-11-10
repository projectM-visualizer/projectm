/*
  LICENSE
  -------
Copyright 2005-2013 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution. 

  * Neither the name of Nullsoft nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission. 
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MilkdropFFT.hpp"

namespace libprojectM {
namespace Audio {

constexpr auto PI = 3.141592653589793238462643383279502884197169399f;

MilkdropFFT::MilkdropFFT(size_t samplesIn, size_t samplesOut, bool equalize, float envelopePower)
    : m_samplesIn(samplesIn)
    , m_numFrequencies(samplesOut * 2)
{
    InitBitRevTable();
    InitCosSinTable();
    InitEnvelopeTable(envelopePower);
    InitEqualizeTable(equalize);
}

void MilkdropFFT::InitEnvelopeTable(float power)
{
    if (power < 0.0f)
    {
        // Keep all values as-is.
        m_envelope = std::vector<float>(m_samplesIn, 1.0f);
        return;
    }

    float const multiplier = 1.0f / static_cast<float>(m_samplesIn) * 2.0f * PI;

    m_envelope.resize(m_samplesIn);

    if (power == 1.0f)
    {
        for (size_t i = 0; i < m_samplesIn; i++)
        {
            m_envelope[i] = 0.5f + 0.5f * std::sin(static_cast<float>(i) * multiplier - PI * 0.5f);
        }
    }
    else
    {
        for (size_t i = 0; i < m_samplesIn; i++)
        {
            m_envelope[i] = std::pow(0.5f + 0.5f * std::sin(static_cast<float>(i) * multiplier - PI * 0.5f), power);
        }
    }
}

void MilkdropFFT::InitEqualizeTable(bool equalize)
{
    if (!equalize)
    {
        m_equalize = std::vector<float>(m_numFrequencies / 2, 1.0f);
        return;
    }

    float const scaling = -0.02f;
    float const inverseHalfNumFrequencies = 1.0f / static_cast<float>(m_numFrequencies / 2);

    m_equalize.resize(m_numFrequencies / 2);

    for (size_t i = 0; i < m_numFrequencies / 2; i++)
    {
        m_equalize[i] = scaling * std::log(static_cast<float>(m_numFrequencies / 2 - i) * inverseHalfNumFrequencies);
    }
}

void MilkdropFFT::InitBitRevTable()
{
    m_bitRevTable.resize(m_numFrequencies);

    for (size_t i = 0; i < m_numFrequencies; i++)
    {
        m_bitRevTable[i] = i;
    }

    size_t j{};
    for (size_t i = 0; i < m_numFrequencies; i++)
    {
        if (j > i)
        {
            size_t const temp{m_bitRevTable[i]};
            m_bitRevTable[i] = m_bitRevTable[j];
            m_bitRevTable[j] = temp;
        }

        size_t m = m_numFrequencies >> 1;

        while (m >= 1 && j >= m)
        {
            j -= m;
            m >>= 1;
        }

        j += m;
    }
}

void MilkdropFFT::InitCosSinTable()
{
    size_t tabsize{};
    size_t dftsize{2};

    while (dftsize <= m_numFrequencies)
    {
        tabsize++;
        dftsize <<= 1;
    }

    m_cosSinTable.resize(tabsize);

    dftsize = 2;
    size_t index{0};
    while (dftsize <= m_numFrequencies)
    {
        auto const theta{-2.0f * PI / static_cast<float>(dftsize)};
        m_cosSinTable[index] = std::polar(1.0f, theta); // Radius 1 is the unity circle.
        index++;
        dftsize <<= 1;
    }
}

void MilkdropFFT::TimeToFrequencyDomain(const std::vector<float>& waveformData, std::vector<float>& spectralData)
{
    if (m_bitRevTable.empty() || m_cosSinTable.empty() || waveformData.size() < m_samplesIn)
    {
        spectralData.clear();
        return;
    }

    // 1. Set up input to the FFT
    std::vector<std::complex<float>> spectrumData(m_numFrequencies, std::complex<float>());
    for (size_t i = 0; i < m_numFrequencies; i++)
    {
        size_t const idx{m_bitRevTable[i]};
        if (idx < m_samplesIn)
        {
            spectrumData[i].real(waveformData[idx] * m_envelope[idx]);
        }
    }

    // 2. Perform FFT
    size_t dftSize{2};
    size_t octave{0};

    while (dftSize <= m_numFrequencies)
    {
        std::complex<float> w{1.0f, 0.0f};
        std::complex<float> const wp{m_cosSinTable[octave]};

        size_t const hdftsize{dftSize >> 1};

        for (size_t m = 0; m < hdftsize; m += 1)
        {
            for (size_t i = m; i < m_numFrequencies; i += dftSize)
            {
                size_t const j{i + hdftsize};
                std::complex<float> const tempNum{spectrumData[j] * w};
                spectrumData[j] = spectrumData[i] - tempNum;
                spectrumData[i] = spectrumData[i] + tempNum;
            }

            w *= wp;
        }

        dftSize <<= 1;
        octave++;
    }

    // 3. Take the magnitude & eventually equalize it (on a log10 scale) for output
    spectralData.resize(m_numFrequencies / 2);
    for (size_t i = 0; i < m_numFrequencies / 2; i++)
    {
        spectralData[i] = m_equalize[i] * std::abs(spectrumData[i]);
    }
}

} // namespace Audio
} // namespace libprojectM
