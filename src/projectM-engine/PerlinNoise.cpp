/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 11, 2008
 *      Author: pete
 */

#include "PerlinNoise.hpp"
#include <iostream>
#include <stdlib.h>

PerlinNoise::PerlinNoise()
{
	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_lq[x][y] = InterpolatedNoise(x,y);

	for (int x = 0; x < 32;x++)
		for (int y = 0; y < 32;y++)
			noise_lq_lite[x][y] = Noise(x,y);


	int seed = rand();

	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_mq[x][y] = perlin_noise_loop(x,y,12413,7,seed,0.5,32);

	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_hq[x][y] = perlin_noise_loop(x,y,12413,7,seed,0.5,64);



}

PerlinNoise::~PerlinNoise()
{
	// TODO Auto-generated destructor stub
}
