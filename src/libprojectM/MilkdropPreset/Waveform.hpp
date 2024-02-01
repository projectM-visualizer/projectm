#pragma once

#include "Constants.hpp"
#include "WaveformMode.hpp"

#include "Waveforms/WaveformMath.hpp"

#include <Renderer/RenderItem.hpp>

#include <memory>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

class PresetState;
class PerFrameContext;

class Waveform : public Renderer::RenderItem
{
public:
    explicit Waveform(PresetState& presetState);

    void Draw(const PerFrameContext& presetPerFrameContext);

    void InitVertexAttrib() override;

private:
    void MaximizeColors(const PerFrameContext& presetPerFrameContext);
    void ModulateOpacityByVolume(const PerFrameContext& presetPerFrameContext);

    PresetState& m_presetState; //!< The preset state.

    WaveformMode m_mode{WaveformMode::Circle}; //!< Line drawing mode.

    std::unique_ptr<Waveforms::WaveformMath> m_waveformMath; //!< The waveform vertex math implementation.

    float m_tempAlpha{0.0f}; //!< Calculated alpha value.
    int m_samples{};         //!< Number of samples in the current waveform. Depends on the mode.

    std::vector<Point> m_wave1Vertices;
    std::vector<Point> m_wave2Vertices;
};

} // namespace MilkdropPreset
} // namespace libprojectM
