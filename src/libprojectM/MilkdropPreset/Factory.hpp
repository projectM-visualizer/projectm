











#pragma once

#include <PresetFactory.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

class Factory : public PresetFactory
{

public:
    std::unique_ptr<Preset> LoadPresetFromFile(const std::string& filename) override;

    std::unique_ptr<Preset> LoadPresetFromStream(std::istream& data) override;

    std::string supportedExtensions() const override
    {
        return ".milk .prjm";
    }
};

}
}
