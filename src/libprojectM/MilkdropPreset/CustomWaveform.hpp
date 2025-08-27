#pragma once

#include "WaveformPerFrameContext.hpp"
#include "WaveformPerPointContext.hpp"

#include <Renderer/RenderItem.hpp>

#include <vector>

namespace libprojectM {

class PresetFileParser;

namespace MilkdropPreset {

class CustomWaveform : public Renderer::RenderItem
{
public:

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

    int m_index{0};
    int m_enabled{0};
    int m_samples{WaveformMaxPoints};
    int m_sep{0};
    float m_scaling{1.0f};
    float m_smoothing{0.5f};
    float m_x{0.5f};
    float m_y{0.5f};
    float m_r{1.0f};
    float m_g{1.0f};
    float m_b{1.0f};
    float m_a{1.0f};
    bool m_spectrum{false};
    bool m_useDots{false};
    bool m_drawThick{false};
    bool m_additive{false};

    PRJM_EVAL_F m_tValuesAfterInitCode[TVarCount]{};

    PresetState& m_presetState;
    WaveformPerFrameContext m_perFrameContext;
    WaveformPerPointContext m_perPointContext;

    std::vector<ColoredPoint> m_points;

    friend class WaveformPerFrameContext;
    friend class WaveformPerPointContext;
};

}
}
