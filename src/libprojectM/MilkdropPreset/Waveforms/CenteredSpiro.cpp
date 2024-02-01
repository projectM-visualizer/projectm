#include "CenteredSpiro.hpp"

#include <Audio/AudioConstants.hpp>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void CenteredSpiro::GenerateVertices(const PresetState&, const PerFrameContext&)
{
    // Alpha calculation is handled in MaximizeColors().
    m_samples = Audio::WaveformSamples;

    m_wave1Vertices.resize(m_samples);

    for (int i = 0; i < m_samples; i++)
    {
        m_wave1Vertices[i].x = m_pcmDataR[i] * m_aspectY + m_waveX;
        m_wave1Vertices[i].y = m_pcmDataL[i + 32] * m_aspectX + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
