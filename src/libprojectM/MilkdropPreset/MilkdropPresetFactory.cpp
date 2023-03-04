//
// C++ Implementation: MilkdropPresetFactory
//
// Description:
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
#include "MilkdropPresetFactory.hpp"

#include "MilkdropPreset.hpp"
#include "IdlePreset.hpp"

std::unique_ptr<Preset>
MilkdropPresetFactory::LoadPresetFromFile(const std::string& filename)
{
    std::string path;
    auto protocol = PresetFactory::Protocol(filename, path);
    if (protocol == PresetFactory::IDLE_PRESET_PROTOCOL)
    {
        return IdlePresets::allocate();
    }
    else if (protocol == "" || protocol == "file")
    {
        return std::make_unique<MilkdropPreset>(path);
    }
    else
    {
        // ToDO: Throw unsupported protocol exception instead to provide more information.
        return nullptr;
    }
}

std::unique_ptr<Preset> MilkdropPresetFactory::LoadPresetFromStream(std::istream& data)
{
    return std::make_unique<MilkdropPreset>(data);
}
