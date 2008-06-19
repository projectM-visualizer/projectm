/*
 * Filters.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#ifndef FILTERS_HPP_
#define FILTERS_HPP_

#include "Renderable.hpp"

class Brighten : public RenderItem
{
	Brighten(){}
	void Draw();
};

class Darken : public RenderItem
{
	Darken(){}
	void Draw();
};

class Invert : public RenderItem
{
	Invert(){}
	void Draw();
};

class Solarize : public RenderItem
{
	Solarize(){}
	void Draw();
};

#endif /* FILTERS_HPP_ */
