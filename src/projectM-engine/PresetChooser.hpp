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
#include "PresetLoader.hpp"
#include <cassert>

class PresetChooser {

public:

    /// \brief Initializes a chooser with an established preset loader.
    /// \param presetLoader an initalized preset loader to choose presets from
    /// \note The preset loader is refreshed via events or otherwise outside this class's scope
    PresetChooser(const PresetLoader & presetLoader): m_presetLoader(&presetLoader) {}

    class PresetIterator {
	
	public:
        PresetIterator()  {}

	/** @brief Instantiate a preset iterator at the given starting position **/
	PresetIterator(std::size_t start):m_currentIndex(start) {}

        void operator++() {
            assert(m_currentIndex < m_presetChooser->getNumPresets());
            m_currentIndex++;		
        }

        bool operator !=(const PresetIterator & presetPos) {
		return (*presetPos != **this);
        }

        void operator--() {
            assert(m_currentIndex > 0);
            m_currentIndex--;
	
        }

        std::size_t operator*() const {
            return m_currentIndex;
        }

        std::auto_ptr<Preset> allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs) {
            return m_presetChooser->directoryIndex(m_currentIndex, presetInputs, presetOutputs);
        }


    // Used for aribtrary randomness
    typedef PresetIterator iterator;

        inline void setChooser(const PresetChooser & chooser) {
            m_presetChooser = &chooser;
        }

    private:
        std::size_t m_currentIndex;
        const PresetChooser * m_presetChooser;


    };

    /** Choose a preset via the passed in index. Must be between 0 and num valid presets in directory **/
    std::auto_ptr<Preset> directoryIndex(std::size_t index, const PresetInputs & presetInputs,
                                         PresetOutputs & presetOutputs) const;

    std::size_t getNumPresets() const {
	return m_presetLoader->getNumPresets();
    }

    class UniformRandomFunctor {

    public:
        UniformRandomFunctor(std::size_t collectionSize):m_collectionSize(collectionSize) {}

        std::size_t operator() (std::size_t index) {
            return (1.0 / m_collectionSize);

        }

    private:
        std::size_t m_collectionSize;

    };

   
    PresetIterator begin() {
	PresetIterator pos;
	pos.setChooser(*this);
	return pos;		
    }

    PresetIterator end() {
	PresetIterator pos(m_presetLoader->getNumPresets());
	return pos;	
    }

    /** Samples from the preset collection **/
    template <class WeightFunctor>
    std::auto_ptr<Preset> weightedRandom(const PresetInputs & presetInputs, PresetOutputs & presetOutputs, WeightFunctor & weightFunctor) const {
        doWeightedSample(weightFunctor);
    }

    /** Samples from the preset collection **/
    template <class WeightFunctor>
    std::auto_ptr<Preset> weightedRandom(const PresetInputs & presetInputs, PresetOutputs & presetOutputs) const {

        WeightFunctor weightFunctor(m_presetLoader->getNumPresets());
        doWeightedSample(weightFunctor);

    }


private:
    template <class WeightFunctor>
    std::auto_ptr<Preset> doWeightedSample(WeightFunctor & weightFunctor, const PresetInputs & presetInputs, PresetOutputs & presetOutputs) {

        // Choose a random index from the preset directory
        float cutoff = ((float)(random())) / RAND_MAX;
        float mass = 0;

        for (PresetIterator pos = this->begin();pos != this->end() ; ++pos) {
            mass += weightFunctor(*pos);
            if (mass >= cutoff)
                return pos.allocate(presetInputs, presetOutputs);
        }
    }


    private:
		const PresetLoader * m_presetLoader;
};
#endif
