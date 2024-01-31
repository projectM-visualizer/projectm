#include "Waveforms/Milkdrop2077WaveStar.hpp"

#include "PerFrameContext.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

bool Milkdrop2077WaveStar::IsLoop()
{
    return true;
}

void Milkdrop2077WaveStar::GenerateVertices(const PresetState& presetState,
                                            const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples + 1);

    int const sampleOffset = (Audio::WaveformSamples - m_samples) / 2;

    float const invertedSamplesMinusOne = 1.0f / static_cast<float>(m_samples - 1);
    float const tenthSamples = static_cast<float>(m_samples) * 0.1f;

    for (int sample = 0; sample < m_samples; sample++)
    {
        float radius = 0.7f + 0.4f * m_pcmDataR[sample + sampleOffset] + m_mysteryWaveParam;
        float const angle = static_cast<float>(sample) * invertedSamplesMinusOne * 6.28f + presetState.renderContext.time * 0.2f;
        if (static_cast<float>(sample) < m_samples / radius)
        {
            float mix = static_cast<float>(sample) / tenthSamples;
            mix = 0.5f - 0.5f * cosf(mix * 3.1416f);
            // Subtracting the sample offset here instead of adding it, as the original Milkdrop2077 code accessed out-of-range data.
            float const radius2 = 0.5f + 0.4f * m_pcmDataR[sample + m_samples - sampleOffset] + m_mysteryWaveParam;
            radius = radius2 * (1.0f - mix) + radius * mix;
        }
        m_wave1Vertices[sample].x = radius * cosf(angle) * m_aspectY + m_waveX;
        m_wave1Vertices[sample].y = radius * sinf(angle) * m_aspectX + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
