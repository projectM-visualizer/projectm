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
        CenteredSpiro,               //!< Centered spiro (alpha constant). Aimed at not being so sound-responsive, but being very "nebula-like".
        CenteredSpiroVolume,               //!< Centered spiro (alpha tied to volume). Aimed at having a strong audio-visual tie-in.
        DerivativeLine,      //!< Horizontal "script", left channel.
        ExplosiveHash,       //!< Weird explosive complex # thingy.
        Line,                //!< Angle-adjustable left channel, with temporal wave alignment.
        DoubleLine,          //!< Same, except there are two channels shown.
        SpectrumLine         //!< Same as "Line", except using the spectrum analyzer (UNFINISHED).
    };

    Waveform(PresetState& presetState);
    ~Waveform() override;

    void Draw(const PerFrameContext& presetPerFrameContext);

    void InitVertexAttrib() override;


private:
    void SetMode();

    void MaximizeColors(const PerFrameContext& presetPerFrameContext);
    void ModulateOpacityByVolume(const PerFrameContext& presetPerFrameContext);
    void WaveformMath(const PerFrameContext& presetPerFrameContext);

    /**
     * @brief Does a better-than-linear smooth on a wave.
     *
     * Roughly doubles the number of points.
     *
     * @param inputVertices Pointer to an array of vertices to be smoothed.
     * @param outputVertices Pointer to a buffer that will receive the smoothed data. Must be able to hold 2 * vertexCount vertices.
     * @return The number of vertices in outputVertices after smoothing.
     */
    auto SmoothWave(const Point* inputVertices, Point* outputVertices) -> int;

    PresetState& m_presetState; //!< The preset state.

    Mode m_mode{Mode::Line}; //!< Line drawing mode.

    float m_tempAlpha{0.0f}; //!< Calculated alpha value.
    bool m_loop{false}; //!< true if the waveform is a closed loop.
    int m_samples{}; //!< Number of samples in the current waveform. Depends on the mode.

    Point* m_wave1Vertices{nullptr};
    Point* m_wave2Vertices{nullptr};
};
