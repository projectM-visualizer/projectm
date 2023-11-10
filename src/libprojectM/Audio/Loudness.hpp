/**
 * @file Loudness.hpp
 * @brief Calculates loudness values in relation to previous frames.
**/

#pragma once

#include "AudioConstants.hpp"

#include <array>
#include <cstdint>

namespace libprojectM {
namespace Audio {

/**
 * @brief Calculates beat-detection loudness relative to the previous frame(s).
 */
class Loudness
{
public:
    /**
     * @brief Frequency bands.
     * Only the first half of the spectrum is used for these bands, each using one third of this half.
     */
    enum class Band : int
    {
        Bass = 0,    //!< Bass band (first sixth of the spectrum)
        Middles = 1, //!< Middles band (second sixth of the spectrum)
        Treble = 2   //!< Treble band (third sixth of the spectrum)
    };

    /**
     * @brief Constructor.
     * @param band The band to use for this loudness instance.
     */
    explicit Loudness(Band band);

    /**
     * @brief Updates the beat detection values and averages.
     * Must only be called once per frame.
     * @param spectrumSamples The current frame's spectrum analyzer samples to use for the update.
     * @param secondsSinceLastFrame (Fractional) seconds passed since the last frame.
     * @param frame The number of frames already rendered since start. For the first few frames, a different dampening factor
     *              will be used to avoid "jumpy" behaviour of the values.
     */
    void Update(const std::array<float, SpectrumSamples>& spectrumSamples, double secondsSinceLastFrame, uint32_t frame);

    /**
     * @brief Returns the current frame's unattenuated loudness relative to the previous frame.
     * This value will revolve around 1.0, with <0.7 being very silent and >1.3 very loud audio.
     * @return The current frame's unattenuated loudness relative to the previous frame.
     */
    auto CurrentRelative() const -> float;

    /**
     * @brief Returns the attenuated loudness averaged over the previous frames.
     * This value will revolve around 1.0, with <0.7 being very silent and >1.3 very loud audio.
     * Does not change as much as the value returned by CurrentRelative().
     * @return The attenuated loudness averaged over the previous frames.
     */
    auto AverageRelative() const -> float;

private:
    /**
     * @brief Sums up the spectrum samples for the configured band.
     * @param spectrumSamples The spectrum analyzer samples to use.
     */
    void SumBand(const std::array<float, SpectrumSamples>& spectrumSamples);

    /**
     * @brief Updates the short- and long-term averages and relative values.
     * @param secondsSinceLastFrame (Fractional) seconds passed since the last frame.
     * @param frame The number of frames already rendered since start.
     */
    void UpdateBandAverage(double secondsSinceLastFrame, uint32_t frame);

    /**
     * @brief Adjusts the dampening rate according the the current FPS.
     * @param rate The rate to be dampened.
     * @param secondsSinceLastFrame (Fractional) seconds passed since the last frame.
     * @return The dampened rate value.
     */
    static auto AdjustRateToFps(float rate, double secondsSinceLastFrame) -> float;

    Band m_band{Band::Bass}; //!< The frequency band to use for this instance.

    float m_current{};     //!< The current frame's sum of all frequency strengths in the current band.
    float m_average{};     //!< The short-term averaged value of m_current.
    float m_longAverage{}; //!< The long-term averaged value of m_current.

    float m_currentRelative{1.0f}; //!< The relative loudness value to the previous frame.
    float m_averageRelative{1.0f}; //!< The attenuated relative loudness value.
};

} // namespace Audio
} // namespace libprojectM
