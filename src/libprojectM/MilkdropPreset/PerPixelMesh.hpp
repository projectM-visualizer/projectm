#pragma once

#include <vector>

struct PixelPoint
{
	float x{};
	float y{};

    PixelPoint(float _x, float _y);
};

class PerPixelMesh
{
public:
	int width{};
	int height{};
	int size{};

	std::vector<PixelPoint> p;
	std::vector<PixelPoint> p_original;

    PerPixelMesh(int _width, int _height);

	void Reset();
};
