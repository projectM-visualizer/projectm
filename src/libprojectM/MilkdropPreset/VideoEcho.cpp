#include "VideoEcho.hpp"

namespace libprojectM {
namespace MilkdropPreset {

VideoEcho::VideoEcho(const PresetState& presetState)
    : m_presetState(presetState)
    , m_echoMesh(Renderer::VertexBufferUsage::DynamicDraw, true, true)
{
    m_echoMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::TriangleStrip);
    m_echoMesh.SetVertexCount(4);
}

void VideoEcho::Draw()
{
    float const aspect = m_presetState.renderContext.viewportSizeX / static_cast<float>(m_presetState.renderContext.viewportSizeY * m_presetState.renderContext.invAspectY);
    float aspectMultX = 1.0f;
    float aspectMultY = 1.0f;

    if (aspect > 1)
    {
        aspectMultY = aspect;
    }
    else
    {
        aspectMultX = 1.0f / aspect;
    }

    float const fOnePlusInvWidth = 1.0f + 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
    float const fOnePlusInvHeight = 1.0f + 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeY);
    m_echoMesh.Vertices().Set({{-fOnePlusInvWidth * aspectMultX, fOnePlusInvHeight * aspectMultY},
                               {fOnePlusInvWidth * aspectMultX, fOnePlusInvHeight * aspectMultY},
                               {-fOnePlusInvWidth * aspectMultX, -fOnePlusInvHeight * aspectMultY},
                               {fOnePlusInvWidth * aspectMultX, -fOnePlusInvHeight * aspectMultY}});

    auto& colors = m_echoMesh.Colors();
    for (int i = 0; i < 4; i++)
    {
        auto const indexFloat = static_cast<float>(i);
        m_shade[i][0] = 0.6f + 0.3f * sinf(m_presetState.renderContext.time * 30.0f * 0.0143f + 3 + indexFloat * 21 + m_presetState.hueRandomOffsets[3]);
        m_shade[i][1] = 0.6f + 0.3f * sinf(m_presetState.renderContext.time * 30.0f * 0.0107f + 1 + indexFloat * 13 + m_presetState.hueRandomOffsets[1]);
        m_shade[i][2] = 0.6f + 0.3f * sinf(m_presetState.renderContext.time * 30.0f * 0.0129f + 6 + indexFloat * 9 + m_presetState.hueRandomOffsets[2]);

        float const max = std::max(m_shade[i][0], std::max(m_shade[i][1], m_shade[i][2]));

        for (int k = 0; k < 3; k++)
        {
            m_shade[i][k] /= max;
            m_shade[i][k] = 0.5f + 0.5f * m_shade[i][k];
        }

        colors[i] = {m_shade[i][0],
                     m_shade[i][1],
                     m_shade[i][2],
                     1.0f};
    }

    auto shader = m_presetState.texturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    shader->SetUniformInt("texture_sampler", 0);

    auto mainTexture = m_presetState.mainTexture.lock();
    if (mainTexture)
    {
        mainTexture->Bind(0);
        m_sampler.Bind(0);
    }

    if (m_presetState.videoEchoAlpha > 0.001f)
    {
        DrawVideoEcho();
    }
    else
    {
        DrawGammaAdjustment();
    }

    glDisable(GL_BLEND);

    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();

    if (mainTexture)
    {
        mainTexture->Unbind(0);
        Renderer::Sampler::Unbind(0);
    }
}

void VideoEcho::DrawVideoEcho()
{
    auto const videoEchoZoom = m_presetState.videoEchoZoom;
    auto const videoEchoAlpha = m_presetState.videoEchoAlpha;
    auto const videoEchoOrientation = m_presetState.videoEchoOrientation % 4;
    auto const gammaAdj = m_presetState.gammaAdj;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    for (int pass = 0; pass < 2; pass++)
    {
        float const zoom = (pass == 0) ? 1.0f : videoEchoZoom;

        float const tempLow = 0.5f - 0.5f / zoom;
        float const tempHigh = 0.5f + 0.5f / zoom;

        m_echoMesh.UVs().Set({{tempLow, tempLow},
                              {tempHigh, tempLow},
                              {tempLow, tempHigh},
                              {tempHigh, tempHigh}});

        // Flipping
        if (pass == 1)
        {
            for (int vertex = 0; vertex < 4; vertex++)
            {
                if (videoEchoOrientation % 2 == 1)
                {
                    m_echoMesh.UVs()[vertex].SetU(1.0f - m_echoMesh.UVs()[vertex].U());
                }
                if (videoEchoOrientation >= 2)
                {
                    m_echoMesh.UVs()[vertex].SetV(1.0f - m_echoMesh.UVs()[vertex].V());
                }
            }
        }

        float const mix = (pass == 1) ? videoEchoAlpha : 1.0f - videoEchoAlpha;
        for (int vertex = 0; vertex < 4; vertex++)
        {
            m_echoMesh.Colors()[vertex] = {
                mix * m_shade[vertex][0],
                mix * m_shade[vertex][1],
                mix * m_shade[vertex][2],
                1.0f};
        }

        m_echoMesh.Update();
        m_echoMesh.Draw();

        if (pass == 0)
        {
            glBlendFunc(GL_ONE, GL_ONE);
        }

        if (gammaAdj > 0.001f)
        {
            int const redrawCount = static_cast<int>(gammaAdj - 0.0001f);

            for (int redraw = 0; redraw < redrawCount; redraw++)
            {
                float gamma;
                if (redraw == redrawCount - 1)
                {
                    gamma = gammaAdj - static_cast<float>(static_cast<int>(gammaAdj - 0.0001f));
                }
                else
                {
                    gamma = 1.0f;
                }

                for (int vertex = 0; vertex < 4; vertex++)
                {
                    m_echoMesh.Colors()[vertex] = {
                        gamma * mix * m_shade[vertex][0],
                        gamma * mix * m_shade[vertex][1],
                        gamma * mix * m_shade[vertex][2],
                        1.0f};
                }

                m_echoMesh.Colors().Update();
                m_echoMesh.Draw();
            }
        }
    }
}

void VideoEcho::DrawGammaAdjustment()
{
    m_echoMesh.UVs().Set({{0.0f, 0.0f},
                          {1.0f, 0.0f},
                          {0.0f, 1.0f},
                          {1.0f, 1.0f}});

    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    auto const gammaAdj = m_presetState.gammaAdj;
    int const redrawCount = static_cast<int>(gammaAdj - 0.0001f) + 1;

    for (int redraw = 0; redraw < redrawCount; redraw++)
    {
        float gamma;
        if (redraw == redrawCount - 1)
        {
            gamma = gammaAdj - static_cast<float>(redraw);
        }
        else
        {
            gamma = 1.0f;
        }

        for (int vertex = 0; vertex < 4; vertex++)
        {
            m_echoMesh.Colors()[vertex] = {
                gamma * m_shade[vertex][0],
                gamma * m_shade[vertex][1],
                gamma * m_shade[vertex][2],
                1.0f};
        }

        m_echoMesh.Update();
        m_echoMesh.Draw();

        if (redraw == 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }
}

} // namespace MilkdropPreset
} // namespace libprojectM
