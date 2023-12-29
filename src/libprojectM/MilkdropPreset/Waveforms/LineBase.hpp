#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class LineBase : public WaveformMath
{
protected:
    void GenerateVertices(const PresetState& presetState, const PerFrameContext& presetPerFrameContext) override;

    /**
     * @brief Resizes the vertex list according to the waveform type.
     * @param presetState The preset state holder.
     */
    virtual void ResizeVertexLists(const PresetState& presetState) = 0;

    /**
     * @brief Calculates the actual vertex positions depending on the actual waveform implementation.
     */
    virtual void CalculateVertexCoordinates() = 0;

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
