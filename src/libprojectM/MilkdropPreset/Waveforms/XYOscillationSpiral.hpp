#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class XYOscillationSpiral : public WaveformMath
{
public:
    auto IsLoop() -> bool override;

protected:
    auto UsesNormalizedMysteryParam() -> bool override;
    void GenerateVertices(const PresetState& presetState,
                          const PerFrameContext& presetPerFrameContext) override;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
