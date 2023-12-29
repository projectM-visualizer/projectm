#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class LineBase : public WaveformMath
{
protected:
    /**
     * @brief Calculates the waveform x/y coordinates and distances and clips them to the screen.
     * @param initialAngle
     */
    void ClipWaveformEdges(float angle);

    int m_sampleOffset{}; //!< Waveform render sample offset. If less than 480, the center part of the waveform is rendered.

    float m_edgeX{}; //!< Waveform left edge offset.
    float m_edgeY{}; //!< Waveform top edge offset.

    float m_distanceX{}; //!< Waveform X distance (stretch)
    float m_distanceY{}; //!< Waveform Y distance (stretch)

    float m_perpetualDX{}; //!< Waveform perpetual X distance
    float m_perpetualDY{}; //!< Waveform perpetual Y distance
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
