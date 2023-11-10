#include "PCM.hpp"

#include <cassert>

namespace libprojectM {
namespace Audio {

template<
    int signalAmplitude,
    int signalOffset,
    typename SampleType>
void PCM::AddToBuffer(
    SampleType const* const samples,
    uint32_t channels,
    size_t const sampleCount)
{
    if (channels == 0 || sampleCount == 0)
    {
        return;
    }

    for (size_t i = 0; i < sampleCount; i++)
    {
        size_t const bufferOffset = (m_start + i) % WaveformSamples;
        m_inputBufferL[bufferOffset] = 128.0f * (static_cast<float>(samples[0 + i * channels]) - float(signalOffset)) / float(signalAmplitude);
        if (channels > 1)
        {
            m_inputBufferR[bufferOffset] = 128.0f * (static_cast<float>(samples[1 + i * channels]) - float(signalOffset)) / float(signalAmplitude);
        }
        else
        {
            m_inputBufferR[bufferOffset] = m_inputBufferL[bufferOffset];
        }
    }
    m_start = (m_start + sampleCount) % WaveformSamples;
}

void PCM::Add(float const* const samples, uint32_t channels, size_t const count)
{
    AddToBuffer<1, 0>(samples, channels, count);
}
void PCM::Add(uint8_t const* const samples, uint32_t channels, size_t const count)
{
    AddToBuffer<128, 128>(samples, channels, count);
}
void PCM::Add(int16_t const* const samples, uint32_t channels, size_t const count)
{
    AddToBuffer<0, 32768>(samples, channels, count);
}

void PCM::UpdateFrameAudioData(double secondsSinceLastFrame, uint32_t frame)
{
    // 1. Copy audio data from input buffer
    CopyNewWaveformData();

    // 2. Align waveforms

    // 3. Update spectrum analyzer data for both channels
    UpdateFftChannel(0);
    UpdateFftChannel(1);

    // 4. Update beat detection values
    m_bass.Update(m_spectrumL, secondsSinceLastFrame, frame);
    m_middles.Update(m_spectrumL, secondsSinceLastFrame, frame);
    m_treble.Update(m_spectrumL, secondsSinceLastFrame, frame);
}

auto PCM::GetFrameAudioData() const -> FrameAudioData
{
    FrameAudioData data{};

    std::copy(m_waveformL.begin(), m_waveformL.end(), data.waveformLeft.begin());
    std::copy(m_waveformR.begin(), m_waveformR.end(), data.waveformRight.begin());
    std::copy(m_spectrumL.begin(), m_spectrumL.end(), data.spectrumLeft.begin());
    std::copy(m_spectrumR.begin(), m_spectrumR.end(), data.spectrumRight.begin());

    data.bass = m_bass.CurrentRelative();
    data.mid = m_middles.CurrentRelative();
    data.treb = m_treble.CurrentRelative();

    data.bassAtt = m_bass.AverageRelative();
    data.midAtt = m_middles.AverageRelative();
    data.trebAtt = m_treble.AverageRelative();

    data.vol = (data.bass + data.mid + data.treb) * 0.333f;
    data.volAtt = (data.bassAtt + data.midAtt + data.trebAtt) * 0.333f;

    return data;
}

void PCM::UpdateFftChannel(size_t const channel)
{
    assert(channel == 0 || channel == 1);

    std::vector<float> waveformSamples(WaveformSamples);
    std::vector<float> spectrumValues;

    auto const& from = channel == 0 ? m_waveformL : m_waveformR;
    auto& spectrum = channel == 0 ? m_spectrumL : m_spectrumR;

    size_t oldI{0};
    for (size_t i = 0; i < WaveformSamples; i++)
    {
        // Damp the input into the FFT a bit, to reduce high-frequency noise:
        waveformSamples[i] = 0.5f * (from[i] + from[oldI]);
        oldI = i;
    }

    m_fft.TimeToFrequencyDomain(waveformSamples, spectrumValues);

    std::copy(spectrumValues.begin(), spectrumValues.end(), spectrum.begin());
}

void PCM::CopyNewWaveformData()
{
    const auto& copyChannel =
        [](size_t start, const std::array<float, WaveformSamples>& inputSamples, std::array<float, WaveformSamples>& outputSamples)
    {
        for (size_t i = 0; i < WaveformSamples; i++)
        {
            outputSamples[i] = inputSamples[(start + i) % WaveformSamples];
        }
    };

    auto const bufferStartIndex = m_start.load();
    copyChannel(bufferStartIndex, m_inputBufferL, m_waveformL);
    copyChannel(bufferStartIndex, m_inputBufferR, m_waveformR);
}

} // namespace Audio
} // namespace libprojectM
