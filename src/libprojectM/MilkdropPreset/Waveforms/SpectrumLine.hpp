#pragma once

#include "Waveforms/LineBase.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class SpectrumLine : public LineBase
{
protected:
    auto IsSpectrumWave() -> bool override;
    void ResizeVertexLists(const PresetState& presetState) override;
    void CalculateVertexCoordinates() override;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
