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


	static inline float noise( int x)
	{
	    x = (x<<13)^x;
	    return (((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0);
	   }

	static inline float noise(int x, int y)
	{
		 int n = x + y * 57;
		 noise(n);
	}

	static inline float noise(int x, int y, int z)
	{
		 int n = x + y * 57 + z * 141;
		 noise(n);
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
		float corners = ( noise(x-1, y-1)+noise(x+1, y-1)+noise(x-1, y+1)+noise(x+1, y+1) ) / 16;
		float sides   = ( noise(x-1, y)  +noise(x+1, y)  +noise(x, y-1)  +noise(x, y+1) ) /  8;
		float center  =  noise(x, y) / 4;

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

	static inline float tri_interp(float a, float b, float c)
	{
	    return a+(b-a)*c*c*(3-2*c);
	}

	static inline float perlin_octave_2d(float x,float y, int width, int seed, float period)
	{

	           float freq=1/(float)(period);

	           int num=(int)(width*freq);
	           int step_x=(int)(x*freq);
	           int step_y=(int)(y*freq);
	           float zone_x=x*freq-step_x;
	           float zone_y=y*freq-step_y;
	           int box=step_x+step_y*num;
	           int noisedata=(box+seed);

	           float u=cubic_interp(noise(noisedata-num-1),noise(noisedata-num),noise(noisedata-num+1),noise(noisedata-num+2),zone_x);
	           float a=cubic_interp(noise(noisedata-1),noise(noisedata),noise(noisedata+1),noise(noisedata+2),zone_x);
	           float b=cubic_interp(noise(noisedata+num -1),noise(noisedata+num),noise(noisedata+1+num),noise(noisedata+2+num),zone_x);
	           float v=cubic_interp(noise(noisedata+2*num -1),noise(noisedata+2*num),noise(noisedata+1+2*num),noise(noisedata+2+2*num),zone_x);

	           float value=cubic_interp(u,a,b,v,zone_y);

	          return value;
	}

	static inline float perlin_noise_2d(int x, int y,  int width, int octaves, int seed, float persistance, float basePeriod)
	{
		float p = persistance;
		float val = 0.0;

		for (int i = 0; i<octaves;i++)
		{
			val += perlin_octave_2d(x,y,width,seed,basePeriod) * p;

			basePeriod *= 0.5;
			p *= persistance;
		}
		return val;
	}

	static inline float perlin_octave_3d(float x,float y, float z,int width, int seed, float period)
		{
		           float freq=1/(float)(period);

		           int num=(int)(width*freq);
		           int step_x=(int)(x*freq);
		           int step_y=(int)(y*freq);
		           int step_z=(int)(z*freq);
		           float zone_x=x*freq-step_x;
		           float zone_y=y*freq-step_y;
		           float zone_z=z*freq-step_z;

		           int boxB=step_x+step_y+step_z*num;
		           int boxC=step_x+step_y+step_z*(num+1);
		           int boxD=step_x+step_y+step_z*(num+2);
		           int boxA=step_x+step_y+step_z*(num-1);

		           float u,a,b,v,noisedata,box;

		           box = boxA;
		           noisedata=(box+seed);
		           u=cubic_interp(noise(noisedata-num-1),noise(noisedata-num),noise(noisedata-num+1),noise(noisedata-num+2),zone_x);
		           a=cubic_interp(noise(noisedata-1),noise(noisedata),noise(noisedata+1),noise(noisedata+2),zone_x);
		           b=cubic_interp(noise(noisedata+num -1),noise(noisedata+num),noise(noisedata+1+num),noise(noisedata+2+num),zone_x);
		           v=cubic_interp(noise(noisedata+2*num -1),noise(noisedata+2*num),noise(noisedata+1+2*num),noise(noisedata+2+2*num),zone_x);
		           float A=cubic_interp(u,a,b,v,zone_y);

		           box = boxB;
		           noisedata=(box+seed);
		           u=cubic_interp(noise(noisedata-num-1),noise(noisedata-num),noise(noisedata-num+1),noise(noisedata-num+2),zone_x);
		           a=cubic_interp(noise(noisedata-1),noise(noisedata),noise(noisedata+1),noise(noisedata+2),zone_x);
		           b=cubic_interp(noise(noisedata+num -1),noise(noisedata+num),noise(noisedata+1+num),noise(noisedata+2+num),zone_x);
		           v=cubic_interp(noise(noisedata+2*num -1),noise(noisedata+2*num),noise(noisedata+1+2*num),noise(noisedata+2+2*num),zone_x);
		           float B=cubic_interp(u,a,b,v,zone_y);

		           box = boxC;
		           noisedata=(box+seed);
		           u=cubic_interp(noise(noisedata-num-1),noise(noisedata-num),noise(noisedata-num+1),noise(noisedata-num+2),zone_x);
		           a=cubic_interp(noise(noisedata-1),noise(noisedata),noise(noisedata+1),noise(noisedata+2),zone_x);
		           b=cubic_interp(noise(noisedata+num -1),noise(noisedata+num),noise(noisedata+1+num),noise(noisedata+2+num),zone_x);
		           v=cubic_interp(noise(noisedata+2*num -1),noise(noisedata+2*num),noise(noisedata+1+2*num),noise(noisedata+2+2*num),zone_x);
		           float C=cubic_interp(u,a,b,v,zone_y);

		           box = boxD;
		           noisedata=(box+seed);
		           u=cubic_interp(noise(noisedata-num-1),noise(noisedata-num),noise(noisedata-num+1),noise(noisedata-num+2),zone_x);
		           a=cubic_interp(noise(noisedata-1),noise(noisedata),noise(noisedata+1),noise(noisedata+2),zone_x);
		           b=cubic_interp(noise(noisedata+num -1),noise(noisedata+num),noise(noisedata+1+num),noise(noisedata+2+num),zone_x);
		           v=cubic_interp(noise(noisedata+2*num -1),noise(noisedata+2*num),noise(noisedata+1+2*num),noise(noisedata+2+2*num),zone_x);
		           float D=cubic_interp(u,a,b,v,zone_y);

		           float value =cubic_interp(A,B,C,D,zone_z);

		           return value;
		}


	static inline float perlin_noise_3d(int x, int y, int z, int width, int octaves, int seed, float persistance, float basePeriod)
		{
			float p = persistance;
			float val = 0.0;

			for (int i = 0; i<octaves;i++)
			{
				val += perlin_octave_3d(x,y,z,width,seed,basePeriod) * p;

				basePeriod *= 0.5;
				p *= persistance;
			}
			return val;
		}



};

#endif /* SHADERENGINE_HPP_ */
