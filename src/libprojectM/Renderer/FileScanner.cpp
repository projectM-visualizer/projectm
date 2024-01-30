#include "FileScanner.hpp"

#include <algorithm>
#include <cctype>

// Fall back to boost if compiler doesn't support C++17
#include PROJECTM_FILESYSTEM_INCLUDE
using namespace PROJECTM_FILESYSTEM_NAMESPACE::filesystem;

namespace libprojectM {
namespace Renderer {

FileScanner::FileScanner(const std::vector<std::string>& rootDirs, std::vector<std::string>& extensions)
    : _rootDirs(rootDirs)
    , _extensions(extensions)
{
    // Convert all extensions to lower-case.
    for (auto& extension : _extensions)
    {
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
    }
}

void FileScanner::Scan(ScanCallback callback)
{
    for (const auto& currentPath : _rootDirs)
    {
        try
        {
            path basePath(currentPath);

            if (!exists(basePath))
            {
                continue;
            }

            // Resolve any symlinks first, so we can check the type.
            while (is_symlink(basePath))
            {
                basePath = read_symlink(basePath);
            }

            // Skip regular files and other stuff
            if (!is_directory(basePath))
            {
                continue;
            }

            for (const auto& entry : recursive_directory_iterator(basePath))
            {
                // Skip files without extensions and everything that's not a normal file.
#ifdef PROJECTM_FILESYSTEM_USE_BOOST
                if (!entry.path().has_extension() || (entry.status().type() != file_type::symlink_file && entry.status().type() != file_type::regular_file))
#else
                if (!entry.path().has_extension() || (is_symlink(entry.status()) && is_regular_file(entry.status())))
#endif
                {
                    continue;
                }

                // Match the lower-case extension of the file with the provided list of valid extensions.
                auto extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
                if (std::find(_extensions.begin(), _extensions.end(), extension) != _extensions.end())
                {
                    callback(entry.path().string(), entry.path().stem().string());
                }
            }
        }
        catch (filesystem_error&)
        {
            // ToDo: Log error. We ignore it for now.
        }
        catch (std::exception&)
        {
            // ToDo: Log error. We ignore it for now.
        }
    }
}

} // namespace Renderer
} // namespace libprojectM
