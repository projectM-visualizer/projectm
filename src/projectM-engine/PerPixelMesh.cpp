#include <math.h>
#include "PerPixelMesh.hpp"

PerPixelMesh::PerPixelMesh(int width, int height) : width(width), height(height),
		x(width, std::vector<float>(height)),
		y(width, std::vector<float>(height)),
		x_tex(width, std::vector<float>(height)),
		y_tex(width, std::vector<float>(height)),
		x_identity(width, std::vector<float>(height)),
		y_identity(width, std::vector<float>(height)),
		rad_identity(width, std::vector<float>(height)),
		theta_identity(width, std::vector<float>(height))
		{
			for (int i=0;i < width; i++ )
				for (int j=0;j < height; j++ )
					{
						float xval=i/(float)(width-1);
						float yval=-((j/(float)(height-1))-1);

						x_tex[i][j] = xval;
						y_tex[i][j] = yval;
						x_identity[i][j]=( xval-.5)*2;
						y_identity[i][j]=( yval-.5)*2;

						//x_identity[i][j]=xval;
						//y_identity[i][j]=yval;

						x[i][j] = x_identity[i][j];
						y[i][j] = y_identity[i][j];

						rad_identity[i][j]=hypot ( ( xval-.5 ) *2, ( yval-.5 ) *2 ) * .7071067;
						theta_identity[i][j]=atan2 ( ( yval-.5 ) *2 ,  ( xval-.5 ) *2  );
					}
		}

void PerPixelMesh::Reset()
{
	for (int i=0;i < width; i++ )
		for (int j=0;j < height; j++ )
		{
			x[i][j]=x_identity[i][j];
			y[i][j]=y_identity[i][j];
		}
}
