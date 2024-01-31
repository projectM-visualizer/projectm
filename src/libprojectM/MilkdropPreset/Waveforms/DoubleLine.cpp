#include "DoubleLine.hpp"

#include "PresetState.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void DoubleLine::GenerateVertices(const PresetState& presetState, const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    if (m_samples > presetState.renderContext.viewportSizeX / 3)
    {
        m_samples /= 3;
    }

    m_wave1Vertices.resize(m_samples);
    m_wave2Vertices.resize(m_samples);

    ClipWaveformEdges(1.57f * m_mysteryWaveParam);

    float const separation = powf(m_waveY * 0.5f + 0.5f, 2.0f);
    for (int i = 0; i < m_samples; i++)
    {
        m_wave1Vertices[i].x = m_edgeX + m_distanceX * static_cast<float>(i) +
                               m_perpetualDX * (0.25f * m_pcmDataL[i + m_sampleOffset] + separation);
        m_wave1Vertices[i].y = m_edgeY + m_distanceY * static_cast<float>(i) +
                               m_perpetualDY * (0.25f * m_pcmDataL[i + m_sampleOffset] + separation);

        m_wave2Vertices[i].x = m_edgeX + m_distanceX * static_cast<float>(i) +
                               m_perpetualDX * (0.25f * m_pcmDataR[i + m_sampleOffset] - separation);
        m_wave2Vertices[i].y = m_edgeY + m_distanceY * static_cast<float>(i) +
                               m_perpetualDY * (0.25f * m_pcmDataR[i + m_sampleOffset] - separation);
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
