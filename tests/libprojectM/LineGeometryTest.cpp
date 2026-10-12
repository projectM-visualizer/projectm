#include <gtest/gtest.h>

#include <MilkdropPreset/LineGeometry.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using libprojectM::MilkdropPreset::DotStyleFor;
using libprojectM::MilkdropPreset::LineBatch;
using libprojectM::MilkdropPreset::LineKind;
using libprojectM::MilkdropPreset::LineScale;
using libprojectM::MilkdropPreset::LineStyleFor;
using libprojectM::MilkdropPreset::MaximizeColorsTextureSize;
using libprojectM::MilkdropPreset::PassOffsetFor;
using libprojectM::MilkdropPreset::SampleDecisionWidth;
using Point = libprojectM::MilkdropPreset::ColoredPoint;

namespace {

constexpr int Width = 1920;
constexpr int Height = 1080;

auto MakePoints(std::initializer_list<float> xs) -> std::vector<Point>
{
    std::vector<Point> points;
    for (float x : xs)
    {
        Point point;
        point.x = x;
        point.y = -x;
        point.a = 1.0f;
        points.push_back(point);
    }
    return points;
}

auto Xs(const LineBatch& batch) -> std::vector<float>
{
    std::vector<float> xs;
    for (const auto& point : batch.Points())
    {
        xs.push_back(point.x);
    }
    return xs;
}

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
    EXPECT_FLOAT_EQ(LineScale(2160, 3840, 1920, 1080), 2.0f); // portrait 4K, same area as landscape
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

TEST(LineBatch, OpenStripRepeatsItsEndPoints)
{
    LineBatch batch;
    const auto points = MakePoints({1, 2, 3});
    const auto strip = batch.Append(points.data(), points.size(), false);
    EXPECT_EQ(strip.firstPoint, 0);
    EXPECT_EQ(strip.segments, 2);
    EXPECT_EQ(Xs(batch), (std::vector<float>{1, 1, 2, 3, 3}));
}

TEST(LineBatch, LoopWrapsAround)
{
    LineBatch batch;
    const auto points = MakePoints({1, 2, 3});
    const auto strip = batch.Append(points.data(), points.size(), true);
    EXPECT_EQ(strip.segments, 3);
    EXPECT_EQ(Xs(batch), (std::vector<float>{3, 1, 2, 3, 1, 2}));
}

TEST(LineBatch, TwoPointLoopHasTwoSegments)
{
    LineBatch batch;
    const auto points = MakePoints({1, 2});
    const auto strip = batch.Append(points.data(), points.size(), true);
    EXPECT_EQ(strip.segments, 2);
    EXPECT_EQ(Xs(batch), (std::vector<float>{2, 1, 2, 1, 2}));
}

TEST(LineBatch, FewerThanTwoPointsDrawNothing)
{
    LineBatch batch;
    const auto one = MakePoints({1});
    EXPECT_EQ(batch.Append(one.data(), one.size(), false).segments, 0);
    EXPECT_EQ(batch.Append(one.data(), one.size(), true).segments, 0);
    EXPECT_EQ(batch.Append(nullptr, 0, false).segments, 0);
    EXPECT_TRUE(batch.Points().empty());
}

TEST(LineBatch, StripsFollowEachOther)
{
    LineBatch batch;
    const auto first = MakePoints({1, 2});
    const auto second = MakePoints({5, 6, 7});
    batch.Append(first.data(), first.size(), false);
    const auto strip = batch.Append(second.data(), second.size(), true);
    EXPECT_EQ(strip.firstPoint, 4);
    EXPECT_EQ(batch.Points().size(), 4u + 6u);
}

TEST(LineBatch, LastInstanceReadsInsideItsStrip)
{
    // Instance i reads points firstPoint + i ... firstPoint + i + 3.
    for (bool loop : {false, true})
    {
        LineBatch batch;
        const auto points = MakePoints({1, 2, 3, 4, 5});
        const auto strip = batch.Append(points.data(), points.size(), loop);
        EXPECT_EQ(static_cast<size_t>(strip.firstPoint + (strip.segments - 1) + 3), batch.Points().size() - 1);
    }
}

TEST(LineBatch, KeepsColors)
{
    LineBatch batch;
    auto points = MakePoints({1, 2});
    points[1].r = 0.25f;
    points[1].a = 0.5f;
    batch.Append(points.data(), points.size(), false);
    EXPECT_FLOAT_EQ(batch.Points()[2].r, 0.25f);
    EXPECT_FLOAT_EQ(batch.Points()[3].a, 0.5f);
}

TEST(LineBatch, ClearEmpties)
{
    LineBatch batch;
    const auto points = MakePoints({1, 2});
    batch.Append(points.data(), points.size(), false);
    batch.Clear();
    EXPECT_TRUE(batch.Points().empty());
    EXPECT_EQ(batch.Append(points.data(), points.size(), false).firstPoint, 0);
}
