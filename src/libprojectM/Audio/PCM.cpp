#include "PCM.hpp"

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
        size_t const bufferOffset = (m_start + i) % AudioBufferSamples;
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
    m_start = (m_start + sampleCount) % AudioBufferSamples;
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
    AddToBuffer<32768, 0>(samples, channels, count);
}

void PCM::UpdateFrameAudioData(double secondsSinceLastFrame, uint32_t frame)
{
    // 1. Copy audio data from input buffer
    CopyNewWaveformData(m_inputBufferL, m_waveformL);
    CopyNewWaveformData(m_inputBufferR, m_waveformR);

    // 2. Update spectrum analyzer data for both channels
    UpdateSpectrum(m_waveformL, m_spectrumL);
    UpdateSpectrum(m_waveformR, m_spectrumR);

    // 3. Align waveforms
    m_alignL.Align(m_waveformL);
    m_alignR.Align(m_waveformR);

    // 4. Update beat detection values
    m_bass.Update(m_spectrumL, secondsSinceLastFrame, frame);
    m_middles.Update(m_spectrumL, secondsSinceLastFrame, frame);
    m_treble.Update(m_spectrumL, secondsSinceLastFrame, frame);

}

auto PCM::GetFrameAudioData() const -> FrameAudioData
{
    FrameAudioData data{};

    std::copy(m_waveformL.begin(), m_waveformL.begin() + WaveformSamples, data.waveformLeft.begin());
    std::copy(m_waveformR.begin(), m_waveformR.begin() + WaveformSamples, data.waveformRight.begin());
    std::copy(m_spectrumL.begin(), m_spectrumL.begin() + SpectrumSamples, data.spectrumLeft.begin());
    std::copy(m_spectrumR.begin(), m_spectrumR.begin() + SpectrumSamples, data.spectrumRight.begin());

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

void PCM::UpdateSpectrum(const WaveformBuffer& waveformData, SpectrumBuffer& spectrumData)
{
    std::vector<float> waveformSamples(AudioBufferSamples);
    std::vector<float> spectrumValues;

    size_t oldI{0};
    for (size_t i = 0; i < AudioBufferSamples; i++)
    {
        // Damp the input into the FFT a bit, to reduce high-frequency noise:
        waveformSamples[i] = 0.5f * (waveformData[i] + waveformData[oldI]);
        oldI = i;
    }

    m_fft.TimeToFrequencyDomain(waveformSamples, spectrumValues);

    std::copy(spectrumValues.begin(), spectrumValues.end(), spectrumData.begin());
}

void PCM::CopyNewWaveformData(const WaveformBuffer& source, WaveformBuffer& destination)
{
    auto const bufferStartIndex = m_start.load();

    for (size_t i = 0; i < AudioBufferSamples; i++)
    {
        destination[i] = source[(bufferStartIndex + i) % AudioBufferSamples];
    }
}


} // namespace Audio
} // namespace libprojectM
