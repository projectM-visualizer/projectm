#pragma once

#include <PlaylistCWrapper.hpp>

#include <gmock/gmock.h>

class PlaylistCWrapperMock : public libprojectM::Playlist::PlaylistCWrapper
{
public:
    PlaylistCWrapperMock()
        : PlaylistCWrapper(nullptr){};

    // PlaylistCWrapper members
    MOCK_METHOD(void, Connect, (projectm_handle));

    // Playlist members
    MOCK_METHOD(uint32_t, Size, (), (const));
    MOCK_METHOD(bool, Empty, (), (const));
    MOCK_METHOD(void, Clear, ());
    MOCK_METHOD(const std::vector<libprojectM::Playlist::Item>&, Items, (), (const));
    MOCK_METHOD(bool, AddItem, (const std::string&, uint32_t, bool) );
    MOCK_METHOD(uint32_t, AddPath, (const std::string&, uint32_t, bool, bool) );
    MOCK_METHOD(bool, RemoveItem, (uint32_t));
    MOCK_METHOD(bool, Shuffle, (), (const));
    MOCK_METHOD(void, SetShuffle, (bool) );
    MOCK_METHOD(void, Sort, (uint32_t, uint32_t, SortPredicate, SortOrder));
    MOCK_METHOD(uint32_t, RetryCount, ());
    MOCK_METHOD(void, SetRetryCount, (uint32_t));
    MOCK_METHOD(uint32_t, NextPresetIndex, (), ());
    MOCK_METHOD(uint32_t, PreviousPresetIndex, (), ());
    MOCK_METHOD(uint32_t, LastPresetIndex, (), ());
    MOCK_METHOD(uint32_t, PresetIndex, (), (const));
    MOCK_METHOD(uint32_t, SetPresetIndex, (uint32_t));
    MOCK_METHOD(void, PlayPresetIndex, (uint32_t, bool, bool) );
    MOCK_METHOD(void, RemoveLastHistoryEntry, ());
    MOCK_METHOD(void, SetPresetSwitchedCallback, (projectm_playlist_preset_switched_event, void*) );
    MOCK_METHOD(void, SetPresetSwitchFailedCallback, (projectm_playlist_preset_switch_failed_event, void*) );
    MOCK_METHOD(class libprojectM::Playlist::Filter&, Filter, ());
    MOCK_METHOD(uint32_t, ApplyFilter, ());
};
