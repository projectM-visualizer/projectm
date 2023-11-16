#include "WaveformAligner.hpp"

#include <cmath>
#include <iterator>

namespace libprojectM {
namespace Audio {

WaveformAligner::WaveformAligner()
{
    static const size_t maxOctaves{10};
    static const size_t numOctaves{static_cast<size_t>(std::floor(std::log(AudioBufferSamples - WaveformSamples) / std::log(2.0f)))};
    m_octaves = numOctaves > maxOctaves ? maxOctaves : numOctaves;

    m_aligmentWeights.resize(m_octaves);
    m_firstNonzeroWeights.resize(m_octaves);
    m_lastNonzeroWeights.resize(m_octaves);
    m_octaveSamples.resize(m_octaves);
    m_octaveSampleSpacing.resize(m_octaves);
    m_oldWaveformMips.resize(m_octaves);

    m_octaveSamples[0] = AudioBufferSamples;
    m_octaveSampleSpacing[0] = AudioBufferSamples - WaveformSamples;
    for (size_t octave = 1; octave < m_octaves; octave++)
    {
        m_octaveSamples[octave] = m_octaveSamples[octave - 1] / 2;
        m_octaveSampleSpacing[octave] = m_octaveSampleSpacing[octave - 1] / 2;
    }
}

void WaveformAligner::Align(WaveformBuffer& newWaveform)
{
    if (m_octaves < 4)
    {
        return;
    }

    int alignOffset{};

    std::vector<WaveformBuffer> newWaveformMips(m_octaves, WaveformBuffer());
    std::copy(newWaveform.begin(), newWaveform.end(), newWaveformMips[0].begin());

    // Calculate mip levels
    for (size_t octave = 1; octave < m_octaves; octave++)
    {
        for (size_t sample = 0; sample < m_octaveSamples[octave]; sample++)
        {
            newWaveformMips[octave][sample] = 0.5f * (newWaveformMips[octave - 1][sample * 2] + newWaveformMips[octave - 1][sample * 2 + 1]);
        }
    }

    if (!m_alignWaveReady)
    {
        m_alignWaveReady = true;
        for (size_t octave = 0; octave < m_octaves; octave++)
        {
            // For example:
            //  m_octaveSampleSpacing[octave] == 4
            //  m_octaveSamples[octave] == 36
            //  (so we test 32 samples, w/4 offsets)
            size_t const compareSamples = m_octaveSamples[octave] - m_octaveSampleSpacing[octave];

            for (size_t sample = 0; sample < compareSamples; sample++)
            {
                auto& tempVal = m_aligmentWeights[octave][sample];

                // Start with pyramid-shaped PDF, from 0..1..0
                if (sample < compareSamples / 2)
                {
                    tempVal = static_cast<float>(sample * 2) / static_cast<float>(compareSamples);
                }
                else
                {
                    tempVal = static_cast<float>((compareSamples - 1 - sample) * 2) / static_cast<float>(compareSamples);
                }

                // TWEAK how much the center matters, vs. the edges:
                tempVal = (tempVal - 0.8f) * 5.0f + 0.8f;

                // Clip
                if (tempVal > 1.0f)
                {
                    tempVal = 1.0f;
                }
                if (tempVal < 0.0f)
                {
                    tempVal = 0.0f;
                }
            }

            size_t sample{};
            while (m_aligmentWeights[octave][sample] == 0 && sample < compareSamples)
            {
                sample++;
            }
            m_firstNonzeroWeights[octave] = sample;

            sample = compareSamples - 1;
            while (m_aligmentWeights[octave][sample] == 0 && sample >= 0)
            {
                sample--;
            }
            m_lastNonzeroWeights[octave] = sample;
        }
    }

    int sample1{};
    int sample2{static_cast<int>(m_octaveSampleSpacing[m_octaves - 1])};

    // Find best match for alignment
    for (int octave = static_cast<int>(m_octaves) - 1; octave >= 0; octave--)
    {
        int lowestErrorOffset{-1};
        float lowestErrorAmount{};

        for (int sample = sample1; sample < sample2; sample++)
        {
            float errorSum{};

            for (size_t i = m_firstNonzeroWeights[octave]; i <= m_lastNonzeroWeights[octave]; i++)
            {
                errorSum += std::abs((newWaveformMips[octave][i + sample] - m_oldWaveformMips[octave][i + sample]) * m_aligmentWeights[octave][i]);
            }

            if (lowestErrorOffset == -1 || errorSum < lowestErrorAmount)
            {
                lowestErrorOffset = static_cast<int>(sample);
                lowestErrorAmount = errorSum;
            }
        }

        // Now use 'lowestErrorOffset' to guide bounds of search in next octave:
        //  m_octaveSampleSpacing[octave] == 8
        //  m_octaveSamples[octave] == 72
        //     -say 'lowestErrorOffset' was 2
        //     -that corresponds to samples 4 & 5 of the next octave
        //     -also, expand about this by 2 samples?  YES.
        //  (so we'd test 64 samples, w/8->4 offsets)
        if (octave > 0)
        {
            sample1 = lowestErrorOffset * 2 - 1;
            sample2 = lowestErrorOffset * 2 + 2 + 1;
            if (sample1 < 0)
            {
                sample1 = 0;
            }
            if (sample2 > static_cast<int>(m_octaveSampleSpacing[octave - 1]))
            {
                sample2 = static_cast<int>(m_octaveSampleSpacing[octave - 1]);
            }
        }
        else
        {
            alignOffset = lowestErrorOffset;
        }
    }

    // Store mip levels for the next frame.
    m_oldWaveformMips.clear();
    std::copy(newWaveformMips.begin(), newWaveformMips.end(), std::back_inserter(m_oldWaveformMips));

    // Finally, apply the results by scooting the aligned samples so that they start at index 0.
    if (alignOffset > 0)
    {
        for (size_t sample = 0; sample < WaveformSamples; sample++)
        {
            newWaveform[sample] = newWaveform[sample + alignOffset];
        }

        // Set remaining samples to zero.
        std::fill_n(newWaveform.begin() + WaveformSamples, AudioBufferSamples - WaveformSamples, 0.0f);
    }
}


} // namespace Audio
} // namespace libprojectM
