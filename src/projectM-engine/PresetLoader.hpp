#ifndef __PRESET_LOADER_HPP
#define __PRESET_LOADER_HPP

#include "Preset.hpp"

#ifdef LINUX
extern "C" {
 #include <sys/inotify.h>
}
#endif

#include "StaticArray.hpp"

class PresetLoader {

	
	public:
		static const std::string PROJECTM_FILE_EXTENSION = ".prjm";
		static const std::string MILKDROP_FILE_EXTENSION = ".milk";
		
		#ifdef LINUX 
			static const std::string PATH_SEPARTOR = "/";
		#endif

		#ifdef MACOS
			static const std::string PATH_SEPARTOR = "/";
		#endif

		#ifdef WIN32
			static const std::string PATH_SEPARTOR = "\";
		#endif
	
	
		/** Initializes the preset loader with the target directory specified */
		PresetLoader(std::string pathname);

		/** Destructor will remove all alllocated presets */
		~PresetLoader();

		/** Load a preset by indexing the collection of presets from a directory */
		/** Autopointers: when you take it, I leave it */
		auto_ptr<Preset> loadPreset(unsigned int index);
		auto_ptr<Preset> loadPreset(std::string filename);
		
		/** Returns the number of presets in the active directory */
		inline std::size_t getNumPresets() {
			return m_entries.size();
		}
					
		/** Sets the directory where the loader will search for files */	
		void setScanDirectory(std::string pathname);		
		
		/** Rescans the active preset directory */
		void rescan();

		
	private:
		const std::string m_dirname;
		int m_activeIndex;
		DIR * m_dir;
		std::vector<std::string> m_entries;
};


#endif