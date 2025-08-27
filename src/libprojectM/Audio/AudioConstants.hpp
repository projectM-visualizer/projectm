#pragma once

#include <array>

namespace libprojectM {
namespace Audio {

static constexpr int AudioBufferSamples = 576;
static constexpr int WaveformSamples = 480;
static constexpr int SpectrumSamples = 512;

using WaveformBuffer = std::array<float, AudioBufferSamples>;
using SpectrumBuffer = std::array<float, SpectrumSamples>;

}
}
