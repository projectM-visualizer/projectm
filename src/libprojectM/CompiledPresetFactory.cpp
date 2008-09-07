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

CompiledPresetFactory::~CompiledPresetFactory() {}


int CompiledPresetFactory::loadLibrary(const std::string & url) {

    // load the preset library
    void* preset = dlopen(url, RTLD_LAZY);
    if (!preset) {
        std::cerr << "[CompiledPresetFactor] Cannot load library: " << dlerror() << '\n';
        return 1;
    }

    // reset errors
    dlerror();
    
    // load the symbols
    create_t* create_preset = (create_t*) dlsym(preset, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
        return 1;
    }
    
    destroy_t* destroy_preset = (destroy_t*) dlsym(preset, "destroy");
    dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';
        return 1;
    }


}