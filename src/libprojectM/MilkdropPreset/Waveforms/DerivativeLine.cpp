#include "DerivativeLine.hpp"

#include "PresetState.hpp"

#include <cassert>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

bool DerivativeLine::UsesNormalizedMysteryParam()
{
    return true;
}

void DerivativeLine::GenerateVertices(const PresetState& presetState, const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples;

    if (m_samples > presetState.renderContext.viewportSizeX / 3)
    {
        m_samples /= 3;
    }

    m_wave1Vertices.resize(m_samples);

    int const sampleOffset = (Audio::WaveformSamples - m_samples) / 2;

    const float w1 = 0.45f + 0.5f * (m_mysteryWaveParam * 0.5f + 0.5f);
    const float w2 = 1.0f - w1;

    const float inverseSamples = 1.0f / static_cast<float>(m_samples);

    for (int i = 0; i < m_samples; i++)
    {
        assert((i + 25 + sampleOffset) < 512);
        m_wave1Vertices[i].x = -1.0f + 2.0f * (static_cast<float>(i) * inverseSamples) + m_waveX;
        m_wave1Vertices[i].y = m_pcmDataL[i + sampleOffset] * 0.47f + m_waveY;
        m_wave1Vertices[i].x += m_pcmDataR[i + 25 + sampleOffset] * 0.44f;

        // Momentum
        if (i > 1)
        {
            m_wave1Vertices[i].x = m_wave1Vertices[i].x * w2 + w1 * (m_wave1Vertices[i - 1].x * 2.0f - m_wave1Vertices[i - 2].x);
            m_wave1Vertices[i].y = m_wave1Vertices[i].y * w2 + w1 * (m_wave1Vertices[i - 1].y * 2.0f - m_wave1Vertices[i - 2].y);
        }
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
