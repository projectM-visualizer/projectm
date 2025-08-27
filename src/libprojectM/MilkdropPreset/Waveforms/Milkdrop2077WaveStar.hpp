#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class Milkdrop2077WaveStar : public WaveformMath
{
public:
    auto IsLoop() -> bool override;

protected:
    void GenerateVertices(const PresetState& presetState,
                          const PerFrameContext& presetPerFrameContext) override;
};

}
}
}
