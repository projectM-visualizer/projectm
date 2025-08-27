#include "FileScanner.hpp"

#include "Utils.hpp"

#include <algorithm>
#include <cctype>


#include PROJECTM_FILESYSTEM_INCLUDE
using namespace PROJECTM_FILESYSTEM_NAMESPACE::filesystem;

namespace libprojectM {
namespace Renderer {

FileScanner::FileScanner(const std::vector<std::string>& rootDirs, std::vector<std::string>& extensions)
    : _rootDirs(rootDirs)
    , _extensions(extensions)
{

    for (auto& extension : _extensions)
    {
        Utils::ToLowerInPlace(extension);
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


            while (is_symlink(basePath))
            {
                basePath = read_symlink(basePath);
            }


            if (!is_directory(basePath))
            {
                continue;
            }

            for (const auto& entry : recursive_directory_iterator(basePath))
            {

#ifdef PROJECTM_FILESYSTEM_USE_BOOST
                if (!entry.path().has_extension() || (entry.status().type() != file_type::symlink_file && entry.status().type() != file_type::regular_file))
#else
                if (!entry.path().has_extension() || (is_symlink(entry.status()) && is_regular_file(entry.status())))
#endif
                {
                    continue;
                }


                auto extension = Utils::ToLower(entry.path().extension().string());
                if (std::find(_extensions.begin(), _extensions.end(), extension) != _extensions.end())
                {
                    callback(entry.path().string(), entry.path().stem().string());
                }
            }
        }
        catch (filesystem_error&)
        {

        }
        catch (std::exception&)
        {

        }
    }
}

}
}
