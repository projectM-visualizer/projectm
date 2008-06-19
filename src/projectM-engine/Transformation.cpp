#include "Transformation.hpp"
#include <vector>
#include <math.h>
#include <stdio.h>

using namespace std;

PerPixelTransform::PerPixelTransform(int width, int height) :
	 width(width), height(height) {}


Warp::Warp(int width, int height, float warp, float speed, float scale) :
	PerPixelTransform(width, height),
	speed(speed),scale(scale), warp(warp){}

PerPixelWarp::PerPixelWarp(int width, int height, float warp, float speed, float scale) :
	PerPixelTransform(width, height),
	speed(speed),scale(scale),
	warp(width,vector<float>(height,warp)){}

Scale::Scale(int width, int height, float cx, float cy, float sx, float sy) :
	PerPixelTransform(width, height),
	cx(cx), cy(cy), sx(sx), sy(sy){}

PerPixelScale::PerPixelScale(int width, int height, float cx, float cy, float sx, float sy) :
	PerPixelTransform(width, height),
	cx(width,vector<float>(height,cx)),
	cy(width,vector<float>(height,cy)),
	sx(width,vector<float>(height,sx)),
	sy(width,vector<float>(height,sy)){}

Rotation::Rotation(int width, int height, float cx, float cy, float angle) :
	PerPixelTransform(width, height),
	cx(cx), cy(cy), angle(angle){}


PerPixelRotation::PerPixelRotation(int width, int height, float cx, float cy, float angle) :
	PerPixelTransform(width, height),
	cx(width,vector<float>(height,cx)), cy(width,vector<float>(height,cy)), angle(width,vector<float>(height,angle)){}

Delta::Delta(int width, int height, float dx, float dy) :
	PerPixelTransform(width, height),
	dx(dx), dy(dy){}
	void Calculate (PerPixelMesh* mesh);

PerPixelDelta::PerPixelDelta(int width, int height, float dx, float dy) :
	PerPixelTransform(width, height),
	dx(width,vector<float>(height,dx)), dy(width,vector<float>(height,dy)){}
	void Calculate (PerPixelMesh* mesh);

Zoom::Zoom(int width, int height, float zoom, float zoomExponent) :
	PerPixelTransform(width, height),
	zoom(zoom), zoomExponent(zoomExponent){}

PerPixelZoom::PerPixelZoom(int width, int height, float zoom, float zoomExponent) :
	PerPixelTransform(width, height),
	zoom(width,vector<float>(height,zoom)), zoomExponent(width,vector<float>(height,zoomExponent)){}


void Warp::Calculate(PerPixelMesh* mesh)
{
	float fWarpTime = time * speed;
	float fWarpScaleInv = 1.0f / scale;
	float f[4];
	f[0] = 11.68f + 4.0f*cosf(fWarpTime*1.413f + 10);
	f[1] =  8.77f + 3.0f*cosf(fWarpTime*1.113f + 7);
	f[2] = 10.54f + 3.0f*cosf(fWarpTime*1.233f + 3);
	f[3] = 11.49f + 4.0f*cosf(fWarpTime*0.933f + 5);

	for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			mesh->x[i][j] += warp*0.0035f*sinf(fWarpTime*0.333f + fWarpScaleInv*(mesh->x_identity[i][j]*f[0] - mesh->y_identity[i][j]*f[3]));
			mesh->y[i][j] += warp*0.0035f*cosf(fWarpTime*0.375f - fWarpScaleInv*(mesh->x_identity[i][j]*f[2] + mesh->y_identity[i][j]*f[1]));
			mesh->x[i][j] += warp*0.0035f*cosf(fWarpTime*0.753f - fWarpScaleInv*(mesh->x_identity[i][j]*f[1] - mesh->y_identity[i][j]*f[2]));
			mesh->y[i][j] += warp*0.0035f*sinf(fWarpTime*0.825f + fWarpScaleInv*(mesh->x_identity[i][j]*f[0] + mesh->y_identity[i][j]*f[3]));
		}
	}
}



void PerPixelWarp::Calculate(PerPixelMesh* mesh)
{
	float fWarpTime = time * speed;
	float fWarpScaleInv = 1.0f / scale;
	float f[4];
	f[0] = 11.68f + 4.0f*cosf(fWarpTime*1.413f + 10);
	f[1] =  8.77f + 3.0f*cosf(fWarpTime*1.113f + 7);
	f[2] = 10.54f + 3.0f*cosf(fWarpTime*1.233f + 3);
	f[3] = 11.49f + 4.0f*cosf(fWarpTime*0.933f + 5);

	for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			mesh->x[i][j] += warp[i][j]*0.0035f*sinf(fWarpTime*0.333f + fWarpScaleInv*(mesh->x_identity[i][j]*f[0] - mesh->y_identity[i][j]*f[3]));
			mesh->y[i][j] += warp[i][j]*0.0035f*cosf(fWarpTime*0.375f - fWarpScaleInv*(mesh->x_identity[i][j]*f[2] + mesh->y_identity[i][j]*f[1]));
			mesh->x[i][j] += warp[i][j]*0.0035f*cosf(fWarpTime*0.753f - fWarpScaleInv*(mesh->x_identity[i][j]*f[1] - mesh->y_identity[i][j]*f[2]));
			mesh->y[i][j] += warp[i][j]*0.0035f*sinf(fWarpTime*0.825f + fWarpScaleInv*(mesh->x_identity[i][j]*f[0] + mesh->y_identity[i][j]*f[3]));
		}
	}
}

void PerPixelScale::Calculate(PerPixelMesh* mesh)
{
	for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			mesh->x[i][j]  = ( mesh->x[i][j] - cx[i][j])/sx[i][j] + cx[i][j];
			mesh->y[i][j] = ( mesh->y[i][j] - cy[i][j])/sy[i][j] + cy[i][j];
		}
	}
}

void Scale::Calculate(PerPixelMesh* mesh)
{
	for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			mesh->x[i][j] = ( mesh->x[i][j] - cx)/sx + cx;
			mesh->y[i][j] = ( mesh->y[i][j] - cy)/sy + cy;
		}
	}
}


void Zoom::Calculate(PerPixelMesh* mesh)
{
	for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			float fZoom2 = powf( zoom, powf( zoomExponent, mesh->rad_identity[i][j]*2.0f - 1.0f));
			float fZoom2Inv = 1.0f/fZoom2;
			mesh->x[i][j]= mesh->x[i][j]*0.5f*fZoom2Inv + 0.5f;
			mesh->y[i][j]= mesh->y[i][j]*0.5f*fZoom2Inv + 0.5f;
		}
	}
}

void PerPixelZoom::Calculate(PerPixelMesh* mesh)
{
	for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			float fZoom2 = powf( zoom[i][j], powf( zoomExponent[i][j], mesh->rad_identity[i][j]*2.0f - 1.0f));
			float fZoom2Inv = 1.0f/fZoom2;
			mesh->x[i][j]= mesh->x[i][j]*0.5f*fZoom2Inv + 0.5f;
			mesh->y[i][j]= mesh->y[i][j]*0.5f*fZoom2Inv + 0.5f;
		}
	}
}

void Rotation::Calculate(PerPixelMesh* mesh)
{
for (int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			float u2 = mesh->x[i][j] - cx;
			float v2 = mesh->y[i][j] - cy;

			float cos_rot = cosf(angle);
			float sin_rot = sinf(angle);

			mesh->x[i][j] = u2*cos_rot - v2*sin_rot + cx;
			mesh->y[i][j] = u2*sin_rot + v2*cos_rot + cy;

		}
	}
}

void PerPixelRotation::Calculate(PerPixelMesh* mesh)
{
for (int i=0;i < width;i++)
	{
		for(int j=0;j<height;j++)
		{
			float u2 = mesh->x[i][j] - cx[i][j];
			float v2 = mesh->y[i][j] - cy[i][j];

			float cos_rot = cosf(angle[i][j]);
			float sin_rot = sinf(angle[i][j]);

			mesh->x[i][j] = u2*cos_rot - v2*sin_rot + cx[i][j];
			mesh->y[i][j] = u2*sin_rot + v2*cos_rot + cy[i][j];

		}
	}
}

void Delta::Calculate(PerPixelMesh* mesh)
{
for (int i=0;i < width;i++)
	{
		for(int j=0;j<height;j++)
		{
			mesh->x[i][j] -= dx;
			mesh->y[i][j] -= dy;
		}
	}
}

void PerPixelDelta::Calculate(PerPixelMesh* mesh)
{

for (int i=0;i < width;i++)
	{
		for(int j=0;j<height;j++)
		{
			mesh->x[i][j] -= dx[i][j];
			mesh->y[i][j] -= dy[i][j];
		}
	}
}
