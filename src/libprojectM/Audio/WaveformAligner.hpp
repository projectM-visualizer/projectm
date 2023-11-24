/**
 * @file WaveformAligner.hpp
 * @brief Mip-based waveform alignment algorithm
 */

#pragma once

#include "AudioConstants.hpp"

#include <cstddef>
#include <vector>

namespace libprojectM {
namespace Audio {

/**
 * @class WaveformAligner
 * @brief Mip-based waveform alignment algorithm
 *
 * Calculates the absolute error between the previous and current waveforms over several octaves
 * and sample offsets, then shifts the new waveform forward to best align with the previous frame.
 * This will keep similar features in-place instead of randomly jumping around on each frame and creates
 * for a smoother-looking waveform visualization.
 */
class WaveformAligner
{
public:
    WaveformAligner();

    /**
     * @brief Aligns waveforms to a best-fit match to the previous frame.
     * @param[in,out] newWaveform The new waveform to be aligned.
     */
    void Align(WaveformBuffer& newWaveform);

private:
    bool m_alignWaveReady{false}; //!< Alignment needs special treatment for the first buffer fill.

    std::vector<std::array<float, AudioBufferSamples>> m_aligmentWeights; //!< Sample weights per octave.

    size_t m_octaves{};                        //!< Number of mip-levels/octaves.
    std::vector<size_t> m_octaveSamples;       //!< Samples per octave.
    std::vector<size_t> m_octaveSampleSpacing; //!< Space between samples per octave.

    std::vector<WaveformBuffer> m_oldWaveformMips; //!< Mip levels of the previous frame's waveform.
    std::vector<size_t> m_firstNonzeroWeights;     //!< First non-zero weight sample index for each octave.
    std::vector<size_t> m_lastNonzeroWeights;      //!< Last non-zero weight sample index for each octave.
};

} // namespace Audio
} // namespace libprojectM
