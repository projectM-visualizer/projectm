/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 11, 2008
 *      Author: pete
 */

#include "PerlinNoise.hpp"
#include <iostream>

PerlinNoise::PerlinNoise()
{
	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
			noise_lq[x][y] = Noise(x,y);

	for (int x = 0; x < 32;x++)
		for (int y = 0; y < 32;y++)
			noise_lq_lite[x][y] = Noise(x,y);

	int scale = 4;
	int width = 12413;
	int seed = 61;

	for (int x = 0; x < 256;x++)
		for (int y = 0; y < 256;y++)
		{

			  float disp1= perlin_noise(x*scale,y*scale,width,1,seed,100);
			  float disp2= perlin_noise(x*scale,y*scale,width,1,seed,25);
			  float disp3= perlin_noise(x*scale,y*scale,width,1,seed,12.5);
			  float disp4= perlin_noise(x*scale,y*scale,width,1,seed,6.25);
			  float disp5= perlin_noise(x*scale,y*scale,width,1,seed,3.125);
			  float disp6= perlin_noise(x*scale,y*scale,width,1,seed,1.56);

			  noise_hq[x][y] = disp1+(disp2*.25)+(disp3*.125)+(disp4*.0625)+(disp5*.03125)+(disp6*.0156);


			//noise_hq[x][y] = perlin_noise_2D(x,y,5,0.5);
		}

}

PerlinNoise::~PerlinNoise()
{
	// TODO Auto-generated destructor stub
}
