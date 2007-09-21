#ifndef __PRESET_LOADER_HPP
#define __PRESET_LOADER_HPP

#include <string> // used for path / filename stuff

#include <memory> // for auto pointers
#include <sys/types.h>
#ifdef WIN32
#include "win32-dirent.h"
#endif

#ifdef LINUX
#include <dirent.h>
#endif

#ifdef MACOS
#include <dirent.h>
#endif

#include <vector>

class Preset;
class PresetInputs;
class PresetOutputs;

class PresetLoader {
	public:
		static const std::string PROJECTM_FILE_EXTENSION;
		static const std::string MILKDROP_FILE_EXTENSION;
		
		/** Initializes the preset loader with the target directory specified */
		PresetLoader(std::string dirname);

		/** Destructor will remove all alllocated presets */
		~PresetLoader();
	
		/** Load a preset by specifying a filename of the directory (that is, NOT full path) */
		/** Autopointers: when you take it, I leave it */		
		std::auto_ptr<Preset> loadPreset(unsigned int index, const PresetInputs & presetInputs, 
			PresetOutputs & presetOutputs) const;
		
		/** Returns the number of presets in the active directory */
		inline std::size_t getNumPresets() const {
			return m_entries.size();
		}
					
		/** Sets the directory where the loader will search for files */	
		void setScanDirectory(std::string pathname);

		/// Returns the directory path associated with this preset chooser
		inline const std::string & directoryName() const {
			return m_dirname;
		}

		/** Rescans the active preset directory */
		void rescan();

	private:
		void handleDirectoryError();
		std::string m_dirname;
		DIR * m_dir;

		// vector chosen for speed, but not great for reverse index lookups
		std::vector<std::string> m_entries;
};

#endif
