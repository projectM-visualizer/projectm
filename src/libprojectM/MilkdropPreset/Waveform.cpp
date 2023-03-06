#include "Waveform.hpp"

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "Audio/BeatDetect.hpp"
#include "projectM-opengl.h"

#include <glm/gtc/type_ptr.hpp>

#include <cmath>

Waveform::Waveform(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();
}

Waveform::~Waveform()
{
    delete[] m_wave1Vertices;
    delete[] m_wave2Vertices;
}

void Waveform::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void Waveform::Draw(const PerFrameContext& presetPerFrameContext)
{
#if USE_GLES == 0
    glDisable(GL_LINE_SMOOTH);
#endif
    glLineWidth(1);

    m_presetState.untexturedShader.Bind();
    m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", m_presetState.orthogonalProjection);

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

    WaveformMath(presetPerFrameContext);

    std::vector<WaveformVertex> smoothedWave(m_samples * 2);

    for (int waveIndex = 0; waveIndex < 2; waveIndex++)
    {
        auto* const waveVertices = (waveIndex == 0) ? m_wave1Vertices : m_wave2Vertices;
        if (!waveVertices)
        {
            continue;
        }

        // Smoothen the waveform
        // Instead of using the "break" index like Milkdrop, we simply have two separate arrays.
        const auto smoothedSamples = SmoothWave(waveVertices, smoothedWave.data());

        m_tempAlpha = static_cast<float>(*presetPerFrameContext.wave_a);
        if (m_presetState.modWaveAlphaByvolume)
        {
            ModulateOpacityByVolume(presetPerFrameContext);
        }
        MaximizeColors(presetPerFrameContext);

        // Always draw "thick" dots.
        const auto iterations = m_presetState.waveThick || m_presetState.waveDots ? 4 : 1;

        // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
        const auto incrementX = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
        const auto incrementY = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeY);

        GLuint drawType = m_presetState.waveDots ? GL_POINTS : (m_loop ? GL_LINE_LOOP : GL_LINE_STRIP);

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

            glBufferData(GL_ARRAY_BUFFER, sizeof(WaveformVertex) * smoothedSamples, smoothedWave.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(drawType, 0, smoothedSamples);
        }
    }

    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader::Unbind();
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
    if (m_mode == Mode::Blob2 || m_mode == Mode::ExplosiveHash)
    {
        switch (m_presetState.renderContext.texsize)
        {
            case 256:
                m_tempAlpha *= 0.07f;
                break;
            case 512:
                m_tempAlpha *= 0.09f;
                break;
            case 1024:
                m_tempAlpha *= 0.11f;
                break;
            case 2048:
                m_tempAlpha *= 0.13f;
                break;
        }
    }
    else if (m_mode == Mode::Blob3)
    {
        switch (m_presetState.renderContext.texsize)
        {
            case 256:
                m_tempAlpha *= 0.075f;
                break;
            case 512:
                m_tempAlpha *= 0.15f;
                break;
            case 1024:
                m_tempAlpha *= 0.22f;
                break;
            case 2048:
                m_tempAlpha *= 0.33f;
                break;
        }
        m_tempAlpha *= 1.3f;
        m_tempAlpha *= std::pow(m_presetState.audioData.treb, 2.0f);
    }

    if (m_tempAlpha < 0.0f)
    {
        m_tempAlpha = 0.0f;
    }

    if (m_tempAlpha > 1.0f)
    {
        m_tempAlpha = 1.0f;
    }

    if (*presetPerFrameContext.wave_brighten)
    {
        constexpr float fMaximizeWaveColorAmount = 1.0f;
        float cr{static_cast<float>(*presetPerFrameContext.wave_r)};
        float cg{static_cast<float>(*presetPerFrameContext.wave_g)};
        float cb{static_cast<float>(*presetPerFrameContext.wave_b)};

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

        glVertexAttrib4f(1, cr, cg, cb, m_tempAlpha);
    }
    else
    {
        glVertexAttrib4f(1,
                         static_cast<float>(*presetPerFrameContext.wave_r),
                         static_cast<float>(*presetPerFrameContext.wave_g),
                         static_cast<float>(*presetPerFrameContext.wave_b),
                         m_tempAlpha);
    }
}


void Waveform::WaveformMath(const PerFrameContext& presetPerFrameContext)
{
    constexpr float PI{3.14159274101257324219f};
    constexpr size_t audioSamples{512};

    delete[] m_wave1Vertices;
    m_wave1Vertices = nullptr;
    delete[] m_wave2Vertices;
    m_wave2Vertices = nullptr;

    // NOTE: Buffer size is always 512 samples, waveform points 480 or less since some waveforms use a positive offset!
    std::array<float, audioSamples> pcmDataL{0.0f};
    std::array<float, audioSamples> pcmDataR{0.0f};

    m_mode = static_cast<Mode>(m_presetState.waveMode % 9);

    if (m_mode != Mode::SpectrumLine)
    {
        std::copy(begin(m_presetState.audioData.waveformLeft),
                  begin(m_presetState.audioData.waveformLeft) + WaveformMaxPoints,
                  begin(pcmDataL));

        std::copy(begin(m_presetState.audioData.waveformRight),
                  begin(m_presetState.audioData.waveformRight) + WaveformMaxPoints,
                  begin(pcmDataR));
    }
    else
    {
        std::copy(begin(m_presetState.audioData.spectrumLeft),
                  begin(m_presetState.audioData.spectrumLeft) + WaveformMaxPoints,
                  begin(pcmDataL));
    }

    // Tie size of waveform to beatSensitivity
    // ToDo: Beat sensitivity was probably not the correct value here?
    const float volumeScale = m_presetState.audioData.vol;
    if (volumeScale != 1.0)
    {
        for (size_t i = 0; i < pcmDataL.size(); ++i)
        {
            pcmDataL[i] *= volumeScale;
            pcmDataR[i] *= volumeScale;
        }
    }

    // Aspect multipliers
    float aspectX{1.0f};
    float aspectY{1.0f};

    if (m_presetState.renderContext.viewportSizeX > m_presetState.renderContext.viewportSizeY)
    {
        aspectY = static_cast<float>(m_presetState.renderContext.viewportSizeY) / static_cast<float>(m_presetState.renderContext.viewportSizeX);
    }
    else
    {
        aspectX = static_cast<float>(m_presetState.renderContext.viewportSizeX) / static_cast<float>(m_presetState.renderContext.viewportSizeY);
    }

    m_loop = false;

    float mysteryWaveParam = static_cast<float>(*presetPerFrameContext.wave_mystery);

    if ((m_mode == Mode::Circle || m_mode == Mode::XYOscillationSpiral || m_mode == Mode::DerivativeLine) && (mysteryWaveParam < 1.0f || mysteryWaveParam > 1.0f))
    {
        mysteryWaveParam = mysteryWaveParam * 0.5f + 0.5f;
        mysteryWaveParam -= floorf(mysteryWaveParam);
        mysteryWaveParam = fabsf(mysteryWaveParam);
        mysteryWaveParam = mysteryWaveParam * 2 - 1;
    }

    auto const waveX = static_cast<float>(*presetPerFrameContext.wave_x);
    auto const waveY = static_cast<float>(*presetPerFrameContext.wave_y);

    switch (m_mode)
    {

        case Mode::Circle: {
            m_loop = true;

            m_samples = RenderWaveformSamples / 2;

            m_wave1Vertices = new WaveformVertex[m_samples]();

            const int sampleOffset{(RenderWaveformSamples - m_samples) / 2};

            const float inverseSamplesMinusOne{1.0f / static_cast<float>(m_samples)};

            for (int i = 0; i < m_samples; i++)
            {
                float radius = 0.5f + 0.4f * pcmDataR[i + sampleOffset] + mysteryWaveParam;
                float const angle = static_cast<float>(i) * inverseSamplesMinusOne * 6.28f + m_presetState.renderContext.time * 0.2f;
                if (i < m_samples / 10)
                {
                    float mix = static_cast<float>(i) / (static_cast<float>(m_samples) * 0.1f);
                    mix = 0.5f - 0.5f * cosf(mix * 3.1416f);
                    float const radius2 = 0.5f + 0.4f * pcmDataR[i + m_samples + sampleOffset] + mysteryWaveParam;
                    radius = radius2 * (1.0f - mix) + radius * (mix);
                }

                radius *= 0.5f;

                m_wave1Vertices[i].x = radius * cosf(angle) * aspectY + waveX;
                m_wave1Vertices[i].y = radius * sinf(angle) * aspectX + waveY;
            }
            break;
        }

        case Mode::XYOscillationSpiral: //circularly moving waveform
        {
            m_samples = RenderWaveformSamples / 2;

            m_wave1Vertices = new WaveformVertex[m_samples]();

            for (int i = 0; i < m_samples; i++)
            {
                float const radius = (0.53f + 0.43f * pcmDataR[i] + mysteryWaveParam) * 0.5f;
                float const angle = pcmDataL[i + 32] * PI * 0.5f + m_presetState.renderContext.time * 2.3f;

                m_wave1Vertices[i].x = radius * cosf(angle) * aspectY + waveX;
                m_wave1Vertices[i].y = radius * sinf(angle) * aspectX + waveY;
            }
            break;
        }

        case Mode::Blob2:
        case Mode::Blob3: { // Both "centered spiro" waveforms are identical. Only difference is the alpha value.
            // Alpha calculation is handled in MaximizeColors().
            m_samples = RenderWaveformSamples;

            m_wave1Vertices = new WaveformVertex[m_samples]();

            for (int i = 0; i < m_samples; i++)
            {
                m_wave1Vertices[i].x = 0.5f * pcmDataR[i] * aspectY + waveX;
                m_wave1Vertices[i].y = 0.5f * pcmDataL[i + 32] * aspectX + waveY;
            }

            break;
        }

        case Mode::DerivativeLine: {
            m_samples = RenderWaveformSamples;

            if (m_samples > m_presetState.renderContext.viewportSizeX / 3)
            {
                m_samples /= 3;
            }

            m_wave1Vertices = new WaveformVertex[m_samples]();

            int const sampleOffset = (RenderWaveformSamples - m_samples) / 2;

            const float w1 = 0.45f + 0.5f * (mysteryWaveParam * 0.5f + 0.5f);
            const float w2 = 1.0f - w1;

            const float inverseSamples = 1.0f / static_cast<float>(m_samples);

            for (int i = 0; i < m_samples; i++)
            {
                assert((i + 25 + sampleOffset) < 512);
                m_wave1Vertices[i].x = (-1.0f + 2.0f * (i * inverseSamples)) * 0.5f + waveX;
                m_wave1Vertices[i].y = pcmDataL[i + sampleOffset] * 0.235f + waveY;
                m_wave1Vertices[i].x += pcmDataR[i + 25 + sampleOffset] * 0.22f;

                // Momentum
                if (i > 1)
                {
                    m_wave1Vertices[i].x =
                        m_wave1Vertices[i].x * w2 + w1 * (m_wave1Vertices[i - 1].x * 2.0f - m_wave1Vertices[i - 2].x);
                    m_wave1Vertices[i].y =
                        m_wave1Vertices[i].y * w2 + w1 * (m_wave1Vertices[i - 1].y * 2.0f - m_wave1Vertices[i - 2].y);
                }
            }
            break;
        }

        case Mode::ExplosiveHash: {
            m_samples = RenderWaveformSamples;

            m_wave1Vertices = new WaveformVertex[m_samples]();

            const float cosineRotation = cosf(m_presetState.renderContext.time * 0.3f);
            const float sineRotation = sinf(m_presetState.renderContext.time * 0.3f);

            for (int i = 0; i < m_samples; i++)
            {
                const float x0 = (pcmDataR[i] * pcmDataL[i + 32] + pcmDataL[i] * pcmDataR[i + 32]);
                const float y0 = (pcmDataR[i] * pcmDataR[i] - pcmDataL[i + 32] * pcmDataL[i + 32]);
                m_wave1Vertices[i].x = ((x0 * cosineRotation - y0 * sineRotation) * 0.5f * aspectY) + waveX;
                m_wave1Vertices[i].y = ((x0 * sineRotation + y0 * cosineRotation) * 0.5f * aspectX) + waveY;
            }
            break;
        }

        case Mode::Line:
        case Mode::DoubleLine:
        case Mode::SpectrumLine: // Unfinished
        {
            if (m_mode == Mode::SpectrumLine)
            {
                m_samples = 256;
            }
            else
            {
                m_samples = RenderWaveformSamples / 2;

                if (m_samples > m_presetState.viewportWidth / 3)
                {
                    m_samples /= 3;
                }
            }

            m_wave1Vertices = new WaveformVertex[m_samples]();
            if (m_mode == Mode::DoubleLine)
            {
                m_wave2Vertices = new WaveformVertex[m_samples]();
            }

            const int sampleOffset = (RenderWaveformSamples - m_samples) / 2;

            const float angle = PI * 0.5f * mysteryWaveParam; // from -PI/2 to PI/2
            float dx = cosf(angle);
            float dy = sinf(angle);

            std::array<float, 2> edgeX{
                waveX * cosf(angle + PI * 0.5f) - dx * 3.0f,
                waveX * cosf(angle + PI * 0.5f) + dx * 3.0f};

            std::array<float, 2> edgeY{
                waveX * sinf(angle + PI * 0.5f) - dy * 3.0f,
                waveX * sinf(angle + PI * 0.5f) + dy * 3.0f};

            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    float t;
                    bool clip{false};

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

            dx = (edgeX[1] - edgeX[0]) / static_cast<float>(m_samples);
            dy = (edgeY[1] - edgeY[0]) / static_cast<float>(m_samples);

            const float angle2 = atan2f(dy, dx);
            const float perpetualDX = cosf(angle2 + PI * 0.5f);
            const float perpetualDY = sinf(angle2 + PI * 0.5f);

            if (m_mode == Mode::Line)
            {
                for (int i = 0; i < m_samples; i++)
                {
                    m_wave1Vertices[i].x =
                        edgeX[0] + dx * static_cast<float>(i) + perpetualDX * 0.25f * pcmDataL[i + sampleOffset];
                    m_wave1Vertices[i].y =
                        edgeY[0] + dy * static_cast<float>(i) + perpetualDY * 0.25f * pcmDataL[i + sampleOffset];
                }
            }
            else if (m_mode == Mode::SpectrumLine)
            {
                for (int i = 0; i < m_samples; i++)
                {
                    const float f = 0.1f * logf(pcmDataL[i * 2] + pcmDataL[i * 2 + 1]);
                    m_wave1Vertices[i].x = edgeX[0] + dx * static_cast<float>(i) + perpetualDX * f;
                    m_wave1Vertices[i].y = edgeY[0] + dy * static_cast<float>(i) + perpetualDY * f;
                }
            }
            else
            {
                float const separation = powf(waveY * 0.25f + 0.25f, 2.0f);
                for (int i = 0; i < m_samples; i++)
                {
                    m_wave1Vertices[i].x = edgeX[0] + dx * static_cast<float>(i) +
                                           perpetualDX * (0.25f * pcmDataL[i + sampleOffset] + separation);
                    m_wave1Vertices[i].y = edgeY[0] + dy * static_cast<float>(i) +
                                           perpetualDY * (0.25f * pcmDataL[i + sampleOffset] + separation);

                    m_wave2Vertices[i].x = edgeX[0] + dx * static_cast<float>(i) +
                                           perpetualDX * (0.25f * pcmDataR[i + sampleOffset] - separation);
                    m_wave2Vertices[i].y = edgeY[0] + dy * static_cast<float>(i) +
                                           perpetualDY * (0.25f * pcmDataR[i + sampleOffset] - separation);
                }
            }
            break;
        }

        default:
            break;
    }

    // Reverse all Y coordinates to stay consistent with the pre-VMS milkdrop
    for (int i = 0; i < m_samples; i++)
    {
        if (m_wave1Vertices)
        {
            m_wave1Vertices[i].y = 1.0f - m_wave1Vertices[i].y;
        }
        if (m_wave2Vertices)
        {
            m_wave2Vertices[i].y = 1.0f - m_wave2Vertices[i].y;
        }
    }
}

int Waveform::SmoothWave(const Waveform::WaveformVertex* inputVertices,
                         Waveform::WaveformVertex* outputVertices)
{
    constexpr float c1{-0.15f};
    constexpr float c2{1.15f};
    constexpr float c3{1.15f};
    constexpr float c4{-0.15f};
    constexpr float inverseSum{1.0f / (c1 + c2 + c3 + c4)};

    int outputIndex = 0;
    int indexBelow = 0;
    int indexAbove2 = 1;

    for (auto inputIndex = 0; inputIndex < m_samples - 1; inputIndex++)
    {
        int const indexAbove = indexAbove2;
        indexAbove2 = std::min(m_samples - 1, inputIndex + 2);
        outputVertices[outputIndex] = inputVertices[inputIndex];
        outputVertices[outputIndex + 1].x = (c1 * inputVertices[indexBelow].x + c2 * inputVertices[inputIndex].x + c3 * inputVertices[indexAbove].x + c4 * inputVertices[indexAbove2].x) * inverseSum;
        outputVertices[outputIndex + 1].y = (c1 * inputVertices[indexBelow].y + c2 * inputVertices[inputIndex].y + c3 * inputVertices[indexAbove].y + c4 * inputVertices[indexAbove2].y) * inverseSum;
        indexBelow = inputIndex;
        outputIndex += 2;
    }

    outputVertices[outputIndex] = inputVertices[m_samples - 1];

    return outputIndex + 1;
}
