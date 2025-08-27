#include "WaveformAligner.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace libprojectM {
namespace Audio {

WaveformAligner::WaveformAligner()
{
    static const uint32_t maxOctaves{10};


    static const uint32_t numOctaves{static_cast<uint32_t>(std::floor(std::log(AudioBufferSamples - WaveformSamples) / std::log(2.0f)))};
    m_octaves = numOctaves > maxOctaves ? maxOctaves : numOctaves;

    m_aligmentWeights.resize(m_octaves);
    m_firstNonzeroWeights.resize(m_octaves);
    m_lastNonzeroWeights.resize(m_octaves);
    m_octaveSamples.resize(m_octaves);
    m_octaveSampleSpacing.resize(m_octaves);
    m_oldWaveformMips.resize(m_octaves);

    m_octaveSamples[0] = AudioBufferSamples;
    m_octaveSampleSpacing[0] = AudioBufferSamples - WaveformSamples;
    for (uint32_t octave = 1; octave < m_octaves; octave++)
    {
        m_octaveSamples[octave] = m_octaveSamples[octave - 1] / 2;
        m_octaveSampleSpacing[octave] = m_octaveSampleSpacing[octave - 1] / 2;
    }
}

void WaveformAligner::ResampleOctaves(std::vector<WaveformBuffer>& dstWaveformMips, WaveformBuffer& newWaveform)
{

    std::copy(newWaveform.begin(), newWaveform.end(), dstWaveformMips[0].begin());



    for (uint32_t octave = 1; octave < m_octaves; octave++)
    {
        for (uint32_t sample = 0; sample < m_octaveSamples[octave]; sample++)
        {
            dstWaveformMips[octave][sample] = 0.5f * (dstWaveformMips[octave - 1][sample * 2] + dstWaveformMips[octave - 1][sample * 2 + 1]);
        }
    }
}

void WaveformAligner::GenerateWeights()
{

    for (uint32_t octave = 0; octave < m_octaves; octave++)
    {




        uint32_t const compareSamples = m_octaveSamples[octave] - m_octaveSampleSpacing[octave];

        for (uint32_t sample = 0; sample < compareSamples; sample++)
        {


            auto& weightRef = m_aligmentWeights[octave][sample];


            if (sample < compareSamples / 2)
            {
                weightRef = static_cast<float>(sample * 2) / static_cast<float>(compareSamples);
            }
            else
            {
                weightRef = static_cast<float>((compareSamples - 1 - sample) * 2) / static_cast<float>(compareSamples);
            }


            weightRef = (weightRef - 0.8f) * 5.0f + 0.8f;



            if (weightRef > 1.0f)
            {
                weightRef = 1.0f;
            }
            if (weightRef < 0.0f)
            {
                weightRef = 0.0f;
            }
        }

        uint32_t sample{};


        while (m_aligmentWeights[octave][sample] == 0 && sample < compareSamples)
        {
            sample++;
        }
        m_firstNonzeroWeights[octave] = sample;

        sample = compareSamples - 1;
        while (m_aligmentWeights[octave][sample] == 0 && compareSamples > 1)
        {
            sample--;
        }
        m_lastNonzeroWeights[octave] = sample;
    }
}

int WaveformAligner::CalculateOffset(std::vector<WaveformBuffer>& newWaveformMips)
{

    int alignOffset{};
    int offsetStart{};
    int offsetEnd{static_cast<int>(m_octaveSampleSpacing[m_octaves - 1])};



    for (int octave = static_cast<int>(m_octaves) - 1; octave >= 0; octave--)
    {
        int lowestErrorOffset{-1};
        float lowestErrorAmount{};


        for (int sample = offsetStart; sample < offsetEnd; sample++)
        {
            float errorSum{};



            for (uint32_t i = m_firstNonzeroWeights[octave]; i <= m_lastNonzeroWeights[octave]; i++)
            {
                errorSum += std::abs((newWaveformMips[octave][i + sample] - m_oldWaveformMips[octave][i]) * m_aligmentWeights[octave][i]);
            }

            if (lowestErrorOffset == -1 || errorSum < lowestErrorAmount)
            {
                lowestErrorOffset = static_cast<int>(sample);
                lowestErrorAmount = errorSum;
            }
        }








        if (octave > 0)
        {
            offsetStart = lowestErrorOffset * 2 - 1;
            offsetEnd = lowestErrorOffset * 2 + 2 + 1;
            if (offsetStart < 0)
            {

                offsetStart = 0;
            }
            if (offsetEnd > static_cast<int>(m_octaveSampleSpacing[octave - 1]))
            {
                offsetEnd = static_cast<int>(m_octaveSampleSpacing[octave - 1]);
            }
        }
        else
        {
            alignOffset = lowestErrorOffset;
        }
    }

    return alignOffset;
}

void WaveformAligner::Align(WaveformBuffer& newWaveform)
{
    if (m_octaves < 4)
    {


        return;
    }


    std::vector<WaveformBuffer> newWaveformMips(m_octaves, WaveformBuffer());
    ResampleOctaves(newWaveformMips, newWaveform);

    if (!m_alignWaveReady)
    {
        GenerateWeights();

        m_alignWaveReady = true;
    }

    int alignOffset = CalculateOffset(newWaveformMips);



    if (alignOffset > 0)
    {
        std::copy_n(newWaveform.begin() + alignOffset, WaveformSamples, newWaveform.begin());


        std::fill_n(newWaveform.begin() + WaveformSamples, AudioBufferSamples - WaveformSamples, 0.0f);
    }



    ResampleOctaves(m_oldWaveformMips, newWaveform);
}


}
}
