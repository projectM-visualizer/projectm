#include "PresetFactory.hpp"

const std::string PresetFactory::IDLE_PRESET_PROTOCOL("idle");

std::string PresetFactory::protocol(const std::string & url, std::string & path) {

	std::size_t pos = url.find_first_of("://");
	if (pos == std::string::npos)
		return std::string();
	else {
 		path = url.substr(pos + 3, url.length());
		std::cout << "[PresetFactory] path is " << path << std::endl;
		std::cout << "[PresetFactory] url is " << url << std::endl;
		return url.substr(0, pos);
	}

}

