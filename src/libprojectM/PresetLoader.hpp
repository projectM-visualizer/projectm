#ifndef __PRESET_LOADER_HPP
#define __PRESET_LOADER_HPP

#include <string> // used for path / filename stuff
#include <memory> // for auto pointers
#include <sys/types.h>

#include <vector>
#include <map>
#include "PresetFactoryManager.hpp"
#include "FileScanner.hpp"

class Preset;
class PresetFactory;

typedef std::size_t PresetIndex;

class PresetLoader {
	public:
		/// Initializes the preset loader with the target directory specified
		PresetLoader(int gx, int gy, std::string dirname);

		~PresetLoader();

		/// Load a preset by specifying its unique identifier given when the preset url
		/// was added to this loader
		std::unique_ptr<Preset> loadPreset(PresetIndex index) const;
		std::unique_ptr<Preset> loadPreset ( const std::string & url )  const;
		/// Add a preset to the loader's collection.
		/// \param url an url referencing the preset
		/// \param presetName a name for the preset
		/// \param ratings an list representing the goodness ratings
		/// \returns The unique index assigned to the preset in the collection. Used with loadPreset
		unsigned long addPresetURL(const std::string & url, const std::string & presetName, const RatingList & ratings);

		/// Add a preset to the loader's collection.
		/// \param index insertion index
		/// \param url an url referencing the preset
		/// \param presetName a name for the preset
        /// \param ratings an list representing the goodness ratings
		void insertPresetURL (PresetIndex index, const std::string & url, const std::string & presetName, const RatingList & ratings);

		/// Clears all presets from the collection
		inline void clear() {
			_entries.clear(); _presetNames.clear();
			_ratings = std::vector<RatingList>(TOTAL_RATING_TYPES, RatingList());
			clearRatingsSum();
 		}

		inline void clearRatingsSum() {
			_ratingsSums = std::vector<int>(TOTAL_RATING_TYPES, 0);
		}

		const std::vector<RatingList> & getPresetRatings() const;
		const std::vector<int> & getPresetRatingsSums() const;

		/// Removes a preset from the loader
		/// \param index the unique identifier of the preset url to be removed
		void removePreset(PresetIndex index);

		/// Sets the rating of a preset to a new value
		void setRating(PresetIndex index, int rating, const PresetRatingType ratingType);

		/// Get a preset rating given an index
		int getPresetRating ( PresetIndex index, const PresetRatingType ratingType) const;

		/// Get a preset url given an index
		const std::string & getPresetURL ( PresetIndex index) const;

		/// Get a preset name given an index
		const std::string & getPresetName ( PresetIndex index) const;

	    /// Get vector of preset names
		const std::vector<std::string> & getPresetNames() const;

		/// Get the preset index given a name
		unsigned int getPresetIndex(const std::string& name) const;

		/// Returns the number of presets in the active directory
		inline std::size_t size() const {
			return _entries.size();
		}

		/// Sets the directory where the loader will search for files
		void setScanDirectory(std::string pathname);

		/// Returns the directory path associated with this preset chooser
		inline const std::string & directoryName() const {
			return _dirname;
		}

		/// Rescans the active preset directory
		void rescan();
		void setPresetName(PresetIndex index, std::string name);

	protected:
        void addScannedPresetFile(const std::string &path, const std::string &name);

		std::string _dirname;
		std::vector<int> _ratingsSums;
		mutable PresetFactoryManager _presetFactoryManager;

		// vector chosen for speed, but not great for reverse index lookups
		std::vector<std::string> _entries;
		std::vector<std::string> _presetNames;

		// Indexed by ratingType, preset position.
		std::vector<RatingList> _ratings;
    
        FileScanner fileScanner;
};

#endif
