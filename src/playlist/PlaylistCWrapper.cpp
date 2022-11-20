#include "PlaylistCWrapper.h"

using ProjectM::Playlist::Playlist;

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
    catch (ProjectM::Playlist::PlaylistEmptyException&)
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

    // Preset switch may fail due to broken presets, retry a few times before giving up.
    if (playlist->m_presetSwitchFailedCount < playlist->m_presetSwitchRetryCount)
    {
        playlist->m_presetSwitchFailedCount++;
        playlist->PlayPresetIndex(playlist->NextPresetIndex(), playlist->m_hardCutRequested, false);
    }
    else
    {
        if (playlist->m_presetSwitchFailedEventCallback != nullptr)
        {
            playlist->m_presetSwitchFailedEventCallback(presetFilename, message,
                                                        playlist->m_presetSwitchFailedEventUserData);
        }
    }
}


void PlaylistCWrapper::SetRetryCount(uint32_t retryCount)
{
    m_presetSwitchRetryCount = retryCount;
}


void PlaylistCWrapper::SetPresetWitchFailedCallback(projectm_playlist_preset_switch_failed_event callback, void* userData)
{
    m_presetSwitchFailedEventCallback = callback;
    m_presetSwitchFailedEventUserData = userData;
}


void PlaylistCWrapper::PlayPresetIndex(size_t index, bool hardCut, bool resetFailureCount)
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
}


auto playlist_handle_to_instance(projectm_playlist_handle instance) -> PlaylistCWrapper*
{
    return reinterpret_cast<PlaylistCWrapper*>(instance);
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
        auto* instance = new PlaylistCWrapper(projectm_instance);
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


void projectm_playlist_set_preset_switch_failed_event_callback(projectm_playlist_handle instance,
                                                               projectm_playlist_preset_switch_failed_event callback,
                                                               void* user_data)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetPresetWitchFailedCallback(callback, user_data);
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


auto projectm_playlist_items(projectm_playlist_handle instance) -> char**
{
    auto* playlist = playlist_handle_to_instance(instance);

    auto& items = playlist->Items();

    auto* array = new char* [items.size() + 1] {};

    int index{0};
    for (const auto& item : items)
    {
        auto filename = item.Filename();
        array[index] = new char[filename.length() + 1]{};
        filename.copy(array[index], filename.length());
        index++;
    }

    return array;
}


auto projectm_playlist_add_path(projectm_playlist_handle instance, const char* path,
                                bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(path, Playlist::InsertAtEnd, recurse_subdirs, allow_duplicates);
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

    return playlist->AddItem(filename, Playlist::InsertAtEnd, allow_duplicates);
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

        if (playlist->AddItem(filenames[index], Playlist::InsertAtEnd, allow_duplicates))
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


void projectm_playlist_set_shuffle(projectm_playlist_handle instance, bool shuffle)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->Shuffle(shuffle);
}


void projectm_playlist_sort(projectm_playlist_handle instance, uint32_t start_index, uint32_t count,
                            projectm_playlist_sort_predicate predicate, projectm_playlist_sort_order order)
{
    auto* playlist = playlist_handle_to_instance(instance);

    Playlist::SortPredicate predicatePlaylist{ProjectM::Playlist::Playlist::SortPredicate::FullPath};
    Playlist::SortOrder orderPlaylist{ProjectM::Playlist::Playlist::SortOrder::Ascending};

    if (predicate == SORT_PREDICATE_FILENAME_ONLY)
    {
        predicatePlaylist = ProjectM::Playlist::Playlist::SortPredicate::FilenameOnly;
    }

    if (order == SORT_ORDER_DESCENDING)
    {
        orderPlaylist = ProjectM::Playlist::Playlist::SortOrder::Descending;
    }

    playlist->Sort(start_index, count, predicatePlaylist, orderPlaylist);
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
    catch (ProjectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}


auto projectm_playlist_set_position(projectm_playlist_handle instance, uint32_t new_position,
                                    bool hard_cut) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);
    try
    {
        auto newIndex = playlist->SetPresetIndex(new_position);
        playlist->PlayPresetIndex(newIndex, hard_cut, true);
        return newIndex;
    }
    catch (ProjectM::Playlist::PlaylistEmptyException&)
    {
        return 0;
    }
}
