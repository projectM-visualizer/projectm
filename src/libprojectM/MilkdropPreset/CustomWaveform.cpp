#include "CustomWaveform.hpp"

#include "PerFrameContext.hpp"
#include "PresetFileParser.hpp"
#include "projectM-opengl.h"

#include <glm/gtc/type_ptr.hpp>

#ifdef _WIN32
#include <functional>
#endif /** _WIN32 */
#include <algorithm>
#include <cmath>

static constexpr int CustomWaveformMaxSamples = std::max(RenderWaveformSamples, libprojectM::Audio::SpectrumSamples);

CustomWaveform::CustomWaveform(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
    , m_perFrameContext(presetState.globalMemory, &presetState.globalRegisters)
    , m_perPointContext(presetState.globalMemory, &presetState.globalRegisters)
{
    RenderItem::Init();

    m_perFrameContext.RegisterBuiltinVariables();
    m_perPointContext.RegisterBuiltinVariables();
}

void CustomWaveform::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), nullptr);                                    // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), reinterpret_cast<void*>(sizeof(float) * 2)); // colors
}

void CustomWaveform::Initialize(PresetFileParser& parsedFile, int index)
{
    std::string const wavecodePrefix = "wavecode_" + std::to_string(index) + "_";
    std::string const wavePrefix = "wave_" + std::to_string(index) + "_";

    m_index = index;
    m_enabled = parsedFile.GetInt(wavecodePrefix + "enabled", m_enabled);
    m_samples = parsedFile.GetInt(wavecodePrefix + "samples", m_samples);
    m_sep = parsedFile.GetInt(wavecodePrefix + "sep", m_sep);
    m_spectrum = parsedFile.GetBool(wavecodePrefix + "bSpectrum", m_spectrum);
    m_useDots = parsedFile.GetBool(wavecodePrefix + "bUseDots", m_useDots);
    m_drawThick = parsedFile.GetBool(wavecodePrefix + "bDrawThick", m_drawThick);
    m_additive = parsedFile.GetBool(wavecodePrefix + "bAdditive", m_additive);
    m_scaling = parsedFile.GetFloat(wavecodePrefix + "scaling", m_scaling);
    m_smoothing = parsedFile.GetFloat(wavecodePrefix + "smoothing", m_smoothing);
    m_r = parsedFile.GetFloat(wavecodePrefix + "r", m_r);
    m_g = parsedFile.GetFloat(wavecodePrefix + "g", m_g);
    m_b = parsedFile.GetFloat(wavecodePrefix + "b", m_b);
    m_a = parsedFile.GetFloat(wavecodePrefix + "a", m_a);

}

void CustomWaveform::CompileCodeAndRunInitExpressions(const PerFrameContext& presetPerFrameContext)
{
    m_perFrameContext.LoadStateVariables(m_presetState, presetPerFrameContext, *this);
    m_perFrameContext.EvaluateInitCode(m_presetState.customWaveInitCode[m_index], *this);

    for (int t = 0; t < TVarCount; t++)
    {
        m_tValuesAfterInitCode[t] = *m_perFrameContext.t_vars[t];
    }

    m_perFrameContext.CompilePerFrameCode(m_presetState.customWavePerFrameCode[m_index], *this);
    m_perPointContext.CompilePerPointCode(m_presetState.customWavePerPointCode[m_index], *this);
}

void CustomWaveform::Draw(const PerFrameContext& presetPerFrameContext)
{
    // Some safety assertions if someone plays around and changes the values in the wrong way.
    static_assert(WaveformMaxPoints <= libprojectM::Audio::SpectrumSamples, "CustomWaveformMaxPoints is larger than SpectrumSamples");
    static_assert(WaveformMaxPoints <= libprojectM::Audio::WaveformSamples, "CustomWaveformMaxPoints is larger than WaveformSamples");
    static_assert(RenderWaveformSamples <= WaveformMaxPoints, "CustomWaveformSamples is larger than CustomWaveformMaxPoints");

    if (!m_enabled)
    {
        return;
    }

    int const maxSampleCount{m_spectrum ? libprojectM::Audio::SpectrumSamples : RenderWaveformSamples};

    // Initialize and execute per-frame code
    LoadPerFrameEvaluationVariables(presetPerFrameContext);
    m_perFrameContext.ExecutePerFrameCode();

    // Copy Q and T vars to per-point context
    InitPerPointEvaluationVariables();

    int sampleCount = std::min(WaveformMaxPoints, static_cast<int>(*m_perFrameContext.samples));

    // If there aren't enough samples to draw a single line or dot, skip drawing the waveform.
    if ((m_useDots && sampleCount < 1) || sampleCount < 2)
    {
        return;
    }

    const auto* pcmL = m_spectrum
                           ? m_presetState.audioData.spectrumLeft.data()
                           : m_presetState.audioData.waveformLeft.data();
    const auto* pcmR = m_spectrum
                           ? m_presetState.audioData.spectrumRight.data()
                           : m_presetState.audioData.waveformRight.data();

    //const float mult = m_scaling * m_presetState.waveScale * (m_spectrum ? 0.15f : 0.004f);
    const float mult = m_scaling * m_presetState.waveScale * (m_spectrum ? 0.05f : 1.0f);

    // PCM data smoothing
    const int offset1 = m_spectrum ? 0 : (maxSampleCount - sampleCount) / 2 - m_sep / 2;
    const int offset2 = m_spectrum ? 0 : (maxSampleCount - sampleCount) / 2 + m_sep / 2;
    const int t = m_spectrum ? static_cast<int>(static_cast<float>(maxSampleCount - m_sep) / static_cast<float>(sampleCount)) : 1;
    const float mix1 = std::pow(m_smoothing * 0.98f, 0.5f);
    const float mix2 = 1.0f - mix1;

    std::array<float, CustomWaveformMaxSamples> sampleDataL{};
    std::array<float, CustomWaveformMaxSamples> sampleDataR{};

    sampleDataL[0] = pcmL[offset1];
    sampleDataR[0] = pcmR[offset2];

    // Smooth forward
    for (int sample = 1; sample < sampleCount; sample++)
    {
        sampleDataL[sample] = pcmL[static_cast<int>(sample * t) + offset1] * mix2 + sampleDataL[sample - 1] * mix1;
        sampleDataR[sample] = pcmR[static_cast<int>(sample * t) + offset2] * mix2 + sampleDataR[sample - 1] * mix1;
    }

    // Smooth backwards (this fixes the asymmetry of the beginning & end)
    for (int sample = sampleCount - 2; sample >= 0; sample--)
    {
        sampleDataL[sample] = sampleDataL[sample] * mix2 + sampleDataL[sample + 1] * mix1;
        sampleDataR[sample] = sampleDataR[sample] * mix2 + sampleDataR[sample + 1] * mix1;
    }

    // Scale waveform to final size
    for (int sample = 0; sample < sampleCount; sample++)
    {
        sampleDataL[sample] *= mult;
        sampleDataR[sample] *= mult;
    }

    std::vector<ColoredPoint> pointsTransformed(sampleCount);

    float const sampleMultiplicator = sampleCount > 1 ? 1.0f / static_cast<float>(sampleCount - 1) : 0.0f;
    for (int sample = 0; sample < sampleCount; sample++)
    {
        float const sampleIndex = static_cast<float>(sample) * sampleMultiplicator;
        LoadPerPointEvaluationVariables(sampleIndex, sampleDataL[sample], sampleDataR[sample]);

        m_perPointContext.ExecutePerPointCode();

        pointsTransformed[sample].x = static_cast<float>((*m_perPointContext.x * 2.0 - 1.0) * m_presetState.renderContext.invAspectX);
        pointsTransformed[sample].y = static_cast<float>((*m_perPointContext.y * -2.0 + 1.0) * m_presetState.renderContext.invAspectY);

        pointsTransformed[sample].r = static_cast<float>(*m_perPointContext.r);
        pointsTransformed[sample].g = static_cast<float>(*m_perPointContext.g);
        pointsTransformed[sample].b = static_cast<float>(*m_perPointContext.b);
        pointsTransformed[sample].a = static_cast<float>(*m_perPointContext.a);
    }

    std::vector<ColoredPoint> pointsSmoothed(sampleCount * 2);
    auto smoothedVertexCount = SmoothWave(pointsTransformed.data(), sampleCount, pointsSmoothed.data());

#if USE_GLES == 0
    glDisable(GL_LINE_SMOOTH);
#endif
    glLineWidth(1);

    // Additive wave drawing (vice overwrite)
    glEnable(GL_BLEND);
    if (m_additive)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    auto iterations = (m_drawThick && !m_useDots) ? 4 : 1;

    // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
    auto incrementX = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
    auto incrementY = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);

    GLuint drawType = m_useDots ? GL_POINTS : GL_LINE_STRIP;

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    // If thick outline is used, draw the shape four times with slight offsets
    // (top left, top right, bottom right, bottom left).
    for (auto iteration = 0; iteration < iterations; iteration++)
    {
        switch (iteration)
        {
            case 0:
            default:
                break;

            case 1:
                for (auto j = 0; j < smoothedVertexCount; j++)
                {
                    pointsSmoothed[j].x += incrementX;
                }
                break;

            case 2:
                for (auto j = 0; j < smoothedVertexCount; j++)
                {
                    pointsSmoothed[j].y += incrementY;
                }
                break;

            case 3:
                for (auto j = 0; j < smoothedVertexCount; j++)
                {
                    pointsSmoothed[j].x -= incrementX;
                }
                break;
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(ColoredPoint) * smoothedVertexCount, pointsSmoothed.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(drawType, 0, smoothedVertexCount);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader::Unbind();

    glDisable(GL_BLEND);
}

void CustomWaveform::LoadPerFrameEvaluationVariables(const PerFrameContext& presetPerFrameContext)
{
    m_perFrameContext.LoadStateVariables(m_presetState, presetPerFrameContext, *this);
    m_perPointContext.LoadReadOnlyStateVariables(presetPerFrameContext);
}

void CustomWaveform::InitPerPointEvaluationVariables()
{
    for (int q = 0; q < QVarCount; q++)
    {
        *m_perPointContext.q_vars[q] = *m_perFrameContext.q_vars[q];
    }
    for (int t = 0; t < TVarCount; t++)
    {
        *m_perPointContext.t_vars[t] = *m_perFrameContext.t_vars[t];
    }
}

void CustomWaveform::LoadPerPointEvaluationVariables(float sample, float value1, float value2)
{
    *m_perPointContext.sample = static_cast<double>(sample);
    *m_perPointContext.value1 = static_cast<double>(value1);
    *m_perPointContext.value2 = static_cast<double>(value2);
    *m_perPointContext.x = static_cast<double>(0.5f + value1);
    *m_perPointContext.y = static_cast<double>(0.5f + value2);
    *m_perPointContext.r = *m_perFrameContext.r;
    *m_perPointContext.g = *m_perFrameContext.g;
    *m_perPointContext.b = *m_perFrameContext.b;
    *m_perPointContext.a = *m_perFrameContext.a;
}

int CustomWaveform::SmoothWave(const CustomWaveform::ColoredPoint* inputVertices,
                               int vertexCount,
                               CustomWaveform::ColoredPoint* outputVertices)
{
    constexpr float c1{-0.15f};
    constexpr float c2{1.15f};
    constexpr float c3{1.15f};
    constexpr float c4{-0.15f};
    constexpr float inverseSum{1.0f / (c1 + c2 + c3 + c4)};

    int outputIndex = 0;
    int iBelow = 0;
    int iAbove2 = 1;

    for (auto inputIndex = 0; inputIndex < vertexCount - 1; inputIndex++)
    {
        int const iAbove = iAbove2;
        iAbove2 = std::min(vertexCount - 1, inputIndex + 2);
        outputVertices[outputIndex] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1].x = (c1 * inputVertices[iBelow].x + c2 * inputVertices[inputIndex].x + c3 * inputVertices[iAbove].x + c4 * inputVertices[iAbove2].x) * inverseSum;
        outputVertices[outputIndex + 1].y = (c1 * inputVertices[iBelow].y + c2 * inputVertices[inputIndex].y + c3 * inputVertices[iAbove].y + c4 * inputVertices[iAbove2].y) * inverseSum;
        iBelow = inputIndex;
        outputIndex += 2;
    }

    outputVertices[outputIndex] = inputVertices[vertexCount - 1];

    return outputIndex + 1;
}
