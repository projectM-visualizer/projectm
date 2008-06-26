/*
 * MilkdropWaveform.hpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */

#ifndef MILKDROPWAVEFORM_HPP_
#define MILKDROPWAVEFORM_HPP_

#include "Renderable.hpp"

class MilkdropWaveform : public RenderItem
{
public:

	enum MilkdropWaveformMode
	{
		RadialBlob, Circle, Blob2, Blob3, DerivativeLine, Blob5, Line, DoubleLine
	};

	float x;
	float y;

	float r;
	float g;
	float b;
	float a;

	float mystery;

    MilkdropWaveformMode mode;

	bool additive;
	bool dots;
	bool thick;
	bool modulateAlphaByVolume;
	bool maximizeColors;
	float scale;
	float smoothing;

	MilkdropWaveform();
	void Draw(RenderContext &context);

	float modOpacityStart;
	float modOpacityEnd;

private:
	float rot;
	float aspectScale;
	int samples;
	bool two_waves;
	bool loop;
	float wavearray[2048][2];
	float wavearray2[2048][2];

	void MaximizeColors(RenderContext &context);
	void ModulateOpacityByVolume(RenderContext &context);
	void WaveformMath(RenderContext &context);

};
#endif /* MILKDROPWAVEFORM_HPP_ */
