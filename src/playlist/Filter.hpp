#pragma once

#include <string>
#include <vector>

namespace ProjectM {
namespace Playlist {

/**
 * @brief Implements a simple filename globbing filter.
 *
 * See API docs of projectm_playlist_set_filter() in playlist.h for syntax details.
 */
class Filter
{
public:
    /**
     * @brief Returns the filter list.
     * @return The current filter list.
     */
    auto List() const -> const std::vector<std::string>&;

    /**
     * @brief Sets the filter list.
     * @param filterList The new filter list.
     */
    void SetList(std::vector<std::string> filterList);

    /**
     * @brief Applies the current filter list to the filename.
     *
     * This will apply all rules in order, and return true if the filename should be included
     * in the playlist. If no rule matches or the filter list is empty, the filename will pass.
     *
     * @param filename The filename to check.
     * @return True if the filename passes the filter, false if it should b skipped.
     */
    auto Passes(const std::string& filename) ->bool;

private:
    /**
     * @brief Applies a single filter to the given filename.
     * @param character The filename to check.
     * @param filterExpression The filter expression. A leading + or - is ignored.
     * @return True if the filter matches the filename, false otherwise.
     */
    static auto ApplyExpression(const std::string& character, const std::string& filterExpression) -> bool;

    std::vector<std::string> m_filters; //!< List of filters to apply.
};

} // namespace Playlist
} // namespace ProjectM
