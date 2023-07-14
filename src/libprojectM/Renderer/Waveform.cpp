
/*
 * Waveform.hpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */

#include "projectM-opengl.h"
#include "Waveform.hpp"
#include <algorithm>
#include <cmath>
#include "BeatDetect.hpp"
#include "ShaderEngine.hpp"
#include <glm/gtc/type_ptr.hpp>
#ifdef WIN32
#include <functional>
#endif /** WIN32 */

typedef float floatPair[2];

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

void Waveform::DrawVisit(RenderContext& context, RenderItemVisitor& visitor)
{
   visitor.drawWaveform(context,*this);
}

void Waveform::Draw(RenderContext& context)
{
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