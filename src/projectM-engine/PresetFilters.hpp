#ifndef PRESET_FILTERS_HPP
#define PRESET_FILTERS_HPP
#include "Preset.hpp"

class PresetFilters {

/// The merge functor decides how to put the presets together.
/// Assumes getMergedPreset() is implemented.
/// A sample is below
/// @bug this is screwed up, but functor idea still has merit once design is resolved
class LinearMergeFunctor {

public:
	LinearMergeFunctor(PresetOutputs & presetOutputsAB):m_presetOutputsAB(presetOutputsAB) {}

	
	void operator() (const PresetInputs & inputsA, const PresetInputs & inputsB, 
		const PresetOutputs & outputsA, const PresetOutputs & outputsB) {
				
		// use time variables from inputsA/B to determine how presets should be merged
				
	}

	PresetOutputs & getMergedOutputs() {
		return m_presetAB;
	}
	

private:	
	PresetOutputs & m_presetOutputsAB;

};

template <class MergeFunctor>
static void merge(const PresetOutput & outputsA, const Preset & outputsB, MergeFunctor & functor);

};
#endif
