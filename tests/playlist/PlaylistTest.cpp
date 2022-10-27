#include <Playlist.hpp>

#include <gtest/gtest.h>

using ProjectM::Playlist::Playlist;

TEST(projectMPlaylist, PlaylistCreate)
{
    ASSERT_NO_THROW(Playlist playlist);
}


TEST(projectMPlaylist, PlaylistSize)
{
    Playlist playlist;

    EXPECT_EQ(playlist.Size(), 0);

    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.Size(), 1);
}


TEST(projectMPlaylist, PlaylistClear)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.Size(), 1);

    playlist.Clear();

    EXPECT_EQ(playlist.Size(), 0);
}


TEST(projectMPlaylist, PlaylistItems)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 2);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 2);
    EXPECT_EQ(items.at(0).Filename(), "/some/file");
    EXPECT_EQ(items.at(1).Filename(), "/some/other/file");
}


TEST(projectMPlaylist, PlaylistAddItemEmptyFilename)
{
    Playlist playlist;
    EXPECT_FALSE(playlist.AddItem("", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 0);
}


TEST(projectMPlaylist, PlaylistAddItemAtEnd)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 3);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 3);
    EXPECT_EQ(items.at(2).Filename(), "/yet/another/file");
}


TEST(projectMPlaylist, PlaylistAddItemAtFront)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", 0, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", 0, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", 0, false));

    ASSERT_EQ(playlist.Size(), 3);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 3);
    EXPECT_EQ(items.at(0).Filename(), "/yet/another/file");
    EXPECT_EQ(items.at(1).Filename(), "/some/other/file");
    EXPECT_EQ(items.at(2).Filename(), "/some/file");
}


TEST(projectMPlaylist, PlaylistAddItemInMiddle)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", 0, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", 1, false));

    ASSERT_EQ(playlist.Size(), 3);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 3);
    EXPECT_EQ(items.at(0).Filename(), "/some/file");
    EXPECT_EQ(items.at(1).Filename(), "/yet/another/file");
    EXPECT_EQ(items.at(2).Filename(), "/some/other/file");
}


TEST(projectMPlaylist, PlaylistAddItemDuplicates)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, true));
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, true));
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, true));

    ASSERT_EQ(playlist.Size(), 3);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 3);
    EXPECT_EQ(items.at(2).Filename(), "/some/file");
}


TEST(projectMPlaylist, PlaylistAddItemNoDuplicates)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_FALSE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_FALSE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 1);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items.at(0).Filename(), "/some/file");
}


TEST(projectMPlaylist, PlaylistRemoveItemFromEnd)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 3);

    EXPECT_TRUE(playlist.RemoveItem(2));

    ASSERT_EQ(playlist.Size(), 2);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 2);
    EXPECT_EQ(items.at(0).Filename(), "/some/file");
    EXPECT_EQ(items.at(1).Filename(), "/some/other/file");
}


TEST(projectMPlaylist, PlaylistRemoveItemFromFront)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 3);

    EXPECT_TRUE(playlist.RemoveItem(0));

    ASSERT_EQ(playlist.Size(), 2);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 2);
    EXPECT_EQ(items.at(0).Filename(), "/some/other/file");
    EXPECT_EQ(items.at(1).Filename(), "/yet/another/file");
}


TEST(projectMPlaylist, PlaylistRemoveItemFromMiddle)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 3);

    EXPECT_TRUE(playlist.RemoveItem(1));

    ASSERT_EQ(playlist.Size(), 2);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 2);
    EXPECT_EQ(items.at(0).Filename(), "/some/file");
    EXPECT_EQ(items.at(1).Filename(), "/yet/another/file");
}


TEST(projectMPlaylist, PlaylistRemoveItemIndexOutOfBounds)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 3);

    EXPECT_FALSE(playlist.RemoveItem(100));

    ASSERT_EQ(playlist.Size(), 3);
}


TEST(projectMPlaylist, PlaylistShuffleEnableDisable)
{
    Playlist playlist;

    EXPECT_FALSE(playlist.Shuffle());

    playlist.Shuffle(true);

    EXPECT_TRUE(playlist.Shuffle());

    playlist.Shuffle(false);

    EXPECT_FALSE(playlist.Shuffle());
}

