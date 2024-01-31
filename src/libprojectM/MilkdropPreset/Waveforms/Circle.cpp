#include "Waveforms/Circle.hpp"

#include "PerFrameContext.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

auto Circle::IsLoop() -> bool
{
    return true;
}

auto Circle::UsesNormalizedMysteryParam() -> bool
{
    return true;
}

void Circle::GenerateVertices(const PresetState& presetState,
                              const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples);

    const int sampleOffset{(Audio::WaveformSamples - m_samples) / 2};

    const float inverseSamplesMinusOne{1.0f / static_cast<float>(m_samples)};

    for (int i = 0; i < m_samples; i++)
    {
        float radius = 0.5f + 0.4f * m_pcmDataR[i + sampleOffset] + m_mysteryWaveParam;
        float const angle = static_cast<float>(i) * inverseSamplesMinusOne * 6.28f + presetState.renderContext.time * 0.2f;
        if (i < m_samples / 10)
        {
            float mix = static_cast<float>(i) / (static_cast<float>(m_samples) * 0.1f);
            mix = 0.5f - 0.5f * cosf(mix * 3.1416f);
            float const radius2 = 0.5f + 0.4f * m_pcmDataR[i + m_samples + sampleOffset] + m_mysteryWaveParam;
            radius = radius2 * (1.0f - mix) + radius * (mix);
        }

        m_wave1Vertices[i].x = radius * cosf(angle) * m_aspectY + m_waveX;
        m_wave1Vertices[i].y = radius * sinf(angle) * m_aspectX + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
