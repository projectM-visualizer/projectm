#include "ExplosiveHash.hpp"

#include "PresetState.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void ExplosiveHash::GenerateVertices(const PresetState& presetState, const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples;

    m_wave1Vertices.resize(m_samples);

    const float cosineRotation = cosf(presetState.renderContext.time * 0.3f);
    const float sineRotation = sinf(presetState.renderContext.time * 0.3f);

    for (int i = 0; i < m_samples; i++)
    {
        const float x0 = (m_pcmDataR[i] * m_pcmDataL[i + 32] + m_pcmDataL[i] * m_pcmDataR[i + 32]);
        const float y0 = (m_pcmDataR[i] * m_pcmDataR[i] - m_pcmDataL[i + 32] * m_pcmDataL[i + 32]);
        m_wave1Vertices[i].x = ((x0 * cosineRotation - y0 * sineRotation) * m_aspectY) + m_waveX;
        m_wave1Vertices[i].y = ((x0 * sineRotation + y0 * cosineRotation) * m_aspectX) + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
