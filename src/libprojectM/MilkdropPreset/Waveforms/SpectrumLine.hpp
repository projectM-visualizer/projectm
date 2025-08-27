#pragma once

#include "Waveforms/LineBase.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class SpectrumLine : public LineBase
{
protected:
    auto IsSpectrumWave() -> bool override;
    void GenerateVertices(const PresetState& presetState, const PerFrameContext& presetPerFrameContext) override;
};

}
}
}
