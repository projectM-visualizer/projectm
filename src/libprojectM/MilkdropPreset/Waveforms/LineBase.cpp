#include "LineBase.hpp"

#include <Audio/AudioConstants.hpp>

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void LineBase::ClipWaveformEdges(const float angle)
{
    m_distanceX = cosf(angle);
    m_distanceY = sinf(angle);

    std::array<float, 2> edgeX{
        m_waveX * cosf(angle + 1.57f) - m_distanceX * 3.0f,
        m_waveX * cosf(angle + 1.57f) + m_distanceX * 3.0f};

    std::array<float, 2> edgeY{
        m_waveX * sinf(angle + 1.57f) - m_distanceY * 3.0f,
        m_waveX * sinf(angle + 1.57f) + m_distanceY * 3.0f};

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float t{0.0};
            bool clip{false};

            switch (j)
            {
                case 0:
                    if (edgeX[i] > 1.1f)
                    {
                        t = (1.1f - edgeX[1 - i]) / (edgeX[i] - edgeX[1 - i]);
                        clip = true;
                    }
                    break;

                case 1:
                    if (edgeX[i] < -1.1f)
                    {
                        t = (-1.1f - edgeX[1 - i]) / (edgeX[i] - edgeX[1 - i]);
                        clip = true;
                    }
                    break;

                case 2:
                    if (edgeY[i] > 1.1f)
                    {
                        t = (1.1f - edgeY[1 - i]) / (edgeY[i] - edgeY[1 - i]);
                        clip = true;
                    }
                    break;

                case 3:
                    if (edgeY[i] < -1.1f)
                    {
                        t = (-1.1f - edgeY[1 - i]) / (edgeY[i] - edgeY[1 - i]);
                        clip = true;
                    }
                    break;
            }

            if (clip)
            {
                const float diffX = edgeX[i] - edgeX[1 - i];
                const float diffY = edgeY[i] - edgeY[1 - i];
                edgeX[i] = edgeX[1 - i] + diffX * t;
                edgeY[i] = edgeY[1 - i] + diffY * t;
            }
        }
    }

    m_sampleOffset = (Audio::WaveformSamples - m_samples) / 2;

    m_distanceX = (edgeX[1] - edgeX[0]) / static_cast<float>(m_samples);
    m_distanceY = (edgeY[1] - edgeY[0]) / static_cast<float>(m_samples);

    m_edgeX = edgeX[0];
    m_edgeY = edgeY[0];

    const float angle2 = atan2f(m_distanceY, m_distanceX);
    m_perpetualDX = cosf(angle2 + 1.57f);
    m_perpetualDY = sinf(angle2 + 1.57f);
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
