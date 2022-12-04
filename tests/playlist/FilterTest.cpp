#include <Filter.hpp>

#include <gtest/gtest.h>

TEST(projectMPlaylistFilter, List)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-TestString.milk",
                    "+AnotherTestString*"});

    const auto& filters = filter.List();
    ASSERT_EQ(filters.size(), 2);

    EXPECT_EQ(filters.at(0), "-TestString.milk");
    EXPECT_EQ(filters.at(1), "+AnotherTestString*");
}


TEST(projectMPlaylistFilter, ExactMatchExclude)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-TestString.milk"});

    EXPECT_FALSE(filter.Passes("TestString.milk"));
    EXPECT_TRUE(filter.Passes("Teststring.milk"));
}


TEST(projectMPlaylistFilter, ExactMatchExcludePath)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-/path/to/TestString.milk"});

    EXPECT_FALSE(filter.Passes("/path/to/TestString.milk"));
    EXPECT_TRUE(filter.Passes("/path/to/Teststring.milk"));
}


TEST(projectMPlaylistFilter, SingleCharacterExclude)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-/path/to/TestStr?ng.milk"});

    EXPECT_FALSE(filter.Passes("/path/to/TestString.milk"));
    EXPECT_FALSE(filter.Passes("/path/to/TestStrung.milk"));
    EXPECT_TRUE(filter.Passes("/path/to/TestStr/ng.milk"));
    EXPECT_TRUE(filter.Passes("/path/to/Teststring.milk"));
}


TEST(projectMPlaylistFilter, MultiCharacterExclude)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-/path/to/Test*.milk"});

    EXPECT_FALSE(filter.Passes("/path/to/TestString.milk"));
    EXPECT_FALSE(filter.Passes("/path/to/TestFile.milk"));
    EXPECT_FALSE(filter.Passes("/path/to/TestALotOfAdditional.Characters.milk"));
    EXPECT_FALSE(filter.Passes("/path/to/Test.milk"));
    EXPECT_TRUE(filter.Passes("/path/to/Test/String.milk"));
}


TEST(projectMPlaylistFilter, MultiCharacterExcludeExamples)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-a"});
    EXPECT_FALSE(filter.Passes("a"));
    EXPECT_FALSE(filter.Passes("x/a"));
    EXPECT_FALSE(filter.Passes("x/y/a"));
    EXPECT_TRUE(filter.Passes("b"));
    EXPECT_TRUE(filter.Passes("x/b"));
    EXPECT_TRUE(filter.Passes("a/a/b"));

    filter.SetList({"-/*"});
    EXPECT_FALSE(filter.Passes("a"));
    EXPECT_FALSE(filter.Passes("b"));
    EXPECT_TRUE(filter.Passes("x/a"));
    EXPECT_TRUE(filter.Passes("x/b"));
    EXPECT_TRUE(filter.Passes("x/y/a"));

    filter.SetList({"-/a"});
    EXPECT_FALSE(filter.Passes("a"));
    EXPECT_FALSE(filter.Passes("/a"));
    EXPECT_FALSE(filter.Passes("./a"));
    EXPECT_TRUE(filter.Passes("x/a"));
    EXPECT_TRUE(filter.Passes("x/y/a"));
}


TEST(projectMPlaylistFilter, PathGlobExclude)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-**/Test.milk"});

    EXPECT_FALSE(filter.Passes("/path/to/Test.milk"));
    EXPECT_FALSE(filter.Passes("/path/Test.milk"));
    EXPECT_FALSE(filter.Passes("Test.milk"));
    EXPECT_FALSE(filter.Passes("\\path\\to\\path\\to\\path\\to/path/to/path/to/Test.milk"));
    EXPECT_TRUE(filter.Passes("/path/to/Test/.milk"));
}


TEST(projectMPlaylistFilter, PathGlobExcludeExamples)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-**/a"});
    EXPECT_FALSE(filter.Passes("a"));
    EXPECT_FALSE(filter.Passes("x/a"));
    EXPECT_FALSE(filter.Passes("x/y/a"));
    EXPECT_TRUE(filter.Passes("b"));
    EXPECT_TRUE(filter.Passes("x/b"));

    filter.SetList({"-a/**/b"});
    EXPECT_FALSE(filter.Passes("a/b"));
    EXPECT_FALSE(filter.Passes("a/x/b"));
    EXPECT_FALSE(filter.Passes("a/x/y/b"));
    EXPECT_TRUE(filter.Passes("x/a/b"));
    EXPECT_TRUE(filter.Passes("a/b/x"));

    filter.SetList({"-a/**"});
    EXPECT_FALSE(filter.Passes("a/x"));
    EXPECT_FALSE(filter.Passes("a/y"));
    EXPECT_FALSE(filter.Passes("a/x/y"));
    EXPECT_TRUE(filter.Passes("a"));
    EXPECT_TRUE(filter.Passes("b/x"));
}


TEST(projectMPlaylistFilter, LargeGlobs)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a"});
    EXPECT_FALSE(filter.Passes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    EXPECT_TRUE(filter.Passes("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));

    filter.SetList({"-/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a/**/a"});
    EXPECT_FALSE(filter.Passes("/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a"));
    EXPECT_TRUE(filter.Passes("/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b/b"));
}


TEST(projectMPlaylistFilter, MultipleFilters)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-/path/to/Test*.milk",
                    "/path/to/another\\Test*.milk",
                    "+/path/to/yet/another\\Test*.milk",
                    "-Test*.milk"});

    EXPECT_FALSE(filter.Passes("/path/to/TestSome.milk"));
    EXPECT_FALSE(filter.Passes("/path/to/another/TestSome.milk"));
    EXPECT_TRUE(filter.Passes("\\path\\to\\yet\\another\\TestCase.milk"));
    EXPECT_FALSE(filter.Passes("/path/of/my/TestPreset.milk"));
    EXPECT_TRUE(filter.Passes("/another/something/completely/different"));
}


TEST(projectMPlaylistFilter, MatchEverything)
{
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-/**"});

    EXPECT_FALSE(filter.Passes("/path/to/TestSome.milk"));
    EXPECT_FALSE(filter.Passes("/path/to/another/TestSome.milk"));
    EXPECT_FALSE(filter.Passes("\\path\\to\\yet\\another\\TestCase.milk"));
    EXPECT_FALSE(filter.Passes("/path/of/my/TestPreset.milk"));
    EXPECT_FALSE(filter.Passes("/another/something/completely/different"));
}