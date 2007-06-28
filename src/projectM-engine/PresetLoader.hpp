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
		/** Initializes the preset loader with the target directory specified */
		PresetLoader(std::string pathname);

		/** Destructor will remove all alllocated presets */
		~PresetLoader();

		/** Load a preset by indexing the collection of presets from a directory */
		auto_ptr<Preset> loadPreset(unsigned int index);
		auto_ptr<Preset> loadPreset(std::string filename);


//		void unloadPreset(unsigned int index);
//		void unloadPreset(Preset & preset);
		
		/** Returns the number of presets in the active directory */
		std::size_t getNumPresets();
			
		
		/** Sets the directory where the loader will search for files */	
		void setScanDirectory(std::string pathname);		

		/** Clears the preset cache forceably. This cache gets larger on each prseset load- the user of
		    this class must manually flush out the buffer. 
		    @idea boost shared pointers would be useful here! */
		void flushCache();

	private:
		int m_notifyFD;
		const std::string m_dirname;
		StaticArray<Preset *> cachedPresets;



};


#endif