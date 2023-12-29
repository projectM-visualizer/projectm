#pragma once

namespace libprojectM {
namespace MilkdropPreset {

enum class WaveformMode : int
{
    Circle = 0,             //!< Circular wave.
    XYOscillationSpiral,    //!< X-Y osc. that goes around in a spiral, in time.
    CenteredSpiro,          //!< Centered spiro (alpha constant). Aimed at not being so sound-responsive, but being very "nebula-like".
    CenteredSpiroVolume,    //!< Centered spiro (alpha tied to volume). Aimed at having a strong audio-visual tie-in.
    DerivativeLine,         //!< Horizontal "script", left channel.
    ExplosiveHash,          //!< Weird explosive complex # thingy.
    Line,                   //!< Angle-adjustable left channel, with temporal wave alignment.
    DoubleLine,             //!< Same, except there are two channels shown.
    SpectrumLine,           //!< Same as "Line", except using the spectrum analyzer (UNFINISHED).

    Count                   //!< Always keep as last item! Used to wrap the mode value if it exceeds this value.
};

} // namespace MilkdropPreset
} // namespace libprojectM
