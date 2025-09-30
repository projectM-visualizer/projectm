#pragma once

#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {

class BlendMode
{
public:
    enum class Function : int
    {
        Zero,
        One,
        SourceColor,
        OneMinusSourceColor,
        DestinationColor,
        OneMinusDestinationColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationAlpha,
        OneMinusDestinationAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SourceAlphaSaturate,
        Source1Color,
        OneMinusSource1Color,
        Source1Alpha,
        OneMinusSource1Alpha
    };

    BlendMode() = delete;

    static void Set(bool enable, Function srcFunc, Function dstFunc)
    {
        SetBlendActive(enable);
        SetBlendFunction(srcFunc, dstFunc);
    }

    static void SetBlendActive(bool enable)
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

    static void SetBlendFunction(Function srcFunc, Function dstFunc)
    {
        glBlendFunc(FunctionToGL(srcFunc), FunctionToGL(dstFunc));
    }

private:
    static GLuint FunctionToGL(Function func)
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
};

} // namespace Renderer
} // namespace libprojectM
