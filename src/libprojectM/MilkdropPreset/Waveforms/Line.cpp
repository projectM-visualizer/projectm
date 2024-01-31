#include "Line.hpp"

#include "PresetState.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void Line::GenerateVertices(const PresetState& presetState, const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    if (m_samples > presetState.renderContext.viewportSizeX / 3)
    {
        m_samples /= 3;
    }

    m_wave1Vertices.resize(m_samples);

    ClipWaveformEdges(1.57f + m_mysteryWaveParam);

    for (int i = 0; i < m_samples; i++)
    {
        m_wave1Vertices[i].x = m_edgeX + m_distanceX * static_cast<float>(i) + m_perpetualDX * 0.25f * m_pcmDataL[i + m_sampleOffset];
        m_wave1Vertices[i].y = m_edgeY + m_distanceY * static_cast<float>(i) + m_perpetualDY * 0.25f * m_pcmDataL[i + m_sampleOffset];
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
