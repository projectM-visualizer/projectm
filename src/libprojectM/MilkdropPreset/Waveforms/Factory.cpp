#include "Waveforms/Factory.hpp"

#include "Waveforms/CenteredSpiro.hpp"
#include "Waveforms/Circle.hpp"
#include "Waveforms/DerivativeLine.hpp"
#include "Waveforms/DoubleLine.hpp"
#include "Waveforms/ExplosiveHash.hpp"
#include "Waveforms/Line.hpp"
#include "Waveforms/Milkdrop2077Wave11.hpp"
#include "Waveforms/Milkdrop2077Wave9.hpp"
#include "Waveforms/Milkdrop2077WaveFlower.hpp"
#include "Waveforms/Milkdrop2077WaveLasso.hpp"
#include "Waveforms/Milkdrop2077WaveSkewed.hpp"
#include "Waveforms/Milkdrop2077WaveStar.hpp"
#include "Waveforms/Milkdrop2077WaveX.hpp"
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

        case WaveformMode::Milkdrop2077Wave9:
            return std::make_unique<Milkdrop2077Wave9>();

        case WaveformMode::Milkdrop2077WaveX:
            return std::make_unique<Milkdrop2077WaveX>();

        case WaveformMode::Milkdrop2077Wave11:
            return std::make_unique<Milkdrop2077Wave11>();

        case WaveformMode::Milkdrop2077WaveSkewed:
            return std::make_unique<Milkdrop2077WaveSkewed>();

        case WaveformMode::Milkdrop2077WaveStar:
            return std::make_unique<Milkdrop2077WaveStar>();

        case WaveformMode::Milkdrop2077WaveFlower:
            return std::make_unique<Milkdrop2077WaveFlower>();

        case WaveformMode::Milkdrop2077WaveLasso:
            return std::make_unique<Milkdrop2077WaveLasso>();

        case WaveformMode::Count:
            break;
    }

    return {};
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
