#pragma once

#include "Constants.hpp"

#include "Renderer/RenderItem.hpp"

class PresetState;

class PerFrameContext;

class Waveform : public RenderItem
{
public:
    enum class Mode : int
    {
        Circle = 0,          //!< Circular wave.
        XYOscillationSpiral, //!< X-Y osc. that goes around in a spiral, in time.
        Blob2,               //!< Centered spiro (alpha constant). Aimed at not being so sound-responsive, but being very "nebula-like".
        Blob3,               //!< Centered spiro (alpha tied to volume). Aimed at having a strong audio-visual tie-in.
        DerivativeLine,      //!< Horizontal "script", left channel.
        ExplosiveHash,       //!< Weird explosive complex # thingy.
        Line,                //!< Angle-adjustable left channel, with temporal wave alignment.
        DoubleLine,          //!< Same, except there are two channels shown.
        SpectrumLine         //!< Same as "Line", except using the spectrum analyzer (UNFINISHED).
    };

    Waveform(PresetState& presetState);
    ~Waveform() override;

    void Draw();

    void InitVertexAttrib() override;


private:
    struct WaveformVertex {
        float x{0.0f};
        float y{0.0f};
    };

    void SetMode();

    void MaximizeColors();
    void ModulateOpacityByVolume();
    void WaveformMath();

    /**
     * @brief Does a better-than-linear smooth on a wave.
     *
     * Roughly doubles the number of points.
     *
     * @param inputVertices Pointer to an array of vertices to be smoothed.
     * @param vertexCount Number of vertices/points in the input data.
     * @param outputVertices Pointer to a buffer that will receive the smoothed data. Must be able to hold 2 * vertexCount vertices.
     * @return The number of vertices in outputVertices after smoothing.
     */
    static auto SmoothWave(const WaveformVertex* inputVertices, WaveformVertex* outputVertices) -> int;

    PresetState& m_presetState;

    Mode m_mode{Mode::Line};

    float m_tempAlpha{0.0f};
    bool m_loop{false};

    WaveformVertex* wave1Vertices{nullptr};
    WaveformVertex* wave2Vertices{nullptr};
};
