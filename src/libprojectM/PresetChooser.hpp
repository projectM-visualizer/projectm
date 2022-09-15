#ifndef PRESET_CHOOSER_HPP
#define PRESET_CHOOSER_HPP

#include "Preset.hpp"

#include "PresetLoader.hpp"
#include "RandomNumberGenerators.hpp"
#include <cassert>
#include <memory>
#include <iostream>
class PresetChooser;

///  A simple iterator class to traverse back and forth a preset directory
class PresetIterator {

public:
    PresetIterator()  {}

    /// Instantiate a preset iterator at the given starting position
    PresetIterator(std::size_t start);

    /// Move iterator forward
    void operator++();

    /// Move iterator backword
    void operator--() ;

    /// Not equal comparator
    bool operator !=(const PresetIterator & presetPos) const ;

    /// Equality comparator
    bool operator ==(const PresetIterator & presetPos) const ;

    /// Returns an integer value representing the iterator position
    /// @bug might become internal
    /// \brief Returns the indexing value used by the current iterator.
    std::size_t operator*() const;

    // Return previous index.
    std::size_t lastIndex() const;

    ///  Allocate a new preset given this iterator's associated preset name
    /// \param presetInputs the preset inputs to associate with the preset upon construction
    /// \param presetOutputs the preset outputs to associate with the preset upon construction
    /// \returns an autopointer of the newly allocated preset
    std::unique_ptr<Preset> allocate();

    ///  Set the chooser asocciated with this iterator
    void setChooser(const PresetChooser & chooser);

private:
    std::size_t _currentIndex;
    const PresetChooser * _presetChooser;

};

/// Provides functions and iterators to select presets. Requires a preset loader upon construction
class PresetChooser {

public:
    typedef PresetIterator iterator;

    /// Initializes a chooser with an established preset loader.
    /// \param presetLoader an initialized preset loader to choose presets from
    /// \note The preset loader is refreshed via events or otherwise outside this class's scope
    PresetChooser(const PresetLoader & presetLoader, bool 	softCutRatingsEnabled);

    inline void setSoftCutRatingsEnabled(bool enabled) {
	_softCutRatingsEnabled = enabled;
    }

    /// Choose a preset via the passed in index. Must be between 0 and num valid presets in directory
    /// \param index An index lying in the interval [0, this->getNumPresets())
    /// \param presetInputs the preset inputs to associate with the preset upon construction
    /// \param presetOutputs the preset outputs to associate with the preset upon construction
    /// \returns an auto pointer of the newly allocated preset
    std::unique_ptr<Preset> directoryIndex(std::size_t index) const;

    /// Gets the number of presets last believed to exist in the preset loader's filename collection
    /// \returns the number of presets in the collection
    std::size_t size() const;

    /// An STL-esque iterator to begin traversing presets from a directory
    /// \param index the index to begin iterating at. Assumed valid between [0, num presets)
    /// \returns the position of the first preset in the collection
    PresetIterator begin(unsigned int index) const;

    /// An STL-esque iterator to begin traversing presets from a directory
    /// \returns the position of the first preset in the collection
    PresetIterator begin();

    /// An STL-esque iterator to retrieve an end position from a directory
    /// \returns the end position of the collection
    PresetIterator end() const;

    /// Perform a weighted sample to select a preset (uses preset rating values)
    /// \returns an iterator to the randomly selected preset
    iterator weightedRandom(bool hardCut) const;

    /// True if no presets in directory
    bool empty() const;


    inline void nextPreset(PresetIterator & presetPos);
    inline void previousPreset(PresetIterator & presetPos);

private:
    std::vector<float> sampleWeights;
    const PresetLoader * _presetLoader;
    bool _softCutRatingsEnabled;
};


inline PresetChooser::PresetChooser(const PresetLoader & presetLoader, bool softCutRatingsEnabled):_presetLoader(&presetLoader), _softCutRatingsEnabled(softCutRatingsEnabled) {

}

inline std::size_t PresetChooser::size() const {
    return _presetLoader->size();
}

inline void PresetIterator::setChooser(const PresetChooser & chooser) {
    _presetChooser = &chooser;
}

inline std::size_t PresetIterator::operator*() const {
    return _currentIndex;
}

inline std::size_t PresetIterator::lastIndex() const {
    return _currentIndex;
}

inline PresetIterator::PresetIterator(std::size_t start):_currentIndex(start) {}

inline void PresetIterator::operator++() {
    assert(_currentIndex < _presetChooser->size());
    _currentIndex++;
}

inline void PresetIterator::operator--() {
    assert(_currentIndex > 0);
    _currentIndex--;
}

inline bool PresetIterator::operator !=(const PresetIterator & presetPos) const {
    return (*presetPos != **this);
}


inline bool PresetIterator::operator ==(const PresetIterator & presetPos) const {
    return (*presetPos == **this);
}

inline std::unique_ptr<Preset> PresetIterator::allocate() {
    return _presetChooser->directoryIndex(_currentIndex);
}

inline void PresetChooser::nextPreset(PresetIterator & presetPos) {

		if (this->empty()) {
			return;
		}

		// Case: idle preset currently running, selected first preset of chooser
		else if (presetPos == this->end())
			presetPos = this->begin();
		else
			++(presetPos);

		// Case: already at last preset, loop to beginning
		if (((presetPos) == this->end())) {
			presetPos = this->begin();
		}

}


inline void PresetChooser::previousPreset(PresetIterator & presetPos) {
		if (this->empty())
			return;

		// Case: idle preset currently running, selected last preset of chooser
		else if (presetPos == this->end()) {
			--(presetPos);
		}

		else if (presetPos != this->begin()) {
			--(presetPos);
		}

		else {
		   presetPos = this->end();
		   --(presetPos);
		}
}

inline PresetIterator PresetChooser::begin() {
    PresetIterator pos(0);
    pos.setChooser(*this);
    return pos;
}

inline PresetIterator PresetChooser::begin(unsigned int index) const{
    PresetIterator pos(index);
    pos.setChooser(*this);
    return pos;
}

inline PresetIterator PresetChooser::end() const {
    PresetIterator pos(_presetLoader->size());
    pos.setChooser(*this);
    return pos;
}


inline bool PresetChooser::empty() const {
	return _presetLoader->size() == 0;
}

inline std::unique_ptr<Preset> PresetChooser::directoryIndex(std::size_t index) const {
	return _presetLoader->loadPreset(index);
}


inline PresetChooser::iterator PresetChooser::weightedRandom(bool hardCut) const {

	
	

	// TODO make a sophisticated function object interface to determine why a certain rating
	// category is chosen, or weighted distribution thereover.
	const PresetRatingType ratingType = hardCut || (!_softCutRatingsEnabled) ? 
		HARD_CUT_RATING_TYPE : SOFT_CUT_RATING_TYPE;		

	const std::size_t ratingsTypeIndex = static_cast<std::size_t>(ratingType);
	
	const std::vector<int> & weights = _presetLoader->getPresetRatings()[ratingsTypeIndex];

    const auto weightsSum = _presetLoader->getPresetRatingsSums()[ratingsTypeIndex];

    std::size_t index;
    if (weightsSum > 0)
    {
        index = RandomNumberGenerators::weightedRandom(weights,
                                                       _presetLoader->getPresetRatingsSums()[ratingsTypeIndex]);
    }
    else
    {
        // No ratings (all zero), so just select a random preset with a uniform distribution.
        index = RandomNumberGenerators::uniformInteger(_presetLoader->size());
    }

    return begin(index);
}

#endif
