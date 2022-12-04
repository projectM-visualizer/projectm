#pragma once

#include "Filter.hpp"
#include "Item.hpp"

#include <cstdint>
#include <limits>
#include <list>
#include <random>
#include <string>
#include <vector>

namespace ProjectM {
namespace Playlist {

/**
 * Exception thrown by methods that require at least one item in the playlist to return a proper result.
 */
class PlaylistEmptyException : public std::exception
{
public:
    /**
     * Returns the exception message.
     * @return The exception message.
     */
    const char* what() const noexcept override;
};


/**
 * @brief Playlist manager class.
 *
 * This class contains a list of the presets (called playlist "Items" here) and additional settings
 * required for playback control.
 */
class Playlist
{
public:
    /**
     * Short-hand constant which can be used in AddItem() to add new presets at the end of the playlist.
     */
    static constexpr auto InsertAtEnd = std::numeric_limits<uint32_t>::max();

    /**
     * Maximum number of items in the playback history.
     */
    static constexpr size_t MaxHistoryItems = 1000;

    /**
     * Sort predicate.
     */
    enum class SortPredicate
    {
        FullPath,    //!< Sort lexically by full filesystem path.
        FilenameOnly //!< Sort lexically by filename only, ignoring the directories.
    };

    /**
     * Sort order.
     */
    enum class SortOrder
    {
        Ascending, //!< Sort in ascending order.
        Descending //!< Sort in descending order.
    };

    /**
     * Constructor.
     */
    Playlist();

    /**
     * Destructor.
     */
    virtual ~Playlist() = default;

    /**
     * @brief Returns the number of items in the current playlist.
     * @return The number of items in the current playlist.
     */
    virtual uint32_t Size() const;

    /**
     * @brief Returns if the playlist is empty.
     * Calling this is a bit more performant than comparing Size() to 0.
     * Don't confuse with Clear(), this method does not clear the playlist.
     * @return True if the playlist is empty, false if it contains at least one item.
     */
    virtual bool Empty() const;

    /**
     * @brief Clears the current playlist.
     */
    virtual void Clear();

    /**
     * @brief Returns the playlist items.
     * @return A vector with items in the current playlist.
     */
    virtual const std::vector<Item>& Items() const;

    /**
     * @brief Adds a preset file to the playlist.
     *
     * Use Playlist::InsertAtEnd as index to always insert an item at the end of the playlist.
     *
     * @param filename The file path and name to add.
     * @param index The index to insert the preset at. If larger than the playlist size, it's added
     *              to the end of the playlist.
     * @param allowDuplicates If true, duplicate files are allowed. If false, identical filenames
     *                        (including the path) are not added if already present.
     * @return True if the preset was added, false if it already existed.
     */
    virtual auto AddItem(const std::string& filename, uint32_t index, bool allowDuplicates) -> bool;

    /**
     * @brief Adds presets (recursively) from the given path.
     *
     * The function will scan the given path (and possible subdirs) for files with a .milk extension
     * and and the to the playlist, starting at the given index.
     *
     * The order of the added files is unspecified. Use the Sort() method to sort the playlist or
     * the newly added range.
     *
     * @param path The path to scan for preset files.
     * @param index The index to insert the files at. If larger than the playlist size, it's added
*                   to the end of the playlist.
     * @param recursive True to recursively scan subdirectories. False to only scan the exact
     *                  directory given.
     * @param allowDuplicates If true, duplicate files are allowed. If false, identical filenames
*                             (including the path) are not added if already present.
     * @return The number of new presets added to the playlist.
     */
    virtual auto AddPath(const std::string& path, uint32_t index, bool recursive,
                         bool allowDuplicates) -> uint32_t;

    /**
     * @brief Removed a playlist item at the given playlist index.
     * @param index The index to remove.
     * @return True if an item was removed, false if the index was out of bounds and no item was
     *         removed..
     */
    virtual auto RemoveItem(uint32_t index) -> bool;

    /**
     * @brief Enables or disabled shuffle mode.
     * @param enabled True to enable shuffle mode, false to disable.
     */
    virtual void SetShuffle(bool enabled);

    /**
     * @brief Returns the enable state of shuffle mode.
     * @return True if shuffle is enabled, false if not.
     */
    virtual auto Shuffle() const -> bool;

    /**
     * @brief Sorts the whole or a part of the playlist according to the options.
     *
     * Sorting is case-sensitive.
     *
     * @param startIndex The index to start sorting at. If the index is larger than the last
     *                   item index, the playlist will remain unchanged.
     * @param count The number of items to sort. If the value exceeds the playlist length, only
     *              items until the playlist end are sorted.
     * @param predicate The sort predicate.
     * @param order The sort order.
     */
    virtual void Sort(uint32_t startIndex, uint32_t count, SortPredicate predicate, SortOrder order);

    /**
     * @brief Returns the next preset index that should be played.
     *
     * Each call will either increment the current index, or select a random preset, depending on
     * the shuffle setting.
     *
     * @throws PlaylistEmptyException Thrown if the playlist is currently empty.
     * @return The index of the next playlist item to be played.
     */
    virtual auto NextPresetIndex() -> size_t;

    /**
     * @brief Returns the previous preset index in the playlist.
     *
     * Each call will either decrement the current index, or select a random preset, depending on
     * the shuffle setting.
     *
     * @throws PlaylistEmptyException Thrown if the playlist is currently empty.
     * @return The index of the previous playlist item.
     */
    virtual auto PreviousPresetIndex() -> size_t;

    /**
     * @brief Returns the last preset index that has been played.
     *
     * Each call will pop the last history item. If the history is empty, it will internally call
     * PreviousPresetIndex(), but not add a history item.
     *
     * @throws PlaylistEmptyException Thrown if the playlist is currently empty.
     * @return The index of the last (or previous) playlist item.
     */
    virtual auto LastPresetIndex() -> size_t;

    /**
     * @brief Returns the current playlist/preset index without changing the position.
     * @throws PlaylistEmptyException Thrown if the playlist is currently empty.
     * @return The current preset index being played.
     */
    virtual auto PresetIndex() const -> size_t;

    /**
     * @brief Sets the playlist/preset index to the given value and returns the new index.
     *
     * Will wrap to 0 if the playlist size is smaller than the given value. Ignores the shuffle
     * setting.
     *
     * @throws PlaylistEmptyException Thrown if the playlist is currently empty.
     * @param presetIndex The new preset index to switch to.
     * @return The newly set preset index, either presetIndex or 0 if out of bounds.
     */
    virtual auto SetPresetIndex(size_t presetIndex) -> size_t;

    /**
     * @brief Removes the newest entry in the playback history.
     * Useful if the last playlist item failed to load, so it won't get selected again.
     */
    virtual void RemoveLastHistoryEntry();

    /**
     * @brief Returns the current playlist filter list.
     * @return The filter list for the current playlist.
     */
    virtual auto Filter() -> class Filter&;

    /**
     * @brief Applies the current filter list to the existing playlist.
     *
     * Note this function only removes items. Previously filtered items are not added again.
     *
     * @return The number of filtered (removed) items.
     */
    virtual auto ApplyFilter() -> size_t;

private:
    /**
     * @brief Adds a preset to the history and trims the list if it gets too long.
     */
    void AddCurrentPresetIndexToHistory();

    std::vector<Item> m_items;         //!< All items in the current playlist.
    class Filter m_filter;             //!< Item filter.
    bool m_shuffle{false};             //!< True if shuffle mode is enabled, false to play presets in order.
    size_t m_currentPosition{0};       //!< Current playlist position.
    std::list<size_t> m_presetHistory; //!< The playback history.

    std::default_random_engine m_randomGenerator;
};

} // namespace Playlist
} // namespace ProjectM
