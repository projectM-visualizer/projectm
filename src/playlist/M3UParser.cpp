#include "M3UParser.hpp"
#include "Playlist.hpp"

#include <fstream>
#include <string>

namespace libprojectM {
namespace Playlist {

auto M3UParser::LoadM3U(const std::string& filename,
                        Playlist& playlist,
                        bool allowDuplicates) -> uint32_t
{
    // Open in binary mode to avoid any platform newline translation
    // that could corrupt multi-byte UTF-8 sequences
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return 0;
    }

    uint32_t addedCount{0};
    std::string line;

    while (std::getline(file, line))
    {
        // Strip Windows-style \r (CR) from line endings.
        // Must be done before any other checks so we don't treat
        // "\r" as a non-empty path.
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        // Skip empty lines
        if (line.empty())
        {
            continue;
        }

        // Safe ASCII check: '#' is 0x23, always a single byte in UTF-8.
        // We compare the raw char value directly — no ctype functions —
        // so high-byte UTF-8 characters (e.g. Cyrillic 0xD0-0xBF) are
        // never misidentified as comments or skipped.
        if (line[0] == '#')
        {
            // M3U metadata line (e.g. #EXTM3U, #EXTINF) — skip it.
            continue;
        }

        // Everything else is treated as a preset file path.
        // std::string handles UTF-8 bytes transparently, so Cyrillic
        // and other non-ASCII paths are passed through unchanged.
        if (playlist.AddItem(line, Playlist::InsertAtEnd, allowDuplicates))
        {
            addedCount++;
        }
    }

    return addedCount;
}

} // namespace Playlist
} // namespace libprojectM
