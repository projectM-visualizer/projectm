#include "Waveform.hpp"

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "Waveforms/Factory.hpp"

#include <Renderer/BlendMode.hpp>

#include <../Renderer/OpenGL.h>

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {

Waveform::Waveform(PresetState& presetState)
    : m_presetState(presetState)
    , m_waveMesh(Renderer::VertexBufferUsage::StreamDraw)
{
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

    auto shader = m_presetState.untexturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjectionFlipped);
    shader->SetUniformFloat("vertex_point_size", 1.0f);

    // Additive wave drawing (vice overwrite)
    if (m_presetState.additiveWaves)
    {
        Renderer::BlendMode::Set(true, Renderer::BlendMode::Function::SourceAlpha, Renderer::BlendMode::Function::One);
    }
    else
    {
        Renderer::BlendMode::Set(true, Renderer::BlendMode::Function::SourceAlpha, Renderer::BlendMode::Function::OneMinusSourceAlpha);
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

        m_waveMesh.SetRenderPrimitiveType(m_presetState.waveDots
                                              ? Renderer::Mesh::PrimitiveType::Points
                                          : m_waveformMath->IsLoop()
                                              ? Renderer::Mesh::PrimitiveType::LineLoop
                                              : Renderer::Mesh::PrimitiveType::LineStrip);

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
                        smoothedWave[j].SetX(smoothedWave[j].X() + incrementX);
                    }
                    break;

                case 2:
                    for (auto j = 0U; j < smoothedWave.size(); j++)
                    {
                        smoothedWave[j].SetY(smoothedWave[j].Y() + incrementY);
                    }
                    break;

                case 3:
                    for (auto j = 0U; j < smoothedWave.size(); j++)
                    {
                        smoothedWave[j].SetX(smoothedWave[j].X() - incrementX);
                    }
                    break;
            }

            m_waveMesh.Vertices().Set(smoothedWave);
            m_waveMesh.Indices().Resize(smoothedWave.size());
            m_waveMesh.Indices().MakeContinuous();
            m_waveMesh.Update();
            m_waveMesh.Draw();
        }
    }

    Renderer::BlendMode::SetBlendActive(false);;
    Renderer::Mesh::Unbind();
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
