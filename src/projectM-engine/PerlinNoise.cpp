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
			noise_lq[x][y] = noise(x + 3240*y);

	for (int x = 0; x < 32;x++)
		for (int y = 0; y < 32;y++)
			noise_lq_lite[x][y] = noise(x,y);

	int seed = rand()%1000;
	int seed2 = rand()%1000;
	int seed3 = rand()%1000;

	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_mq[x][y] = perlin_noise_2d(x,y,1813,7,seed,0.5,64);

	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_hq[x][y] = perlin_noise_2d(x,y,6321,7,seed2,0.5,64);

	for (int x = 0; x < 32;x++)
		for (int y = 0; y < 32;y++)
			for (int z = 0; z < 32;z++)
				noise_lq_vol[x][y][z] = noise(x,y,z);

	for (int x = 0; x < 32;x++)
		for (int y = 0; y < 32;y++)
			for (int z = 0; z < 32;z++)
				noise_hq_vol[x][y][z] = perlin_noise_3d(x,y,z,6121,7,seed3,0.5,64);


}

PerlinNoise::~PerlinNoise()
{
	// TODO Auto-generated destructor stub
}
