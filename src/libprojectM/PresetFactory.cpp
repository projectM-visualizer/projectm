#include "PresetFactory.hpp"

#ifdef DEBUG
#include <iostream>
#endif

const std::string PresetFactory::IDLE_PRESET_PROTOCOL("idle");

std::string PresetFactory::Protocol(const std::string& url, std::string& path)
{
    auto pos = url.find("://");

    if (pos == std::string::npos)
    {
        path = url;
        return {};
    }

    path = url.substr(pos + 3, url.length());
#ifdef DEBUG
    std::cout << "[PresetFactory] Filename is URL: " << url << std::endl;
#endif
    return url.substr(0, pos);
}
