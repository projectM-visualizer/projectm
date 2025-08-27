#pragma once

#include "Waveforms/WaveformMath.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class Milkdrop2077WaveSkewed : public WaveformMath
{
protected:
    void GenerateVertices(const PresetState& presetState,
                          const PerFrameContext& presetPerFrameContext) override;
};

}
}
}
