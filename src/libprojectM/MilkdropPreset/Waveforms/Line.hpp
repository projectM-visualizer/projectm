#pragma once

#include "Waveforms/LineBase.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class Line : public LineBase
{
protected:
    void ResizeVertexLists(const PresetState& presetState) override;
    void CalculateVertexCoordinates() override;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
