#ifndef Transformation_HPP
#define Transformation_HPP

#include "PerPixelMesh.hpp"
#include <vector>

class PerPixelTransform
{
public:
	int width;
	int height;

	PerPixelTransform(int width, int height);
	virtual void Calculate(PerPixelMesh* mesh) = 0;
};

class Warp : public PerPixelTransform
{
public:
	float warp;
	float speed;
	float scale;
	float time;

	Warp(int width, int height, float warp, float speed, float scale);
 	void Calculate(PerPixelMesh* mesh);
};

class PerPixelWarp : public PerPixelTransform
{
public:
	std::vector< std::vector<float> > warp;
	float speed;
	float scale;
	float time;

	PerPixelWarp(int width, int height, float warp, float speed, float scale);
 	void Calculate(PerPixelMesh* mesh);
};

class Scale : public PerPixelTransform
{
public:
	float cx;
	float cy;
	float sx;
	float sy;

	Scale(int width, int height, float cx, float cy, float sx, float sy);
	void Calculate(PerPixelMesh* mesh);
};

class PerPixelScale : public PerPixelTransform
{
public:
	std::vector< std::vector<float> > cx;
	std::vector< std::vector<float> > cy;
	std::vector< std::vector<float> > sx;
	std::vector< std::vector<float> > sy;

	PerPixelScale(int width, int height, float cx, float cy, float sx, float sy);
 	void Calculate(PerPixelMesh* mesh);
};

class Rotation : public PerPixelTransform
{
public:
	float cx;
	float cy;
	float angle;

	Rotation(int width, int height, float cx, float cy, float angle);
 	void Calculate(PerPixelMesh* mesh);
};

class PerPixelRotation : public PerPixelTransform
{
public:
	std::vector< std::vector<float> > cx;
	std::vector< std::vector<float> > cy;
	std::vector< std::vector<float> > angle;

	PerPixelRotation(int width, int height, float cx, float cy, float angle);
 	void Calculate(PerPixelMesh* mesh);
};

class Delta : public PerPixelTransform
{
public:
	float dx;
	float dy;

	Delta(int width, int height, float dx, float dy);
	void Calculate (PerPixelMesh* mesh);
};

class PerPixelDelta : public PerPixelTransform
{
public:
	std::vector< std::vector<float> > dx;
	std::vector< std::vector<float> > dy;

	PerPixelDelta(int width, int height, float dx, float dy);
	virtual void Calculate (PerPixelMesh* mesh);
};

class Zoom : public PerPixelTransform
{
public:
	float zoom;
	float zoomExponent;

	Zoom(int width, int height, float zoom, float zoomExponent);
	void Calculate (PerPixelMesh* mesh);
};

class PerPixelZoom : public PerPixelTransform
{
public:
	std::vector< std::vector<float> > zoom;
	std::vector< std::vector<float> > zoomExponent;

	PerPixelZoom(int width, int height, float zoom, float zoomExponent);
	void Calculate (PerPixelMesh* mesh);
};

#endif
