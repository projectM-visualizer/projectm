#include "Renderer/BlendMode.hpp"

namespace libprojectM {
namespace Renderer {

void BlendMode::Set(bool enable, Function srcFunc, Function dstFunc)
{
    SetBlendActive(enable);
    SetBlendFunction(srcFunc, dstFunc);
}

void BlendMode::SetBlendActive(bool enable)
{
    if (enable)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void BlendMode::SetBlendFunction(Function srcFunc, Function dstFunc)
{
    glBlendFunc(FunctionToGL(srcFunc), FunctionToGL(dstFunc));
}

auto BlendMode::FunctionToGL(Function func) -> GLuint
{
    switch (func)
    {
        case Function::Zero:
            return GL_ZERO;
        case Function::One:
        default:
            return GL_ONE;
        case Function::SourceColor:
            return GL_SRC_COLOR;
        case Function::OneMinusSourceColor:
            return GL_ONE_MINUS_SRC_COLOR;
        case Function::DestinationColor:
            return GL_DST_COLOR;
        case Function::OneMinusDestinationColor:
            return GL_ONE_MINUS_DST_COLOR;
        case Function::SourceAlpha:
            return GL_SRC_ALPHA;
        case Function::OneMinusSourceAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case Function::DestinationAlpha:
            return GL_DST_ALPHA;
        case Function::OneMinusDestinationAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
        case Function::ConstantColor:
            return GL_CONSTANT_COLOR;
        case Function::OneMinusConstantColor:
            return GL_ONE_MINUS_CONSTANT_COLOR;
        case Function::ConstantAlpha:
            return GL_CONSTANT_ALPHA;
        case Function::OneMinusConstantAlpha:
            return GL_ONE_MINUS_CONSTANT_ALPHA;
        case Function::SourceAlphaSaturate:
            return GL_SRC_ALPHA_SATURATE;
    }
}
} // namespace Renderer
} // namespace libprojectM
