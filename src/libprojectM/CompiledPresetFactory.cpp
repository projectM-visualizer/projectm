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

typedef void Handle;
typedef Preset * CreateFunctor();
typedef void DestroyFunctor(Preset*);

class PresetLibrary {

	public:
		PresetLibrary(Handle * h, CreateFunctor * create, DestroyFunctor * destroy) :
			_handle(h), _createFunctor(create), _destroyFunctor(destroy) {}

		Handle * handle() { return _handle; }
		CreateFunctor * createFunctor() { return _createFunctor; }
		DestroyFunctor * destroyFunctor() { return _destroyFunctor; }

		~PresetLibrary() {
			dlclose(handle());
		}

	private:
		Handle * _handle; 
		CreateFunctor * _createFunctor;
		DestroyFunctor * _destroyFunctor;
	
};

CompiledPresetFactory::CompiledPresetFactory() {}

CompiledPresetFactory::~CompiledPresetFactory() {

for (PresetLibraryMap::iterator pos = _libraries.begin(); pos != _libraries.end(); ++pos)
	delete(pos->second);

}


PresetLibrary * CompiledPresetFactory::loadLibrary(const std::string & url) {

    if (_libraries.count(url))
	return _libraries[url];

    // load the preset library
    void* handle = dlopen(url.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "[CompiledPresetFactory] Cannot load library: " << dlerror() << '\n';
        return 0;
    }

    // reset errors
    dlerror();

    // load the symbols
    CreateFunctor * create = (CreateFunctor*) dlsym(handle, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "[CompiledPresetFactory] Cannot load symbol create: " << dlsym_error << '\n';
        return 0;
    }
    
    DestroyFunctor * destroy = (DestroyFunctor*) dlsym(handle, "destroy");
    dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "[CompiledPresetFactory] Cannot load symbol destroy: " << dlsym_error << '\n';
        return 0;
    }

    PresetLibrary * library = new PresetLibrary(handle, create, destroy);

    _libraries.insert(std::make_pair(url, library));
    return library;
}


std::auto_ptr<Preset> CompiledPresetFactory::allocate(const std::string & url, const std::string & name,
	const std::string & author) {
	
	PresetLibrary * library;
	
	if ((library = loadLibrary(url)) == 0)
		return std::auto_ptr<Preset>(0);
	
	return std::auto_ptr<Preset>(library->createFunctor()());
}
