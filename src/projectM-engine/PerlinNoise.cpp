/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 11, 2008
 *      Author: pete
 */

#include "PerlinNoise.hpp"

PerlinNoise::PerlinNoise()
{
	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_lq[x][y] = Noise(x,y);

	for (int x = 0; x < 32;x++)
		for (int y = 0; y < 32;y++)
			noise_lq_lite[x][y] = Noise(x,y);

	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_hq[x][y] = perlin_noise_2D(x,y,5,1.414);
}

PerlinNoise::~PerlinNoise()
{
	// TODO Auto-generated destructor stub
}
