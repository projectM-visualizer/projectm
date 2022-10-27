#include "PlaylistCWrapper.h"

#include <limits>

using ProjectM::Playlist::Playlist;

PlaylistCWrapper::PlaylistCWrapper(projectm_handle projectMInstance)
{
}


void PlaylistCWrapper::Connect(projectm_handle projectMInstance)
{
    m_projectMInstance = projectMInstance;
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
