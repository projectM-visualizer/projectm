#include "PlaylistCWrapperMock.h"

#include <api/projectM-4/playlist.h>

#include <gtest/gtest.h>

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Throw;

/**
 * This suite only tests the API forwarding to the wrapper, not the actual playlist functionality!
 *
 * Also note that the playlist wrapper class should never be instantiated directly in production
 * code. ALWAYS use projectm_playlist_create() to create the instance and only use the C API
 * functions to access the functionality. The extreme use of reinterpret_cast<>() in this test suite
 * should make that quite obvious.
 */
TEST(projectMPlaylistAPI, Create)
{
    auto* playlistHandle = projectm_playlist_create(nullptr);

    ASSERT_NE(playlistHandle, nullptr);

    projectm_playlist_destroy(playlistHandle);
}


TEST(projectMPlaylistAPI, Connect)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Connect(nullptr))
        .Times(1);

    projectm_playlist_connect(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), nullptr);

    projectm_handle someHandle{reinterpret_cast<projectm_handle>(2537)};
    EXPECT_CALL(mockPlaylist, Connect(someHandle)).Times(1);

    projectm_playlist_connect(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), someHandle);
}


TEST(projectMPlaylistAPI, Size)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Size())
        .Times(1)
        .WillOnce(Return(2336));

    ASSERT_EQ(projectm_playlist_size(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)), 2336);
}


TEST(projectMPlaylistAPI, Clear)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Clear())
        .Times(1);

    projectm_playlist_clear(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist));
}


TEST(projectMPlaylistAPI, ItemsAll)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file")};

    EXPECT_CALL(mockPlaylist, Items())
        .Times(1)
        .WillOnce(ReturnRef(items));

    // Passing INT_MAX as count should make the function always return all elements.
    auto* returnedItems = projectm_playlist_items(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, std::numeric_limits<uint32_t>::max());
    ASSERT_NE(returnedItems, nullptr);
    ASSERT_NE(*returnedItems, nullptr);
    EXPECT_STREQ(*returnedItems, items.at(0).Filename().c_str());
    ASSERT_NE(*(returnedItems + 1), nullptr);
    EXPECT_STREQ(*(returnedItems + 1), items.at(1).Filename().c_str());
    EXPECT_EQ(*(returnedItems + 2), nullptr);

    projectm_playlist_free_string_array(returnedItems);
}


TEST(projectMPlaylistAPI, ItemsPartial)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file1"),
        ProjectM::Playlist::Item("/another/file2"),
        ProjectM::Playlist::Item("/another/file3"),
        ProjectM::Playlist::Item("/another/file4"),
        ProjectM::Playlist::Item("/another/file5")};

    EXPECT_CALL(mockPlaylist, Items())
        .Times(1)
        .WillOnce(ReturnRef(items));

    auto* returnedItems = projectm_playlist_items(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 1, 3);
    ASSERT_NE(returnedItems, nullptr);
    ASSERT_NE(*returnedItems, nullptr);
    EXPECT_STREQ(*returnedItems, items.at(1).Filename().c_str());
    ASSERT_NE(*(returnedItems + 1), nullptr);
    EXPECT_STREQ(*(returnedItems + 1), items.at(2).Filename().c_str());
    EXPECT_NE(*(returnedItems + 2), nullptr);
    EXPECT_STREQ(*(returnedItems + 2), items.at(3).Filename().c_str());
    EXPECT_EQ(*(returnedItems + 3), nullptr);

    projectm_playlist_free_string_array(returnedItems);
}


TEST(projectMPlaylistAPI, ItemsOutOfRange)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file1"),
        ProjectM::Playlist::Item("/another/file2"),
        ProjectM::Playlist::Item("/another/file3"),
        ProjectM::Playlist::Item("/another/file4"),
        ProjectM::Playlist::Item("/another/file5")};

    EXPECT_CALL(mockPlaylist, Items())
        .Times(1)
        .WillOnce(ReturnRef(items));

    auto* returnedItems = projectm_playlist_items(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 10, 10);
    ASSERT_NE(returnedItems, nullptr);
    ASSERT_EQ(*returnedItems, nullptr);

    projectm_playlist_free_string_array(returnedItems);
}


TEST(projectMPlaylistAPI, Item)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file")};

    EXPECT_CALL(mockPlaylist, Empty())
        .Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(mockPlaylist, Size())
        .Times(1)
        .WillOnce(Return(2));
    EXPECT_CALL(mockPlaylist, Items())
        .Times(1)
        .WillOnce(ReturnRef(items));

    auto* returnedItem = projectm_playlist_item(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 1);
    ASSERT_NE(returnedItem, nullptr);
    EXPECT_STREQ(returnedItem, items.at(1).Filename().c_str());

    projectm_playlist_free_string(returnedItem);
}


TEST(projectMPlaylistAPI, ItemEmptyPlaylist)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file")};

    EXPECT_CALL(mockPlaylist, Empty())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_EQ(projectm_playlist_item(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 1), nullptr);
}


TEST(projectMPlaylistAPI, ItemIndexOutOfBounds)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<ProjectM::Playlist::Item> items{
        ProjectM::Playlist::Item("/some/file"),
        ProjectM::Playlist::Item("/another/file")};

    EXPECT_CALL(mockPlaylist, Empty())
        .Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(mockPlaylist, Size())
        .Times(1)
        .WillOnce(Return(2));

    EXPECT_EQ(projectm_playlist_item(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 5), nullptr);
}


TEST(projectMPlaylistAPI, AddPath)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddPath("/some/path", ProjectM::Playlist::Playlist::InsertAtEnd, true, false))
        .Times(1)
        .WillOnce(Return(35));

    EXPECT_EQ(projectm_playlist_add_path(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/path", true, false), 35);
}


TEST(projectMPlaylistAPI, InsertPath)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddPath("/some/path", 6782, false, true))
        .Times(1)
        .WillOnce(Return(84));

    EXPECT_EQ(projectm_playlist_insert_path(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/path", 6782, false, true), 84);
}


TEST(projectMPlaylistAPI, AddPreset)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddItem("/some/file", ProjectM::Playlist::Playlist::InsertAtEnd, false))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(projectm_playlist_add_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/file", false));
}


TEST(projectMPlaylistAPI, InsertPreset)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddItem("/some/file", 34, true))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(projectm_playlist_insert_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), "/some/file", 34, true));
}


TEST(projectMPlaylistAPI, AddPresets)
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


TEST(projectMPlaylistAPI, InsertPresets)
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


TEST(projectMPlaylistAPI, RemovePreset)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, RemoveItem(0))
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_TRUE(projectm_playlist_remove_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0));
    EXPECT_FALSE(projectm_playlist_remove_preset(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0));
}


TEST(projectMPlaylistAPI, RemovePresets)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, RemoveItem(0))
        .Times(3)
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(false));

    EXPECT_EQ(projectm_playlist_remove_presets(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 0, 3), 2);
}


TEST(projectMPlaylistAPI, GetShuffle)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, Shuffle())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_TRUE(projectm_playlist_get_shuffle(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)));
}


TEST(projectMPlaylistAPI, SetShuffle)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, SetShuffle(true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, SetShuffle(false))
        .Times(1);

    projectm_playlist_set_shuffle(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true);
    projectm_playlist_set_shuffle(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false);
}


TEST(projectMPlaylistAPI, Sort)
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


TEST(projectMPlaylistAPI, GetRetryCount)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, RetryCount())
        .Times(1)
        .WillOnce(Return(5));

    EXPECT_EQ(projectm_playlist_get_retry_count(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)), 5);
}


TEST(projectMPlaylistAPI, SetRetryCount)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, SetRetryCount(5))
        .Times(1);

    projectm_playlist_set_retry_count(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 5);
}


TEST(projectMPlaylistAPI, GetPosition)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, PresetIndex())
        .Times(1)
        .WillOnce(Return(512));

    EXPECT_EQ(projectm_playlist_get_position(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)), 512);
}


TEST(projectMPlaylistAPI, GetPositionException)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, PresetIndex())
        .Times(1)
        .WillOnce(Throw(ProjectM::Playlist::PlaylistEmptyException()));

    EXPECT_EQ(projectm_playlist_get_position(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)), 0);
}


TEST(projectMPlaylistAPI, SetPosition)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, SetPresetIndex(256))
        .Times(2)
        .WillRepeatedly(Return(512));
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, true, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, false, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));

    EXPECT_EQ(projectm_playlist_set_position(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 256, true), 512);
    EXPECT_EQ(projectm_playlist_set_position(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 256, false), 512);
}


TEST(projectMPlaylistAPI, SetPositionException)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, SetPresetIndex(256))
        .Times(2)
        .WillRepeatedly(Throw(ProjectM::Playlist::PlaylistEmptyException()));

    EXPECT_EQ(projectm_playlist_set_position(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 256, true), 0);
    EXPECT_EQ(projectm_playlist_set_position(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), 256, false), 0);
}


TEST(projectMPlaylistAPI, PlayNext)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, NextPresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, true, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, false, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));

    EXPECT_EQ(projectm_playlist_play_next(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true), 512);
    EXPECT_EQ(projectm_playlist_play_next(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false), 512);
}


TEST(projectMPlaylistAPI, PlayNextException)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, NextPresetIndex())
        .Times(2)
        .WillRepeatedly(Throw(ProjectM::Playlist::PlaylistEmptyException()));

    EXPECT_EQ(projectm_playlist_play_next(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true), 0);
    EXPECT_EQ(projectm_playlist_play_next(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false), 0);
}


TEST(projectMPlaylistAPI, PlayPrevious)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, PreviousPresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, true, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, false, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));

    EXPECT_EQ(projectm_playlist_play_previous(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true), 512);
    EXPECT_EQ(projectm_playlist_play_previous(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false), 512);
}


TEST(projectMPlaylistAPI, PlayPreviousException)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, PreviousPresetIndex())
        .Times(2)
        .WillRepeatedly(Throw(ProjectM::Playlist::PlaylistEmptyException()));

    EXPECT_EQ(projectm_playlist_play_previous(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true), 0);
    EXPECT_EQ(projectm_playlist_play_previous(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false), 0);
}


TEST(projectMPlaylistAPI, PlayLast)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, PreviousPresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, true, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PlayPresetIndex(512, false, true))
        .Times(1);
    EXPECT_CALL(mockPlaylist, PresetIndex())
        .Times(2)
        .WillRepeatedly(Return(512));

    EXPECT_EQ(projectm_playlist_play_previous(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true), 512);
    EXPECT_EQ(projectm_playlist_play_previous(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false), 512);
}


TEST(projectMPlaylistAPI, PlayLastException)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, LastPresetIndex())
        .Times(2)
        .WillRepeatedly(Throw(ProjectM::Playlist::PlaylistEmptyException()));

    EXPECT_EQ(projectm_playlist_play_last(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), true), 0);
    EXPECT_EQ(projectm_playlist_play_last(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), false), 0);
}


TEST(projectMPlaylistAPI, SetPresetSwitchedCallback)
{
    PlaylistCWrapperMock mockPlaylist;

    projectm_playlist_preset_switched_event dummyCallback = [](bool is_hard_cut,
                                                               uint32_t index,
                                                               void* user_data) {};
    void* dummyData{reinterpret_cast<void*>(2973246)};

    EXPECT_CALL(mockPlaylist, SetPresetSwitchedCallback(dummyCallback, dummyData))
        .Times(1);

    projectm_playlist_set_preset_switched_event_callback(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), dummyCallback, dummyData);
}


TEST(projectMPlaylistAPI, SetPresetSwitchFailedCallback)
{
    PlaylistCWrapperMock mockPlaylist;

    projectm_playlist_preset_switch_failed_event dummyCallback = [](const char* preset_filename,
                                                                    const char* message,
                                                                    void* user_data) {};
    void* dummyData{reinterpret_cast<void*>(348564)};

    EXPECT_CALL(mockPlaylist, SetPresetSwitchFailedCallback(dummyCallback, dummyData))
        .Times(1);

    projectm_playlist_set_preset_switch_failed_event_callback(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), dummyCallback, dummyData);
}


TEST(projectMPlaylistAPI, SetFilter)
{
    PlaylistCWrapperMock mockPlaylist;
    ProjectM::Playlist::Filter filter;

    EXPECT_CALL(mockPlaylist, Filter())
        .Times(1)
        .WillOnce(ReturnRef(filter));

    const char firstFilter[]{"-/some/BadPreset.milk"};
    const char secondFilter[]{"+/another/AwesomePreset.milk"};
    const char thirdFilter[]{"-/unwanted/Preset.milk"};
    const char* filterList[]{firstFilter, secondFilter, thirdFilter};

    projectm_playlist_set_filter(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), filterList, 3);

    const auto& internalFilterList = filter.List();
    ASSERT_EQ(internalFilterList.size(), 3);
    EXPECT_EQ(internalFilterList.at(0), "-/some/BadPreset.milk");
    EXPECT_EQ(internalFilterList.at(1), "+/another/AwesomePreset.milk");
    EXPECT_EQ(internalFilterList.at(2), "-/unwanted/Preset.milk");
}


TEST(projectMPlaylistAPI, GetFilter)
{
    PlaylistCWrapperMock mockPlaylist;
    ProjectM::Playlist::Filter filter;

    filter.SetList({"-/some/BadPreset.milk",
                    "+/another/AwesomePreset.milk",
                    "-/unwanted/Preset.milk"});

    EXPECT_CALL(mockPlaylist, Filter())
        .Times(1)
        .WillOnce(ReturnRef(filter));

    size_t count{};
    auto filterList = projectm_playlist_get_filter(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist), &count);

    ASSERT_EQ(count, 3);
    ASSERT_NE(filterList, nullptr);
    EXPECT_STREQ(filterList[0], "-/some/BadPreset.milk");
    EXPECT_STREQ(filterList[1], "+/another/AwesomePreset.milk");
    EXPECT_STREQ(filterList[2], "-/unwanted/Preset.milk");

    projectm_playlist_free_string_array(filterList);
}


TEST(projectMPlaylistAPI, ApplyFilter)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, ApplyFilter())
        .Times(1)
        .WillOnce(Return(5));

    EXPECT_EQ(projectm_playlist_apply_filter(reinterpret_cast<projectm_playlist_handle>(&mockPlaylist)), 5);
}