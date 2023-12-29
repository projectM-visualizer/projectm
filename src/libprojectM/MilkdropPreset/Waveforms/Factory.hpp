#pragma once

#include "WaveformMath.hpp"

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

class Factory
{
public:
    Factory() = delete;

    ~Factory() = delete;

    static auto Create(WaveformMode mode) -> std::unique_ptr<WaveformMath>;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
