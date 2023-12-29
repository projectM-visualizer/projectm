#include "Waveforms/Milkdrop2077WaveSkewed.hpp"

#include "PerFrameContext.hpp"

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void Milkdrop2077WaveSkewed::GenerateVertices(const PresetState& presetState,
                                              const PerFrameContext& presetPerFrameContext)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples);

    float alpha = static_cast<float>(*presetPerFrameContext.wave_a) * 1.25f;
    if (presetState.modWaveAlphaByvolume)
    {
        alpha *= presetState.audioData.vol;
    }
    alpha = std::max(0.0f, std::min(1.0f, alpha));

    for (size_t i = 0; i < static_cast<size_t>(m_samples); i++)
    {
        float rad = 0.63f + 0.23f * m_pcmDataR[i] + m_mysteryWaveParam;
        float ang = m_pcmDataL[i + 32] * 0.9f + presetState.renderContext.time * 3.3f;
        m_wave1Vertices[i].x = rad * cosf(ang + alpha) * m_aspectY + m_waveX;
        m_wave1Vertices[i].y = rad * sinf(ang) * m_aspectX + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
