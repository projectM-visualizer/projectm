#include "PlaylistCWrapper.hpp"

#include "projectM-4/callbacks.h"
#include "projectM-4/core.h"

#include <PathEncoding.hpp>

namespace libprojectM {
namespace Playlist {

namespace {

/**
 * @brief Registers the playlist's preset switch failed callback with projectM, or removes it.
 *
 * On Windows, the wide-character callback is used, so file names and messages arrive unchanged.
 * @param projectMInstance The projectM instance.
 * @param playlist The playlist to register, or nullptr to remove the callback.
 */
void RegisterPresetSwitchFailedCallback(projectm_handle projectMInstance, PlaylistCWrapper* playlist)
{
#ifdef _WIN32
    projectm_set_preset_switch_failed_event_callback(projectMInstance, nullptr, nullptr);
    projectm_set_preset_switch_failed_event_callbackW(projectMInstance,
                                                      playlist != nullptr ? &PlaylistCWrapper::OnPresetSwitchFailedW : nullptr,
                                                      playlist);
#else
    projectm_set_preset_switch_failed_event_callback(projectMInstance,
                                                     playlist != nullptr ? &PlaylistCWrapper::OnPresetSwitchFailed : nullptr,
                                                     playlist);
#endif
}

} // namespace

PlaylistCWrapper::PlaylistCWrapper(projectm_handle projectMInstance)
    : m_projectMInstance(projectMInstance)
{
    if (m_projectMInstance != nullptr)
    {
        projectm_set_preset_switch_requested_event_callback(m_projectMInstance, &OnPresetSwitchRequested, this);
        RegisterPresetSwitchFailedCallback(m_projectMInstance, this);
    }
}


void PlaylistCWrapper::Connect(projectm_handle projectMInstance)
{
    if (m_projectMInstance != nullptr)
    {
        projectm_set_preset_switch_requested_event_callback(m_projectMInstance, nullptr, nullptr);
        RegisterPresetSwitchFailedCallback(m_projectMInstance, nullptr);
    }

    m_projectMInstance = projectMInstance;

    if (m_projectMInstance != nullptr)
    {
        projectm_set_preset_switch_requested_event_callback(m_projectMInstance, &OnPresetSwitchRequested, this);
        RegisterPresetSwitchFailedCallback(m_projectMInstance, this);
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
        playlist->PlayPresetIndex(playlist->NextPresetIndex(), isHardCut);
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

    playlist->m_lastPresetSwitchFailed = true;
    playlist->m_lastFailedPresetFileName = PathEncoding::FromNarrowApi(presetFilename);
    playlist->m_lastFailedPresetError = PathEncoding::FromNarrowApi(message);

    auto lastDirection = playlist->GetLastNavigationDirection();

    if (lastDirection != NavigationDirection::Last)
    {
        // Don't let the user go back to a broken preset.
        playlist->RemoveLastHistoryEntry();
    }
}


#ifdef _WIN32
void PlaylistCWrapper::OnPresetSwitchFailedW(const wchar_t* presetFilename, const wchar_t* message, void* userData)
{
    if (userData == nullptr)
    {
        return;
    }

    auto* playlist = reinterpret_cast<PlaylistCWrapper*>(userData);

    playlist->m_lastPresetSwitchFailed = true;
    playlist->m_lastFailedPresetFileName = PathEncoding::WideToUtf8(presetFilename);
    playlist->m_lastFailedPresetError = PathEncoding::WideToUtf8(message);

    auto lastDirection = playlist->GetLastNavigationDirection();

    if (lastDirection != NavigationDirection::Last)
    {
        // Don't let the user go back to a broken preset.
        playlist->RemoveLastHistoryEntry();
    }
}
#endif


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


void PlaylistCWrapper::SetPresetLoadCallback(projectm_playlist_preset_load_event callback, void* userData)
{
    m_presetLoadEventCallback = callback;
    m_presetLoadEventUserData = userData;
}


#ifdef _WIN32
void PlaylistCWrapper::SetPresetSwitchFailedCallbackW(projectm_playlist_preset_switch_failed_eventW callback, void* userData)
{
    m_presetSwitchFailedEventCallbackW = callback;
    m_presetSwitchFailedEventUserDataW = userData;
}


void PlaylistCWrapper::SetPresetLoadCallbackW(projectm_playlist_preset_load_eventW callback, void* userData)
{
    m_presetLoadEventCallbackW = callback;
    m_presetLoadEventUserDataW = userData;
}
#endif


void PlaylistCWrapper::PlayPresetIndex(uint32_t index, bool hardCut)
{
    m_hardCutRequested = hardCut;

    auto& playlistItems = Items();

    uint32_t failedCount = 0;
    while (true)
    {
        if (playlistItems.size() <= index)
        {
            return;
        }

        const auto& filename = playlistItems.at(index).Filename();

        // If a preset load callback is set, give the application a chance to handle loading.
        // Application handled the load - return without further action.
        // The app is responsible for calling projectm_load_preset_file/data,
        // handling errors, and firing the switched event when ready.
#ifdef _WIN32
        if (m_presetLoadEventCallbackW != nullptr &&
            m_presetLoadEventCallbackW(index, PathEncoding::Utf8ToWide(filename).c_str(), hardCut, m_presetLoadEventUserDataW))
        {
            return;
        }
#endif
        if (m_presetLoadEventCallback != nullptr &&
            m_presetLoadEventCallback(index, PathEncoding::ToNarrowApi(filename).c_str(), hardCut, m_presetLoadEventUserData))
        {
            return;
        }

        // Default behavior: load from filesystem. Item file names are UTF-8, so on Windows, use the
        // wide-character function, which can load files with any name.
#ifdef _WIN32
        projectm_load_preset_fileW(m_projectMInstance, PathEncoding::Utf8ToWide(filename).c_str(), !hardCut);
#else
        projectm_load_preset_file(m_projectMInstance, filename.c_str(), !hardCut);
#endif

        if (!m_lastPresetSwitchFailed)
        {
            break;
        }

        failedCount++;

        if (failedCount >= m_presetSwitchRetryCount)
        {
            if (m_presetSwitchFailedEventCallback != nullptr)
            {
                m_presetSwitchFailedEventCallback(PathEncoding::ToNarrowApi(m_lastFailedPresetFileName).c_str(),
                                                  PathEncoding::ToNarrowApi(m_lastFailedPresetError).c_str(),
                                                  m_presetSwitchFailedEventUserData);
            }
#ifdef _WIN32
            if (m_presetSwitchFailedEventCallbackW != nullptr)
            {
                m_presetSwitchFailedEventCallbackW(PathEncoding::Utf8ToWide(m_lastFailedPresetFileName).c_str(),
                                                   PathEncoding::Utf8ToWide(m_lastFailedPresetError).c_str(),
                                                   m_presetSwitchFailedEventUserDataW);
            }
#endif

            return;
        }

        m_lastPresetSwitchFailed = false;

        // Remove failed preset from playlist
        RemoveItem(index);

        if (playlistItems.empty())
        {
            return;
        }

        // Set next index to proper value depending on navigation
        switch (GetLastNavigationDirection())
        {
            case NavigationDirection::Last:
                index = LastPresetIndex();
                break;
            case NavigationDirection::Next:
                index = NextPresetIndex();
                break;
            case NavigationDirection::Previous:
                index = PreviousPresetIndex();
                break;
        }
    }

    if (m_presetSwitchedEventCallback != nullptr)
    {
        m_presetSwitchedEventCallback(hardCut, index, m_presetSwitchedEventUserData);
    }
}


void PlaylistCWrapper::SetLastNavigationDirection(NavigationDirection direction)
{
    m_lastNavigationDirection = direction;
}


auto PlaylistCWrapper::GetLastNavigationDirection() const -> NavigationDirection
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


namespace {

/**
 * @brief Copies a string into a new buffer for the API, freed with projectm_playlist_free_string().
 * @param text The string to copy.
 * @return A newly allocated, zero-terminated copy of the string.
 */
auto CopyToApi(const std::string& text) -> char*
{
    auto* buffer = new char[text.length() + 1]{};
    text.copy(buffer, text.length());
    return buffer;
}

#ifdef _WIN32
/**
 * @brief Copies a UTF-8 string as UTF-16 into a new buffer, freed with projectm_playlist_free_wstring().
 * @param utf8 The UTF-8 string to copy.
 * @return A newly allocated, zero-terminated UTF-16 copy of the string.
 */
auto CopyToApiW(const std::string& utf8) -> wchar_t*
{
    const auto wide = libprojectM::PathEncoding::Utf8ToWide(utf8);
    auto* buffer = new wchar_t[wide.length() + 1]{};
    wide.copy(buffer, wide.length());
    return buffer;
}
#endif

/**
 * @brief Returns a newly allocated, null-terminated array of copies of the items in the given range.
 * @tparam CharType char or wchar_t.
 * @param items The playlist items.
 * @param start The first item.
 * @param count The maximum number of items.
 * @param copy The function copying an item's UTF-8 file name into a new buffer.
 * @return The array.
 */
template<typename CharType, typename CopyFunction>
auto CopyItems(const std::vector<libprojectM::Playlist::Item>& items, uint32_t start, uint32_t count, CopyFunction copy) -> CharType**
{
    if (start >= items.size())
    {
        return new CharType*[1]{};
    }

    size_t endPos = std::min(static_cast<size_t>(start) + count, items.size());
    auto* array = new CharType*[endPos - start + 1]{};

    for (size_t index{start}; index < endPos; index++)
    {
        array[index - start] = copy(items[index].Filename());
    }

    return array;
}

} // namespace


#ifdef _WIN32
void projectm_playlist_free_wstring(wchar_t* string)
{
    delete[] string;
}


void projectm_playlist_free_wstring_array(wchar_t** array)
{
    int index{0};
    while (array[index] != nullptr)
    {
        delete[] array[index];
        index++;
    }
    delete[] array;
}
#endif


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


#ifdef _WIN32
void projectm_playlist_set_preset_switch_failed_event_callbackW(projectm_playlist_handle instance,
                                                                projectm_playlist_preset_switch_failed_eventW callback,
                                                                void* user_data)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetPresetSwitchFailedCallbackW(callback, user_data);
}
#endif


void projectm_playlist_set_preset_load_event_callback(projectm_playlist_handle instance,
                                                      projectm_playlist_preset_load_event callback,
                                                      void* user_data)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetPresetLoadCallback(callback, user_data);
}


#ifdef _WIN32
void projectm_playlist_set_preset_load_event_callbackW(projectm_playlist_handle instance,
                                                       projectm_playlist_preset_load_eventW callback,
                                                       void* user_data)
{
    auto* playlist = playlist_handle_to_instance(instance);
    playlist->SetPresetLoadCallbackW(callback, user_data);
}
#endif


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


// File names are stored as UTF-8. The narrow-string functions convert from and to the narrow API
// encoding (the active ANSI code page on Windows), the "W" functions from and to UTF-16.

auto projectm_playlist_items(projectm_playlist_handle instance, uint32_t start, uint32_t count) -> char**
{
    auto* playlist = playlist_handle_to_instance(instance);

    return CopyItems<char>(playlist->Items(), start, count, [](const std::string& filename) {
        return CopyToApi(libprojectM::PathEncoding::ToNarrowApi(filename));
    });
}


char* projectm_playlist_item(projectm_playlist_handle instance, uint32_t index)
{
    auto* playlist = playlist_handle_to_instance(instance);

    if (playlist->Empty() || index >= playlist->Size())
    {
        return nullptr;
    }

    return CopyToApi(libprojectM::PathEncoding::ToNarrowApi(playlist->Items().at(index).Filename()));
}


auto projectm_playlist_add_path(projectm_playlist_handle instance, const char* path,
                                bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(libprojectM::PathEncoding::FromNarrowApi(path), libprojectM::Playlist::Playlist::InsertAtEnd, recurse_subdirs, allow_duplicates);
}


auto projectm_playlist_insert_path(projectm_playlist_handle instance, const char* path,
                                   uint32_t index, bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(libprojectM::PathEncoding::FromNarrowApi(path), index, recurse_subdirs, allow_duplicates);
}


auto projectm_playlist_add_preset(projectm_playlist_handle instance, const char* filename,
                                  bool allow_duplicates) -> bool
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddItem(libprojectM::PathEncoding::FromNarrowApi(filename), libprojectM::Playlist::Playlist::InsertAtEnd, allow_duplicates);
}


auto projectm_playlist_insert_preset(projectm_playlist_handle instance, const char* filename,
                                     uint32_t index, bool allow_duplicates) -> bool
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddItem(libprojectM::PathEncoding::FromNarrowApi(filename), index, allow_duplicates);
}


namespace {

/**
 * @brief Adds a list of presets to the playlist.
 * @tparam CharType char or wchar_t.
 * @param instance The playlist handle.
 * @param filenames The file names, nullptr entries are skipped.
 * @param count The number of file names.
 * @param index The index to insert the first preset at, or InsertAtEnd.
 * @param allowDuplicates If true, presets already in the playlist are added again.
 * @param toUtf8 The function converting a file name to UTF-8.
 * @return The number of added presets.
 */
template<typename CharType, typename ConvertFunction>
auto AddPresets(projectm_playlist_handle instance, const CharType** filenames, uint32_t count,
                uint32_t index, bool allowDuplicates, ConvertFunction toUtf8) -> uint32_t
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

        uint32_t currentIndex{libprojectM::Playlist::Playlist::InsertAtEnd};
        if (index != libprojectM::Playlist::Playlist::InsertAtEnd)
        {
            currentIndex = index + addCount;
        }

        if (playlist->AddItem(toUtf8(filenames[filenameIndex]), currentIndex, allowDuplicates))
        {
            addCount++;
        }
    }

    return addCount;
}

} // namespace


uint32_t projectm_playlist_add_presets(projectm_playlist_handle instance, const char** filenames,
                                       uint32_t count, bool allow_duplicates)
{
    return AddPresets(instance, filenames, count, libprojectM::Playlist::Playlist::InsertAtEnd, allow_duplicates,
                      &libprojectM::PathEncoding::FromNarrowApi);
}


auto projectm_playlist_insert_presets(projectm_playlist_handle instance, const char** filenames,
                                      uint32_t count, unsigned int index, bool allow_duplicates) -> uint32_t
{
    return AddPresets(instance, filenames, count, index, allow_duplicates,
                      &libprojectM::PathEncoding::FromNarrowApi);
}


#ifdef _WIN32
auto projectm_playlist_itemsW(projectm_playlist_handle instance, uint32_t start, uint32_t count) -> wchar_t**
{
    auto* playlist = playlist_handle_to_instance(instance);

    return CopyItems<wchar_t>(playlist->Items(), start, count, &CopyToApiW);
}


wchar_t* projectm_playlist_itemW(projectm_playlist_handle instance, uint32_t index)
{
    auto* playlist = playlist_handle_to_instance(instance);

    if (playlist->Empty() || index >= playlist->Size())
    {
        return nullptr;
    }

    return CopyToApiW(playlist->Items().at(index).Filename());
}


auto projectm_playlist_add_pathW(projectm_playlist_handle instance, const wchar_t* path,
                                 bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(libprojectM::PathEncoding::WideToUtf8(path), libprojectM::Playlist::Playlist::InsertAtEnd, recurse_subdirs, allow_duplicates);
}


auto projectm_playlist_insert_pathW(projectm_playlist_handle instance, const wchar_t* path,
                                    uint32_t index, bool recurse_subdirs, bool allow_duplicates) -> uint32_t
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddPath(libprojectM::PathEncoding::WideToUtf8(path), index, recurse_subdirs, allow_duplicates);
}


auto projectm_playlist_add_presetW(projectm_playlist_handle instance, const wchar_t* filename,
                                   bool allow_duplicates) -> bool
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddItem(libprojectM::PathEncoding::WideToUtf8(filename), libprojectM::Playlist::Playlist::InsertAtEnd, allow_duplicates);
}


auto projectm_playlist_insert_presetW(projectm_playlist_handle instance, const wchar_t* filename,
                                      uint32_t index, bool allow_duplicates) -> bool
{
    auto* playlist = playlist_handle_to_instance(instance);

    return playlist->AddItem(libprojectM::PathEncoding::WideToUtf8(filename), index, allow_duplicates);
}


auto projectm_playlist_add_presetsW(projectm_playlist_handle instance, const wchar_t** filenames,
                                    uint32_t count, bool allow_duplicates) -> uint32_t
{
    return AddPresets(instance, filenames, count, libprojectM::Playlist::Playlist::InsertAtEnd, allow_duplicates,
                      [](const wchar_t* filename) { return libprojectM::PathEncoding::WideToUtf8(filename); });
}


auto projectm_playlist_insert_presetsW(projectm_playlist_handle instance, const wchar_t** filenames,
                                       uint32_t count, unsigned int index, bool allow_duplicates) -> uint32_t
{
    return AddPresets(instance, filenames, count, index, allow_duplicates,
                      [](const wchar_t* filename) { return libprojectM::PathEncoding::WideToUtf8(filename); });
}
#endif


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
        playlist->PlayPresetIndex(newIndex, hard_cut);
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
        playlist->PlayPresetIndex(newIndex, hard_cut);
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
        playlist->PlayPresetIndex(newIndex, hard_cut);
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
        playlist->PlayPresetIndex(newIndex, hard_cut);
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
        filterList.emplace_back(libprojectM::PathEncoding::FromNarrowApi(filter_list[index]));
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
        array[index] = CopyToApi(libprojectM::PathEncoding::ToNarrowApi(filter));
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