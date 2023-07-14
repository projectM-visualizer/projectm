/*
 * Filters.hpp
 *
 *  Created on: Jun 18, 2008
 *      Author: pete
 */

#ifndef FILTERS_HPP_
#define FILTERS_HPP_

#include "RenderItem.hpp"
#include "RenderItemVisitor.hpp"

class Brighten : public RenderItem
{
public:
    Brighten(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context) override;
	void DrawVisit(RenderContext& context, RenderItemVisitor& visitor) override;
};

class Darken : public RenderItem
{
public:
    Darken(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context) override;
	void DrawVisit(RenderContext& context, RenderItemVisitor& visitor) override;
};

class Invert : public RenderItem
{
public:
    Invert(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context) override;
	void DrawVisit(RenderContext& context, RenderItemVisitor& visitor) override;
};

class Solarize : public RenderItem
{
public:
    Solarize(){ Init(); }
    void InitVertexAttrib();
	void Draw(RenderContext &context) override;
	void DrawVisit(RenderContext& context, RenderItemVisitor& visitor) override;
};

#endif /* FILTERS_HPP_ */
