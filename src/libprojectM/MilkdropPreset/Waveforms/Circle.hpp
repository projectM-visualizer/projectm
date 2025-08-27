#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class Circle : public WaveformMath
{
public:
    auto IsLoop() -> bool override;

protected:
    auto UsesNormalizedMysteryParam() -> bool override;
    void GenerateVertices(const PresetState& presetState,
                          const PerFrameContext& presetPerFrameContext) override;
};

}
}
}
