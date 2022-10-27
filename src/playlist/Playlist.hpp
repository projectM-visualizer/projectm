#pragma once

#include "Item.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace ProjectM {
namespace Playlist {

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
     * Destructor.
     */
    virtual ~Playlist() = default;

    /**
     * @brief Returns the number of items in the current playlist.
     * @return The number of items in the current playlist.
     */
    virtual uint32_t Size() const;

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
    virtual void Shuffle(bool enabled);

    /**
     * @brief Returns the enable state of shuffle mode.
     * @return True if shuffle is enabled, false if not.
     */
    virtual auto Shuffle() -> bool;

private:
    std::vector<Item> m_items; //!< Items in the current playlist.
    bool m_shuffle{false};     //!< True if shuffle mode is enabled, false to play presets in order.
};

} // namespace Playlist
} // namespace ProjectM
