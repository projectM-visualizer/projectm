#include "PlaylistCWrapper.hpp"

#include "projectM-4/callbacks.h"
#include "projectM-4/core.h"

namespace libprojectM {
namespace Playlist {

PlaylistCWrapper::PlaylistCWrapper(projectm_handle projectMInstance)
    : m_projectMInstance(projectMInstance)
{
    if (m_projectMInstance != nullptr)
    {
        projectm_set_preset_switch_requested_event_callback(m_projectMInstance, &OnPresetSwitchRequested, this);
        projectm_set_preset_switch_failed_event_callback(m_projectMInstance, &OnPresetSwitchFailed, this);
    }
}


void PlaylistCWrapper::Connect(projectm_handle projectMInstance)
{
    if (m_projectMInstance != nullptr)
    {
        projectm_set_preset_switch_requested_event_callback(m_projectMInstance, nullptr, nullptr);
        projectm_set_preset_switch_failed_event_callback(m_projectMInstance, nullptr, nullptr);
    }

    m_projectMInstance = projectMInstance;

    if (m_projectMInstance != nullptr)
    {
        projectm_set_preset_switch_requested_event_callback(m_projectMInstance, &OnPresetSwitchRequested, this);
        projectm_set_preset_switch_failed_event_callback(m_projectMInstance, &OnPresetSwitchFailed, this);
    }
}


void PlaylistCWrapper::OnPresetSwitchRequested(bool isHardCut, void* userData)
{
    if (userData == nullptr)
    {
        return;
    }

    auto* playlist = reinterpret_cast<PlaylistCWrapper*>(userData);

    try
    {
        playlist->PlayPresetIndex(playlist->NextPresetIndex(), isHardCut, true);
    }
    catch (PlaylistEmptyException&)
    {
    }
}


void PlaylistCWrapper::OnPresetSwitchFailed(const char* presetFilename, const char* message, void* userData)
{
    if (userData == nullptr)
    {
        return;
    }

    auto* playlist = reinterpret_cast<PlaylistCWrapper*>(userData);
    auto lastDirection = playlist->GetLastNavigationDirection();

    if (lastDirection != NavigationDirection::Last)
    {
        // Don't let the user go back to a broken preset.
        playlist->RemoveLastHistoryEntry();
    }

    // Preset switch may fail due to broken presets, retry a few times before giving up.
    if (playlist->m_presetSwitchFailedCount >= playlist->m_presetSwitchRetryCount)
    {
        if (playlist->m_presetSwitchFailedEventCallback != nullptr)
        {
            playlist->m_presetSwitchFailedEventCallback(presetFilename, message,
                                                        playlist->m_presetSwitchFailedEventUserData);
        }

        return;
    }

    playlist->m_presetSwitchFailedCount++;

    uint32_t playlistIndex{};
    switch (lastDirection)
    {
        case NavigationDirection::Previous:
            playlistIndex = playlist->PreviousPresetIndex();
            break;

        case NavigationDirection::Next:
            playlistIndex = playlist->NextPresetIndex();
            break;

        case NavigationDirection::Last:
            playlistIndex = playlist->LastPresetIndex();
            break;
    }

    playlist->PlayPresetIndex(playlistIndex, playlist->m_hardCutRequested, false);
}


void PlaylistCWrapper::SetRetryCount(uint32_t retryCount)
{
    m_presetSwitchRetryCount = retryCount;
}


auto PlaylistCWrapper::RetryCount() -> uint32_t
{
    return m_presetSwitchRetryCount;
}


void PlaylistCWrapper::SetPresetSwitchedCallback(projectm_playlist_preset_switched_event callback, void* userData)
{
    m_presetSwitchedEventCallback = callback;
    m_presetSwitchedEventUserData = userData;
}


void PlaylistCWrapper::SetPresetSwitchFailedCallback(projectm_playlist_preset_switch_failed_event callback, void* userData)
{
    m_presetSwitchFailedEventCallback = callback;
    m_presetSwitchFailedEventUserData = userData;
}


void PlaylistCWrapper::PlayPresetIndex(uint32_t index, bool hardCut, bool resetFailureCount)
{
    if (resetFailureCount)
    {
        m_presetSwitchFailedCount = 0;
    }

    m_hardCutRequested = hardCut;

    const auto& playlistItems = Items();

    if (playlistItems.size() <= index)
    {
        return;
    }

    projectm_load_preset_file(m_projectMInstance,
                              playlistItems.at(index).Filename().c_str(), !hardCut);

    if (m_presetSwitchedEventCallback != nullptr)
    {
        m_presetSwitchedEventCallback(hardCut, index, m_presetSwitchedEventUserData);
    }
}


void PlaylistCWrapper::SetLastNavigationDirection(PlaylistCWrapper::NavigationDirection direction)
{
    m_lastNavigationDirection = direction;
}


auto PlaylistCWrapper::GetLastNavigationDirection() const -> PlaylistCWrapper::NavigationDirection
{
    return m_lastNavigationDirection;
}

} // namespace Playlist
} // namespace libprojectM

auto playlist_handle_to_instance(projectm_playlist_handle instance) -> libprojectM::Playlist::PlaylistCWrapper*
{
    return reinterpret_cast<libprojectM::Playlist::PlaylistCWrapper*>(instance);
}


void projectm_playlist_free_string(char* string)
{
    delete[] string;
}


void projectm_playlist_free_string_array(char** array)
{
    int index{0};
    while (array[index] != nullptr)
    {
        delete[] array[index];
        index++;
    }
    delete[] array;
}


auto projectm_playlist_create(projectm_handle projectm_instance) -> projectm_playlist_handle
{
    try
    {
        auto* instance = new libprojectM::Playlist::PlaylistCWrapper(projectm_instance);
        return reinterpret_cast<projectm_playlist*>(instance);
    }
    catch (...)
    {
    }

    return nullptr;
}


void projectm_playlist_destroy(projectm_playlist_handle instance)
{
    auto* playlist = playlist_handle_to_instance(instance);
    delete playlist;
}


void projectm_playlist_set_preset_switched_event_callback(projectm_playlist_handle instance,
                                                          projectm_playlist_preset_switched_event callback,
                                                          void* user_data)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetPresetSwitchedCallback(callback, user_data);
}


void projectm_playlist_set_preset_switch_failed_event_callback(projectm_playlist_handle instance,
                                                               projectm_playlist_preset_switch_failed_event callback,
                                                               void* user_data)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetPresetSwitchFailedCallback(callback, user_data);
}


void projectm_playlist_connect(projectm_playlist_handle instance, projectm_handle projectm_instance)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->Connect(projectm_instance);
}


auto projectm_playlist_size(projectm_playlist_handle instance) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);
    return playlist->Size();
}


void projectm_playlist_clear(projectm_playlist_handle instance)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->Clear();
}


auto projectm_playlist_items(projectm_playlist_handle instance, uint32_t start, uint32_t count) -> char**
{
    auto* playlist = playlist_handle_to_instance(instance);

    const auto& items = playlist->Items();

    if (start >= items.size())
    {
        auto* array = new char* [1] {};
        return array;
    }

    size_t endPos = std::min(static_cast<size_t>(start + count), items.size());
    auto* array = new char* [endPos - start + 1] {};

    for (size_t index{start}; index < endPos; index++)
    {
        auto filename = items[index].Filename();
        array[index - start] = new char[filename.length() + 1]{};
        filename.copy(array[index - start], filename.length());
    }

    return array;
}


char* projectm_playlist_item(projectm_playlist_handle instance, uint32_t index)
{
    auto* playlist = playlist_handle_to_instance(instance);

    if (playlist->Empty() || index >= playlist->Size())
    {
        return nullptr;
    }

    auto filename = playlist->Items().at(index).Filename();
    auto buffer = new char[filename.length() + 1]{};
    filename.copy(buffer, filename.length());

    return buffer;
}


auto projectm_playlist_add_path(projectm_playlist_handle instance, const char* path,
                                bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(path, libprojectM::Playlist::Playlist::InsertAtEnd, recurse_subdirs, allow_duplicates);
}


auto projectm_playlist_insert_path(projectm_playlist_handle instance, const char* path,
                                   uint32_t index, bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(path, index, recurse_subdirs, allow_duplicates);
}


auto projectm_playlist_add_preset(projectm_playlist_handle instance, const char* filename,
                                  bool allow_duplicates) -> bool
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddItem(filename, libprojectM::Playlist::Playlist::InsertAtEnd, allow_duplicates);
}


auto projectm_playlist_insert_preset(projectm_playlist_handle instance, const char* filename,
                                     uint32_t index, bool allow_duplicates) -> bool
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddItem(filename, index, allow_duplicates);
}


uint32_t projectm_playlist_add_presets(projectm_playlist_handle instance, const char** filenames,
                                       uint32_t count, bool allow_duplicates)
{
    if (filenames == nullptr)
    {
        return 0;
    }

    auto* playlist = playlist_handle_to_instance(instance);

    uint32_t addCount{0};

    for (uint32_t index{0}; index < count; ++index)
    {
        if (filenames[index] == nullptr)
        {
            continue;
        }

        if (playlist->AddItem(filenames[index], libprojectM::Playlist::Playlist::InsertAtEnd, allow_duplicates))
        {
            addCount++;
        }
    }

    return addCount;
}


auto projectm_playlist_insert_presets(projectm_playlist_handle instance, const char** filenames,
                                      uint32_t count, unsigned int index, bool allow_duplicates) -> uint32_t
{
    if (filenames == nullptr)
    {
        return 0;
    }

    auto* playlist = playlist_handle_to_instance(instance);

    uint32_t addCount{0};

    for (uint32_t filenameIndex{0}; filenameIndex < count; ++filenameIndex)
    {
        if (filenames[filenameIndex] == nullptr)
        {
            continue;
        }

        if (playlist->AddItem(filenames[filenameIndex], index + addCount, allow_duplicates))
        {
            addCount++;
        }
    }

    return addCount;
}


bool projectm_playlist_remove_preset(projectm_playlist_handle instance, uint32_t index)
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->RemoveItem(index);
}


uint32_t projectm_playlist_remove_presets(projectm_playlist_handle instance, uint32_t index,
                                          uint32_t count)
{
    auto* playlist = playlist_handle_to_instance(instance);

    uint32_t removeCount{0};

    for (uint32_t iteration{0}; iteration < count; ++iteration)
    {
        if (!playlist->RemoveItem(index))
        {
            // No need to iterate further, as the end of the playlist was reached.
            break;
        }

        removeCount++;
    }

    return removeCount;
}


bool projectm_playlist_get_shuffle(projectm_playlist_handle instance)
{
    auto* playlist = playlist_handle_to_instance(instance);
    return playlist->Shuffle();
}


void projectm_playlist_set_shuffle(projectm_playlist_handle instance, bool shuffle)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetShuffle(shuffle);
}


void projectm_playlist_sort(projectm_playlist_handle instance, uint32_t start_index, uint32_t count,
                            projectm_playlist_sort_predicate predicate, projectm_playlist_sort_order order)
{
    auto* playlist = playlist_handle_to_instance(instance);

    libprojectM::Playlist::Playlist::SortPredicate predicatePlaylist{libprojectM::Playlist::Playlist::SortPredicate::FullPath};
    libprojectM::Playlist::Playlist::SortOrder orderPlaylist{libprojectM::Playlist::Playlist::SortOrder::Ascending};

    if (predicate == SORT_PREDICATE_FILENAME_ONLY)
    {
        predicatePlaylist = libprojectM::Playlist::Playlist::SortPredicate::FilenameOnly;
    }

    if (order == SORT_ORDER_DESCENDING)
    {
        orderPlaylist = libprojectM::Playlist::Playlist::SortOrder::Descending;
    }

    playlist->Sort(start_index, count, predicatePlaylist, orderPlaylist);
}


uint32_t projectm_playlist_get_retry_count(projectm_playlist_handle instance)
{
    auto* playlist = playlist_handle_to_instance(instance);
    return playlist->RetryCount();
}


void projectm_playlist_set_retry_count(projectm_playlist_handle instance, uint32_t retry_count)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetRetryCount(retry_count);
}


auto projectm_playlist_get_position(projectm_playlist_handle instance) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);
    try
    {
        return playlist->PresetIndex();
    }
    catch (libprojectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}


auto projectm_playlist_set_position(projectm_playlist_handle instance, uint32_t new_position,
                                    bool hard_cut) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetLastNavigationDirection(libprojectM::Playlist::PlaylistCWrapper::NavigationDirection::Next);
    try
    {
        auto newIndex = playlist->SetPresetIndex(new_position);
        playlist->PlayPresetIndex(newIndex, hard_cut, true);
        return playlist->PresetIndex();
    }
    catch (libprojectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}


uint32_t projectm_playlist_play_next(projectm_playlist_handle instance, bool hard_cut)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetLastNavigationDirection(libprojectM::Playlist::PlaylistCWrapper::NavigationDirection::Next);
    try
    {
        auto newIndex = playlist->NextPresetIndex();
        playlist->PlayPresetIndex(newIndex, hard_cut, true);
        return playlist->PresetIndex();
    }
    catch (libprojectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}


uint32_t projectm_playlist_play_previous(projectm_playlist_handle instance, bool hard_cut)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetLastNavigationDirection(libprojectM::Playlist::PlaylistCWrapper::NavigationDirection::Previous);
    try
    {
        auto newIndex = playlist->PreviousPresetIndex();
        playlist->PlayPresetIndex(newIndex, hard_cut, true);
        return playlist->PresetIndex();
    }
    catch (libprojectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}


uint32_t projectm_playlist_play_last(projectm_playlist_handle instance, bool hard_cut)
{
    auto* playlist = playlist_handle_to_instance(instance);

    playlist->SetLastNavigationDirection(libprojectM::Playlist::PlaylistCWrapper::NavigationDirection::Last);
    try
    {
        auto newIndex = playlist->LastPresetIndex();
        playlist->PlayPresetIndex(newIndex, hard_cut, true);
        return playlist->PresetIndex();
    }
    catch (libprojectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}


void projectm_playlist_set_filter(projectm_playlist_handle instance, const char** filter_list,
                                  size_t count)
{
    auto* playlist = playlist_handle_to_instance(instance);

    std::vector<std::string> filterList;
    for (size_t index = 0; index < count; index++)
    {
        if (filter_list[index] == nullptr)
        {
            continue;
        }
        filterList.emplace_back(filter_list[index]);
    }

    playlist->Filter().SetList(filterList);
}


auto projectm_playlist_get_filter(projectm_playlist_handle instance, size_t* count) -> char**
{
    auto* playlist = playlist_handle_to_instance(instance);

    const auto& filterList = playlist->Filter().List();

    auto* array = new char* [filterList.size() + 1] {};

    int index{0};
    for (const auto& filter : filterList)
    {
        array[index] = new char[filter.length() + 1]{};
        filter.copy(array[index], filter.length());
        index++;
    }

    *count = filterList.size();
    return array;
}


auto projectm_playlist_apply_filter(projectm_playlist_handle instance) -> size_t
{
    auto* playlist = playlist_handle_to_instance(instance);
    return playlist->ApplyFilter();
}