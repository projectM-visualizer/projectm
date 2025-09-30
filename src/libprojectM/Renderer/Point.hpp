#pragma once

#include <projectM-opengl.h>

#include <cstdint>

namespace libprojectM {
namespace Renderer {

/**
 * @brief A 2D point.
 */
class Point
{
public:
    Point() = default;

    /**
     * Constructs a point with the given coordinates.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     */
    Point(float x, float y)
        : m_x(x)
        , m_y(y) {};

    /**
     * Returns the X coordinate of the point.
     * @return The X coordinate of the point.
     */
    auto X() const -> float
    {
        return m_x;
    }

    /**
     * Sets the X coordinate of the point.
     * @param x The new X coordinate.
     */
    void SetX(float x)
    {
        m_x = x;
    }

    /**
     * Returns the Y coordinate of the point.
     * @return The Y coordinate of the point.
     */
    auto Y() const -> float
    {
        return m_y;
    }

    /**
     * Sets the Y coordinate of the point.
     * @param y The new Y coordinate.
     */
    void SetY(float y)
    {
        m_y = y;
    }

    /**
     * @brief Initializes the attribute array pointer for this storage type.
     * @param attributeIndex the attribute index to use.
     */
    static void InitializeAttributePointer(uint32_t attributeIndex)
    {
        glVertexAttribPointer(attributeIndex, sizeof(Point) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Point), nullptr);
    }

private:
    float m_x{}; //!< Vertex X coordinate.
    float m_y{}; //!< Vertex Y coordinate.
};

} // namespace Renderer
} // namespace libprojectM
