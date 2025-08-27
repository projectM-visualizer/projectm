

#pragma once

#include "AudioConstants.hpp"
#include "FrameAudioData.hpp"
#include "Loudness.hpp"
#include "MilkdropFFT.hpp"
#include "WaveformAligner.hpp"

#include <projectM-4/projectM_export.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>


namespace libprojectM {
namespace Audio {

class PCM
{
public:

    PROJECTM_EXPORT void Add(const float* samples, uint32_t channels, size_t count);


    PROJECTM_EXPORT void Add(const uint8_t* samples, uint32_t channels, size_t count);


    PROJECTM_EXPORT void Add(const int16_t* samples, uint32_t channels, size_t count);


    PROJECTM_EXPORT void UpdateFrameAudioData(double secondsSinceLastFrame, uint32_t frame);


    PROJECTM_EXPORT auto GetFrameAudioData() const -> FrameAudioData;

private:
    template<
        int signalAmplitude,
        int signalOffset,
        typename SampleType>
    void AddToBuffer(const SampleType* samples, uint32_t channel, size_t sampleCount);


    void UpdateSpectrum(const WaveformBuffer& waveformData, SpectrumBuffer& spectrumData);


    void CopyNewWaveformData(const WaveformBuffer& source, WaveformBuffer& destination);


    WaveformBuffer m_inputBufferL{0.f};
    WaveformBuffer m_inputBufferR{0.f};
    std::atomic<size_t> m_start{0};


    WaveformBuffer m_waveformL{0.f};
    WaveformBuffer m_waveformR{0.f};


    SpectrumBuffer m_spectrumL{0.f};
    SpectrumBuffer m_spectrumR{0.f};

    MilkdropFFT m_fft{WaveformSamples, SpectrumSamples, true};


    WaveformAligner m_alignL;
    WaveformAligner m_alignR;


    Loudness m_bass{Loudness::Band::Bass};
    Loudness m_middles{Loudness::Band::Middles};
    Loudness m_treble{Loudness::Band::Treble};
};

}
}
