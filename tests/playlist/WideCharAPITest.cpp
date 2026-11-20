/**
 * Tests for the file name encoding of the playlist API: file names are stored as UTF-8, the narrow
 * functions use the active ANSI code page on Windows, and the "W" functions use UTF-16.
 */
#include "PlaylistCWrapperMock.h"

#include <api/projectM-4/playlist.h>

#include <Playlist.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

namespace {

// "B_<U+26A0>_Warning.milk" in UTF-8. U+26A0 isn't part of any ANSI code page.
const std::string WarningFileUtf8{"B_\xE2\x9A\xA0_Warning.milk"};

auto Handle(PlaylistCWrapperMock& mock) -> projectm_playlist_handle
{
    return reinterpret_cast<projectm_playlist_handle>(&mock);
}

} // namespace


// Scanning stores all file names as UTF-8, including those the ANSI code page can't represent.
TEST(projectMPlaylistEncoding, AddPathKeepsAllFilesAsUtf8)
{
    for (bool recursive : {false, true})
    {
        libprojectM::Playlist::Playlist playlist;

        EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets-unicode", 0, recursive, false), 3);

        std::vector<std::string> names;
        for (const auto& item : playlist.Items())
        {
            const auto& filename = item.Filename();
            names.push_back(filename.substr(filename.find_last_of("/\\") + 1));
        }
        std::sort(names.begin(), names.end());

        ASSERT_EQ(names.size(), 3);
        EXPECT_EQ(names.at(0), "A_Before.milk");
        EXPECT_EQ(names.at(1), WarningFileUtf8);
        EXPECT_EQ(names.at(2), "C_After.milk");
    }
}


TEST(projectMPlaylistEncoding, SortByFilenameWithUtf8Names)
{
    libprojectM::Playlist::Playlist playlist;
    playlist.AddItem("/z/" + WarningFileUtf8, libprojectM::Playlist::Playlist::InsertAtEnd, false);
    playlist.AddItem("/y/C_After.milk", libprojectM::Playlist::Playlist::InsertAtEnd, false);
    playlist.AddItem("/x/A_Before.milk", libprojectM::Playlist::Playlist::InsertAtEnd, false);

    playlist.Sort(0, 3, libprojectM::Playlist::Playlist::SortPredicate::FilenameOnly,
                  libprojectM::Playlist::Playlist::SortOrder::Ascending);

    ASSERT_EQ(playlist.Items().size(), 3);
    EXPECT_EQ(playlist.Items().at(0).Filename(), "/x/A_Before.milk");
    EXPECT_EQ(playlist.Items().at(1).Filename(), "/z/" + WarningFileUtf8);
    EXPECT_EQ(playlist.Items().at(2).Filename(), "/y/C_After.milk");
}


#ifdef _WIN32

TEST(projectMPlaylistEncoding, AddPathW)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddPath("C:\\presets\\\xE2\x9A\xA0", libprojectM::Playlist::Playlist::InsertAtEnd, true, false))
        .WillOnce(Return(35));

    EXPECT_EQ(projectm_playlist_add_pathW(Handle(mockPlaylist), L"C:\\presets\\\u26A0", true, false), 35);
}


TEST(projectMPlaylistEncoding, InsertPathW)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddPath("C:\\presets\\\xE2\x9A\xA0", 12, false, true))
        .WillOnce(Return(4));

    EXPECT_EQ(projectm_playlist_insert_pathW(Handle(mockPlaylist), L"C:\\presets\\\u26A0", 12, false, true), 4);
}


TEST(projectMPlaylistEncoding, AddAndInsertPresetW)
{
    PlaylistCWrapperMock mockPlaylist;

    EXPECT_CALL(mockPlaylist, AddItem("C:\\" + WarningFileUtf8, libprojectM::Playlist::Playlist::InsertAtEnd, false))
        .WillOnce(Return(true));
    EXPECT_CALL(mockPlaylist, AddItem("C:\\" + WarningFileUtf8, 3, true))
        .WillOnce(Return(true));

    EXPECT_TRUE(projectm_playlist_add_presetW(Handle(mockPlaylist), L"C:\\B_\u26A0_Warning.milk", false));
    EXPECT_TRUE(projectm_playlist_insert_presetW(Handle(mockPlaylist), L"C:\\B_\u26A0_Warning.milk", 3, true));
}


TEST(projectMPlaylistEncoding, AddAndInsertPresetsW)
{
    PlaylistCWrapperMock mockPlaylist;

    const wchar_t* filenames[]{L"C:\\B_\u26A0_Warning.milk", nullptr, L"C:\\A_Before.milk"};

    EXPECT_CALL(mockPlaylist, AddItem("C:\\" + WarningFileUtf8, libprojectM::Playlist::Playlist::InsertAtEnd, false))
        .WillOnce(Return(true));
    EXPECT_CALL(mockPlaylist, AddItem("C:\\A_Before.milk", libprojectM::Playlist::Playlist::InsertAtEnd, false))
        .WillOnce(Return(false));

    EXPECT_EQ(projectm_playlist_add_presetsW(Handle(mockPlaylist), filenames, 3, false), 1);

    EXPECT_CALL(mockPlaylist, AddItem("C:\\" + WarningFileUtf8, 5, true))
        .WillOnce(Return(true));
    EXPECT_CALL(mockPlaylist, AddItem("C:\\A_Before.milk", 6, true))
        .WillOnce(Return(true));

    EXPECT_EQ(projectm_playlist_insert_presetsW(Handle(mockPlaylist), filenames, 3, 5, true), 2);
}


TEST(projectMPlaylistEncoding, ItemAndItemsW)
{
    PlaylistCWrapperMock mockPlaylist;

    std::vector<libprojectM::Playlist::Item> items{
        libprojectM::Playlist::Item("C:\\" + WarningFileUtf8),
        libprojectM::Playlist::Item("C:\\A_Before.milk")};

    EXPECT_CALL(mockPlaylist, Empty()).WillRepeatedly(Return(false));
    EXPECT_CALL(mockPlaylist, Size()).WillRepeatedly(Return(2));
    EXPECT_CALL(mockPlaylist, Items()).WillRepeatedly(ReturnRef(items));

    auto* item = projectm_playlist_itemW(Handle(mockPlaylist), 0);
    ASSERT_NE(item, nullptr);
    EXPECT_STREQ(item, L"C:\\B_\u26A0_Warning.milk");
    projectm_playlist_free_wstring(item);

    EXPECT_EQ(projectm_playlist_itemW(Handle(mockPlaylist), 2), nullptr);

    auto* list = projectm_playlist_itemsW(Handle(mockPlaylist), 0, 10);
    ASSERT_NE(list, nullptr);
    EXPECT_STREQ(list[0], L"C:\\B_\u26A0_Warning.milk");
    EXPECT_STREQ(list[1], L"C:\\A_Before.milk");
    EXPECT_EQ(list[2], nullptr);
    projectm_playlist_free_wstring_array(list);
}


// The narrow functions still return all items, with characters the ANSI code page can't represent replaced.
TEST(projectMPlaylistEncoding, NarrowItemReplacesUnrepresentableCharacters)
{
    if (GetACP() == CP_UTF8)
    {
        GTEST_SKIP() << "The active code page is UTF-8, which can represent all characters.";
    }

    PlaylistCWrapperMock mockPlaylist;

    std::vector<libprojectM::Playlist::Item> items{libprojectM::Playlist::Item("C:\\" + WarningFileUtf8)};

    EXPECT_CALL(mockPlaylist, Empty()).WillRepeatedly(Return(false));
    EXPECT_CALL(mockPlaylist, Size()).WillRepeatedly(Return(1));
    EXPECT_CALL(mockPlaylist, Items()).WillRepeatedly(ReturnRef(items));

    auto* item = projectm_playlist_item(Handle(mockPlaylist), 0);
    ASSERT_NE(item, nullptr);
    EXPECT_STREQ(item, "C:\\B_?_Warning.milk");
    projectm_playlist_free_string(item);
}


// Narrow strings in the ANSI code page are converted to UTF-8 and back.
TEST(projectMPlaylistEncoding, NarrowFunctionsUseTheAnsiCodePage)
{
    if (GetACP() != 1252)
    {
        GTEST_SKIP() << "This test needs the Windows-1252 code page.";
    }

    PlaylistCWrapperMock mockPlaylist;

    // "caf<U+00E9>.milk": E9 in Windows-1252, C3 A9 in UTF-8.
    EXPECT_CALL(mockPlaylist, AddItem("C:\\caf\xC3\xA9.milk", libprojectM::Playlist::Playlist::InsertAtEnd, false))
        .WillOnce(Return(true));
    EXPECT_TRUE(projectm_playlist_add_preset(Handle(mockPlaylist), "C:\\caf\xE9.milk", false));

    std::vector<libprojectM::Playlist::Item> items{libprojectM::Playlist::Item("C:\\caf\xC3\xA9.milk")};
    EXPECT_CALL(mockPlaylist, Empty()).WillRepeatedly(Return(false));
    EXPECT_CALL(mockPlaylist, Size()).WillRepeatedly(Return(1));
    EXPECT_CALL(mockPlaylist, Items()).WillRepeatedly(ReturnRef(items));

    auto* item = projectm_playlist_item(Handle(mockPlaylist), 0);
    ASSERT_NE(item, nullptr);
    EXPECT_STREQ(item, "C:\\caf\xE9.milk");
    projectm_playlist_free_string(item);
}

#endif
