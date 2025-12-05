#include "PresetFactory.hpp"

#include <Logging.hpp>

namespace libprojectM {

std::string PresetFactory::Protocol(const std::string& url, std::string& path)
{
    auto pos = url.find("://");

    if (pos == std::string::npos)
    {
        path = url;
        return {};
    }

    path = url.substr(pos + 3, url.length());

    LOG_DEBUG("[PresetFactory] Preset filename is URL: " + url);
    return url.substr(0, pos);
}

} // namespace libprojectM
