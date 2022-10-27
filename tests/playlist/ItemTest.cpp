#include <Item.hpp>

#include <gtest/gtest.h>

TEST(projectMPlaylist, ItemCreate)
{
    ASSERT_NO_THROW(ProjectM::Playlist::Item item("/some/file"));
}


TEST(projectMPlaylist, ItemGetFilename)
{
    ProjectM::Playlist::Item item("/some/file");

    ASSERT_EQ(item.Filename(), "/some/file");
}


TEST(projectMPlaylist, ItemFilenameEquality)
{
    ProjectM::Playlist::Item item("/some/file");

    EXPECT_TRUE(item == "/some/file");
    EXPECT_FALSE(item == "/some/other/file");
}
