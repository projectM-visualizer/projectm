#pragma once

#include "Waveforms/LineBase.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class Milkdrop2077WaveX : public LineBase
{
protected:
    void GenerateVertices(const PresetState& presetState, const PerFrameContext& presetPerFrameContext) override;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
