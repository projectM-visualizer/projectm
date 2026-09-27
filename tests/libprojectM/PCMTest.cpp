#include <gtest/gtest.h>

#include <Audio/AudioConstants.hpp>

#include <projectM-4/audio.h>

TEST(PCM, MaxSamplesIsStoredBufferSize)
{
    EXPECT_EQ(projectm_pcm_get_max_samples(), static_cast<unsigned int>(libprojectM::Audio::AudioBufferSamples));
}
