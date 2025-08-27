#pragma once

#include <cmath>
#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {


inline float color_modulo(float x)
{
    const float m = 256.0f / 255.0f;
    return std::fmod(std::fmod(x, m) + m, m);
}

inline float color_modulo(double x)
{

    return color_modulo(static_cast<float>(x));
}


class RenderItem
{
public:

    struct Point {
        Point() = default;
        Point(float pointX, float pointY)
            : x(pointX)
            , y(pointY){};

        float x{};
        float y{};
    };


    struct ColoredPoint {
        float x{};
        float y{};
        float r{};
        float g{};
        float b{};
        float a{};
    };


    struct TexturedPoint {
        float x{};
        float y{};
        float r{};
        float g{};
        float b{};
        float a{};
        float u{};
        float v{};
    };


    RenderItem() = default;


    virtual ~RenderItem();


    virtual void InitVertexAttrib() = 0;

protected:

    void Init();

    GLuint m_vboID{0};
    GLuint m_vaoID{0};
};

}
}
