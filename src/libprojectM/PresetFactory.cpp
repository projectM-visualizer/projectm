#include "PresetFactory.hpp"

const std::string PresetFactory::IDLE_PRESET_PROTOCOL("idle");

std::string PresetFactory::Protocol(const std::string& url, std::string& path) {

#ifdef __APPLE__
	// NOTE: Brian changed this from url.find_first_of to url.find, since presumably we want to find the first occurence of
	// :// and not the first occurence of any colon or forward slash.  At least that fixed a bug in the Mac OS X build.
	std::size_t pos = url.find("://");
#else
	std::size_t pos = url.find_first_of("://");
#endif
	if (pos == std::string::npos)
    {
        path = url;
        return std::string();
    }
    else
    {
		path = url.substr(pos + 3, url.length());
#ifdef DEBUG
        std::cout << "[PresetFactory] Filename is URL: " << url << std::endl;
#endif
		return url.substr(0, pos);
	}

}

