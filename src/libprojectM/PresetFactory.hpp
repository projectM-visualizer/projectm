#pragma once

#include "Preset.hpp"

#include <memory>

namespace libprojectM {

class PresetFactory
{

public:
    PresetFactory() = default;

    virtual ~PresetFactory() = default;


    static std::string Protocol(const std::string& url, std::string& path);


    virtual std::unique_ptr<Preset> LoadPresetFromFile(const std::string& filename) = 0;


    virtual std::unique_ptr<Preset> LoadPresetFromStream(std::istream& data) = 0;


    virtual std::string supportedExtensions() const = 0;
};

}
