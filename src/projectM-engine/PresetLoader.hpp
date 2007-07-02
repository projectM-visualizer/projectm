#ifndef __PRESET_LOADER_HPP
#define __PRESET_LOADER_HPP

#include <string>
#include "Preset.hpp"


class PresetLoader {
	public:
		static const std::string PROJECTM_FILE_EXTENSION;
		static const std::string MILKDROP_FILE_EXTENSION;
		
		#ifdef LINUX 
			static const char PATH_SEPARTOR = '/';
		#endif

		#ifdef MACOS
			static const char  PATH_SEPARTOR = '/';
		#endif

		#ifdef WIN32
			static const char PATH_SEPARATOR = '\';
		#endif
	
	
		/** Initializes the preset loader with the target directory specified */
		PresetLoader(std::string pathname);

		/** Destructor will remove all alllocated presets */
		~PresetLoader();

		/** Load a preset by indexing the collection of presets from a directory */
		/** Autopointers: when you take it, I leave it */
//		auto_ptr<Preset> loadPreset(unsigned int index, const PresetInputs & presetInputs, PresetOutputs & presetOutputs);
//		auto_ptr<Preset> loadPreset(std::string filename);
		
		/** Returns the number of presets in the active directory */
		inline std::size_t getNumPresets() {
			return m_entries.size();
		}
					
		/** Sets the directory where the loader will search for files */	
		void setScanDirectory(std::string pathname);
		
		/** Rescans the active preset directory */
		void rescan();

		
	private:
		void handleDirectoryError();
		std::string m_dirname;		
		DIR * m_dir;
		std::vector<std::string> m_entries;
};


#endif
