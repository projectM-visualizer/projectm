
#pragma once

#include "AudioConstants.hpp"
#include "projectM-4/projectM_export.h"

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

}
}
