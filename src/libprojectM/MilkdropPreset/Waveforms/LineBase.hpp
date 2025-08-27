#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class LineBase : public WaveformMath
{
protected:

    void ClipWaveformEdges(float angle);

    int m_sampleOffset{};

    float m_edgeX{};
    float m_edgeY{};

    float m_distanceX{};
    float m_distanceY{};

    float m_perpetualDX{};
    float m_perpetualDY{};
};

}
}
}
