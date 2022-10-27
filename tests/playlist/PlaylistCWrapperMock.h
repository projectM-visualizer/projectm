#pragma once

#include <PlaylistCWrapper.h>

#include <gmock/gmock.h>

class PlaylistCWrapperMock : public PlaylistCWrapper
{
public:
    PlaylistCWrapperMock() : PlaylistCWrapper(nullptr) {};

    // PlaylistCWrapper members
    MOCK_METHOD(void, Connect, (projectm_handle));

    // Playlist members
    MOCK_METHOD(uint32_t, Size, (), (const));
    MOCK_METHOD(void, Clear, ());
    MOCK_METHOD(const std::vector<ProjectM::Playlist::Item>&, Items, (), (const));
    MOCK_METHOD(bool, AddItem, (const std::string&, uint32_t, bool));
    MOCK_METHOD(uint32_t, AddPath, (const std::string&, uint32_t, bool, bool));
    MOCK_METHOD(bool, RemoveItem, (uint32_t));
    MOCK_METHOD(void, Shuffle, (bool));
    MOCK_METHOD(void, Sort, (uint32_t, uint32_t, SortPredicate, SortOrder));
};
