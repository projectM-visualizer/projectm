//
// C++ Interface: CompiledPresetFactory
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __COMPILED_PRESET_FACTORY_HPP
#define __COMPILED_PRESET_FACTORY_HPP

#include <memory>
#include "PresetFactory.hpp"

class CompiledPresetFactory : public PresetFactory {
private:
typedef void * Handle;
typedef Preset * CreateFunctor();
typedef void DestroyFunctor(Preset*);

struct PresetLibrary {

	Handle handle; 
	CreateFunctor createFunctor;
	DestroyFunctor destroyFunctor;

	PresetHandler(preset_lib 
	~PresetHandler() {
		dlclose(preset_lib);
	}
};

public:


 CompiledPresetFactory();

 virtual ~CompiledPresetFactory();

 std::auto_ptr<Preset> allocate(const std::string & url, const std::string & name = std::string(), 
	const std::string & author = std::string());

private:
	PresetHandler * loadLibrary(const std::string & url);
	typedef std::map<std::string, PresetHandler*> PresetHandlerMap;
	PresetHandlerMap _handlers;
	
};

#endif