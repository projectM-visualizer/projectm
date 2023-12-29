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
    Milkdrop2077Wave9,      //!< Additional Milkdrop2077 waveform. Same as "Line", but 4x larger amplitude.
    Milkdrop2077WaveX,      //!< Additional Milkdrop2077 waveform. X shape.
    Milkdrop2077Wave11,     //!< Additional Milkdrop2077 waveform.
    Milkdrop2077WaveSkewed, //!< Additional Milkdrop2077 waveform, skewed shape.
    Milkdrop2077WaveStar,   //!< Additional Milkdrop2077 waveform, star shape.
    Milkdrop2077WaveFlower, //!< Additional Milkdrop2077 waveform, flower shape.
    Milkdrop2077WaveLasso,  //!< Additional Milkdrop2077 waveform, lassoshape.

    Count                   //!< Always keep as last item! Used to wrap the mode value if it exceeds this value.
};

} // namespace MilkdropPreset
} // namespace libprojectM
