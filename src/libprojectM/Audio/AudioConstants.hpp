#pragma once

#include <array>

namespace libprojectM {
namespace Audio {

static constexpr int AudioBufferSamples = 576; //!< Number of waveform data samples stored in the buffer for analysis.
static constexpr int WaveformSamples = 480;    //!< Number of waveform data samples available for rendering a frame.
static constexpr int SpectrumSamples = 512;    //!< Number of spectrum analyzer samples.

using WaveformBuffer = std::array<float, AudioBufferSamples>; //!< Buffer with waveform data. Only the first WaveformSamples number of samples are valid.
using SpectrumBuffer = std::array<float, SpectrumSamples>;    //!< Buffer with spectrum data.

} // namespace Audio
} // namespace libprojectM
