#ifndef PRESET_MERGE_HPP
#define PRESET_MERGE_HPP
#include "Preset.hpp"

class PresetMerger 
{
public:
    void MergePresets(PresetOutputs *A, const PresetOutputs *B, double ratio);

};

#endif
