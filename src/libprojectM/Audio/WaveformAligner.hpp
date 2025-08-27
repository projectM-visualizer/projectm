

#pragma once

#include "AudioConstants.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace libprojectM {
namespace Audio {


class WaveformAligner
{
public:
    WaveformAligner();


    void Align(WaveformBuffer& newWaveform);

protected:
    void GenerateWeights();
    int CalculateOffset(std::vector<WaveformBuffer>& newWaveformMips);
    void ResampleOctaves(std::vector<WaveformBuffer>& dstWaveformMips, WaveformBuffer& newWaveform);

    bool m_alignWaveReady{false};

    std::vector<std::array<float, AudioBufferSamples>> m_aligmentWeights;

    uint32_t m_octaves{};
    std::vector<uint32_t> m_octaveSamples;
    std::vector<uint32_t> m_octaveSampleSpacing;

    std::vector<WaveformBuffer> m_oldWaveformMips;
    std::vector<uint32_t> m_firstNonzeroWeights;
    std::vector<uint32_t> m_lastNonzeroWeights;
};

}
}
