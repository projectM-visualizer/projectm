//
// C++ Implementation: PresetFactoryManager
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PresetFactoryManager.hpp"

/// Begin: Preset factories we will handle ///
#ifndef DISABLE_MILKDROP_PRESETS
#include "MilkdropPresetFactory.hpp"
#endif

#ifndef DISABLE_COMPILED_PRESETS
#include "CompiledPresetFactory.hpp"
#endif

PresetFactoryManager::PresetFactoryManager() : _gx(0), _gy(0) {}

PresetFactoryManager::~PresetFactoryManager() {
	for (std::map<std::string, PresetFactory *>::iterator pos = _factories.begin(); 
		pos != _factories.end(); ++pos) 
		delete(pos->second);

}
void PresetFactoryManager::initialize(int gx, int gy) {
	_gx = gx;
	_gy = gy;
	PresetFactory * factory;
	
	#ifndef DISABLE_MILKDROP_PRESETS
	factory = new MilkdropPresetFactory(_gx, _gy);
	registerFactory(factory->extension(), factory);
	#endif

	#ifndef DISABLE_COMPILED_PRESETS
	factory = new CompiledPresetFactory();
	registerFactory(factory->extension(), factory);
	#endif
}

// Current behavior if a conflict is occurs is to override the previous request
void PresetFactoryManager::registerFactory(const std::string & extension, PresetFactory * factory) {

	if (_factories.count(extension)) {
		std::cerr << "[PresetFactoryManager] Warning: extension \"" << extension << 
			"\" has been overriden by another factory." << std::endl;
		delete(_factories[extension]);
	}
	else
		_factories.insert(std::make_pair(extension, factory));

}

PresetFactory & PresetFactoryManager::factory(const std::string & extension) {
	return *_factories[extension];
}

