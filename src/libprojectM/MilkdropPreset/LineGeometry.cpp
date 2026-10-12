#include "LineGeometry.hpp"

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {

namespace {

constexpr float ThinHalfWidth = 0.5f; //!< MilkDrop's lines are 1 px wide.

// MilkDrop draws a thick line as the thin one four times, offset by (0, 0), (x, 0), (x, y), (0, y):
// a pixel for the main wave, about half a pixel for custom waves and shape outlines. The quads do the
// same (LineStyle::passOffsetX/Y), so every pixel is hit exactly as often as with the GL lines. A
// wider line drawn fewer times cannot match both the lit pixels and the hits per pixel (0.75 px drawn
// 3 times was 4.5 hits per column instead of 4, which feedback presets amplified to +10-30 %).
// Checked on feedback-free synthetic presets (circle, straight and 45 degree lines) against the GL lines.
// (Shape outlines on desktop GL also enable GL_LINE_SMOOTH, which GLES lacks; that is not emulated.)
constexpr int ThickPasses = 4;

} // namespace

auto LineScale(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> float
{
    if (viewportWidth <= 0 || viewportHeight <= 0 || referenceWidth <= 0 || referenceHeight <= 0)
    {
        return 0.0f;
    }
    // In double, so a render size of the reference's aspect ratio gets exactly the float the height
    // ratio gives (16:9 frames are unchanged from the height-based scale).
    const double area = static_cast<double>(viewportWidth) * static_cast<double>(viewportHeight);
    const double referenceArea = static_cast<double>(referenceWidth) * static_cast<double>(referenceHeight);
    // Below the reference area lines are drawn as at it, MilkDrop's 1 px lines at full brightness:
    // thinner lines faded by their width made presets that feed their image back go dark in testing.
    return std::max(1.0f, static_cast<float>(std::sqrt(area / referenceArea)));
}

auto SampleDecisionWidth(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> int
{
    const float scale = LineScale(viewportWidth, viewportHeight, referenceWidth, referenceHeight);
    if (scale <= 0.0f)
    {
        return viewportWidth;
    }
    return static_cast<int>(std::lround(static_cast<float>(viewportWidth) / scale));
}

auto MaximizeColorsTextureSize(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> int
{
    if (LineScale(viewportWidth, viewportHeight, referenceWidth, referenceHeight) > 1.0f)
    {
        return std::max(referenceWidth, referenceHeight);
    }
    return std::max(viewportWidth, viewportHeight);
}

auto LineStyleFor(LineKind kind, bool thick, float scale, int viewportWidth, int viewportHeight) -> LineStyle
{
    // MilkDrop's thick-line offsets in the strip's own -1...1 coordinates, before vertex_transformation:
    // 2 / viewport size (a pixel) for the main wave, 1 / viewport size (half a pixel) for custom waves
    // and shape outlines. The custom waves' vertical offset uses the width too, as in CustomWaveform's
    // GL-line path.
    const float incrementX = viewportWidth > 0 ? 1.0f / static_cast<float>(viewportWidth) : 0.0f;
    const float incrementY = viewportHeight > 0 ? 1.0f / static_cast<float>(viewportHeight) : 0.0f;

    float offsetX = 0.0f;
    float offsetY = 0.0f;
    switch (kind)
    {
        case LineKind::MainWave:
            offsetX = 2.0f * incrementX;
            offsetY = 2.0f * incrementY;
            break;
        case LineKind::CustomWave:
            offsetX = incrementX;
            offsetY = incrementX;
            break;
        case LineKind::ShapeBorder:
            offsetX = incrementX;
            offsetY = incrementY;
            break;
        case LineKind::MotionVector:
            thick = false;
            break;
    }

    LineStyle style;
    style.halfWidth = ThinHalfWidth * scale;
    style.passes = thick ? ThickPasses : 1;
    // The offsets keep their share of the picture: a pixel or half a pixel at the reference size.
    style.passOffsetX = thick ? offsetX * scale : 0.0f;
    style.passOffsetY = thick ? offsetY * scale : 0.0f;
    return style;
}

auto PassOffsetFor(const LineStyle& style, int pass) -> PassOffset
{
    // MilkDrop's iterations: as is, x += increment, y += increment, x -= increment.
    const int step = pass % 4;
    PassOffset offset;
    offset.x = (step == 1 || step == 2) ? style.passOffsetX : 0.0f;
    offset.y = (step == 2 || step == 3) ? style.passOffsetY : 0.0f;
    return offset;
}

auto DotStyleFor(LineKind kind, bool thick, float scale) -> DotStyle
{
    // MilkDrop's main wave always draws its dots as 2x2 pixel blocks, custom waves only when thick.
    const float base = (kind == LineKind::MainWave || thick) ? 2.0f : 1.0f;
    const float size = base * scale;

    DotStyle style;
    // Points rasterise to whole pixels: draw the next whole size and fade by the area difference,
    // so a dot keeps its share of the picture at every render size.
    style.size = std::max(1.0f, std::ceil(size - 0.0001f));
    style.alphaScale = (size * size) / (style.size * style.size);
    return style;
}

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

} // namespace MilkdropPreset
} // namespace libprojectM
