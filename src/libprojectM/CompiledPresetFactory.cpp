//
// C++ Implementation: CompiledPresetFactory
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <dlfcn.h>
#include "CompiledPresetFactory.hpp"

CompiledPresetFactory::CompiledPresetFactory() {}

CompiledPresetFactory::~CompiledPresetFactory() {

for (PresetHandlerMap::iterator pos = _handlers.begin(); pos != _handlers.end(); ++pos)
	delete(pos->second);

}



CompiledPresetFactory::PresetHandler * CompiledPresetFactory::loadLibrary(const std::string & url) {

    // load the preset library
    void* preset = dlopen(url.c_str(), RTLD_LAZY);
    if (!preset) {
        std::cerr << "[CompiledPresetFactory] Cannot load library: " << dlerror() << '\n';
        return 0;
    }

    // reset errors
    dlerror();

    // load the symbols
    create_t* create_preset = (create_t*) dlsym(preset, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "[CompiledPresetFactory] Cannot load symbol create: " << dlsym_error << '\n';
        return 0;
    }
    
    destroy_t* destroy_preset = (destroy_t*) dlsym(preset, "destroy");
    dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "[CompiledPresetFactory] Cannot load symbol destroy: " << dlsym_error << '\n';
        return false;
    }
	_handlers.add(std::make_pair(new PresetHandler(
    return 
}


PresetHandler * CompiledPresetFactory::getHandler(const std::string & url) {
	if (_handlers.count(url))
		return _handler[url];
	else {
		if (!loadLibrary(url))
			return 0;
		else
			return _handler[url];
	}
		
}
std::auto_ptr<Preset> CompiledPresetFactory::allocate(const std::string & url, const std::string & name,
	const std::string & author) {
	
	

}
