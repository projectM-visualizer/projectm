//
// Created by matthew on 12/22/18.
//

#ifndef PROJECTM_GLSL_TAFFYPRESET_H
#define PROJECTM_GLSL_TAFFYPRESET_H

#include "MilkdropPreset.hpp"
#include <iostream>
#include <sstream>

class SaltWaterTaffyPreset : public MilkdropPreset
{
public:
    SaltWaterTaffyPreset(const std::string & absoluteFilePath, const std::string & presetName, PresetOutputs & presetOutputs);

protected:
    virtual int readIn(std::istream & fs) override;
};


#endif //PROJECTM_GLSL_TAFFYPRESET_H
