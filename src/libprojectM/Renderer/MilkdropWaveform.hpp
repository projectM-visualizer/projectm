/*
 * MilkdropWaveform.hpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */

#ifndef MILKDROPWAVEFORM_HPP_
#define MILKDROPWAVEFORM_HPP_

#include "RenderItem.hpp"

enum MilkdropWaveformMode
	{
	  Circle = 0, //!< Circular wave.
	  XYOscillationSpiral, //!< X-Y osc. that goes around in a spiral, in time.
	  Blob2, //!< Centered spiro (alpha constant). Aimed at not being so sound-responsive, but being very "nebula-like".
	  Blob3, //!< Centered spiro (alpha tied to volume). Aimed at having a strong audio-visual tie-in.
	  DerivativeLine, //!< Horizontal "script", left channel.
	  ExplosiveHash, //!< Weird explosive complex # thingy.
	  Line, //!< Angle-adjustable left channel, with temporal wave alignment.
	  DoubleLine, //!< Same, except there are two channels shown.
	  SpectrumLine, //!< Same as "Line", except using the spectrum analyzer (UNFINISHED).
	  last // last is a placeholder to find enum size. Please ignore and leave this here.
	};


class MilkdropWaveform : public RenderItem
{
public:
    MilkdropWaveform();
    ~MilkdropWaveform() override;

    void Draw(RenderContext &context) override;
    void InitVertexAttrib() override;

	float x{ 0.5f };
	float y{ 0.5f };

	float r{ 1.0f };
	float g{ 0.0f };
	float b{ 0.0f };
	float a{ 1.0f };

	float mystery{ 0.0f };

    MilkdropWaveformMode mode{ MilkdropWaveformMode::Line };

	bool additive{ false };
	bool dots{ false };
	bool thick{ false };
	bool modulateAlphaByVolume{ false };
	bool maximizeColors{ false };
	float scale{ 10.0f };
	float smoothing{ 0.0f }; // DOES NOTHING

	float modOpacityStart{ 0.0f };
	float modOpacityEnd{ 1.0f };

private:
    static const int NumWaveformSamples{ 480 };

    struct WaveformVertex
    {
        float x{ 0.0f };
        float y{ 0.0f };
    };

	float temp_a{ 0.0f };
	float rot{ 0.0f };
	int samples{ NumWaveformSamples };
	bool loop{ false };

    WaveformVertex* wave1Vertices{ nullptr };
    WaveformVertex* wave2Vertices{ nullptr };

	void MaximizeColors(RenderContext &context);
	void ModulateOpacityByVolume(RenderContext &context);
	void WaveformMath(RenderContext &context);

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
    static int SmoothWave(const WaveformVertex* inputVertices, int vertexCount, WaveformVertex* outputVertices);

};
#endif /* MILKDROPWAVEFORM_HPP_ */
