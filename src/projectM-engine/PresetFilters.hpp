#ifndef PRESET_FILTERS_HPP
#define PRESET_FILTERS_HPP
#include "Preset.hpp"
/// This is probably where merging stuff will be placed
/// ISSUE: merge presets or just PresetInputs?
class PresetFilters {

/// The merge functor decides how to put the presets together.
/// Assumes getMergedPreset() is implemented.
/// A sample is below
class LinearMergeFunctor {

public:
	LinearMergeFunctor(Preset & presetAB):m_alpha(0), m_presetAB(presetAB) {}

	void operator() (const Preset & presetA, const Preset & presetB) {
		;
	}

	PresetOutputs & getMergedOutputs() {
		return m_presetAB;
	}
	
	void setAlpha(float alpha) {
		m_alpha = alpha;
	}

private:
	float m_alpha;
	PresetOutputs & m_presetAB;

};

template <class MergeFunctor>
static void merge(const PresetOutput & outputsA, const Preset & outputsB, MergeFunctor & functor);




};

#endif
