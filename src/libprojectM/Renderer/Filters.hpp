/*
 * Filters.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#ifndef FILTERS_HPP_
#define FILTERS_HPP_

#include "RenderItem.hpp"

class Brighten : public RenderItem
{
public:
    Brighten(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context);
};

class Darken : public RenderItem
{
public:
    Darken(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context);
};

class Invert : public RenderItem
{
public:
    Invert(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context);
};

class Solarize : public RenderItem
{
public:
    Solarize(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context);
};

#endif /* FILTERS_HPP_ */
