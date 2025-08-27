

#include "MilkdropFFT.hpp"

namespace libprojectM {
namespace Audio {

constexpr auto PI = 3.141592653589793238462643383279502884197169399f;

MilkdropFFT::MilkdropFFT(size_t samplesIn, size_t samplesOut, bool equalize, float envelopePower)
    : m_samplesIn(samplesIn)
    , m_numFrequencies(samplesOut * 2)
{
    InitBitRevTable();
    InitCosSinTable();
    InitEnvelopeTable(envelopePower);
    InitEqualizeTable(equalize);
}

void MilkdropFFT::InitEnvelopeTable(float power)
{
    if (power < 0.0f)
    {

        m_envelope = std::vector<float>(m_samplesIn, 1.0f);
        return;
    }

    float const multiplier = 1.0f / static_cast<float>(m_samplesIn) * 2.0f * PI;

    m_envelope.resize(m_samplesIn);

    if (power == 1.0f)
    {
        for (size_t i = 0; i < m_samplesIn; i++)
        {
            m_envelope[i] = 0.5f + 0.5f * std::sin(static_cast<float>(i) * multiplier - PI * 0.5f);
        }
    }
    else
    {
        for (size_t i = 0; i < m_samplesIn; i++)
        {
            m_envelope[i] = std::pow(0.5f + 0.5f * std::sin(static_cast<float>(i) * multiplier - PI * 0.5f), power);
        }
    }
}

void MilkdropFFT::InitEqualizeTable(bool equalize)
{
    if (!equalize)
    {
        m_equalize = std::vector<float>(m_numFrequencies / 2, 1.0f);
        return;
    }

    float const scaling = -0.02f;
    float const inverseHalfNumFrequencies = 1.0f / static_cast<float>(m_numFrequencies / 2);

    m_equalize.resize(m_numFrequencies / 2);

    for (size_t i = 0; i < m_numFrequencies / 2; i++)
    {
        m_equalize[i] = scaling * std::log(static_cast<float>(m_numFrequencies / 2 - i) * inverseHalfNumFrequencies);
    }
}

void MilkdropFFT::InitBitRevTable()
{
    m_bitRevTable.resize(m_numFrequencies);

    for (size_t i = 0; i < m_numFrequencies; i++)
    {
        m_bitRevTable[i] = i;
    }

    size_t j{};
    for (size_t i = 0; i < m_numFrequencies; i++)
    {
        if (j > i)
        {
            size_t const temp{m_bitRevTable[i]};
            m_bitRevTable[i] = m_bitRevTable[j];
            m_bitRevTable[j] = temp;
        }

        size_t m = m_numFrequencies >> 1;

        while (m >= 1 && j >= m)
        {
            j -= m;
            m >>= 1;
        }

        j += m;
    }
}

void MilkdropFFT::InitCosSinTable()
{
    size_t tabsize{};
    size_t dftsize{2};

    while (dftsize <= m_numFrequencies)
    {
        tabsize++;
        dftsize <<= 1;
    }

    m_cosSinTable.resize(tabsize);

    dftsize = 2;
    size_t index{0};
    while (dftsize <= m_numFrequencies)
    {
        auto const theta{-2.0f * PI / static_cast<float>(dftsize)};
        m_cosSinTable[index] = std::polar(1.0f, theta);
        index++;
        dftsize <<= 1;
    }
}

void MilkdropFFT::TimeToFrequencyDomain(const std::vector<float>& waveformData, std::vector<float>& spectralData)
{
    if (m_bitRevTable.empty() || m_cosSinTable.empty() || waveformData.size() < m_samplesIn)
    {
        spectralData.clear();
        return;
    }


    std::vector<std::complex<float>> spectrumData(m_numFrequencies, std::complex<float>());
    for (size_t i = 0; i < m_numFrequencies; i++)
    {
        size_t const idx{m_bitRevTable[i]};
        if (idx < m_samplesIn)
        {
            spectrumData[i].real(waveformData[idx] * m_envelope[idx]);
        }
    }


    size_t dftSize{2};
    size_t octave{0};

    while (dftSize <= m_numFrequencies)
    {
        std::complex<float> w{1.0f, 0.0f};
        std::complex<float> const wp{m_cosSinTable[octave]};

        size_t const hdftsize{dftSize >> 1};

        for (size_t m = 0; m < hdftsize; m += 1)
        {
            for (size_t i = m; i < m_numFrequencies; i += dftSize)
            {
                size_t const j{i + hdftsize};
                std::complex<float> const tempNum{spectrumData[j] * w};
                spectrumData[j] = spectrumData[i] - tempNum;
                spectrumData[i] = spectrumData[i] + tempNum;
            }

            w *= wp;
        }

        dftSize <<= 1;
        octave++;
    }


    spectralData.resize(m_numFrequencies / 2);
    for (size_t i = 0; i < m_numFrequencies / 2; i++)
    {
        spectralData[i] = m_equalize[i] * std::abs(spectrumData[i]);
    }
}

}
}
