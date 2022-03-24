/*
 * MilkdropWaveform.cpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */
#include "MilkdropWaveform.hpp"

#include "BeatDetect.hpp"
#include "projectM-opengl.h"

#include <glm/gtc/type_ptr.hpp>

#include <cmath>

MilkdropWaveform::MilkdropWaveform()
    : RenderItem()
{
    Init();
}

MilkdropWaveform::~MilkdropWaveform()
{
    delete[] wave1Vertices;
    delete[] wave2Vertices;
}

void MilkdropWaveform::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glDisableVertexAttribArray(1);
}


void MilkdropWaveform::Draw(RenderContext& context)
{
    WaveformVertex smoothedWave[NumWaveformSamples * 2];
    WaveformMath(context);

    // NOTE MilkdropWaveform does not have a "samples" parameter
    // so this member variable is just being set in WaveformMath and used here
    assert(samples <= NumWaveformSamples);

    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1);

    for (int waveIndex = 0; waveIndex < 2; waveIndex++)
    {
        auto* const waveVertices = (waveIndex == 0) ? wave1Vertices : wave2Vertices;
        if (!waveVertices)
        {
            continue;
        }

        // Smoothen the waveform
        // Instead of using the "break" index like Milkdrop, we simply have two separate arrays.
        const auto smoothedSamples = SmoothWave(waveVertices, samples, smoothedWave);

        glUseProgram(context.programID_v2f_c4f);

        temp_a = a;
        if (modulateAlphaByVolume)
        {
            ModulateOpacityByVolume(context);
        }
        MaximizeColors(context);

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
        const auto iterations = thick || dots ? 4 : 1;

        // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
        const auto incrementX = 1.0f / static_cast<float>(context.viewportSizeX);
        const auto incrementY = 1.0f / static_cast<float>(context.viewportSizeY);

        GLuint drawType = dots ? GL_POINTS : (loop ? GL_LINE_LOOP : GL_LINE_STRIP);

        glBindVertexArray(m_vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        // If thick outline is used, draw the shape four times with slight offsets
        // (top left, top right, bottom right, bottom left).
        for (auto iteration = 0; iteration < iterations; iteration++)
        {
            switch (iteration)
            {
                case 0:
                    break;

                case 1:
                    for (auto j = 0; j < smoothedSamples; j++)
                    {
                        smoothedWave[j].x += incrementX;
                    }
                    break;

                case 2:
                    for (auto j = 0; j < smoothedSamples; j++)
                    {
                        smoothedWave[j].y += incrementY;
                    }
                    break;

                case 3:
                    for (auto j = 0; j < smoothedSamples; j++)
                    {
                        smoothedWave[j].x -= incrementX;
                    }
                    break;
            }

            glBufferData(GL_ARRAY_BUFFER, sizeof(WaveformVertex) * smoothedSamples, smoothedWave, GL_DYNAMIC_DRAW);
            glDrawArrays(drawType, 0, smoothedSamples);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glUseProgram(0);
}

void MilkdropWaveform::ModulateOpacityByVolume(RenderContext& context)
{
    //modulate volume by opacity
    //
    //set an upper and lower bound and linearly
    //calculate the opacity from 0=lower to 1=upper
    //based on current volume
    if (context.beatDetect->vol <= modOpacityStart)
    {
        temp_a = 0.0;
    }
    else if (context.beatDetect->vol >= modOpacityEnd)
    {
        temp_a = a;
    }
    else
    {
        temp_a = a * ((context.beatDetect->vol - modOpacityStart) / (modOpacityEnd - modOpacityStart));
    }
}

void MilkdropWaveform::MaximizeColors(RenderContext& context)
{
    //wave color brightening
    //
    //forces max color value to 1.0 and scales
    // the rest accordingly
    if (mode == MilkdropWaveformMode::Blob2 || mode == MilkdropWaveformMode::ExplosiveHash)
    {
        switch (context.texsize)
        {
            case 256:
                temp_a *= 0.07f;
                break;
            case 512:
                temp_a *= 0.09f;
                break;
            case 1024:
                temp_a *= 0.11f;
                break;
            case 2048:
                temp_a *= 0.13f;
                break;
        }
    }
    else if (mode == MilkdropWaveformMode::Blob3)
    {
        switch (context.texsize)
        {
            case 256:
                temp_a *= 0.075f;
                break;
            case 512:
                temp_a *= 0.15f;
                break;
            case 1024:
                temp_a *= 0.22f;
                break;
            case 2048:
                temp_a *= 0.33f;
                break;
        }
        temp_a *= 1.3f;
        temp_a *= std::pow(context.beatDetect->treb, 2.0f);
    }

    if (temp_a < 0.0f)
    {
        temp_a = 0.0f;
    }

    if (temp_a > 1.0f)
    {
        temp_a = 1.0f;
    }

    if (maximizeColors)
    {
        constexpr float fMaximizeWaveColorAmount = 1.0f;
        float cr{ r };
        float cg{ g };
        float cb{ b };

        float max = cr;
        if (max < cg)
        {
            max = cg;
        }
        if (max < cb)
        {
            max = cb;
        }
        if (max > 0.01f)
        {
            cr = cr / max * fMaximizeWaveColorAmount + cr * (1.0f - fMaximizeWaveColorAmount);
            cg = cg / max * fMaximizeWaveColorAmount + cg * (1.0f - fMaximizeWaveColorAmount);
            cb = cb / max * fMaximizeWaveColorAmount + cb * (1.0f - fMaximizeWaveColorAmount);
        }

        glVertexAttrib4f(1, cr, cg, cb, temp_a * masterAlpha);
    }
    else
    {
        glVertexAttrib4f(1, r, g, b, temp_a * masterAlpha);
    }
}


void MilkdropWaveform::WaveformMath(RenderContext& context)
{
    constexpr float PI{ 3.14159274101257324219f };
    constexpr size_t audioSamples{ 512 };

    delete[] wave1Vertices;
    wave1Vertices = nullptr;

    delete[] wave2Vertices;
    wave2Vertices = nullptr;

    // NOTE: Buffer size is always 512 samples, waveform points 480 or less since some waveforms use a positive offset!
    std::array<float, audioSamples> pcmDataL{ 0.0f };
    std::array<float, audioSamples> pcmDataR{ 0.0f };

    if (mode != MilkdropWaveformMode::SpectrumLine)
    {
        context.beatDetect->pcm.GetPcm(pcmDataL.data(), CHANNEL_0, pcmDataL.size());
        context.beatDetect->pcm.GetPcm(pcmDataR.data(), CHANNEL_1, pcmDataL.size());
    }
    else
    {
        context.beatDetect->pcm.GetSpectrum(pcmDataL.data(), CHANNEL_0, pcmDataL.size());
    }

    // tie size of waveform to beatSensitivity
    const float vol_scale = context.beatDetect->getPCMScale();
    if (vol_scale != 1.0)
    {
        for (int i = 0; i < pcmDataL.size(); i++)
        {
            pcmDataL[i] *= vol_scale;
            pcmDataR[i] *= vol_scale;
        }
    }

    // Aspect multipliers
    float aspectX{ 1.0f };
    float aspectY{ 1.0f };

    if (context.viewportSizeX > context.viewportSizeY)
    {
        aspectY = static_cast<float>(context.viewportSizeY) / static_cast<float>(context.viewportSizeX);
    }
    else
    {
        aspectX = static_cast<float>(context.viewportSizeX) / static_cast<float>(context.viewportSizeY);
    }

    loop = false;

    float mysteryWaveParam = mystery;

    if ((mode == MilkdropWaveformMode::Circle
         || mode == MilkdropWaveformMode::XYOscillationSpiral
         || mode == MilkdropWaveformMode::DerivativeLine)
        && (mysteryWaveParam < 1.0f || mysteryWaveParam > 1.0f))
    {
        mysteryWaveParam = mysteryWaveParam * 0.5f + 0.5f;
        mysteryWaveParam -= floorf(mysteryWaveParam);
        mysteryWaveParam = fabsf(mysteryWaveParam);
        mysteryWaveParam = mysteryWaveParam * 2 - 1;
    }

    switch (mode)
    {

        case MilkdropWaveformMode::Circle:
        {
            loop = true;

            samples = NumWaveformSamples / 2;

            wave1Vertices = new WaveformVertex[samples]();

            const int sampleOffset = (NumWaveformSamples - samples) / 2;

            const float inverseSamplesMinusOne = 1.0f / static_cast<float>(samples);

            for (int i = 0; i < samples; i++)
            {
                float radius = 0.5f + 0.4f * pcmDataR[i + sampleOffset] + mysteryWaveParam;
                float angle = static_cast<float>(i) * inverseSamplesMinusOne * 6.28f + context.time * 0.2f;
                if (i < samples / 10)
                {
                    float mix = static_cast<float>(i) / (static_cast<float>(samples) * 0.1f);
                    mix = 0.5f - 0.5f * cosf(mix * 3.1416f);
                    float radius2 = 0.5f + 0.4f * pcmDataR[i + samples + sampleOffset] + mysteryWaveParam;
                    radius = radius2 * (1.0f - mix) + radius * (mix);
                }

                radius *= 0.5f;

                wave1Vertices[i].x = radius * cosf(angle) * aspectY + x;
                wave1Vertices[i].y = radius * sinf(angle) * aspectX + y;
            }
            break;
        }

        case MilkdropWaveformMode::XYOscillationSpiral://circularly moving waveform
        {
            samples = NumWaveformSamples / 2;

            wave1Vertices = new WaveformVertex[samples]();

            for (int i = 0; i < samples; i++)
            {
                float radius = (0.53f + 0.43f * pcmDataR[i] + mysteryWaveParam) * 0.5f;
                float angle = pcmDataL[i + 32] * PI * 0.5f + context.time * 2.3f;

                wave1Vertices[i].x = radius * cosf(angle) * aspectY + x;
                wave1Vertices[i].y = radius * sinf(angle) * aspectX + y;
            }
            break;
        }

        case MilkdropWaveformMode::Blob2:
        case MilkdropWaveformMode::Blob3:
        {         // Both "centered spiro" waveforms are identical. Only difference is the alpha value.
            // Alpha calculation is handled in MaximizeColors().
            samples = NumWaveformSamples;

            wave1Vertices = new WaveformVertex[samples]();

            for (int i = 0; i < samples; i++)
            {
                wave1Vertices[i].x = 0.5f * pcmDataR[i] * aspectY + x;
                wave1Vertices[i].y = 0.5f * pcmDataL[i + 32] * aspectX + y;
            }

            break;
        }

        case MilkdropWaveformMode::DerivativeLine:
        {
            samples = NumWaveformSamples;

            if (samples > context.viewportSizeX / 3)
            {
                samples /= 3;
            }

            wave1Vertices = new WaveformVertex[samples]();

            int sampleOffset = (NumWaveformSamples - samples) / 2;

            const float w1 = 0.45f + 0.5f * (mysteryWaveParam * 0.5f + 0.5f);
            const float w2 = 1.0f - w1;

            const float inverseSamples = 1.0f / static_cast<float>(samples);

            for (int i = 0; i < samples; i++)
            {
                assert((i + 25 + sampleOffset) < 512);
                wave1Vertices[i].x = (-1.0f + 2.0f * (i * inverseSamples)) * 0.5f + x;
                wave1Vertices[i].y = pcmDataL[i + sampleOffset] * 0.235f + y;
                wave1Vertices[i].x += pcmDataR[i + 25 + sampleOffset] * 0.22f;

                // Momentum
                if (i > 1)
                {
                    wave1Vertices[i].x =
                        wave1Vertices[i].x * w2 + w1 * (wave1Vertices[i - 1].x * 2.0f - wave1Vertices[i - 2].x);
                    wave1Vertices[i].y =
                        wave1Vertices[i].y * w2 + w1 * (wave1Vertices[i - 1].y * 2.0f - wave1Vertices[i - 2].y);
                }
            }
            break;
        }

        case MilkdropWaveformMode::ExplosiveHash:
        {
            samples = NumWaveformSamples;

            wave1Vertices = new WaveformVertex[samples]();

            const float cosineRotation = cosf(context.time * 0.3f);
            const float sineRotation = sinf(context.time * 0.3f);

            for (int i = 0; i < samples; i++)
            {
                const float x0 = (pcmDataR[i] * pcmDataL[i + 32] + pcmDataL[i] * pcmDataR[i + 32]);
                const float y0 = (pcmDataR[i] * pcmDataR[i] - pcmDataL[i + 32] * pcmDataL[i + 32]);
                wave1Vertices[i].x = ((x0 * cosineRotation - y0 * sineRotation) * 0.5f * aspectY) + x;
                wave1Vertices[i].y = ((x0 * sineRotation + y0 * cosineRotation) * 0.5f * aspectX) + y;
            }
            break;
        }

        case MilkdropWaveformMode::Line:
        case MilkdropWaveformMode::DoubleLine:
        case MilkdropWaveformMode::SpectrumLine: // Unfinished
        {
            if (mode == MilkdropWaveformMode::SpectrumLine)
            {
                samples = 256;
            }
            else
            {
                samples = NumWaveformSamples / 2;

                if (samples > context.viewportSizeX / 3)
                {
                    samples /= 3;
                }
            }

            wave1Vertices = new WaveformVertex[samples]();
            if (mode == MilkdropWaveformMode::DoubleLine)
            {
                wave2Vertices = new WaveformVertex[samples]();
            }

            const int sampleOffset = (NumWaveformSamples - samples) / 2;

            const float angle = PI * 0.5f * mysteryWaveParam; // from -PI/2 to PI/2
            float dx = cosf(angle);
            float dy = sinf(angle);

            std::array<float, 2> edgeX{
                x * cosf(angle + PI * 0.5f) - dx * 3.0f,
                x * cosf(angle + PI * 0.5f) + dx * 3.0f
            };

            std::array<float, 2> edgeY{
                x * sinf(angle + PI * 0.5f) - dy * 3.0f,
                x * sinf(angle + PI * 0.5f) + dy * 3.0f
            };

            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    float t;
                    bool clip{ false };

                    switch (j)
                    {
                        case 0:
                            if (edgeX[i] > 1.1f)
                            {
                                t = (1.1f - edgeX[1 - i]) / (edgeX[i] - edgeX[1 - i]);
                                clip = true;
                            }
                            break;

                        case 1:
                            if (edgeX[i] < -1.1f)
                            {
                                t = (-1.1f - edgeX[1 - i]) / (edgeX[i] - edgeX[1 - i]);
                                clip = true;
                            }
                            break;

                        case 2:
                            if (edgeY[i] > 1.1f)
                            {
                                t = (1.1f - edgeY[1 - i]) / (edgeY[i] - edgeY[1 - i]);
                                clip = true;
                            }
                            break;

                        case 3:
                            if (edgeY[i] < -1.1f)
                            {
                                t = (-1.1f - edgeY[1 - i]) / (edgeY[i] - edgeY[1 - i]);
                                clip = true;
                            }
                            break;
                    }

                    if (clip)
                    {
                        const float diffX = edgeX[i] - edgeX[1 - i];
                        const float diffY = edgeY[i] - edgeY[1 - i];
                        edgeX[i] = edgeX[1 - i] + diffX * t;
                        edgeY[i] = edgeY[1 - i] + diffY * t;
                    }
                }
            }

            dx = (edgeX[1] - edgeX[0]) / static_cast<float>(samples);
            dy = (edgeY[1] - edgeY[0]) / static_cast<float>(samples);

            const float angle2 = atan2f(dy, dx);
            const float perpetualDX = cosf(angle2 + PI * 0.5f);
            const float perpetualDY = sinf(angle2 + PI * 0.5f);

            if (mode == MilkdropWaveformMode::Line)
            {
                for (int i = 0; i < samples; i++)
                {
                    wave1Vertices[i].x =
                        edgeX[0] + dx * static_cast<float>(i) + perpetualDX * 0.25f * pcmDataL[i + sampleOffset];
                    wave1Vertices[i].y =
                        edgeY[0] + dy * static_cast<float>(i) + perpetualDY * 0.25f * pcmDataL[i + sampleOffset];
                }
            }
            else if (mode == MilkdropWaveformMode::SpectrumLine)
            {
                for (int i = 0; i < samples; i++)
                {
                    const float f = 0.1f * logf(pcmDataL[i * 2] + pcmDataL[i * 2 + 1]);
                    wave1Vertices[i].x = edgeX[0] + dx * static_cast<float>(i) + perpetualDX * f;
                    wave1Vertices[i].y = edgeY[0] + dy * static_cast<float>(i) + perpetualDY * f;
                }
            }
            else
            {
                float separation = powf(y * 0.25f + 0.25f, 2.0f);
                for (int i = 0; i < samples; i++)
                {
                    wave1Vertices[i].x = edgeX[0] + dx * static_cast<float>(i) +
                                         perpetualDX * (0.25f * pcmDataL[i + sampleOffset] + separation);
                    wave1Vertices[i].y = edgeY[0] + dy * static_cast<float>(i) +
                                         perpetualDY * (0.25f * pcmDataL[i + sampleOffset] + separation);

                    wave2Vertices[i].x = edgeX[0] + dx * static_cast<float>(i) +
                                         perpetualDX * (0.25f * pcmDataR[i + sampleOffset] - separation);
                    wave2Vertices[i].y = edgeY[0] + dy * static_cast<float>(i) +
                                         perpetualDY * (0.25f * pcmDataR[i + sampleOffset] - separation);
                }
            }
            break;
        }

        default:
            break;
    }

    // Reverse all Y coordinates to stay consistent with the pre-VMS milkdrop
    for (int i = 0; i < samples; i++)
    {
        if (wave1Vertices)
        {
            wave1Vertices[i].y = 1.0f - wave1Vertices[i].y;
        }
        if (wave2Vertices)
        {
            wave2Vertices[i].y = 1.0f - wave2Vertices[i].y;
        }
    }
}

int MilkdropWaveform::SmoothWave(const MilkdropWaveform::WaveformVertex* inputVertices, int vertexCount,
                                 MilkdropWaveform::WaveformVertex* outputVertices)
{
    constexpr float c1{ -0.15f };
    constexpr float c2{ 1.15f };
    constexpr float c3{ 1.15f };
    constexpr float c4{ -0.15f };
    constexpr float inverseSum{ 1.0f / (c1 + c2 + c3 + c4) };

    int outputIndex = 0;
    int indexBelow = 0;
    int indexAbove2 = 1;

    for (auto inputIndex = 0; inputIndex < vertexCount - 1; inputIndex++)
    {
        int indexAbove = indexAbove2;
        indexAbove2 = std::min(vertexCount - 1, inputIndex + 2);
        outputVertices[outputIndex] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1].x = (c1 * inputVertices[indexBelow].x
                                             + c2 * inputVertices[inputIndex].x
                                             + c3 * inputVertices[indexAbove].x
                                             + c4 * inputVertices[indexAbove2].x) * inverseSum;
        outputVertices[outputIndex + 1].y = (c1 * inputVertices[indexBelow].y
                                             + c2 * inputVertices[inputIndex].y
                                             + c3 * inputVertices[indexAbove].y
                                             + c4 * inputVertices[indexAbove2].y) * inverseSum;
        indexBelow = inputIndex;
        outputIndex += 2;
    }

    outputVertices[outputIndex] = inputVertices[vertexCount - 1];

    return outputIndex + 1;
}
