

#pragma once

#include <complex>
#include <vector>

namespace libprojectM {
namespace Audio {


class MilkdropFFT
{
public:

    MilkdropFFT(size_t samplesIn, size_t samplesOut, bool equalize = true, float envelopePower = 1.0f);


    void TimeToFrequencyDomain(const std::vector<float>& waveformData, std::vector<float>& spectralData);


    auto NumFrequencies() const -> size_t
    {
        return m_numFrequencies;
    };

private:

    void InitEnvelopeTable(float power);


    void InitEqualizeTable(bool equalize);


    void InitBitRevTable();


    void InitCosSinTable();

    size_t m_samplesIn{};
    size_t m_numFrequencies{};

    std::vector<size_t> m_bitRevTable;
    std::vector<float> m_envelope;
    std::vector<float> m_equalize;
    std::vector<std::complex<float>> m_cosSinTable;
};

}
}
