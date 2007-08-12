#ifndef PRESET_MERGE_HPP
#define PRESET_MERGE_HPP
#include "Preset.hpp"

class PresetMerger 
{
public:
  static void MergePresets(PresetOutputs & A,  PresetOutputs & B, double ratio, int gx, int gy);
  static void mergeMesh(bool & isMeshA, bool & isMeshB, float & varA, float & varB,  float** meshA, float** meshB, int gx, int gy, float ratio);
};

#endif
