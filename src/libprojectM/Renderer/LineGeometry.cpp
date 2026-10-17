#include "LineGeometry.hpp"

namespace libprojectM {
namespace Renderer {

void LineBatch::Clear()
{
    m_points.clear();
}

auto LineBatch::Append(const ColoredPoint* points, size_t count, bool loop) -> Strip
{
    Strip strip;
    strip.firstPoint = static_cast<int>(m_points.size());
    if (points == nullptr || count < 2)
    {
        return strip;
    }

    if (loop)
    {
        m_points.push_back(points[count - 1]);
        m_points.insert(m_points.end(), points, points + count);
        m_points.push_back(points[0]);
        m_points.push_back(points[1]);
        strip.segments = static_cast<int>(count);
    }
    else
    {
        m_points.push_back(points[0]);
        m_points.insert(m_points.end(), points, points + count);
        m_points.push_back(points[count - 1]);
        strip.segments = static_cast<int>(count - 1);
    }
    return strip;
}

auto LineBatch::Points() const -> const std::vector<ColoredPoint>&
{
    return m_points;
}

} // namespace Renderer
} // namespace libprojectM
