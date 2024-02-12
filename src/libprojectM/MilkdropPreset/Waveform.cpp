#include "Waveform.hpp"

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "Waveforms/Factory.hpp"

#include <projectM-opengl.h>

#include <Audio/AudioConstants.hpp>

#include <algorithm>
#include <cmath>

using libprojectM::Renderer::RenderItem;

namespace libprojectM {
namespace MilkdropPreset {

Waveform::Waveform(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void Waveform::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    std::vector<Point> vertexData;
    vertexData.resize(std::max(libprojectM::Audio::SpectrumSamples, libprojectM::Audio::WaveformSamples) * 2 + 2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * vertexData.size(), vertexData.data(), GL_STREAM_DRAW);
}

void Waveform::Draw(const PerFrameContext& presetPerFrameContext)
{
    m_mode = static_cast<WaveformMode>(m_presetState.waveMode % static_cast<int>(WaveformMode::Count));

    if (!m_waveformMath)
    {
        m_waveformMath = Waveforms::Factory::Create(m_mode);
        if (!m_waveformMath)
        {
            return;
        }
    }

#ifndef USE_GLES
    glDisable(GL_LINE_SMOOTH);
#endif
    glLineWidth(1);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    // Additive wave drawing (vice overwrite)
    glEnable(GL_BLEND);
    if (m_presetState.additiveWaves)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    auto smoothedVertices = m_waveformMath->GetVertices(m_presetState, presetPerFrameContext);

    for (auto& smoothedWave : smoothedVertices)
    {
        if (smoothedWave.empty())
        {
            continue;
        }

        m_tempAlpha = static_cast<float>(*presetPerFrameContext.wave_a);
        MaximizeColors(presetPerFrameContext);

        // Always draw "thick" dots.
        const auto iterations = m_presetState.waveThick || m_presetState.waveDots ? 4 : 1;

        const auto incrementX = 2.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
        const auto incrementY = 2.0f / static_cast<float>(m_presetState.renderContext.viewportSizeY);

        GLuint drawType = m_presetState.waveDots ? GL_POINTS : (m_waveformMath->IsLoop() ? GL_LINE_LOOP : GL_LINE_STRIP);

        // If thick outline is used, draw the shape four times with slight offsets
        // (top left, top right, bottom right, bottom left).
        for (auto iteration = 0; iteration < iterations; iteration++)
        {
            switch (iteration)
            {
                case 0:
                    break;

                case 1:
                    for (auto j = 0U; j < smoothedWave.size(); j++)
                    {
                        smoothedWave[j].x += incrementX;
                    }
                    break;

                case 2:
                    for (auto j = 0U; j < smoothedWave.size(); j++)
                    {
                        smoothedWave[j].y += incrementY;
                    }
                    break;

                case 3:
                    for (auto j = 0U; j < smoothedWave.size(); j++)
                    {
                        smoothedWave[j].x -= incrementX;
                    }
                    break;
            }

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Point) * smoothedWave.size(), smoothedWave.data());
            glDrawArrays(drawType, 0, static_cast<GLsizei>(smoothedWave.size()));
        }
    }

    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Renderer::Shader::Unbind();
}

void Waveform::ModulateOpacityByVolume(const PerFrameContext& presetPerFrameContext)
{
    //modulate volume by opacity
    //
    //set an upper and lower bound and linearly
    //calculate the opacity from 0=lower to 1=upper
    //based on current volume
    if (m_presetState.audioData.vol <= m_presetState.modWaveAlphaStart)
    {
        m_tempAlpha = 0.0;
    }
    else if (m_presetState.audioData.vol >= m_presetState.modWaveAlphaEnd)
    {
        m_tempAlpha = static_cast<float>(*presetPerFrameContext.wave_a);
    }
    else
    {
        m_tempAlpha = static_cast<float>(*presetPerFrameContext.wave_a) * ((m_presetState.audioData.vol - m_presetState.modWaveAlphaStart) / (m_presetState.modWaveAlphaEnd - m_presetState.modWaveAlphaStart));
    }
}

void Waveform::MaximizeColors(const PerFrameContext& presetPerFrameContext)
{
    //wave color brightening
    //
    //forces max color value to 1.0 and scales
    // the rest accordingly
    int texsize = std::max(m_presetState.renderContext.viewportSizeX, m_presetState.renderContext.viewportSizeY);

    if (m_mode == WaveformMode::CenteredSpiro || m_mode == WaveformMode::ExplosiveHash)
    {
        if (texsize <= 256)
        {
            m_tempAlpha *= 0.07f;
        }
        else if (texsize <= 512)
        {
            m_tempAlpha *= 0.09f;
        }
        else if (texsize <= 1024)
        {
            m_tempAlpha *= 0.11f;
        }
        else if (texsize <= 2048)
        {
            m_tempAlpha *= 0.13f;
        }
        else
        {
            // For larger screens, e.g. 4K
            m_tempAlpha *= 0.15f;
        }
    }
    else if (m_mode == WaveformMode::CenteredSpiroVolume)
    {
        if (texsize <= 256)
        {
            m_tempAlpha *= 0.075f;
        }
        else if (texsize <= 512)
        {
            m_tempAlpha *= 0.15f;
        }
        else if (texsize <= 1024)
        {
            m_tempAlpha *= 0.22f;
        }
        else if (texsize <= 2048)
        {
            m_tempAlpha *= 0.33f;
        }
        else
        {
            // For larger screens, e.g. 4K
            m_tempAlpha *= 0.44f;
        }
        m_tempAlpha *= 1.3f;
        m_tempAlpha *= std::pow(m_presetState.audioData.treb, 2.0f);
    }

    if (m_presetState.modWaveAlphaByvolume)
    {
        ModulateOpacityByVolume(presetPerFrameContext);
    }

    if (m_tempAlpha < 0.0f)
    {
        m_tempAlpha = 0.0f;
    }

    if (m_tempAlpha > 1.0f)
    {
        m_tempAlpha = 1.0f;
    }

    float waveR{static_cast<float>(*presetPerFrameContext.wave_r)};
    float waveG{static_cast<float>(*presetPerFrameContext.wave_g)};
    float waveB{static_cast<float>(*presetPerFrameContext.wave_b)};

    if (*presetPerFrameContext.wave_brighten > 0)
    {
        constexpr float fMaximizeWaveColorAmount = 1.0f;

        float max = waveR;
        if (max < waveG)
        {
            max = waveG;
        }
        if (max < waveB)
        {
            max = waveB;
        }
        if (max > 0.01f)
        {
            waveR = waveR / max * fMaximizeWaveColorAmount + waveR * (1.0f - fMaximizeWaveColorAmount);
            waveG = waveG / max * fMaximizeWaveColorAmount + waveG * (1.0f - fMaximizeWaveColorAmount);
            waveB = waveB / max * fMaximizeWaveColorAmount + waveB * (1.0f - fMaximizeWaveColorAmount);
        }
    }

    glVertexAttrib4f(1, waveR, waveG, waveB, m_tempAlpha);
}

} // namespace MilkdropPreset
} // namespace libprojectM
