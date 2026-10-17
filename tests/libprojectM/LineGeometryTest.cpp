#include <gtest/gtest.h>

#include <MilkdropPreset/LineGeometry.hpp>

#include <algorithm>
#include <cmath>

using libprojectM::MilkdropPreset::BlurSourceFor;
using libprojectM::MilkdropPreset::DotStyleFor;
using libprojectM::MilkdropPreset::LineKind;
using libprojectM::MilkdropPreset::LineScale;
using libprojectM::MilkdropPreset::LineStyleFor;
using libprojectM::MilkdropPreset::MaximizeColorsTextureSize;
using libprojectM::MilkdropPreset::PassOffsetFor;
using libprojectM::MilkdropPreset::SampleDecisionWidth;
using libprojectM::MilkdropPreset::ShaderCanvasSize;

namespace {

constexpr int Width = 1920;
constexpr int Height = 1080;

} // namespace

TEST(LineGeometry, ScaleIsZeroWithoutReferenceOrViewport)
{
    EXPECT_EQ(LineScale(1920, 1080, 0, 1080), 0.0f);
    EXPECT_EQ(LineScale(1920, 1080, 1920, 0), 0.0f);
    EXPECT_EQ(LineScale(1920, 1080, 0, 0), 0.0f);
    EXPECT_EQ(LineScale(1920, 1080, -1, 1080), 0.0f);
    EXPECT_EQ(LineScale(1920, 1080, 1920, -1), 0.0f);
    EXPECT_EQ(LineScale(0, 1080, 1920, 1080), 0.0f);
    EXPECT_EQ(LineScale(1920, 0, 1920, 1080), 0.0f);
    EXPECT_EQ(LineScale(-5, 1080, 1920, 1080), 0.0f);
}

// At 16:9 the area rule gives what the height rule gave: the render height over the reference height.
TEST(LineGeometry, ScaleAtSixteenByNineFollowsTheHeight)
{
    EXPECT_FLOAT_EQ(LineScale(1920, 1080, 1920, 1080), 1.0f);
    EXPECT_FLOAT_EQ(LineScale(2560, 1440, 1920, 1080), 1440.0f / 1080.0f);
    EXPECT_FLOAT_EQ(LineScale(3840, 2160, 1920, 1080), 2.0f);
    // Exactly the height rule's value, so 16:9 frames stay byte-identical.
    for (int height : {1080, 1152, 1440, 1620, 1800, 2160, 2880})
    {
        const int width = height * 16 / 9;
        EXPECT_EQ(LineScale(width, height, 1920, 1080), std::max(1.0f, static_cast<float>(height) / 1080.0f)) << height;
    }
}

// A line covers the same share of the picture at any aspect ratio: the scale is the square root of
// the render area over the reference area.
TEST(LineGeometry, ScaleFollowsTheRenderArea)
{
    EXPECT_FLOAT_EQ(LineScale(2560, 1080, 1920, 1080), std::sqrt(2560.0f / 1920.0f)); // 21:9, 1.155
    EXPECT_FLOAT_EQ(LineScale(5120, 2160, 1920, 1080), 2.0f * std::sqrt(2560.0f / 1920.0f));
    EXPECT_FLOAT_EQ(LineScale(2880, 2160, 1920, 1080), std::sqrt(2880.0f * 2160.0f / (1920.0f * 1080.0f))); // 4:3, 1.732
    EXPECT_FLOAT_EQ(LineScale(1440, 2560, 1920, 1080), std::sqrt(1440.0f * 2560.0f / (1920.0f * 1080.0f))); // portrait, 1.333
    EXPECT_FLOAT_EQ(LineScale(2160, 3840, 1920, 1080), 2.0f);                                               // portrait 4K, same area as landscape
    EXPECT_FLOAT_EQ(LineScale(2560, 1440, 2560, 1440), 1.0f);
}

// At or below the reference area lines are drawn as at it (MilkDrop's 1 px lines at full brightness):
// thinner, faded lines made feedback presets go dark.
TEST(LineGeometry, ScaleNeverDropsBelowOne)
{
    EXPECT_FLOAT_EQ(LineScale(1280, 720, 1920, 1080), 1.0f);
    EXPECT_FLOAT_EQ(LineScale(854, 480, 1920, 1080), 1.0f);
    EXPECT_FLOAT_EQ(LineScale(1440, 1080, 1920, 1080), 1.0f); // 4:3 at the reference height: 0.866 by area
    EXPECT_FLOAT_EQ(LineScale(1080, 1920, 1920, 1080), 1.0f); // portrait, same area
    EXPECT_FLOAT_EQ(LineScale(1, 1, 1920, 1080), 1.0f);
}

// MilkDrop's line waveforms draw a third of their samples when the render is narrower than three
// times the sample count (under 1440 px for 480 samples). Without a line reference size the rule sees
// the render width, as in MilkDrop.
TEST(LineGeometry, SampleDecisionWidthIsTheRenderWidthWithoutReference)
{
    EXPECT_EQ(SampleDecisionWidth(1920, 1080, 0, 0), 1920);
    EXPECT_EQ(SampleDecisionWidth(1024, 768, 0, 0), 1024);
    EXPECT_EQ(SampleDecisionWidth(3840, 2160, 0, 768), 3840);
    EXPECT_EQ(SampleDecisionWidth(3840, 2160, 1024, 0), 3840);
    EXPECT_EQ(SampleDecisionWidth(0, 0, 1024, 768), 0);
}

// With a reference size the rule sees the reference-equivalent width (render width / line scale), so
// a 1080p or 4K render draws as many samples as MilkDrop does at the reference size.
TEST(LineGeometry, SampleDecisionWidthFollowsTheLineScale)
{
    EXPECT_EQ(SampleDecisionWidth(1920, 1080, 1024, 768),
              static_cast<int>(std::lround(1920.0f / LineScale(1920, 1080, 1024, 768)))); // 1920 / 1.624
    EXPECT_EQ(SampleDecisionWidth(1920, 1080, 1024, 768), 1182);
    EXPECT_EQ(SampleDecisionWidth(3840, 2160, 1024, 768), 1182);
    EXPECT_EQ(SampleDecisionWidth(1024, 768, 1024, 768), 1024);
    EXPECT_EQ(SampleDecisionWidth(1920, 1080, 1920, 1080), 1920);
    EXPECT_EQ(SampleDecisionWidth(3840, 2160, 1920, 1080), 1920);
    // 480 samples: a third below 1440 px. MilkDrop at 1024x768 draws 160; so does a 1080p or 4K render
    // with that reference. With the 1080p reference they keep all 480.
    EXPECT_LT(SampleDecisionWidth(3840, 2160, 1024, 768), 3 * 480);
    EXPECT_GE(SampleDecisionWidth(3840, 2160, 1920, 1080), 3 * 480);
}

// The line scale is at least 1, so the width never exceeds the render width (smaller renders keep it).
TEST(LineGeometry, SampleDecisionWidthNeverExceedsTheRenderWidth)
{
    EXPECT_EQ(SampleDecisionWidth(800, 600, 1024, 768), 800);
    EXPECT_EQ(SampleDecisionWidth(1280, 720, 1920, 1080), 1280);
    for (int width : {320, 1024, 1280, 1366, 1920, 2560, 3840, 7680})
    {
        for (int height : {240, 720, 768, 1080, 1440, 2160, 4320})
        {
            EXPECT_LE(SampleDecisionWidth(width, height, 1024, 768), width) << width << "x" << height;
            EXPECT_GT(SampleDecisionWidth(width, height, 1024, 768), 0) << width << "x" << height;
        }
    }
}

// MilkDrop fades the spiro and hash waves by its texture size, max(width, height). Without a reference
// size (GL lines) that is the render size, as in MilkDrop.
TEST(LineGeometry, MaximizeColorsTextureSizeIsTheRenderSizeWithoutReference)
{
    EXPECT_EQ(MaximizeColorsTextureSize(1920, 1080, 0, 0), 1920);
    EXPECT_EQ(MaximizeColorsTextureSize(768, 1024, 0, 0), 1024);
    EXPECT_EQ(MaximizeColorsTextureSize(3840, 2160, 0, 768), 3840);
    EXPECT_EQ(MaximizeColorsTextureSize(3840, 2160, 1024, 0), 3840);
}

// Above the reference area the lines keep the reference's share of the picture, so the fade is the
// reference's (the size the preset was made for), not the render's.
TEST(LineGeometry, MaximizeColorsTextureSizeFollowsTheReferenceAboveIt)
{
    EXPECT_EQ(MaximizeColorsTextureSize(1920, 1080, 1024, 768), 1024);
    EXPECT_EQ(MaximizeColorsTextureSize(3840, 2160, 1024, 768), 1024);
    EXPECT_EQ(MaximizeColorsTextureSize(3840, 2160, 1920, 1080), 1920);
    EXPECT_EQ(MaximizeColorsTextureSize(1080, 1920, 768, 1024), 1024);
}

// At or below the reference area the lines are MilkDrop's 1 px lines, and so is the fade.
TEST(LineGeometry, MaximizeColorsTextureSizeIsTheRenderSizeAtOrBelowTheReference)
{
    EXPECT_EQ(MaximizeColorsTextureSize(1024, 768, 1024, 768), 1024);
    EXPECT_EQ(MaximizeColorsTextureSize(800, 600, 1024, 768), 800);
    EXPECT_EQ(MaximizeColorsTextureSize(1182, 665, 1024, 768), 1182); // area just under the reference's
    EXPECT_EQ(MaximizeColorsTextureSize(1280, 720, 1920, 1080), 1280);
}

// Without line scaling the blur levels are made from the source itself, as in MilkDrop.
TEST(LineGeometry, BlurSourceIsTheSourceWithoutScale)
{
    for (float scale : {0.0f, 1.0f})
    {
        const auto source = BlurSourceFor(1920, 1080, scale);
        EXPECT_EQ(source.width, 1920);
        EXPECT_EQ(source.height, 1080);
        EXPECT_EQ(source.lod, 0.0f);
    }
}

// Above the reference area the blur levels are made as from a source of the reference's area (the
// source divided by the line scale), read from the source's mipmaps at log2 of that scale.
TEST(LineGeometry, BlurSourceFollowsTheLineScale)
{
    const auto hd = BlurSourceFor(1920, 1080, LineScale(1920, 1080, 1024, 768));
    EXPECT_EQ(hd.width, 1182);
    EXPECT_EQ(hd.height, 665);
    EXPECT_NEAR(hd.lod, std::log2(1920.0f / 1182.0f), 1e-6f);

    const auto uhd = BlurSourceFor(3840, 2160, LineScale(3840, 2160, 1024, 768));
    EXPECT_EQ(uhd.width, 1182);
    EXPECT_EQ(uhd.height, 665);
    EXPECT_NEAR(uhd.lod, std::log2(3840.0f / 1182.0f), 1e-6f);

    const auto ref4x3 = BlurSourceFor(2048, 1536, LineScale(2048, 1536, 1024, 768));
    EXPECT_EQ(ref4x3.width, 1024);
    EXPECT_EQ(ref4x3.height, 768);
    EXPECT_FLOAT_EQ(ref4x3.lod, 1.0f);

    const auto ref1080 = BlurSourceFor(3840, 2160, LineScale(3840, 2160, 1920, 1080));
    EXPECT_EQ(ref1080.width, 1920);
    EXPECT_EQ(ref1080.height, 1080);
    EXPECT_FLOAT_EQ(ref1080.lod, 1.0f);
}

// The scaled source is never empty and never larger than the source.
TEST(LineGeometry, BlurSourceStaysWithinTheSource)
{
    for (float scale : {1.01f, 1.5f, 3.0f, 100.0f, 10000.0f})
    {
        const auto source = BlurSourceFor(3840, 2160, scale);
        EXPECT_GE(source.width, 1) << scale;
        EXPECT_GE(source.height, 1) << scale;
        EXPECT_LE(source.width, 3840) << scale;
        EXPECT_LE(source.height, 2160) << scale;
        EXPECT_GE(source.lod, 0.0f) << scale;
    }
}

// Without a reference size, and at or below its area, presets see the render size (texsize, pixelsx/y),
// as in MilkDrop.
TEST(LineGeometry, ShaderCanvasSizeIsTheRenderSizeWithoutScale)
{
    const auto noReference = ShaderCanvasSize(3840, 2160, 0, 0);
    EXPECT_EQ(noReference.width, 3840);
    EXPECT_EQ(noReference.height, 2160);

    const auto partialReference = ShaderCanvasSize(3840, 2160, 1024, 0);
    EXPECT_EQ(partialReference.width, 3840);
    EXPECT_EQ(partialReference.height, 2160);

    const auto atReference = ShaderCanvasSize(1024, 768, 1024, 768);
    EXPECT_EQ(atReference.width, 1024);
    EXPECT_EQ(atReference.height, 768);

    const auto belowReference = ShaderCanvasSize(1182, 665, 1024, 768); // area just under the reference's
    EXPECT_EQ(belowReference.width, 1182);
    EXPECT_EQ(belowReference.height, 665);

    const auto small = ShaderCanvasSize(640, 360, 1024, 768);
    EXPECT_EQ(small.width, 640);
    EXPECT_EQ(small.height, 360);
}

// Above the reference area presets see the reference's area at the render's aspect ratio (the render
// size divided by the line scale), so texel steps and uv-to-pixel factors keep their share of the
// picture. It is the size the blur levels are made from.
TEST(LineGeometry, ShaderCanvasSizeFollowsTheReferenceAboveIt)
{
    const auto hd = ShaderCanvasSize(1920, 1080, 1024, 768);
    EXPECT_EQ(hd.width, 1182);
    EXPECT_EQ(hd.height, 665);

    const auto uhd = ShaderCanvasSize(3840, 2160, 1024, 768);
    EXPECT_EQ(uhd.width, 1182);
    EXPECT_EQ(uhd.height, 665);

    const auto ref4x3 = ShaderCanvasSize(2048, 1536, 1024, 768);
    EXPECT_EQ(ref4x3.width, 1024);
    EXPECT_EQ(ref4x3.height, 768);

    const auto portrait = ShaderCanvasSize(1080, 1920, 768, 1024);
    EXPECT_EQ(portrait.width, 665);
    EXPECT_EQ(portrait.height, 1182);

    for (int height : {1260, 1330, 1440})
    {
        const int width = height * 16 / 9;
        const auto canvas = ShaderCanvasSize(width, height, 1024, 768);
        const auto blur = BlurSourceFor(width, height, LineScale(width, height, 1024, 768));
        EXPECT_EQ(canvas.width, blur.width) << height;
        EXPECT_EQ(canvas.height, blur.height) << height;
    }
}

// The reported canvas is never empty and never larger than the render.
TEST(LineGeometry, ShaderCanvasSizeStaysWithinTheRender)
{
    for (int reference : {1, 2, 100, 1024})
    {
        const auto canvas = ShaderCanvasSize(3840, 2160, reference, reference);
        EXPECT_GE(canvas.width, 1) << reference;
        EXPECT_GE(canvas.height, 1) << reference;
        EXPECT_LE(canvas.width, 3840) << reference;
        EXPECT_LE(canvas.height, 2160) << reference;
    }
}

TEST(LineGeometry, ThinLinesAreOnePixelAtReference)
{
    for (auto kind : {LineKind::MainWave, LineKind::CustomWave, LineKind::ShapeBorder, LineKind::MotionVector})
    {
        const auto style = LineStyleFor(kind, false, 1.0f, Width, Height);
        EXPECT_FLOAT_EQ(style.halfWidth, 0.5f);
        EXPECT_EQ(style.passes, 1);
    }
}

TEST(LineGeometry, MainWaveThickIsFourOffsetThinLines)
{
    // MilkDrop offsets the main wave by 2 / viewport size on each axis in its -1...1 coordinates
    // (a whole pixel), and draws the 1 px line four times.
    const auto style = LineStyleFor(LineKind::MainWave, true, 1.0f, Width, Height);
    EXPECT_FLOAT_EQ(style.halfWidth, 0.5f);
    EXPECT_EQ(style.passes, 4);
    EXPECT_FLOAT_EQ(style.passOffsetX, 2.0f / Width);
    EXPECT_FLOAT_EQ(style.passOffsetY, 2.0f / Height);
}

TEST(LineGeometry, MainWaveThickOffsetsFollowTheViewport)
{
    const auto style = LineStyleFor(LineKind::MainWave, true, 1.0f, 1280, 1024);
    EXPECT_FLOAT_EQ(style.passOffsetX, 2.0f / 1280.0f);
    EXPECT_FLOAT_EQ(style.passOffsetY, 2.0f / 1024.0f);
    const auto none = LineStyleFor(LineKind::MainWave, true, 1.0f, 0, 0);
    EXPECT_EQ(none.passOffsetX, 0.0f);
    EXPECT_EQ(none.passOffsetY, 0.0f);
}

TEST(LineGeometry, ThinLinesHaveNoPassOffset)
{
    for (auto kind : {LineKind::MainWave, LineKind::CustomWave, LineKind::ShapeBorder, LineKind::MotionVector})
    {
        const auto style = LineStyleFor(kind, false, 1.0f, Width, Height);
        EXPECT_EQ(style.passOffsetX, 0.0f);
        EXPECT_EQ(style.passOffsetY, 0.0f);
    }
}

TEST(LineGeometry, CustomWaveThickIsFourOffsetThinLines)
{
    // MilkDrop offsets custom waves by 1 / viewport width on both axes, in the strip's own -1...1
    // coordinates (before vertex_transformation).
    const auto style = LineStyleFor(LineKind::CustomWave, true, 1.0f, Width, Height);
    EXPECT_FLOAT_EQ(style.halfWidth, 0.5f);
    EXPECT_EQ(style.passes, 4);
    EXPECT_FLOAT_EQ(style.passOffsetX, 1.0f / Width);
    EXPECT_FLOAT_EQ(style.passOffsetY, 1.0f / Width);
}

TEST(LineGeometry, ShapeBorderThickIsFourOffsetThinLines)
{
    // MilkDrop offsets shape outlines by 1 / viewport width and 1 / viewport height, in the strip's
    // own -1...1 coordinates (before vertex_transformation).
    const auto style = LineStyleFor(LineKind::ShapeBorder, true, 1.0f, Width, Height);
    EXPECT_FLOAT_EQ(style.halfWidth, 0.5f);
    EXPECT_EQ(style.passes, 4);
    EXPECT_FLOAT_EQ(style.passOffsetX, 1.0f / Width);
    EXPECT_FLOAT_EQ(style.passOffsetY, 1.0f / Height);
}

TEST(LineGeometry, PassOffsetsScaleWithTheLines)
{
    const float scale = 720.0f / 1080.0f;
    const auto style = LineStyleFor(LineKind::CustomWave, true, scale, 1280, 720);
    EXPECT_FLOAT_EQ(style.halfWidth, 0.5f * scale);
    EXPECT_FLOAT_EQ(style.passOffsetX, scale / 1280.0f);
    EXPECT_FLOAT_EQ(style.passOffsetY, scale / 1280.0f);
    const auto mainWave = LineStyleFor(LineKind::MainWave, true, 2.0f, 3840, 2160);
    EXPECT_FLOAT_EQ(mainWave.halfWidth, 1.0f);
    EXPECT_FLOAT_EQ(mainWave.passOffsetX, 2.0f * 2.0f / 3840.0f);
    EXPECT_FLOAT_EQ(mainWave.passOffsetY, 2.0f * 2.0f / 2160.0f);
}

TEST(LineGeometry, NoPassOffsetWithoutViewport)
{
    const auto style = LineStyleFor(LineKind::ShapeBorder, true, 1.0f, 0, 0);
    EXPECT_EQ(style.passOffsetX, 0.0f);
    EXPECT_EQ(style.passOffsetY, 0.0f);
}

TEST(LineGeometry, PassOffsetsFollowMilkDropsOrder)
{
    // (0, 0), (+x, 0), (+x, +y), (0, +y), as MilkDrop's four thick-line iterations.
    const auto style = LineStyleFor(LineKind::ShapeBorder, true, 1.0f, Width, Height);
    const float x = 1.0f / Width;
    const float y = 1.0f / Height;
    const float expected[5][2] = {{0.0f, 0.0f}, {x, 0.0f}, {x, y}, {0.0f, y}, {0.0f, 0.0f}};
    for (int pass = 0; pass < 5; pass++)
    {
        const auto offset = PassOffsetFor(style, pass);
        EXPECT_FLOAT_EQ(offset.x, expected[pass][0]) << "pass " << pass;
        EXPECT_FLOAT_EQ(offset.y, expected[pass][1]) << "pass " << pass;
    }
    const auto thin = LineStyleFor(LineKind::CustomWave, false, 1.0f, Width, Height);
    EXPECT_EQ(PassOffsetFor(thin, 0).x, 0.0f);
    EXPECT_EQ(PassOffsetFor(thin, 0).y, 0.0f);
}

TEST(LineGeometry, WidthScalesButPassesDoNot)
{
    const auto style = LineStyleFor(LineKind::MainWave, true, 2.0f, Width, Height);
    EXPECT_FLOAT_EQ(style.halfWidth, 1.0f);
    EXPECT_EQ(style.passes, 4);
    EXPECT_FLOAT_EQ(LineStyleFor(LineKind::CustomWave, false, 0.5f, Width, Height).halfWidth, 0.25f);
}

TEST(LineGeometry, MotionVectorsIgnoreThick)
{
    const auto style = LineStyleFor(LineKind::MotionVector, true, 1.0f, Width, Height);
    EXPECT_FLOAT_EQ(style.halfWidth, 0.5f);
    EXPECT_EQ(style.passes, 1);
}

TEST(LineGeometry, MainWaveDotsAreTwoPixelBlocks)
{
    const auto dot = DotStyleFor(LineKind::MainWave, false, 1.0f);
    EXPECT_FLOAT_EQ(dot.size, 2.0f);
    EXPECT_FLOAT_EQ(dot.alphaScale, 1.0f);
}

TEST(LineGeometry, CustomWaveDotsAreTwoPixelsOnlyWhenThick)
{
    EXPECT_FLOAT_EQ(DotStyleFor(LineKind::CustomWave, false, 1.0f).size, 1.0f);
    EXPECT_FLOAT_EQ(DotStyleFor(LineKind::CustomWave, true, 1.0f).size, 2.0f);
}

TEST(LineGeometry, DotsBelowOnePixelFadeByArea)
{
    const auto dot = DotStyleFor(LineKind::CustomWave, false, 0.5f);
    EXPECT_FLOAT_EQ(dot.size, 1.0f);
    EXPECT_FLOAT_EQ(dot.alphaScale, 0.25f);
    const auto mainDot = DotStyleFor(LineKind::MainWave, false, 0.25f); // 2 px * 0.25
    EXPECT_FLOAT_EQ(mainDot.size, 1.0f);
    EXPECT_FLOAT_EQ(mainDot.alphaScale, 0.25f);
}

TEST(LineGeometry, DotsRoundUpToWholePixelsAndFadeByArea)
{
    const auto mainDot = DotStyleFor(LineKind::MainWave, false, 2.0f / 3.0f); // 1.33 px
    EXPECT_FLOAT_EQ(mainDot.size, 2.0f);
    EXPECT_NEAR(mainDot.alphaScale, 0.4444f, 0.0001f);
    const auto customDot = DotStyleFor(LineKind::CustomWave, true, 1.5f); // 3 px wanted
    EXPECT_FLOAT_EQ(customDot.size, 3.0f);
    EXPECT_FLOAT_EQ(customDot.alphaScale, 1.0f);
    const auto thinDot = DotStyleFor(LineKind::CustomWave, false, 1.5f); // 1.5 px
    EXPECT_FLOAT_EQ(thinDot.size, 2.0f);
    EXPECT_FLOAT_EQ(thinDot.alphaScale, 0.5625f);
    const auto exact = DotStyleFor(LineKind::MainWave, false, 1.0f);
    EXPECT_EQ(exact.size, 2.0f);
    EXPECT_EQ(exact.alphaScale, 1.0f);
}
