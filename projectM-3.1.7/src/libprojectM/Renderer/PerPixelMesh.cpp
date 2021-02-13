#include <math.h>
#include <algorithm>
#include "PerPixelMesh.hpp"
#include "omptl/omptl"
#include "omptl/omptl_algorithm"

PerPixelMesh::PerPixelMesh(int _width, int _height) : width(_width), height(_height), size (_width * _height),
        p(_width * _height, PixelPoint(0,0)),
        p_original(_width * _height, PixelPoint(0,0)),
        identity(_width * _height, PerPixelContext(0,0,0,0,0,0))
		{
		for (int j=0;j<height;j++)
			for(int i=0;i<width;i++)
			{
				int index = j * width + i;

						float xval=i/(float)(width-1);
						float yval=-((j/(float)(height-1))-1);

						p[index].x = xval;
						p[index].y = yval;

						p_original[index].x = xval;
						p_original[index].y = yval;

						identity[index].x= xval;
						identity[index].y= yval;

						//identity[index].x= (xval-.5)*2;
						//identity[index].y= (yval-.5)*2;

						identity[index].i= i;
						identity[index].j= j;

						identity[index].rad=hypot ( ( xval-.5 ) *2, ( yval-.5 ) *2 ) * .7071067;
						identity[index].theta=atan2 ( ( yval-.5 ) *2 ,  ( xval-.5 ) *2  );
			}
		}

void PerPixelMesh::Reset()
{
	omptl::copy(p_original.begin(), p_original.end(), p.begin());
}

PixelPoint::PixelPoint(float _x, float _y)
    : x(_x), y(_y) {}
PerPixelContext::PerPixelContext(float _x, float _y, float _rad, float _theta, int _i, int _j)
    : x(_x), y(_y), rad(_rad), theta(_theta), i(_i), j(_j) {}
