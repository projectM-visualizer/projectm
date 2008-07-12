/*
 * ShaderEngine.hpp
 *
 *  Created on: Jul 11, 2008
 *      Author: pete
 */

#ifndef SHADERENGINE_HPP_
#define SHADERENGINE_HPP_

#include <math.h>

class PerlinNoise
{
public:

	float noise_lq[256][256];
	float noise_lq_lite[32][32];
	float noise_mq[256][256];
	float noise_hq[256][256];
	float noise_lq_vol[32][32][32];
	float noise_hq_vol[32][32][32];


	PerlinNoise();
	virtual ~PerlinNoise();

private:


	static inline float Noise( int x)
	{
	    x = (x<<13)^x;
	    return (((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0);
	   }

	static inline float Noise(int x, int y)
	{
		 int n = x + y * 57;
		 n = (n<<13) ^ n;
		 return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

	}

	static inline float cos_interp(float a, float b, float x)
	{
		float ft = x * 3.1415927;
		float f = (1 - cos(ft)) * .5;

		return  a*(1-f) + b*f;
	}

	static inline float cubic_interp(float v0, float v1, float v2, float v3, float x)
	{
		float P = (v3 - v2) - (v0 - v1);
		float Q = (v0 - v1) - P;
		float R = v2 - v0;

		return P*pow(x,3) + Q * pow(x,2) + R*x + v1;
	}

	static inline float SmoothedNoise(int x, int y)
	{
		float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
		float sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
		float center  =  Noise(x, y) / 4;

		return corners + sides + center;
	}

	static inline float InterpolatedNoise(float x, float y)
	{
		int integer_X = int(x);
		float fractional_X = x - integer_X;

		int integer_Y = int(y);
		float fractional_Y = y - integer_Y;

		float v1 = SmoothedNoise(integer_X,     integer_Y);
		float v2 = SmoothedNoise(integer_X + 1, integer_Y);
		float v3 = SmoothedNoise(integer_X,     integer_Y + 1);
		float v4 = SmoothedNoise(integer_X + 1, integer_Y + 1);

		float i1 = cos_interp(v1 , v2 , fractional_X);
		float i2 = cos_interp(v3 , v4 , fractional_X);

		return cos_interp(i1 , i2 , fractional_Y);

	}

	static inline float  perlin_noise_2D(int x, int y, int n, float p)
	{
		float total = 0;

		for (int i = 0; i < n; i++)
		{
			float frequency = pow(2,i);
			float amplitude = pow(p,i);

			total += InterpolatedNoise(x * frequency, y * frequency) * amplitude;
		}

		return total;
	}


	static inline float InterPolation(float a, float b, float c)
	{
	    return a+(b-a)*c*c*(3-2*c);

	}

	static inline float perlin_noise(float x,float y, int width, int seed, float periode){

	           float a,b,value,freq,tam_pas,zone_x,zone_y;
	           int box,num,step_x,step_y;

	           int noisedata;

	           freq=1/(float)(periode);


	           num=(int)(width*freq);
	           step_x=(int)(x*freq);
	           step_y=(int)(y*freq);
	           zone_x=x*freq-step_x;
	           zone_y=y*freq-step_y;
	           box=step_x+step_y*num;
	           noisedata=(box+seed);
	           a=InterPolation(Noise(noisedata),Noise(noisedata+1),zone_x);
	           b=InterPolation(Noise(noisedata+num),Noise(noisedata+1+num),zone_x);
	           value=InterPolation(a,b,zone_y);

	          return value;
	}

	static inline float perlin_noise_loop(int x, int y,  int width, int octaves, int seed, float persistance, float basePeriod)
	{
		float p = persistance;
		float val = 0.0;

		for (int i = 0; i<octaves;i++)
		{
			val += perlin_noise(x,y,width,seed,basePeriod) * p;

			basePeriod *= 0.5;
			p *= persistance;
		}
		return val;
	}


};

#endif /* SHADERENGINE_HPP_ */
