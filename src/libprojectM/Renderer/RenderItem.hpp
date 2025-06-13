#pragma once

#include <projectM-opengl.h>
#include <cmath>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Computes the modulus to wrap float values into the range of [0.0, 1.0].
 * 
 * This code mimics the following equations used by the original Milkdrop code:
 * 
 * v[0].Diffuse =
 *     ((((int)(*pState->m_shape[i].var_pf_a * 255 * alpha_mult)) & 0xFF) << 24) |
 *     ((((int)(*pState->m_shape[i].var_pf_r * 255)) & 0xFF) << 16) |
 *     ((((int)(*pState->m_shape[i].var_pf_g * 255)) & 0xFF) <<  8) |
 *     ((((int)(*pState->m_shape[i].var_pf_b * 255)) & 0xFF)      );
 * 
 * In projectM, we use float values when drawing primitives or configuring vertices.
 * Converting the above back to a float looks like this:
 * 
 * d = (f * 255.0) & 0xFF = int((f * 255.0) % 256.0); *
 * f' = float(d)/255.0;
 * 
 * * Here % represents the Euclidean modulus, not the traditional (signed) fractional
 * remainder.
 * 
 * To avoid limiting ourselves to 8 bits, we combine the above equations into one that
 * does not discard any information:
 * 
 * f' = ((f * 255.0) % 256.0) / 255.0;
 *    = f % (256.0/255.0);
 * 
 * Since we're using the Euclidean modulus we need to generate it from the fractional
 * remainder using a standard equation.
 */
inline float color_modulo(float x)
{
    const float m = 256.0f / 255.0f;
    return std::fmod(std::fmod(x, m) + m, m);
}

inline float color_modulo(double x)
{
    // Convert the input to float before performing the computation.
    return color_modulo(static_cast<float>(x));
}

/**
 * @brief Base class for render meshes.
 * Also defines a few standard vertex attribute structures for use with the shaders.
 */
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
        float x{}, y{}, r{}, g{}, b{}, a{};
    };

    struct TexturedPoint {
        float x{}, y{}, r{}, g{}, b{}, a{}, u{}, v{};
    };

    virtual ~RenderItem() = default;

    // Abstract interface for backend implementations
    virtual void InitVertexAttrib() = 0;

    // For legacy compatibility: allow (optional) backend-specific initialization
    virtual void Init() {}
};

} // namespace Renderer
} // namespace libprojectM
