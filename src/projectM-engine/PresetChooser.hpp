/** PresetChooser.hpp:
 * Provides functions to select presets. Requires a preset loader.
 */

/// @idea Weighted random based on user stats

#ifndef PRESET_CHOOSER_HPP
#define PRESET_CHOOSER_HPP

#include "Preset.hpp"

#include "PresetLoader.hpp"

#include <cassert>
#include <memory>


class PresetChooser;

///  A simple iterator class to traverse back and forth a preset directory
class PresetIterator {

public:
    PresetIterator()  {}

    /// Instantiate a preset iterator at the given starting position 
    PresetIterator(std::size_t start);

    ///  Move iterator forward
    void operator++();

    ///   Move iterator backword
    void operator--() ;

    bool operator !=(const PresetIterator & presetPos) const ;

    /// Returns an integer value representing the iterator pos.
    /// @bug might become internal
    /// \brief Returns the indexing value used by the current iterator.
    std::size_t operator*() const;

    ///  Allocate a new preset given this iterator's associated preset name
    std::auto_ptr<Preset> allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs);

    ///  Set the chooser asocciated with this iterator
    void setChooser(const PresetChooser & chooser);

private:
    std::size_t m_currentIndex;
    const PresetChooser * m_presetChooser;

};

class PresetChooser {

public:
    typedef PresetIterator iterator;
    /// Initializes a chooser with an established preset loader.
    /// \param presetLoader an initalized preset loader to choose presets from
    /// \note The preset loader is refreshed via events or otherwise outside this class's scope
    PresetChooser(const PresetLoader & presetLoader);


    /// Choose a preset via the passed in index. Must be between 0 and num valid presets in directory
    std::auto_ptr<Preset> directoryIndex(std::size_t index, const PresetInputs & presetInputs,
                                         PresetOutputs & presetOutputs) const;

    /// Gets the number of presets last believed to exist in the preset loader's filename collection
    std::size_t getNumPresets() const;

    /// \brief A functor, for all preset indices, returns probability 1 / (number of presets in directory)
    class UniformRandomFunctor {

    public:
        /// \brief Initialize a new functor with collection size
        /// \param collectionSize the number of presets one is sampling over
        UniformRandomFunctor(std::size_t collectionSize);

        /// \brief Returns uniform (fixed) probability for any index
        float  operator() (std::size_t index) const;

    private:
        std::size_t m_collectionSize;

    };


    /// \brief An STL-esque iterator to beginning traversing presets from a directory
    PresetIterator begin();

    /// \brief An STL-esque iterator to retrieve an end position from a directory
    PresetIterator end();

    /// \brief Do a weighted sample given a weight functor.
    template <class WeightFunctor>
    std::auto_ptr<Preset> weightedRandom(const PresetInputs & presetInputs, PresetOutputs & presetOutputs, WeightFunctor & weightFunctor) const;


    /// \brief Do a weighted sample given a weight functor and default construction (ie. element size) of the weight functor
    template <class WeightFunctor>
    std::auto_ptr<Preset> weightedRandom(const PresetInputs & presetInputs, PresetOutputs & preseOutputs) const;

private:
    template <class WeightFunctor>
    std::auto_ptr<Preset> doWeightedSample(WeightFunctor & weightFunctor, const PresetInputs & presetInputs, PresetOutputs & presetOutputs);

    const PresetLoader * m_presetLoader;
};


inline std::size_t PresetChooser::getNumPresets() const {
    return m_presetLoader->getNumPresets();
}

inline void PresetIterator::setChooser(const PresetChooser & chooser) {
    m_presetChooser = &chooser;
}

inline std::size_t PresetIterator::operator*() const {
    return m_currentIndex;
}

inline PresetIterator::PresetIterator(std::size_t start):m_currentIndex(start) {}

inline void PresetIterator::operator++() {
    assert(m_currentIndex < m_presetChooser->getNumPresets());
    m_currentIndex++;
}

inline void PresetIterator::operator--() {
    assert(m_currentIndex > 0);
    m_currentIndex--;
}

inline bool PresetIterator::operator !=(const PresetIterator & presetPos) const {
    return (*presetPos != **this);
}

inline std::auto_ptr<Preset> PresetIterator::allocate(const PresetInputs & presetInputs, PresetOutputs & presetOutputs) {
    return m_presetChooser->directoryIndex(m_currentIndex, presetInputs, presetOutputs);
}

inline float PresetChooser::UniformRandomFunctor::operator() (std::size_t index) const {
    return (1.0 / m_collectionSize);
}


inline PresetIterator PresetChooser::begin() {
    PresetIterator pos;
    pos.setChooser(*this);
    return pos;
}

inline PresetIterator PresetChooser::end() {
    PresetIterator pos(m_presetLoader->getNumPresets());
    return pos;
}

template <class WeightFunctor>
std::auto_ptr<Preset> PresetChooser::weightedRandom(const PresetInputs & presetInputs, PresetOutputs & presetOutputs, WeightFunctor & weightFunctor) const {
    doWeightedSample(weightFunctor);
}

template <class WeightFunctor>
inline  std::auto_ptr<Preset> PresetChooser::weightedRandom(const PresetInputs & presetInputs, PresetOutputs & presetOutputs) const {

    WeightFunctor weightFunctor(m_presetLoader->getNumPresets());
    doWeightedSample(weightFunctor, presetInputs, presetOutputs);

}

template <class WeightFunctor>
inline std::auto_ptr<Preset> PresetChooser::doWeightedSample(WeightFunctor & weightFunctor, const PresetInputs & presetInputs, PresetOutputs & presetOutputs) {

    // Choose a random mass bounded mass between 0 and 1
    float cutoff = ((float)(random())) / RAND_MAX;

    // Sum up mass, stopping when cutoff is reached. This is the typical
    // weighted sampling algorithm.
    float mass = 0;
    for (PresetIterator pos = this->begin();pos != this->end() ; ++pos) {
        mass += weightFunctor(*pos);
        if (mass >= cutoff)
            return pos.allocate(presetInputs, presetOutputs);
    }
}

#endif
