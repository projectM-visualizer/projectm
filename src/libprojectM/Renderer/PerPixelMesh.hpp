#ifndef PerPixelMesh_HPP
#define PerPixelMesh_HPP

#include <vector>

struct PixelPoint
{
	float x;
	float y;

    PixelPoint(float _x, float _y);
};

struct PerPixelContext
{
	float x;
	float y;
	float rad;
	float theta;

	int i;
	int j;

    PerPixelContext(float x, float _y, float _rad, float _theta, int _i, int _j);
};

class PerPixelMesh
{
public:
	int width;
	int height;
	int size;

	std::vector<PixelPoint> p;
	std::vector<PixelPoint> p_original;
	std::vector<PerPixelContext> identity;

    PerPixelMesh(int _width, int _height);

	void Reset();
};



#endif
