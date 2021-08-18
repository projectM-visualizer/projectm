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

#include <memory>
#include "../PresetFactory.hpp"

class PresetOutputs;

class PresetInputs;

class MilkdropPresetFactory : public PresetFactory
{

public:
    MilkdropPresetFactory(int gx, int gy);

    ~MilkdropPresetFactory() override;

    // called by ~MilkdropPreset
    void releasePreset(Preset* preset);

    std::unique_ptr<Preset> allocate(const std::string& url, const std::string& name,
                                     const std::string& author) override;

    std::string supportedExtensions() const override
    {
        return ".milk .prjm";
    }

private:
    static PresetOutputs* createPresetOutputs(int gx, int gy);

    void reset();

    int gx{ 0 };
    int gy{ 0 };
    PresetOutputs* _presetOutputsCache{ nullptr };
};
