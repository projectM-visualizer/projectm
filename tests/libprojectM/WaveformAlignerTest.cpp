#include "Audio/WaveformAligner.hpp"

#include <gtest/gtest.h>

using namespace libprojectM::Audio;

/**
 * Class to make protected function accessible to tests.
 */
class WaveformAlignerMock : public WaveformAligner
{
private:
    /*
     * gtest docs discourage access to private members using this, but
     * we're testing the modern port of legacy code here so we want to
     * stick to the original structure as much as possible.
     */
    FRIEND_TEST(projectMWaveformAligner, AlignDelta);
};

TEST(projectMWaveformAligner, AlignDelta)
{
    auto aligner = WaveformAlignerMock();
    ASSERT_EQ(aligner.m_octaves, 6);

    std::array<float, AudioBufferSamples> wf;
    std::fill(wf.begin(), wf.end(), 0.0f);

    // Create a delta waveform by setting a single value non-zero.
    wf[AudioBufferSamples/2] = 1.0f;

    aligner.Align(wf);
    // Ensure that the waveform has not shifted when first sampled.
    EXPECT_FLOAT_EQ(wf[AudioBufferSamples/2], 1.0f);

    // Verify weights
    for (uint32_t octave=0; octave < aligner.m_octaves; octave++)
    {
        size_t const compareSamples = aligner.m_octaveSamples[octave] - aligner.m_octaveSampleSpacing[octave];
        // Non-zero range should be (0.32, 0.68)*compareSamples based on
        // the "TWEAK" calculation. If the weight calculation changes this
        // range will need to change.
        // Note that the test below is not a requirement for the algorithm. Instead
        // of testing the correctness of the algorithm, this is testing that we
        // understand the implications of the legacy code.
        EXPECT_NEAR(aligner.m_firstNonzeroWeights[octave], 0.32*compareSamples, 2.0);
        EXPECT_NEAR(aligner.m_lastNonzeroWeights[octave], 0.68*compareSamples, 2.0);
    }

    // Reset test waveform to all zeroes
    wf[AudioBufferSamples/2] = 0.0f;

    for (int i=-8; i<98; i++)
    {
        wf[AudioBufferSamples/2 + i] = 1.0f;

        std::vector<WaveformBuffer> newWaveformMips(aligner.m_octaves, WaveformBuffer());
        aligner.ResampleOctaves(newWaveformMips, wf);
        int alignOffset = aligner.CalculateOffset(newWaveformMips);
        if (i < 0 || i >= static_cast<int>(aligner.m_octaveSampleSpacing[0]))
        {
            // Only offsets between 0 and (AudioBufferSamples - WaveformSamples) are recognized
            EXPECT_EQ(alignOffset, 0);
        } else {
            // Verify that the actual waveform offset matches the reported value
            EXPECT_EQ(alignOffset, i);
        }

        wf[AudioBufferSamples/2 + i] = 0.0f;
    }

    for (int i=0; i<96; i++)
    {
        wf[AudioBufferSamples/2 + i] = 1.0f;
        aligner.Align(wf);
        // Verify that the new waveform has been shifted to match the first
        EXPECT_EQ(wf[AudioBufferSamples/2], 1.0f);
        if (i != 0)
        {
            EXPECT_EQ(wf[AudioBufferSamples/2 + i], 0.0f);
        }
        // Reset waveform
        wf[AudioBufferSamples/2] = 0.0f;
    }
}
