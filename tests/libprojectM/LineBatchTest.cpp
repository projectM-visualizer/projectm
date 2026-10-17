#include <gtest/gtest.h>

#include <Renderer/LineGeometry.hpp>

#include <vector>

using libprojectM::Renderer::LineBatch;
using Point = libprojectM::Renderer::ColoredPoint;

namespace {

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
