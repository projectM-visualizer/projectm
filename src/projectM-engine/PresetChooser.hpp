/** PresetChooser.hpp:
 * 	This class is a WIP that provides methods to load presets, switch presets, and
 *	merge them. The user of this class decides when / how to switch by calling functions
 *	of this class at the appropriate times in the renderer.
 *	DECISION: Merging / etc. goes in PresetFilters.hpp! Loading goes in here, switching goes in here
 */

/// More specifically, we need to 
/// (1) load presets from a path(s)
/// (2) set which preset is the "active" presets
/// (3) merge two presets together - probably doesn't belong in this class
///      how about another class called PresetFilters?


/// @idea Weighted random based on user stats

#ifndef PRESET_CHOOSER_HPP
#define PRESET_CHOOSER_HPP
#include "Preset.hpp"
#include "projectM.h"

class PresetChooser {

public:

    // Used for aribtrary randomness
    typedef PresetIterator iterator;

    /** Initializes a chooser with an established preset loader. Note that the loader must be 
      * refreshed via events or otherwise *outside* this class's scope **/
    PresetChooser(const PresetLoader & presetLoader);

    class PresetIterator {

	PresetIterator():m_chooser(0) {}

	void operator++() {
		assert(m_currentIndex < (m_presetLoader.getNumPresets()-1));
		m_currentIndex++;
	}

	void operator--() {
		assert(m_currentIndex > 0);
		m_currentIndex--;	
	}

	std::size_t operator*() {
		return m_currentIndex;
	}

	std::auto_ptr<Preset> allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs) {
		return m_presetChooser.directoryIndex(m_currentIndex, presetInputs, presetOutputs);
	}

	private:
		std::size_t m_currentIndex;
		inline void setChooser(const PresetChooser & chooser) {
		
	m_presetChooser = &chooser;
		}
		const PresetChooser * m_presetChooser;
    };

    /** Choose a preset via the passed in index. Must be between 0 and num valid presets in directory **/
    std::auto_ptr<Preset> directoryIndex(std::size_int index, const PresetInputs & presetInputs,
	PresetOutputs & presetOutputs) const;

    class  UniformRandomFunctor {
	
	public:
		UniformRandomFunctor(std::size_t collectionSize):m_collectionSize(collectionSize) {}
		
		std::size_t operator() (std::size_t index) {
			return (1.0 / (float)m_collectionSize);
		}

	private:
		std::size_t m_collectionSize;

    };


   /** Samples from the preset collection **/
    template <class WeightFunctor>
    std::auto_ptr<Preset> weightedRandom(const PresetInputs & presetInputs, PresetOutputs & presetOutputs, WeightFunctor & weightFunctor) const {
	

	// Choose a random index from the preset directory	
	float cutoff = ((float)(random())) / RAND_MAX;
	float mass = 0;

	for(iterator pos = this->begin();pos != this->end() ; ++pos) {
		mass += weightFunctor(*pos);
		if (mass >= cutoff)
			return pos.allocate(presetInputs,presetOutputs);
	}

	}

    };


   
/* destroyPresetLoader: closes the preset
   loading library. This should be done when 
   PresetChooser does cleanup */
};
#endif
