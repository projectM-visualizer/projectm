#pragma once

#include "Renderer/RenderItem.hpp"

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
