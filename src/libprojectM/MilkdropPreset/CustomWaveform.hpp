#pragma once

#include "WaveformPerFrameContext.hpp"
#include "WaveformPerPointContext.hpp"

#include <Renderer/Color.hpp>
#include <Renderer/Mesh.hpp>
#include "Renderer/Point.hpp"

#include <vector>

namespace libprojectM {

class PresetFileParser;

namespace MilkdropPreset {

class CustomWaveform
{
public:

    /**
     * @brief Creates a new waveform with the given number of samples.
     * @param presetState The preset state container.
     */
    explicit CustomWaveform(PresetState& presetState);

    /**
     * @brief Loads the initial values and code from the preset file.
     * @param parsedFile The file parser with the preset data.
     * @param index The waveform index.
     */
    void Initialize(::libprojectM::PresetFileParser& parsedFile, int index);

    /**
     * @brief Compiles all code blocks and runs the init expression.
     * @throws MilkdropCompileException Thrown if one of the code blocks couldn't be compiled.
     * @param presetPerFrameContext The per-frame context to retrieve the init Q vars from.
     */
    void CompileCodeAndRunInitExpressions(const PerFrameContext& presetPerFrameContext);

    /**
     * @brief Renders the waveform.
     * @param presetPerFrameContext The per-frame context to retrieve the init Q vars from.
     */
    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    /**
     * @brief Initializes the per-frame context with the preset per-frame state.
     * @param presetPerFrameContext The preset per-frame context to pull q vars from.
     */
    void LoadPerFrameEvaluationVariables(const PerFrameContext& presetPerFrameContext);

    /**
     * @brief Loads the Q and T variables from the per-frame code into the per-point context.
     */
    void InitPerPointEvaluationVariables();

    /**
     * @brief Loads the variables for each point into the per-point evaluation context.
     * @param sample The sample index being rendered.
     * @param value1 The left channel value.
     * @param value2 The right channel value.
     */
    void LoadPerPointEvaluationVariables(float sample, float value1, float value2);

    /**
     * @brief Does a better-than-linear smooth on a wave.
     *
     * Roughly doubles the number of points.
     *
     * @param points A vector of points to be smoothed.
     * @param colors A vector of colors for the points.
     */
    void SmoothWave(const std::vector<Renderer::Point>& points, const std::vector<Renderer::Color>& colors);

    int m_index{0}; //!< Custom waveform index in the preset.
    bool m_enabled{false}; //!< Render waveform if non-zero.
    int m_samples{WaveformMaxPoints}; //!< Number of samples/vertices in the waveform.
    int m_sep{0}; //!< Separation distance of dual waveforms.
    float m_scaling{1.0f}; //!< Scale factor of waveform.
    float m_smoothing{0.5f}; //!< Smooth factor of waveform.
    float m_x{0.5f};
    float m_y{0.5f};
    float m_r{1.0f};
    float m_g{1.0f};
    float m_b{1.0f};
    float m_a{1.0f};
    bool m_spectrum{false}; //!< Spectrum data or PCM data.
    bool m_useDots{false}; //!< If non-zero, draw wave as dots instead of lines.
    bool m_drawThick{false}; //!< Draw thicker lines.
    bool m_additive{false}; //!< Add color values together.

    PRJM_EVAL_F m_tValuesAfterInitCode[TVarCount]{};

    PresetState& m_presetState; //!< The global preset state.
    WaveformPerFrameContext m_perFrameContext; //!< Holds the code execution context for per-frame expressions
    WaveformPerPointContext m_perPointContext; //!< Holds the code execution context for per-point expressions

    Renderer::Mesh m_mesh; //!< Points in this waveform.

    friend class WaveformPerFrameContext;
    friend class WaveformPerPointContext;
};

} // namespace MilkdropPreset
} // namespace libprojectM
