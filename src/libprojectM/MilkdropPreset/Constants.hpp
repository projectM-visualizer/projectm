/**
 * @file Constants.hpp
 * @brief Holds Milkdrop constants like number of Q variables etc.
 */
#pragma once

namespace libprojectM {
namespace MilkdropPreset {

static constexpr int QVarCount = 32; //!< Number of Q variables available.
static constexpr int TVarCount = 8; //!< Number of T variables available.

static constexpr int CustomWaveformCount = 4; //!< Number of custom waveforms (expression-driven) which can be used in a preset.
static constexpr int CustomShapeCount = 4; //!< Number of custom shapes (expression-driven) which can be used in a preset.

static constexpr int WaveformMaxPoints = 512; //!< Maximum number of waveform points.

} // namespace MilkdropPreset
} // namespace libprojectM
