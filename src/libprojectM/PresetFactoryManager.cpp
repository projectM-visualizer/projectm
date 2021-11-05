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
#include "MilkdropPresetFactory/MilkdropPresetFactory.hpp"

#ifdef ENABLE_NATIVE_PRESETS
#include "NativePresetFactory/NativePresetFactory.hpp"
#endif

#include "config.h"
#include "Common.hpp"
#include <sstream>

PresetFactoryManager::PresetFactoryManager() : _gx(0), _gy(0), initialized(false) {}

PresetFactoryManager::~PresetFactoryManager() {
	for (std::vector<PresetFactory *>::iterator pos = _factoryList.begin(); 
		pos != _factoryList.end(); ++pos) {
		assert(*pos);
		delete(*pos);
	}

  initialized = false;
}

void PresetFactoryManager::initialize(int gx, int gy) {
	_gx = gx;
	_gy = gy;
	
	if (!initialized) {
	  initialized = true;
	} else {
	  std::cout << "already initialized " << std::endl;
	  return;
	}
	  
	PresetFactory * factory;

	factory = new MilkdropPresetFactory(_gx, _gy);
	registerFactory(factory->supportedExtensions(), factory);		

	#ifdef ENABLE_NATIVE_PRESETS
	factory = new NativePresetFactory();
	registerFactory(factory->supportedExtensions(), factory);
	#endif
}

// Current behavior if a conflict is occurs is to override the previous request

void PresetFactoryManager::registerFactory(const std::string & extensions, PresetFactory * factory) {
	
	std::stringstream ss(extensions);	
	std::string extension;

	_factoryList.push_back(factory);

	while (ss >> extension) {
		if (_factoryMap.count(extension)) {
			std::cerr << "[PresetFactoryManager] Warning: extension \"" << extension << 
				"\" already has a factory. New factory handler ignored." << std::endl;			
		} else {
			_factoryMap.insert(std::make_pair(extension, factory));			
		}
	}
}



std::unique_ptr<Preset> PresetFactoryManager::allocate(const std::string & url, const std::string & name)
{
	try {
		const std::string extension = "." + parseExtension(url);

		return factory(extension).allocate(url, name);
	} catch (const PresetFactoryException & e) {
		throw e;
	} catch (const std::exception & e) {
		throw PresetFactoryException(e.what());
	} catch (...) {
		throw PresetFactoryException("uncaught preset factory exception");
	}
    return std::unique_ptr<Preset>();
}

PresetFactory & PresetFactoryManager::factory(const std::string & extension) {

	if (!extensionHandled(extension)) {
		std::ostringstream os;
		os << "No preset factory associated with \"" << extension << "\"." << std::endl;
		throw PresetFactoryException(os.str());
	}
	return *_factoryMap[extension];
}

bool PresetFactoryManager::extensionHandled(const std::string & extension) const {		
	return _factoryMap.count(extension);
}

std::vector<std::string> PresetFactoryManager::extensionsHandled() const {
    std::vector<std::string> retval;
    for (auto const& element : _factoryMap) {
      retval.push_back(element.first);
    }
    return retval;
}
