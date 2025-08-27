

#pragma once

#include "AudioConstants.hpp"

#include <array>
#include <cstdint>

namespace libprojectM {
namespace Audio {


class Loudness
{
public:

    enum class Band : int
    {
        Bass = 0,
        Middles = 1,
        Treble = 2
    };


    explicit Loudness(Band band);


    void Update(const std::array<float, SpectrumSamples>& spectrumSamples, double secondsSinceLastFrame, uint32_t frame);


    auto CurrentRelative() const -> float;


    auto AverageRelative() const -> float;

private:

    void SumBand(const std::array<float, SpectrumSamples>& spectrumSamples);


    void UpdateBandAverage(double secondsSinceLastFrame, uint32_t frame);


    static auto AdjustRateToFps(float rate, double secondsSinceLastFrame) -> float;

    Band m_band{Band::Bass};

    float m_current{};
    float m_average{};
    float m_longAverage{};

    float m_currentRelative{1.0f};
    float m_averageRelative{1.0f};
};

}
}
