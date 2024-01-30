#pragma once

#include <functional>
#include <string>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @class FileScanner
 * @brief Simple recursive scanner which returns all files matching a given list of filename extensions.
 *
 * All extensions are matched case-insensitively.
 */
class FileScanner
{
public:
    /**
     * Callback which gets invoked for each matching file found in the given path(s).
     * path contains the full path of the file, basename contains the filename without path and any extensions.
     */
    using ScanCallback = std::function<void(const std::string& path, const std::string& basename)>;

    /**
     * @brief Creates a new file scanner.
     * @param rootDirs A list of root directories to scan.
     * @param extensions A list of file extensions to search for. Matching is performed case-insensitively.
     */
	FileScanner(const std::vector<std::string> &rootDirs, std::vector<std::string> &extensions);

    /**
     * @brief Scans the configured paths for files with valid extensions and calls the provided callback function with each match.
     * @note If root directories overlap, files will be found multiple times.
     * @param callback The callback to invoke for each matching file.
     */
	void Scan(ScanCallback callback);

private:
	std::vector<std::string> _rootDirs; //!< List of base directories to scan recursively.
	std::vector<std::string> _extensions; //!< List of filename extensions to match.
};

} // namespace Renderer
} // namespace libprojectM
