#pragma once

#include "WaveformPerFrameContext.hpp"
#include "WaveformPerPointContext.hpp"

#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

class PresetFileParser;

namespace MilkdropPreset {

class CustomWaveform : public libprojectM::Renderer::Backend::OpenGL::OpenGLRenderItem
{
public:

    /**
     * @brief Creates a new waveform with the given number of samples.
     * @param presetState The preset state container.
     */
    explicit CustomWaveform(PresetState& presetState);

    void InitVertexAttrib() override;

    void Initialize(::libprojectM::PresetFileParser& parsedFile, int index);

    void CompileCodeAndRunInitExpressions(const PerFrameContext& presetPerFrameContext);

    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    void LoadPerFrameEvaluationVariables(const PerFrameContext& presetPerFrameContext);

    void InitPerPointEvaluationVariables();

    void LoadPerPointEvaluationVariables(float sample, float value1, float value2);

    static int SmoothWave(const ColoredPoint* inputVertices, int vertexCount, ColoredPoint* outputVertices);

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
