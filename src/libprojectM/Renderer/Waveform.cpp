#include "projectM-opengl.h"
#include "Waveform.hpp"
#include <algorithm>
#include <cmath>
#include "BeatDetect.hpp"

#include <glm/gtc/type_ptr.hpp>

#ifdef WIN32
#include <functional>
#endif /** WIN32 */

Waveform::Waveform(int _samples)
    : RenderItem()
    , samples(_samples)
    , points(_samples)
{
    Init();
}

void Waveform::InitVertexAttrib() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), (void*)0);    // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), (void*)(sizeof(float)*2));    // colors
}

void Waveform::Draw(RenderContext &context)
{
    // scale PCM data based on vol_history to make it more or less independent of the application output volume
    const float vol_scale = context.beatDetect->getPCMScale();

	// Make sure samples<=points.size().  We could reallocate points, but 512 is probably big enough.
    int sampleCount{ std::min(this->samples, static_cast<int>(this->points.size())) };

    float pcmL[512]{ 0.0f };
    float pcmR[512]{ 0.0f };

    if (spectrum)
    {
        context.beatDetect->pcm.GetSpectrum(pcmL, CHANNEL_0, sampleCount);
        context.beatDetect->pcm.GetSpectrum(pcmR, CHANNEL_1, sampleCount);
    }
    else
    {
        context.beatDetect->pcm.GetPcm(pcmL, CHANNEL_0, sampleCount);
        context.beatDetect->pcm.GetPcm(pcmR, CHANNEL_1, sampleCount);
    }

    const float mult = scaling * vol_scale * (spectrum ? 0.05f : 1.0f);

    // PCM data smoothing
    int offset1 = spectrum ? 0 : (512 - sampleCount) / 2 - sep / 2;
    int offset2 = spectrum ? 0 : (512 - sampleCount) / 2 + sep / 2;
    int t = spectrum ? (512 - sep) / static_cast<float>(sampleCount) : 1;
    float mix1 = std::pow(smoothing * 0.98f, 0.5f);
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

	Context waveContext(sampleCount, context.beatDetect);
    std::vector<ColoredPoint> pointsTransformed(sampleCount);

    for (int x = 0; x < sampleCount; x++)
    {
        waveContext.sample = static_cast<float>(x) / static_cast<float>(sampleCount - 1);
        waveContext.sample_int = x;
        waveContext.left = value1[x] * mult;
        waveContext.right = value2[x] * mult;

        pointsTransformed[x] = PerPoint(points[x], waveContext);
    }

    for (auto& point : pointsTransformed)
    {
        point.y = 1.0f - point.y;
        point.a *= masterAlpha;
    }

    std::vector<ColoredPoint> pointsSmoothed(sampleCount * 2);
    auto smoothedVertexCount = SmoothWave(pointsTransformed.data(), sampleCount, pointsSmoothed.data());

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_transformation, 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

	if (additive)  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1);

    // Additive wave drawing (vice overwrite)
    if (additive == 1)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Always draw "thick" dots.
    auto iterations = thick || dots ? 4 : 1;

    // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
    auto incrementX = 1.0f / static_cast<float>(context.viewportSizeX);
    auto incrementY = 1.0f / static_cast<float>(context.viewportSizeY);

    GLuint drawType = dots ? GL_POINTS :  GL_LINE_STRIP;

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

        glBufferData(GL_ARRAY_BUFFER, sizeof(ColoredPoint) * smoothedVertexCount, &pointsSmoothed[0], GL_DYNAMIC_DRAW);
        glDrawArrays(drawType, 0, smoothedVertexCount);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int Waveform::SmoothWave(const Waveform::ColoredPoint* inputVertices, int vertexCount,
                         Waveform::ColoredPoint* outputVertices)
{
    constexpr float c1{ -0.15f };
    constexpr float c2{ 1.15f };
    constexpr float c3{ 1.15f };
    constexpr float c4{ -0.15f };
    constexpr float inverseSum{ 1.0f / (c1 + c2 + c3 + c4) };

    int outputIndex = 0;
    int iBelow = 0;
    int iAbove2 = 1;

    for (auto inputIndex = 0; inputIndex < vertexCount - 1; inputIndex++)
    {
        int iAbove = iAbove2;
        iAbove2 = std::min(vertexCount - 1, inputIndex + 2);
        outputVertices[outputIndex] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1].x = (c1 * inputVertices[iBelow].x
                                             + c2 * inputVertices[inputIndex].x
                                             + c3 * inputVertices[iAbove].x
                                             + c4 * inputVertices[iAbove2].x) * inverseSum;
        outputVertices[outputIndex + 1].y = (c1 * inputVertices[iBelow].y
                                             + c2 * inputVertices[inputIndex].y
                                             + c3 * inputVertices[iAbove].y
                                             + c4 * inputVertices[iAbove2].y) * inverseSum;
        iBelow = inputIndex;
        outputIndex += 2;
    }

    outputVertices[outputIndex] = inputVertices[vertexCount - 1];

    return outputIndex + 1;
}
