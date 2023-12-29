#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class DerivativeLine : public WaveformMath
{
protected:
    auto UsesNormalizedMysteryParam() -> bool override;
    void GenerateVertices(const PresetState& presetState, const PerFrameContext& presetPerFrameContext) override;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
