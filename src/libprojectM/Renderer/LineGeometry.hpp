#pragma once

#include <cstddef>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief A line point with its color, as stored for the instanced line renderer.
 */
struct ColoredPoint
{
    float x{}, y{}, r{}, g{}, b{}, a{};
};

/**
 * @brief Collects line strips for one upload, padded for instanced drawing.
 *
 * Every segment is one instance that reads four consecutive stored points: the point before the
 * segment, its two ends, and the point after it. A strip's first and last point are repeated (an
 * open strip ends flat there), a loop's are wrapped around.
 */
class LineBatch
{
public:
    using ColoredPoint = libprojectM::Renderer::ColoredPoint;

    /**
     * @brief Where a strip's points start in the batch and how many segments it has.
     */
    struct Strip
{
        int firstPoint{0}; //!< Index of the strip's leading pad point.
        int segments{0};   //!< Number of segments (instances); 0 draws nothing.
    };

    void Clear();

    /**
     * @brief Adds a strip of points.
     * @param points The strip's points; may be null if count is 0.
     * @param count Number of points. Fewer than two draw nothing.
     * @param loop True to connect the last point back to the first.
     */
    auto Append(const ColoredPoint* points, size_t count, bool loop) -> Strip;

    auto Points() const -> const std::vector<ColoredPoint>&;

private:
    std::vector<ColoredPoint> m_points; //!< All strips, padded, in append order.
};

} // namespace Renderer
} // namespace libprojectM
