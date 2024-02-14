#include "Waveforms/WaveformMath.hpp"

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

auto WaveformMath::GetVertices(const PresetState& presetState,
                               const PerFrameContext& presetPerFrameContext) -> std::array<VertexList, 2>
{
    static_assert(WaveformMaxPoints >= libprojectM::Audio::SpectrumSamples, "WaveformMaxPoints is smaller than SpectrumSamples");
    static_assert(WaveformMaxPoints >= libprojectM::Audio::WaveformSamples, "WaveformMaxPoints is smaller than WaveformSamples");

    using libprojectM::Audio::WaveformSamples;

    // Get the correct audio sample type for the current waveform mode.
    if (IsSpectrumWave())
    {
        std::copy(begin(presetState.audioData.spectrumLeft),
                  begin(presetState.audioData.spectrumLeft) + Audio::SpectrumSamples,
                  begin(m_pcmDataL));

        std::copy(begin(presetState.audioData.spectrumRight),
                  begin(presetState.audioData.spectrumRight) + Audio::SpectrumSamples,
                  begin(m_pcmDataR));
    }
    else
    {
        std::copy(begin(presetState.audioData.waveformLeft),
                  begin(presetState.audioData.waveformLeft) + Audio::WaveformSamples,
                  begin(m_pcmDataL));

        std::copy(begin(presetState.audioData.waveformRight),
                  begin(presetState.audioData.waveformRight) + Audio::WaveformSamples,
                  begin(m_pcmDataR));
    }

    // Scale and smooth waveform data
    float scale = presetState.waveScale / 128.0f;
    // The first sample gets scaled directly because it is not mixed with other samples.
    m_pcmDataL[0] *= scale;
    m_pcmDataR[0] *= scale;
    /* If s[i] is output sample i and p[i] is input sample i, then:
     * s[0] = waveScale*p[i];
     * s[i] = waveScale*(1-waveSmoothing)*p[i] + waveSmoothing*s[i-1]
     * Note that this is an IIR filter with alpha = waveSmoothing.
     */
    float mix2 = presetState.waveSmoothing; // amount of previous sample to add to this sample
    float mix1 = scale * (1.0f - mix2); // amount to scale this sample
    // Scale and mix samples after the first one.
    for (size_t i = 1; i < m_pcmDataL.size(); ++i)
    {
        m_pcmDataL[i] = m_pcmDataL[i]*mix1 + m_pcmDataL[i-1]*mix2;
        m_pcmDataR[i] = m_pcmDataR[i]*mix1 + m_pcmDataR[i-1]*mix2;
    }

    // Aspect multipliers
    if (presetState.renderContext.viewportSizeX > presetState.renderContext.viewportSizeY)
    {
        m_aspectY = static_cast<float>(presetState.renderContext.viewportSizeY) / static_cast<float>(presetState.renderContext.viewportSizeX);
    }
    else
    {
        m_aspectX = static_cast<float>(presetState.renderContext.viewportSizeX) / static_cast<float>(presetState.renderContext.viewportSizeY);
    }

    m_mysteryWaveParam = static_cast<float>(*presetPerFrameContext.wave_mystery);

    if (UsesNormalizedMysteryParam() && (m_mysteryWaveParam < 1.0f || m_mysteryWaveParam > 1.0f))
    {
        m_mysteryWaveParam = m_mysteryWaveParam * 0.5f + 0.5f;
        m_mysteryWaveParam -= std::floor(m_mysteryWaveParam);
        m_mysteryWaveParam = std::fabs(m_mysteryWaveParam);
        m_mysteryWaveParam = m_mysteryWaveParam * 2 - 1;
    }

    m_waveX = 2.0f * static_cast<float>(*presetPerFrameContext.wave_x) - 1.0f;
    m_waveY = 2.0f * static_cast<float>(*presetPerFrameContext.wave_y) - 1.0f;

    GenerateVertices(presetState, presetPerFrameContext);

    std::array<VertexList, 2> smoothedVertices;
    SmoothWave(m_wave1Vertices, smoothedVertices.at(0));
    SmoothWave(m_wave2Vertices, smoothedVertices.at(1));

    return smoothedVertices;
}

auto WaveformMath::IsLoop() -> bool
{
    return false;
}

auto WaveformMath::IsSpectrumWave() -> bool
{
    return false;
}

auto WaveformMath::UsesNormalizedMysteryParam() -> bool
{
    return false;
}

void WaveformMath::SmoothWave(const VertexList& inputVertices, VertexList& outputVertices)
{
    constexpr float c1{-0.15f};
    constexpr float c2{1.15f};
    constexpr float c3{1.15f};
    constexpr float c4{-0.15f};
    constexpr float inverseSum{1.0f / (c1 + c2 + c3 + c4)};

    size_t outputIndex = 0;
    size_t indexBelow = 0;
    size_t indexAbove2 = 1;

    if (inputVertices.empty())
    {
        outputVertices.clear();
        return;
    }

    outputVertices.resize(static_cast<size_t>(m_samples) * 2 - 1);

    for (size_t inputIndex = 0; inputIndex < static_cast<size_t>(m_samples) - 1; inputIndex++)
    {
        size_t const indexAbove = indexAbove2;
        indexAbove2 = std::min(static_cast<size_t>(m_samples) - 1, inputIndex + 2);
        outputVertices[outputIndex] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1].x = (c1 * inputVertices[indexBelow].x + c2 * inputVertices[inputIndex].x + c3 * inputVertices[indexAbove].x + c4 * inputVertices[indexAbove2].x) * inverseSum;
        outputVertices[outputIndex + 1].y = (c1 * inputVertices[indexBelow].y + c2 * inputVertices[inputIndex].y + c3 * inputVertices[indexAbove].y + c4 * inputVertices[indexAbove2].y) * inverseSum;
        indexBelow = inputIndex;
        outputIndex += 2;
    }

    outputVertices.at(outputIndex) = inputVertices.at(m_samples - 1);
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
