#include "PlaylistCWrapperMock.h"

#include <playlist.h>

#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::ReturnRef;

/**
 * This suite only tests the API forwarding to the wrapper, not the actual playlist functionality!
 *
 * Also note that the playlist wrapper class should never be instantiated directly in production
 * code. ALWAYS use projectm_playlist_create() to create the instance and only use the C API
 * functions to access the functionality. The extreme use of reinterpret_cast<>() in this test suite
 * should make that quite obvious.
 */
TEST(projectMPlaylist, APICreate)
{
    auto* playlistHandle = projectm_playlist_create(nullptr);

    ASSERT_NE(playlistHandle, nullptr);

    projectm_playlist_destroy(playlistHandle);
}


TEST(projectMPlaylist, APIConnect)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Connect(nullptr))
        .Times(1);

    projectm_playlist_connect(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), nullptr);

    projectm_handle someHandle{reinterpret_cast<projectm_handle>(2537)};
    EXPECT_CALL(mockPlaylist, Connect(someHandle)).Times(1);

    projectm_playlist_connect(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), someHandle);
}


TEST(projectMPlaylist, APISize)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Size())
        .Times(1)
        .WillOnce(Return(2336));

    ASSERT_EQ(projectm_playlist_size(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)), 2336);
}


TEST(projectMPlaylist, APIClear)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Clear())
        .Times(1);

    projectm_playlist_clear(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist));
}


TEST(projectMPlaylist, APIItems)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file")};

    EXPECT_CALL(mockPlaylist, Items())
        .Times(1)
        .WillOnce(ReturnRef(items));

    auto* returnedItems = projectm_playlist_items(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist));
    ASSERT_NE(returnedItems, nullptr);
    ASSERT_NE(*returnedItems, nullptr);
    EXPECT_STREQ(*returnedItems, items.at(0).Filename().c_str());
    ASSERT_NE(*(returnedItems + 1), nullptr);
    EXPECT_STREQ(*(returnedItems + 1), items.at(1).Filename().c_str());
    EXPECT_EQ(*(returnedItems + 2), nullptr);

    projectm_playlist_free_string_array(returnedItems);
}


TEST(projectMPlaylist, APIAddPath)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddPath("/some/path", ProjectM::Playlist::Playlist::InsertAtEnd, true, false))
        .Times(1)
        .WillOnce(Return(35));

    EXPECT_EQ(projectm_playlist_add_path(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/path", true, false), 35);
}


TEST(projectMPlaylist, APIInsertPath)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddPath("/some/path", 6782, false, true))
        .Times(1)
        .WillOnce(Return(84));

    EXPECT_EQ(projectm_playlist_insert_path(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/path", 6782, false, true), 84);
}


TEST(projectMPlaylist, APIAddPreset)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddItem("/some/file", ProjectM::Playlist::Playlist::InsertAtEnd, false))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(projectm_playlist_add_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/file", false));
}


TEST(projectMPlaylist, APIInsertPreset)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddItem("/some/file", 34, true))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(projectm_playlist_insert_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/file", 34, true));
}


TEST(projectMPlaylist, APIAddPresets)
{
    PlaylistCWrapperMock mockPlaylist;

    const char firstFile[]{"/some/file"};
    const char secondFile[]{"/another/file"};
    const char thirdFile[]{"/another/file"};
    const char* presetList[]{firstFile, secondFile, thirdFile};

    EXPECT_CALL(mockPlaylist, AddItem("/some/file", ProjectM::Playlist::Playlist::InsertAtEnd, false))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(mockPlaylist, AddItem("/another/file", ProjectM::Playlist::Playlist::InsertAtEnd, false))
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_EQ(projectm_playlist_add_presets(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), presetList, 3, false), 2);
}


TEST(projectMPlaylist, APIInsertPresets)
{
    PlaylistCWrapperMock mockPlaylist;

    const char firstFile[]{"/some/file"};
    const char secondFile[]{"/some/file"};
    const char thirdFile[]{"/another/file"};
    const char* presetList[]{firstFile, secondFile, thirdFile};

    EXPECT_CALL(mockPlaylist, AddItem("/some/file", 34, false))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(mockPlaylist, AddItem("/some/file", 35, false))
        .Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(mockPlaylist, AddItem("/another/file", 35, false)) // Index not incremented!
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_EQ(projectm_playlist_insert_presets(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), presetList, 3, 34, false), 2);
}


TEST(projectMPlaylist, APIRemovePreset)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, RemoveItem(0))
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_TRUE(projectm_playlist_remove_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0));
    EXPECT_FALSE(projectm_playlist_remove_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0));
}


TEST(projectMPlaylist, APIRemovePresets)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, RemoveItem(0))
        .Times(3)
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_EQ(projectm_playlist_remove_presets(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 3), 2);
}


TEST(projectMPlaylist, APISetShuffle)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Shuffle(true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, Shuffle(false))
        .Times(1);

    projectm_playlist_set_shuffle(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true);
    projectm_playlist_set_shuffle(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false);
}


TEST(projectMPlaylist, APISort)
{
    using ProjectM::Playlist::Playlist;

    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Sort(0, 5000, Playlist::SortPredicate::FullPath, Playlist::SortOrder::Ascending))
        .Times(3); // Defaults - invalid values should call the Sort() function like this.
    EXPECT_CALL(mockPlaylist, Sort(0, 5000, Playlist::SortPredicate::FullPath, Playlist::SortOrder::Descending))
        .Times(1);
    EXPECT_CALL(mockPlaylist, Sort(0, 5000, Playlist::SortPredicate::FilenameOnly, Playlist::SortOrder::Ascending))
        .Times(1);
    EXPECT_CALL(mockPlaylist, Sort(0, 5000, Playlist::SortPredicate::FilenameOnly, Playlist::SortOrder::Descending))
        .Times(1);

    projectm_playlist_sort(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 5000,
                           SORT_PREDICATE_FULL_PATH, SORT_ORDER_ASCENDING);
    projectm_playlist_sort(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 5000,
                           SORT_PREDICATE_FULL_PATH, SORT_ORDER_DESCENDING);
    projectm_playlist_sort(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 5000,
                           SORT_PREDICATE_FILENAME_ONLY, SORT_ORDER_ASCENDING);
    projectm_playlist_sort(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 5000,
                           SORT_PREDICATE_FILENAME_ONLY, SORT_ORDER_DESCENDING);
    projectm_playlist_sort(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 5000,
                           static_cast<projectm_playlist_sort_predicate>(100), SORT_ORDER_ASCENDING);
    projectm_playlist_sort(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 5000,
                           SORT_PREDICATE_FULL_PATH, static_cast<projectm_playlist_sort_order>(200));
}
