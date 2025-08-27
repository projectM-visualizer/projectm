#pragma once

#include <functional>
#include <string>
#include <vector>

namespace libprojectM {
namespace Renderer {


class FileScanner
{
public:

    using ScanCallback = std::function<void(const std::string& path, const std::string& basename)>;


    FileScanner(const std::vector<std::string>& rootDirs, std::vector<std::string>& extensions);


    void Scan(ScanCallback callback);

private:
    std::vector<std::string> _rootDirs;
    std::vector<std::string> _extensions;
};

}
}
