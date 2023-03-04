#include <gtest/gtest.h>

#include <Audio/PCM.hpp>

#include <cmath>

using namespace libprojectM::Audio;

class PcmMock : public PCM
{
public:
    void CopyPcm(float* to, size_t channel, size_t count) const
    {
        PCM::CopyPcm(to, channel, count);
    }

    void CopyPcm(double* to, size_t channel, size_t count) const
    {
        PCM::CopyPcm(to, channel, count);
    }
};

TEST(PCM, AddDataMonoFloat)
{
    PcmMock pcm;

    size_t constexpr samples = 301;
    std::array<float, samples> data{};
    for (size_t i = 0; i < samples; i++)
    {
        data[i] = ((float) i) / (samples - 1);
    }
    for (size_t i = 0; i < 10; i++)
    {
        pcm.AddMono(data.data(), samples);
    }

    std::array<float, samples> copy{};
    pcm.ResetAutoLevel();
    pcm.CopyPcm(copy.data(), 0, copy.size());
    for (size_t i = 0; i < samples; i++)
    {
        EXPECT_FLOAT_EQ(copy[i], ((float) samples - 1 - i) / (samples - 1));
    }
    pcm.CopyPcm(copy.data(), 1, copy.size());
    for (size_t i = 0; i < samples; i++)
    {
        EXPECT_FLOAT_EQ(copy[i], ((float) samples - 1 - i) / (samples - 1));
    }
}

TEST(PCM, AddDataStereoFloat)
{
    PcmMock pcm;

    size_t constexpr samples = 301;
    std::array<float, 2 * samples> data{};
    for (size_t i = 0; i < samples; i++)
    {
        data[i * 2] = ((float) i) / (samples - 1);
        data[i * 2 + 1] = 1.f - data[i * 2];
    }
    for (size_t i = 0; i < 10; i++)
    {
        pcm.AddStereo(data.data(), samples);
    }

    std::array<float, samples> copy0{};
    std::array<float, samples> copy1{};
    pcm.ResetAutoLevel();
    pcm.CopyPcm(copy0.data(), 0, copy0.size());
    pcm.CopyPcm(copy1.data(), 1, copy1.size());
    for (size_t i = 0; i < samples; i++)
    {
        EXPECT_FLOAT_EQ(copy0[i] + copy1[i], 1.0);
    }
}

TEST(PCM, DISABLED_FastFourierTransformLowFrequency)
{
    // Test currently fails for unknown reasons. Will fix later.
    PcmMock pcm;

    size_t constexpr samples = 1024;
    std::array<float, 2 * samples> data{};
    for (size_t i = 0; i < samples; i++)
    {
        float const f = static_cast<float>(i) * 2 * 3.141592653589793f / (samples - 1);
        data[i * 2] = std::sin(f);
        data[i * 2 + 1] = std::sin(f + 1.f);// out of phase
    }
    pcm.AddStereo(data.data(), samples);
    pcm.AddStereo(data.data(), samples);

    std::array<float, fftLength> freq0{};
    std::array<float, fftLength> freq1{};
    pcm.ResetAutoLevel();
    pcm.GetSpectrum(freq0.data(), CHANNEL_0, fftLength);
    pcm.GetSpectrum(freq1.data(), CHANNEL_1, fftLength);

    // freq0 and freq1 should be equal
    for (size_t i = 0; i < fftLength; i++)
    {
        EXPECT_FLOAT_EQ(freq0[i], freq1[i]);
    }
    EXPECT_GT(freq0[0], 500);
    for (size_t i = 1; i < fftLength; i++)
    {
        EXPECT_LT(freq0[i], 0.1);
    }
}

TEST(PCM, FastFourierTransformHighFrequency)
{
    PcmMock pcm;

    size_t constexpr samples = 2;
    std::array<float, 4> constexpr data{1.0, 0.0, 0.0, 1.0};
    for (size_t i = 0; i < 1024; i++)
    {
        pcm.AddStereo(data.data(), samples);
    }

    std::array<float, fftLength> freq0{};
    std::array<float, fftLength> freq1{};
    pcm.ResetAutoLevel();
    pcm.GetSpectrum(freq0.data(), CHANNEL_0, fftLength);
    pcm.GetSpectrum(freq1.data(), CHANNEL_1, fftLength);

    // freq0 and freq1 should be equal
    for (size_t i = 0; i < fftLength; i++)
    {
        EXPECT_FLOAT_EQ(freq0[i], freq1[i]);
    }
    for (size_t i = 0; i < fftLength - 1; i++)
    {
        EXPECT_FLOAT_EQ(freq0[i], 0);
    }
    EXPECT_GT(freq0[fftLength - 1], 100000);
}
