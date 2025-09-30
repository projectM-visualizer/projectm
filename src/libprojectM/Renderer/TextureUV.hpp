#pragma once

#include <projectM-opengl.h>

#include <cstdint>

namespace libprojectM {
namespace Renderer {

/**
 * @brief A texture U/V coordinate.
 */
class TextureUV
{
public:
    TextureUV() = default;

    /**
     * Constructs a UV coordinate with the given U/V values.
     * @param u The U coordinate.
     * @param v The V coordinate.
     */
    TextureUV(float u, float v)
        : m_u(u)
        , m_v(v) {};

    /**
     * Returns the U coordinate of the texture.
     * @return The U coordinate of the texture.
     */
    auto U() const -> float
    {
        return m_u;
    }

    /**
     * Sets the U coordinate of the texture.
     * @param u The new U coordinate.
     */
    void SetU(float u)
    {
        m_u = u;
    }

    /**
     * Returns the V coordinate of the texture.
     * @return The V coordinate of the texture.
     */
    auto V() const -> float
    {
        return m_v;
    }

    /**
     * Sets the V coordinate of the texture.
     * @param v The new V coordinate.
     */
    void SetV(float v)
    {
        m_v = v;
    }

    /**
     * @brief Initializes the attribute array pointer for this storage type.
     * @param attributeIndex the attribute index to use.
     */
    static void InitializeAttributePointer(uint32_t attributeIndex)
    {
        glVertexAttribPointer(attributeIndex, sizeof(TextureUV) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(TextureUV), nullptr);
    }

private:
    float m_u{}; //!< Texture U coordinate.
    float m_v{}; //!< Texture V coordinate.
};

} // namespace Renderer
} // namespace libprojectM
