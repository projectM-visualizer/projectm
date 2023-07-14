#include "projectM-opengl.h"
#include <algorithm>
#include <cmath>
#include "BeatDetect.hpp"
#include "RenderItemVisitor.hpp"
#include "Waveform.hpp"
#include <glm/gtc/type_ptr.hpp>


#ifdef _WIN32
#include <functional>
#endif /** _WIN32 */

void RenderItemVisitor::drawWaveform(RenderContext& context, Waveform& waveform)
{
    // scale PCM data based on vol_history to make it more or less independent of the application output volume
    const float vol_scale = context.beatDetect->GetPCMScale();

	// Make sure samples<=points.size().  We could reallocate points, but 512 is probably big enough.
    int sampleCount{ std::min(waveform.samples, static_cast<int>(waveform.points.size())) };

    float pcmL[512]{ 0.0f };
    float pcmR[512]{ 0.0f };

    if (waveform.spectrum)
    {
        context.beatDetect->pcm.GetSpectrum(pcmL, CHANNEL_0, sampleCount);
        context.beatDetect->pcm.GetSpectrum(pcmR, CHANNEL_1, sampleCount);
    }
    else
    {
        context.beatDetect->pcm.GetPcm(pcmL, CHANNEL_0, sampleCount);
        context.beatDetect->pcm.GetPcm(pcmR, CHANNEL_1, sampleCount);
    }

    const float mult = waveform.scaling * vol_scale * (waveform.spectrum ? 0.05f : 1.0f);

    // PCM data smoothing
    int offset1 = waveform.spectrum ? 0 : (512 - sampleCount) / 2 - waveform.sep / 2;
    int offset2 = waveform.spectrum ? 0 : (512 - sampleCount) / 2 + waveform.sep / 2;
    int t = waveform.spectrum ? (512 - waveform.sep) / static_cast<float>(sampleCount) : 1;
    float mix1 = std::pow(waveform.smoothing * 0.98f, 0.5f);
    float mix2 = 1.0f - mix1;

    float value1[512]{ 0.0f };
    float value2[512]{ 0.0f };

    value1[0] = pcmL[offset1];
    value2[0] = pcmR[offset2];

    for (int j = 0; j < sampleCount; j++)
    {
        value1[j] = pcmL[static_cast<int>(j * t) + offset1] * mix2 + value1[j - 1] * mix1;
        value2[j] = pcmR[static_cast<int>(j * t) + offset2] * mix2 + value2[j - 1] * mix1;
    }

    for (int j = sampleCount - 2; j >= 0; j--)
    {
        value1[j] = value1[j] * mix2 + value1[j + 1] * mix1;
        value2[j] = value2[j] * mix2 + value2[j + 1] * mix1;
    }

	Waveform::Context waveContext(sampleCount, context.beatDetect);
    std::vector<Waveform::ColoredPoint> pointsTransformed(sampleCount);

    for (int x = 0; x < sampleCount; x++)
    {
        waveContext.sample = static_cast<float>(x) / static_cast<float>(sampleCount - 1);
        waveContext.sample_int = x;
        waveContext.left = value1[x] * mult;
        waveContext.right = value2[x] * mult;

        pointsTransformed[x] = waveform.PerPoint(waveform.points[x], waveContext);
    }

    for (auto& point : pointsTransformed)
    {
        point.y = 1.0f - point.y;
        point.a *= waveform.masterAlpha;
    }

    std::vector<Waveform::ColoredPoint> pointsSmoothed(sampleCount * 2);
    auto smoothedVertexCount = waveform.SmoothWave(pointsTransformed.data(), sampleCount, pointsSmoothed.data());

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

	if (waveform.additive)  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if USE_GLES == 0
    glDisable(GL_LINE_SMOOTH);
#endif
    glLineWidth(1);

    // Additive wave drawing (vice overwrite)
    if (waveform.additive == 1)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Always draw "thick" dots.
    auto iterations = waveform.thick || waveform.dots ? 4 : 1;

    // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
    auto incrementX = 1.0f / static_cast<float>(context.viewportSizeX);
    auto incrementY = 1.0f / static_cast<float>(context.viewportSizeY);

    GLuint drawType = waveform.dots ? GL_POINTS :  GL_LINE_STRIP;

    glBindVertexArray(waveform.m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, waveform.m_vboID);

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

        glBufferData(GL_ARRAY_BUFFER, sizeof(Waveform::ColoredPoint) * smoothedVertexCount, &pointsSmoothed[0], GL_DYNAMIC_DRAW);
        glDrawArrays(drawType, 0, smoothedVertexCount);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
