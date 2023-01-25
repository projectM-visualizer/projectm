#include <Playlist.hpp>

#include <gtest/gtest.h>

#include <algorithm>

using ProjectM::Playlist::Playlist;

TEST(projectMPlaylistPlaylist, Create)
{
    ASSERT_NO_THROW(Playlist playlist);
}


TEST(projectMPlaylistPlaylist, Size)
{
    Playlist playlist;

    EXPECT_EQ(playlist.Size(), 0);

    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.Size(), 1);
}


TEST(projectMPlaylistPlaylist, Empty)
{
    Playlist playlist;

    EXPECT_TRUE(playlist.Empty());

    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));

    EXPECT_FALSE(playlist.Empty());
}


TEST(projectMPlaylistPlaylist, Clear)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.Size(), 1);

    playlist.Clear();

    EXPECT_EQ(playlist.Size(), 0);
}


TEST(projectMPlaylistPlaylist, Items)
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


TEST(projectMPlaylistPlaylist, AddItemEmptyFilename)
{
    Playlist playlist;
    EXPECT_FALSE(playlist.AddItem("", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 0);
}


TEST(projectMPlaylistPlaylist, AddItemAtEnd)
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


TEST(projectMPlaylistPlaylist, AddItemAtFront)
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


TEST(projectMPlaylistPlaylist, AddItemInMiddle)
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


TEST(projectMPlaylistPlaylist, AddItemDuplicates)
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


TEST(projectMPlaylistPlaylist, AddItemNoDuplicates)
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


TEST(projectMPlaylistPlaylist, AddPathRecursively)
{
    Playlist playlist;

    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, true, false), 4);

    ASSERT_EQ(playlist.Size(), 4);
    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);

    EXPECT_NE(std::find_if(items.cbegin(), items.cend(), [](const ProjectM::Playlist::Item& item) {
                  return item.Filename().substr(item.Filename().length() - 11, 11) == "Test_D.milk";
              }),
              items.cend())
        << "Expected file not found in playlist.";

    EXPECT_EQ(std::find_if(items.cbegin(), items.cend(), [](const ProjectM::Playlist::Item& item) {
                  return item.Filename().substr(item.Filename().length() - 10, 10) == "Other.file";
              }),
              items.cend())
        << "Unexpected file found in playlist.";
}


TEST(projectMPlaylistPlaylist, AddPathRecursivelyNoDuplicates)
{
    Playlist playlist;

    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, true, false), 4);
    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, true, false), 0);

    ASSERT_EQ(playlist.Size(), 4);
}


TEST(projectMPlaylistPlaylist, AddPathNonRecursively)
{
    Playlist playlist;

    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, false, false), 3);

    ASSERT_EQ(playlist.Size(), 3);
    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 3);

    EXPECT_NE(std::find_if(items.cbegin(), items.cend(), [](const ProjectM::Playlist::Item& item) {
                  return item.Filename().substr(item.Filename().length() - 11, 11) == "Test_B.milk";
              }),
              items.cend())
        << "Expected file not found in playlist.";

    EXPECT_EQ(std::find_if(items.cbegin(), items.cend(), [](const ProjectM::Playlist::Item& item) {
                  return item.Filename().substr(item.Filename().length() - 11, 11) == "Test_D.milk";
              }),
              items.cend())
        << "Unexpected file found in playlist.";

    EXPECT_EQ(std::find_if(items.cbegin(), items.cend(), [](const ProjectM::Playlist::Item& item) {
                  return item.Filename().substr(item.Filename().length() - 10, 10) == "Other.file";
              }),
              items.cend())
        << "Unexpected file found in playlist.";
}


TEST(projectMPlaylistPlaylist, AddPathnonRecursivelyNoDuplicates)
{
    Playlist playlist;

    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, false, false), 3);
    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, false, false), 0);

    ASSERT_EQ(playlist.Size(), 3);
}


TEST(projectMPlaylistPlaylist, RemoveItemFromEnd)
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


TEST(projectMPlaylistPlaylist, RemoveItemFromFront)
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


TEST(projectMPlaylistPlaylist, RemoveItemFromMiddle)
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


TEST(projectMPlaylistPlaylist, RemoveItemIndexOutOfBounds)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/file", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/file", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 3);

    EXPECT_FALSE(playlist.RemoveItem(100));

    ASSERT_EQ(playlist.Size(), 3);
}


TEST(projectMPlaylistPlaylist, ShuffleEnableDisable)
{
    Playlist playlist;

    EXPECT_FALSE(playlist.Shuffle());

    playlist.SetShuffle(true);

    EXPECT_TRUE(playlist.Shuffle());

    playlist.SetShuffle(false);

    EXPECT_FALSE(playlist.Shuffle());
}


TEST(projectMPlaylistPlaylist, SortFullPathAscending)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/PresetD.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 4);

    playlist.Sort(0, playlist.Size(), Playlist::SortPredicate::FullPath, Playlist::SortOrder::Ascending);

    ASSERT_EQ(playlist.Size(), 4);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);
    EXPECT_EQ(items.at(0).Filename(), "/some/PresetA.milk");
    EXPECT_EQ(items.at(1).Filename(), "/some/PresetZ.milk");
    EXPECT_EQ(items.at(2).Filename(), "/some/other/PresetC.milk");
    EXPECT_EQ(items.at(3).Filename(), "/yet/another/PresetD.milk");
}


TEST(projectMPlaylistPlaylist, SortFullPathDescending)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/PresetD.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 4);

    playlist.Sort(0, playlist.Size(), Playlist::SortPredicate::FullPath, Playlist::SortOrder::Descending);

    ASSERT_EQ(playlist.Size(), 4);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);
    EXPECT_EQ(items.at(0).Filename(), "/yet/another/PresetD.milk");
    EXPECT_EQ(items.at(1).Filename(), "/some/other/PresetC.milk");
    EXPECT_EQ(items.at(2).Filename(), "/some/PresetZ.milk");
    EXPECT_EQ(items.at(3).Filename(), "/some/PresetA.milk");
}


TEST(projectMPlaylistPlaylist, SortFilenameOnlyAscending)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/PresetD.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 4);

    playlist.Sort(0, playlist.Size(), Playlist::SortPredicate::FilenameOnly, Playlist::SortOrder::Ascending);

    ASSERT_EQ(playlist.Size(), 4);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);
    EXPECT_EQ(items.at(0).Filename(), "/some/PresetA.milk");
    EXPECT_EQ(items.at(1).Filename(), "/some/other/PresetC.milk");
    EXPECT_EQ(items.at(2).Filename(), "/yet/another/PresetD.milk");
    EXPECT_EQ(items.at(3).Filename(), "/some/PresetZ.milk");
}


TEST(projectMPlaylistPlaylist, SortFilenameOnlyDescending)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/PresetD.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 4);

    playlist.Sort(0, playlist.Size(), Playlist::SortPredicate::FilenameOnly, Playlist::SortOrder::Descending);

    ASSERT_EQ(playlist.Size(), 4);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);
    EXPECT_EQ(items.at(0).Filename(), "/some/PresetZ.milk");
    EXPECT_EQ(items.at(1).Filename(), "/yet/another/PresetD.milk");
    EXPECT_EQ(items.at(2).Filename(), "/some/other/PresetC.milk");
    EXPECT_EQ(items.at(3).Filename(), "/some/PresetA.milk");
}


TEST(projectMPlaylistPlaylist, SortOutOfBoundsStart)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/PresetD.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 4);

    playlist.Sort(std::numeric_limits<uint32_t>::max(), 1, Playlist::SortPredicate::FilenameOnly, Playlist::SortOrder::Ascending);

    ASSERT_EQ(playlist.Size(), 4);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);
    EXPECT_EQ(items.at(0).Filename(), "/some/PresetZ.milk");
    EXPECT_EQ(items.at(1).Filename(), "/some/PresetA.milk");
    EXPECT_EQ(items.at(2).Filename(), "/some/other/PresetC.milk");
    EXPECT_EQ(items.at(3).Filename(), "/yet/another/PresetD.milk");
}


TEST(projectMPlaylistPlaylist, SortOutOfBoundsCount)
{
    Playlist playlist;
    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/yet/another/PresetD.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 4);

    playlist.Sort(0, std::numeric_limits<uint32_t>::max(), Playlist::SortPredicate::FilenameOnly, Playlist::SortOrder::Ascending);

    ASSERT_EQ(playlist.Size(), 4);

    const auto& items = playlist.Items();
    ASSERT_EQ(items.size(), 4);
    EXPECT_EQ(items.at(0).Filename(), "/some/PresetA.milk");
    EXPECT_EQ(items.at(1).Filename(), "/some/other/PresetC.milk");
    EXPECT_EQ(items.at(2).Filename(), "/yet/another/PresetD.milk");
    EXPECT_EQ(items.at(3).Filename(), "/some/PresetZ.milk");
}


TEST(projectMPlaylistPlaylist, NextPresetIndexEmptyPlaylist)
{
    Playlist playlist;

    EXPECT_THROW(playlist.NextPresetIndex(), ProjectM::Playlist::PlaylistEmptyException);
}


TEST(projectMPlaylistPlaylist, NextPresetIndexShuffle)
{
    Playlist playlist;

    playlist.SetShuffle(true);

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));

    // Shuffle 100 times, this will have an (almost) 100% chance that both presets were played.
    std::set<size_t> playlistIndices;
    for (int i = 0; i < 100; i++)
    {
        EXPECT_NO_THROW(playlistIndices.insert(playlist.NextPresetIndex()));
    }

    EXPECT_TRUE(playlistIndices.find(0) != playlistIndices.end());
    EXPECT_TRUE(playlistIndices.find(1) != playlistIndices.end());
}


TEST(projectMPlaylistPlaylist, NextPresetIndexSequential)
{
    Playlist playlist;

    playlist.SetShuffle(false);

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.NextPresetIndex(), 1);
    EXPECT_EQ(playlist.NextPresetIndex(), 2);

    // Also test wrap to 0
    EXPECT_EQ(playlist.NextPresetIndex(), 0);
    EXPECT_EQ(playlist.NextPresetIndex(), 1);
}


TEST(projectMPlaylistPlaylist, PreviousPresetIndexEmptyPlaylist)
{
    Playlist playlist;

    EXPECT_THROW(playlist.PreviousPresetIndex(), ProjectM::Playlist::PlaylistEmptyException);
}


TEST(projectMPlaylistPlaylist, PreviousPresetIndexShuffle)
{
    Playlist playlist;

    playlist.SetShuffle(true);

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));

    // Shuffle 100 times, this will have an (almost) 100% chance that both presets were played.
    std::set<size_t> playlistIndices;
    for (int i = 0; i < 100; i++)
    {
        EXPECT_NO_THROW(playlistIndices.insert(playlist.PreviousPresetIndex()));
    }

    EXPECT_TRUE(playlistIndices.find(0) != playlistIndices.end());
    EXPECT_TRUE(playlistIndices.find(1) != playlistIndices.end());
}


TEST(projectMPlaylistPlaylist, PreviousPresetIndexSequential)
{
    Playlist playlist;

    playlist.SetShuffle(false);

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.PreviousPresetIndex(), 2);
    EXPECT_EQ(playlist.PreviousPresetIndex(), 1);
    EXPECT_EQ(playlist.PreviousPresetIndex(), 0);
    EXPECT_EQ(playlist.PreviousPresetIndex(), 2);
}


TEST(projectMPlaylistPlaylist, LastPresetIndex)
{
    Playlist playlist;

    playlist.SetShuffle(false);

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/the/last/Preset.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.SetPresetIndex(1), 1);
    EXPECT_EQ(playlist.SetPresetIndex(2), 2);
    EXPECT_EQ(playlist.SetPresetIndex(1), 1);
    EXPECT_EQ(playlist.SetPresetIndex(1), 1);
    EXPECT_EQ(playlist.SetPresetIndex(0), 0);

    EXPECT_EQ(playlist.PresetIndex(), 0);

    // Index 1 should only be added once here, even if played twice.
    EXPECT_EQ(playlist.LastPresetIndex(), 1);
    EXPECT_EQ(playlist.PresetIndex(), 1);
    EXPECT_EQ(playlist.LastPresetIndex(), 2);
    EXPECT_EQ(playlist.PresetIndex(), 2);
    EXPECT_EQ(playlist.LastPresetIndex(), 1);
    EXPECT_EQ(playlist.PresetIndex(), 1);

    // Starting index 0 is always be in the history.
    EXPECT_EQ(playlist.LastPresetIndex(), 0);
    EXPECT_EQ(playlist.PresetIndex(), 0);

    // History empty, wrap back to last item.
    EXPECT_EQ(playlist.LastPresetIndex(), 3);
    EXPECT_EQ(playlist.PresetIndex(), 3);

    // History should stell be empty, go back one item.
    EXPECT_EQ(playlist.LastPresetIndex(), 2);
    EXPECT_EQ(playlist.PresetIndex(), 2);
}


TEST(projectMPlaylistPlaylist, LastPresetIndexEmptyPlaylist)
{
    Playlist playlist;

    EXPECT_THROW(playlist.LastPresetIndex(), ProjectM::Playlist::PlaylistEmptyException);
}


TEST(projectMPlaylistPlaylist, SetPresetIndex)
{
    Playlist playlist;

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.SetPresetIndex(1), 1);
    EXPECT_EQ(playlist.SetPresetIndex(2), 2);
    EXPECT_EQ(playlist.SetPresetIndex(0), 0);
}


TEST(projectMPlaylistPlaylist, SetPresetIndexOutOfBounds)
{
    Playlist playlist;

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.SetPresetIndex(5), 0);
}


TEST(projectMPlaylistPlaylist, SetPresetIndexException)
{
    Playlist playlist;

    EXPECT_THROW(playlist.SetPresetIndex(0), ProjectM::Playlist::PlaylistEmptyException);
}


TEST(projectMPlaylistPlaylist, PresetIndex)
{
    Playlist playlist;

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.PresetIndex(), 0);
    EXPECT_NO_THROW(playlist.SetPresetIndex(2));
    EXPECT_EQ(playlist.PresetIndex(), 2);
}


TEST(projectMPlaylistPlaylist, PresetIndexException)
{
    Playlist playlist;

    EXPECT_THROW(playlist.PresetIndex(), ProjectM::Playlist::PlaylistEmptyException);
}


TEST(projectMPlaylistPlaylist, RemoveLastHistoryEntry)
{
    Playlist playlist;

    EXPECT_TRUE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));

    EXPECT_EQ(playlist.SetPresetIndex(1), 1);
    EXPECT_EQ(playlist.SetPresetIndex(2), 2);

    // History: 0,1
    playlist.RemoveLastHistoryEntry();
    // History: 0

    EXPECT_EQ(playlist.LastPresetIndex(), 0);
}


TEST(projectMPlaylistPlaylist, AddItemWithFilter)
{
    Playlist playlist;

    playlist.Filter().SetList({"-/**/Preset*.milk"});

    EXPECT_FALSE(playlist.AddItem("/some/PresetZ.milk", Playlist::InsertAtEnd, false));
    EXPECT_FALSE(playlist.AddItem("/some/PresetA.milk", Playlist::InsertAtEnd, false));
    EXPECT_FALSE(playlist.AddItem("/some/other/PresetC.milk", Playlist::InsertAtEnd, false));
    EXPECT_TRUE(playlist.AddItem("/some/MyFavorite.milk", Playlist::InsertAtEnd, false));

    ASSERT_EQ(playlist.Size(), 1);
}


TEST(projectMPlaylistPlaylist, AddPathWithFilter)
{
    Playlist playlist;

    playlist.Filter().SetList({"-**/presets/Test_*.milk"});

    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, true, false), 1);

    ASSERT_EQ(playlist.Size(), 1);
}


TEST(projectMPlaylistPlaylist, ApplyFilter)
{
    Playlist playlist;

    // Remove Test_A on load
    playlist.Filter().SetList({"-**/Test_A.milk"});

    EXPECT_EQ(playlist.AddPath(PROJECTM_PLAYLIST_TEST_DATA_DIR "/presets", 0, true, false), 3);
    ASSERT_EQ(playlist.Size(), 3);

    // Apply new filter that only removes Test_B
    playlist.Filter().SetList({"-**/Test_B.milk"});

    EXPECT_EQ(playlist.ApplyFilter(), 1);

    // Test_A will not reappear.
    ASSERT_EQ(playlist.Size(), 2);
}
