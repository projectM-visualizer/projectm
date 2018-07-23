/*
 * PerlinNoise.cpp
 *
 *  Created on: Jul 11, 2008
 *      Author: pete
 */

#include "PerlinNoise.hpp"
#include <iostream>
#include <stdlib.h>

PerlinNoise::PerlinNoise()
{
    for (int x = 0; x < 256;x++) {
        for (int y = 0; y < 256;y++) {
            noise_lq[x][y][0] = noise(x , y);
            noise_lq[x][y][1] = noise_lq[x][y][0];
            noise_lq[x][y][2] = noise_lq[x][y][0];
        }
    }

    for (int x = 0; x < 32;x++) {
        for (int y = 0; y < 32;y++) {
            noise_lq_lite[x][y][0] = noise(4*x,16*y);
            noise_lq_lite[x][y][1] = noise_lq_lite[x][y][0];
            noise_lq_lite[x][y][2] = noise_lq_lite[x][y][0];
        }
    }

    for (int x = 0; x < 256;x++) {
        for (int y = 0; y < 256;y++) {
            noise_mq[x][y][0] = InterpolatedNoise((float)x/(float)2.0,(float)y/(float)2.0);
            noise_mq[x][y][1] = noise_mq[x][y][0];
            noise_mq[x][y][2] = noise_mq[x][y][0];
        }
    }

    for (int x = 0; x < 256;x++) {
        for (int y = 0; y < 256;y++) {
            noise_hq[x][y][0] = InterpolatedNoise((float)x/(float)3.0,(float)y/(float)3.0);
            noise_hq[x][y][1] = noise_hq[x][y][0];
            noise_hq[x][y][2] = noise_hq[x][y][0];
        }
    }

    for (int x = 0; x < 32;x++) {
        for (int y = 0; y < 32;y++) {
            for (int z = 0; z < 32;z++) {
                noise_lq_vol[x][y][z][0] = noise(x,y,z);
                noise_lq_vol[x][y][z][1] = noise_lq_vol[x][y][z][0];
                noise_lq_vol[x][y][z][2] = noise_lq_vol[x][y][z][0];
            }
        }
    }

    for (int x = 0; x < 32;x++) {
        for (int y = 0; y < 32;y++) {
            for (int z = 0; z < 32;z++) {
                noise_hq_vol[x][y][z][0] = noise(x,y,z);
                noise_hq_vol[x][y][z][1] = noise_hq_vol[x][y][z][0];
                noise_hq_vol[x][y][z][2] = noise_hq_vol[x][y][z][0];
            }
        }
    }
}

PerlinNoise::~PerlinNoise()
{
	// TODO Auto-generated destructor stub
}
