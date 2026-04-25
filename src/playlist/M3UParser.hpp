#pragma once

#include <cstdint>
#include <string>

namespace libprojectM {
namespace Playlist {

class Playlist;

/**
 * @brief Parses M3U and extended M3U (M3U8) playlist files.
 *
 * Supports UTF-8 encoded filenames, including non-ASCII characters
 * such as Cyrillic, CJK, and other Unicode scripts.
 */
class M3UParser
{
public:
    /**
     * @brief Loads presets from an M3U file into the given playlist.
     * @param filename Path to the .m3u or .m3u8 file.
     * @param playlist The playlist to add items to.
     * @param allowDuplicates If true, duplicate entries are allowed.
     * @return Number of presets successfully added.
     */
    static auto LoadM3U(const std::string& filename,
                        Playlist& playlist,
                        bool allowDuplicates) -> uint32_t;
};

} // namespace Playlist
} // namespace libprojectM
