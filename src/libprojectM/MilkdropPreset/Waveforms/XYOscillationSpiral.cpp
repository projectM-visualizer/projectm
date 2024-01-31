#include "Waveforms/XYOscillationSpiral.hpp"

#include "PerFrameContext.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

auto XYOscillationSpiral::IsLoop() -> bool
{
    return true;
}

auto XYOscillationSpiral::UsesNormalizedMysteryParam() -> bool
{
    return true;
}

void XYOscillationSpiral::GenerateVertices(const PresetState& presetState,
                                           const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples);

    for (int i = 0; i < m_samples; i++)
    {
        float const radius = (0.53f + 0.43f * m_pcmDataR[i] + m_mysteryWaveParam);
        float const angle = m_pcmDataL[i + 32] * 1.57f + presetState.renderContext.time * 2.3f;

        m_wave1Vertices[i].x = radius * cosf(angle) * m_aspectY + m_waveX;
        m_wave1Vertices[i].y = radius * sinf(angle) * m_aspectX + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
