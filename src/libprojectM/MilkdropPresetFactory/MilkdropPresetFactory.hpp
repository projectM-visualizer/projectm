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

class PresetOutputs;

class PresetInputs;

class MilkdropPresetFactory : public PresetFactory
{

public:
    MilkdropPresetFactory(int meshX, int meshY);

    ~MilkdropPresetFactory() override;

    // called by ~MilkdropPreset
    void releasePreset(Preset* preset);

    std::unique_ptr<Preset> LoadPresetFromFile(const std::string& filename) override;

    std::unique_ptr<Preset> LoadPresetFromStream(std::istream& data) override;

    std::string supportedExtensions() const override
    {
        return ".milk .prjm";
    }

private:
    void ResetPresetOutputs(PresetOutputs* presetOutputs);

    static PresetOutputs* CreatePresetOutputs(int meshX, int meshY);

    void reset();

    int m_meshX{0};
    int m_meshY{0};

    PresetOutputs* m_presetOutputsCache{nullptr};
};
