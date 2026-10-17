#pragma once

#include <Renderer/LineGeometry.hpp>

namespace libprojectM {
namespace Renderer {
class LineRenderer;
}
namespace MilkdropPreset {

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
 * @brief Size and mipmap level of the source the blur levels are made from.
 */
struct BlurSource
{
    int width{0};    //!< Width the blur chain treats its source as having.
    int height{0};   //!< Height the blur chain treats its source as having.
    float lod{0.0f}; //!< Mipmap level the first blur pass reads the source at (0: the source itself).
};

/**
 * @brief The source the blur levels are made from: the source itself, or above the line reference
 *        area a source of the reference's area (the source size divided by the line scale), read from
 *        the source's mipmaps at log2 of the actual ratio.
 *
 * MilkDrop's blur levels are fixed fractions of its texture, blurred by a few of their texels, so their
 * radius is a share of the picture only at one render size. With the reference's size they keep the
 * share they had at the reference, like the lines.
 * @param referenceScale LineScale(); 0 or 1 keep the source (MilkDrop).
 */
auto BlurSourceFor(int sourceWidth, int sourceHeight, float referenceScale) -> BlurSource;

/**
 * @brief Canvas size in pixels as told to preset code: texsize, texsize_main, mip_x/y/avg and
 *        pixelsx/pixelsy.
 */
struct CanvasSize
{
    int width{0};
    int height{0};
};

/**
 * @brief The canvas size preset shaders and equations see: above the line reference area the
 *        reference's area at the render's aspect ratio (the render size divided by LineScale(), the
 *        size BlurSourceFor() makes the blur levels from), otherwise the render size.
 *
 * In MilkDrop texsize is the internal canvas, and presets step by texsize.zw texels and turn uv into
 * pixels with texsize.xy, tuned on a canvas of about the reference size. The textures stay at the
 * render size and are sampled as before; only the reported size follows the reference, so the steps
 * and frequencies keep their share of the picture, like the lines.
 * @return The render size when quad lines are off (no reference size), or at or below the reference
 *         area, as in MilkDrop.
 */
auto ShaderCanvasSize(int viewportWidth, int viewportHeight, int referenceWidth, int referenceHeight) -> CanvasSize;

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

/** Draws the MilkDrop style's thin or four offset thick-line passes. */
void DrawLine(Renderer::LineRenderer& renderer, const Renderer::LineBatch::Strip& strip, const LineStyle& style);

} // namespace MilkdropPreset
} // namespace libprojectM
