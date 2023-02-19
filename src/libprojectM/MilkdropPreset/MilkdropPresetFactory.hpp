//
// C++ Interface: MilkdropPresetFactory
//
// Description:
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#pragma once

#include <PresetFactory.hpp>

#include <memory>

class MilkdropPresetFactory : public PresetFactory
{

public:
    std::unique_ptr<Preset> LoadPresetFromFile(const std::string& filename) override;

    std::unique_ptr<Preset> LoadPresetFromStream(std::istream& data) override;

    std::string supportedExtensions() const override
    {
        return ".milk .prjm";
    }

};
