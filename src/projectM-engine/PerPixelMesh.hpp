#ifndef PerPixelMesh_HPP
#define PerPixelMesh_HPP

#include <vector>

class PerPixelMesh
{
public:
	int width;
	int height;

	std::vector< std::vector <float> > x;
	std::vector< std::vector <float> > y;
	std::vector< std::vector <float> > x_tex;
	std::vector< std::vector <float> > y_tex;

	std::vector< std::vector <float> > x_identity;
	std::vector< std::vector <float> > y_identity;
	std::vector< std::vector <float> > rad_identity;
	std::vector< std::vector <float> > theta_identity;

	PerPixelMesh(int width, int height);

	void Reset();
};

#endif
