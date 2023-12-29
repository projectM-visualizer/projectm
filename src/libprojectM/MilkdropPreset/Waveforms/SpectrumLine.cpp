#include "SpectrumLine.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

bool SpectrumLine::IsSpectrumWave()
{
    return true;
}

void SpectrumLine::ResizeVertexLists(const PresetState& presetState)
{
    m_samples = 256;
    m_wave1Vertices.resize(m_samples);
}

void SpectrumLine::CalculateVertexCoordinates()
{
    for (int i = 0; i < m_samples; i++)
    {
        const float f = 0.1f * logf(m_pcmDataL[i * 2] + m_pcmDataL[i * 2 + 1]);
        m_wave1Vertices[i].x = m_edgeX + m_distanceX * static_cast<float>(i) + m_perpetualDX * f;
        m_wave1Vertices[i].y = m_edgeY + m_distanceY * static_cast<float>(i) + m_perpetualDY * f;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
