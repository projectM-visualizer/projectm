#pragma once

#include "Renderer/OpenGL.h"

#include <cmath>
#include <cstdint>

namespace libprojectM {
namespace Renderer {

/**
 * @brief A color with RGBA channels.
 */
class Color
{
public:
    Color() = default;

    /**
     * Constructs a color with the given values.
     * @param r The color's r value.
     * @param g The color's g value.
     * @param b The color's b value.
     * @param a The color's a value.
     */
    Color(float r, float g, float b, float a)
        : m_r(r)
        , m_g(g)
        , m_b(b)
        , m_a(a) {};

    /**
     * Returns the color's r value.
     * @return The color's r value.
     */
    auto R() const -> float
    {
        return m_r;
    }

    /**
     * Sets the color's r value.
     * @param r The new r value.
     */
    void SetR(float r)
    {
        m_r = r;
    }

    /**
     * Returns the color's g value.
     * @return The color's g value.
     */
    auto G() const -> float
    {
        return m_g;
    }

    /**
     * Sets the color's g value.
     * @param g The new g value.
     */
    void SetG(float g)
    {
        m_g = g;
    }

    /**
     * Returns the color's b value.
     * @return The color's b value.
     */
    auto B() const -> float
    {
        return m_b;
    }

    /**
     * Sets the color's b value.
     * @param b The new b value.
     */
    void SetB(float b)
    {
        m_b = b;
    }

    /**
     * Returns the color's a value.
     * @return The color's a value.
     */
    auto A() const -> float
    {
        return m_a;
    }

    /**
     * Sets the color's a value.
     * @param a The new a value.
     */
    void SetA(float a)
    {
        m_a = a;
    }

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
     * @param x The color channel value to clamp.
     * @return The color value clamped to the range of [0.0, 1.0].
     */
    static auto Modulo(float x) -> float
    {
        const float m = 256.0f / 255.0f;
        return std::fmod(std::fmod(x, m) + m, m);
    }

    /**
     * @brief Computes the modulus to wrap float values into the range of [0.0, 1.0].
     * @param x The color channel value to clamp.
     * @return The color value clamped to the range of [0.0, 1.0].
     */
    static auto Modulo(double x) -> float
    {
        // Convert the input to float before performing the computation.
        return Modulo(static_cast<float>(x));
    }

    /**
     * @brief Computes the modulus to wrap float values into the range of [0.0, 1.0].
     * @param col The Color instance to clamp.
     * @return The color value clamped to the range of [0.0, 1.0].
     */
    static auto Modulo(Color col) -> Color
    {
        return Color(Modulo(col.R()),
                     Modulo(col.G()),
                     Modulo(col.B()),
                     Modulo(col.A()));
    }

    /**
     * @brief Initializes the attribute array pointer for this storage type.
     * @param attributeIndex the attribute index to use.
     */
    static void InitializeAttributePointer(uint32_t attributeIndex)
    {
        glVertexAttribPointer(attributeIndex, sizeof(Color) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Color), nullptr);
    }

private:
    float m_r{}; //!< The color's r value.
    float m_g{}; //!< The color's g value.
    float m_b{}; //!< The color's b value.
    float m_a{}; //!< The color's a value.
};

} // namespace Renderer
} // namespace libprojectM
