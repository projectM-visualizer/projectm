#pragma once

#include <Renderer/OpenGL.h>

#include <cstddef>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief A line point with its color, as stored for the instanced line renderer.
 */
struct ColoredPoint
{
    float x{}, y{}, r{}, g{}, b{}, a{};
};

/**
 * @brief The kinds of lines a preset draws. Each emulates MilkDrop's "thick" lines differently.
 */
enum class LineKind
{
    MainWave,
    CustomWave,
    ShapeBorder,
    MotionVector
};

/**
 * @brief Width and pass count of a quad line.
 */
struct LineStyle
{
    float halfWidth{0.0f}; //!< Half the line width in render pixels.
    int passes{1};         //!< How often the line is drawn (MilkDrop draws thick lines four times).
    /**
     * Offset of passes 1-3 in the strip's own coordinates (the -1...1 space of the points, before
     * vertex_transformation), where MilkDrop adds its thick-line increments: passes 0-3 are moved by
     * (0, 0), (passOffsetX, 0), (passOffsetX, passOffsetY), (0, passOffsetY), see PassOffsetFor().
     * Thick lines use it; 0 otherwise.
     */
    float passOffsetX{0.0f};
    float passOffsetY{0.0f}; //!< See passOffsetX.
};

/**
 * @brief Translation of one pass of a line, in the strip's own coordinates.
 */
struct PassOffset
{
    float x{0.0f};
    float y{0.0f};
};

/**
 * @brief Size of a dot drawn as a GL point.
 */
struct DotStyle
{
    float size{1.0f};       //!< gl_PointSize: a whole number of render pixels, at least 1.
    float alphaScale{1.0f}; //!< Alpha factor: wanted dot area divided by drawn area.
};

/**
 * @brief Line width factor: the square root of the render area over the reference area, but at least 1.
 *
 * A line keeps its share of the picture at any render size and aspect ratio; at the reference's
 * aspect ratio this is the render height over the reference height. At or below the reference area
 * lines are drawn as at it (MilkDrop's 1 px lines at full brightness), above it they widen.
 * @return 0 when quad lines are off (no reference size) or the viewport is empty.
 */
auto LineScale(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> float;

/**
 * @brief Render width for MilkDrop's waveform sample rule: a third of the samples when there are more
 *        than a third of this width (480 samples: under 1440 px).
 *
 * MilkDrop's rule makes the sample count depend on the render size: at its 1024x768 a line waveform
 * draws 160 dots or segments, at 1080p 480. With a line reference size the lines and dots are sized as
 * at the reference, so the sample count must follow the reference too, or a large render draws three
 * times the dots and feedback presets wash out. The rule then uses the reference-equivalent width,
 * render width / LineScale() (never above the render width).
 * @return The render width when quad lines are off (no reference size), as in MilkDrop.
 */
auto SampleDecisionWidth(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> int;

/**
 * @brief Texture size for MilkDrop's spiro and hash wave fade (Waveform::MaximizeColors()).
 *
 * MilkDrop fades the CenteredSpiro, ExplosiveHash and CenteredSpiroVolume modes by its texture size,
 * max(width, height): a bigger texture spreads the dense wave over more pixels. Quad lines keep the
 * wave's share of the picture as at the line reference size, so above it the fade is the reference's,
 * max(referenceWidth, referenceHeight); otherwise (no reference size, or at or below its area) the
 * render's, as in MilkDrop.
 */
auto MaximizeColorsTextureSize(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> int;

/**
 * @brief Quad line style matching MilkDrop's thin (1 px) or thick line of this kind, at this scale.
 * @param viewportWidth Render width in pixels; MilkDrop's thick-line offsets depend on it.
 * @param viewportHeight Render height in pixels; MilkDrop's thick-line offsets depend on it.
 */
auto LineStyleFor(LineKind kind, bool thick, float scale, int viewportWidth, int viewportHeight) -> LineStyle;

/**
 * @brief The translation of one pass: (0, 0), (x, 0), (x, y), (0, y) for passes 0-3 (repeating), in
 *        the order of MilkDrop's four thick-line iterations, with x, y = passOffsetX, passOffsetY.
 */
auto PassOffsetFor(const LineStyle& style, int pass) -> PassOffset;

/**
 * @brief Point size matching MilkDrop's dots of this kind, at this scale.
 */
auto DotStyleFor(LineKind kind, bool thick, float scale) -> DotStyle;

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
    using ColoredPoint = libprojectM::MilkdropPreset::ColoredPoint;

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

} // namespace MilkdropPreset
} // namespace libprojectM
