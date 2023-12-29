#include "Waveforms/Factory.hpp"

#include "Waveforms/CenteredSpiro.hpp"
#include "Waveforms/Circle.hpp"
#include "Waveforms/DerivativeLine.hpp"
#include "Waveforms/DoubleLine.hpp"
#include "Waveforms/ExplosiveHash.hpp"
#include "Waveforms/Line.hpp"
#include "Waveforms/SpectrumLine.hpp"
#include "Waveforms/XYOscillationSpiral.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

auto Factory::Create(WaveformMode mode) -> std::unique_ptr<WaveformMath>
{
    switch (mode)
    {
        case WaveformMode::Circle:
            return std::make_unique<Circle>();

        case WaveformMode::XYOscillationSpiral:
            return std::make_unique<XYOscillationSpiral>();

        case WaveformMode::CenteredSpiro:
        case WaveformMode::CenteredSpiroVolume:
            // Both "centered spiro" waveforms are identical. Only difference is the alpha value.
            return std::make_unique<CenteredSpiro>();

        case WaveformMode::DerivativeLine:
            return std::make_unique<DerivativeLine>();

        case WaveformMode::ExplosiveHash:
            return std::make_unique<ExplosiveHash>();

        case WaveformMode::Line:
            return std::make_unique<Line>();

        case WaveformMode::DoubleLine:
            return std::make_unique<DoubleLine>();

        case WaveformMode::SpectrumLine:
            return std::make_unique<SpectrumLine>();
    }

    return {};
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
