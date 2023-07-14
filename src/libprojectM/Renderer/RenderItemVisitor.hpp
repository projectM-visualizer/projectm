#ifndef RENDERITEMVISITOR_HPP_
#define RENDERITEMVISITOR_HPP_

#include "RenderItem.hpp"

class RenderItemVisitor 
{ 

public:
    void drawWaveform(RenderContext& context, RenderItem& waveform);
private:
};

#endif /* RENDERITEMVISITOR_HPP_ */
