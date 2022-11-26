#pragma once

#include <string>

namespace ProjectM {
namespace Playlist {

class Item
{
public:
    Item() = delete;

    explicit Item(std::string filename);

    /**
     * @brief Returns the filename of the playlist item.
     * @return The full path and filename of the playlist item.
     */
    auto Filename() const -> std::string;

    /**
     * @brief Filename comparator.
     * @param other The preset filename to compare.
     * @return True if the filename of the current item is equal to the given one, false if not.
     */
    auto operator==(const std::string& other) const -> bool;

private:
    std::string m_filename;
};

} // namespace Playlist
} // namespace ProjectM
