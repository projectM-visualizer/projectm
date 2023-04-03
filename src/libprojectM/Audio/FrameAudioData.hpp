/**
 * @file FrameAudioData.hpp
 * @brief Holds all audio data to be used to render a single frame.
 *
 * This includes the actual waveform data, spectrum and beat detection values.
 */
#pragma once

#include "projectM-4/projectM_export.h"
#include "AudioConstants.hpp"

#include <array>

namespace libprojectM {
namespace Audio {

class PROJECTM_EXPORT FrameAudioData
{
public:
    float bass{0.f};
    float bassAtt{0.f};
    float mid{0.f};
    float midAtt{0.f};
    float treb{0.f};
    float trebAtt{0.f};

    float vol{0.f};
    float volAtt{0.f};

    std::array<float, WaveformSamples> waveformLeft;
    std::array<float, WaveformSamples> waveformRight;

    std::array<float, SpectrumSamples> spectrumLeft;
    std::array<float, SpectrumSamples> spectrumRight;
};

} // namespace Audio
} // namespace libprojectM
