#include <Item.hpp>

#include <gtest/gtest.h>

TEST(projectMPlaylistItem, Create)
{
    ASSERT_NO_THROW(ProjectM::Playlist::Item item("/some/file"));
}


TEST(projectMPlaylistItem, GetFilename)
{
    ProjectM::Playlist::Item item("/some/file");

    ASSERT_EQ(item.Filename(), "/some/file");
}


TEST(projectMPlaylistItem, FilenameEquality)
{
    ProjectM::Playlist::Item item("/some/file");

    EXPECT_TRUE(item == "/some/file");
    EXPECT_FALSE(item == "/some/other/file");
}
