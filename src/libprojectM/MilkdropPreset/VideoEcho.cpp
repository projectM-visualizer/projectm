#include "VideoEcho.hpp"

#include <glm/gtc/type_ptr.hpp>

VideoEcho::VideoEcho(const PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

void VideoEcho::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, r))); // Color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, u))); // Texture coordinates
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
    m_vertices[0].x = -fOnePlusInvWidth * aspectMultX;
    m_vertices[1].x = fOnePlusInvWidth * aspectMultX;
    m_vertices[2].x = -fOnePlusInvWidth * aspectMultX;
    m_vertices[3].x = fOnePlusInvWidth * aspectMultX;
    m_vertices[0].y = fOnePlusInvHeight * aspectMultY;
    m_vertices[1].y = fOnePlusInvHeight * aspectMultY;
    m_vertices[2].y = -fOnePlusInvHeight * aspectMultY;
    m_vertices[3].y = -fOnePlusInvHeight * aspectMultY;

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

        m_vertices[i].r = m_shade[i][0];
        m_vertices[i].g = m_shade[i][1];
        m_vertices[i].b = m_shade[i][2];
        m_vertices[i].a = 1.0f;
    }

    m_presetState.texturedShader.Bind();
    m_presetState.texturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    m_presetState.texturedShader.SetUniformInt("texture_sampler", 0);

    auto mainTexture = m_presetState.mainTexture.lock();
    if (mainTexture)
    {
        mainTexture->Bind(0);
        m_sampler.Bind(0);
    }

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    if (m_presetState.videoEchoAlpha > 0.001f)
    {
        DrawVideoEcho();
    }
    else
    {
        DrawGammaAdjustment();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);

    Shader::Unbind();

    if (mainTexture)
    {
        mainTexture->Unbind(0);
        Sampler::Unbind(0);
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
        float const temphigh = 0.5f + 0.5f / zoom;
        m_vertices[0].u = tempLow;
        m_vertices[0].v = tempLow;
        m_vertices[1].u = temphigh;
        m_vertices[1].v = tempLow;
        m_vertices[2].u = tempLow;
        m_vertices[2].v = temphigh;
        m_vertices[3].u = temphigh;
        m_vertices[3].v = temphigh;

        // Flipping
        if (pass == 1)
        {
            for (int vertex = 0; vertex < 4; vertex++)
            {
                if (videoEchoOrientation % 2 == 1)
                {
                    m_vertices[vertex].u = 1.0f - m_vertices[vertex].u;
                }
                if (videoEchoOrientation >= 2)
                {
                    m_vertices[vertex].v = 1.0f - m_vertices[vertex].v;
                }
            }
        }

        float const mix = (pass == 1) ? videoEchoAlpha : 1.0f - videoEchoAlpha;
        for (int vertex = 0; vertex < 4; vertex++)
        {
            m_vertices[vertex].r = mix * m_shade[vertex][0];
            m_vertices[vertex].g = mix * m_shade[vertex][1];
            m_vertices[vertex].b = mix * m_shade[vertex][2];
            m_vertices[vertex].a = 1.0f;
        }

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(TexturedPoint) * m_vertices.size()), m_vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertices.size());

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
                    m_vertices[vertex].r = gamma * mix * m_shade[vertex][0];
                    m_vertices[vertex].g = gamma * mix * m_shade[vertex][1];
                    m_vertices[vertex].b = gamma * mix * m_shade[vertex][2];
                    m_vertices[vertex].a = 1.0f;
                }

                glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(TexturedPoint) * m_vertices.size()), m_vertices.data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_vertices.size()));
            }
        }
    }
}

void VideoEcho::DrawGammaAdjustment()
{
    m_vertices[0].u = 0.0f;
    m_vertices[0].v = 0.0f;
    m_vertices[1].u = 1.0f;
    m_vertices[1].v = 0.0f;
    m_vertices[2].u = 0.0f;
    m_vertices[2].v = 1.0f;
    m_vertices[3].u = 1.0f;
    m_vertices[3].v = 1.0f;

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
            m_vertices[vertex].r = gamma * m_shade[vertex][0];
            m_vertices[vertex].g = gamma * m_shade[vertex][1];
            m_vertices[vertex].b = gamma * m_shade[vertex][2];
            m_vertices[vertex].a = 1.0f;
        }

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(TexturedPoint) * m_vertices.size()), m_vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_vertices.size()));

        if (redraw == 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }
}
