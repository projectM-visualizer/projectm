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

struct PresetHandler {
	
};

public:


 CompiledPresetFactory();

 virtual ~CompiledPresetFactory();

 std::auto_ptr<Preset> allocate(const std::string & url, const std::string & name = std::string(), 	const std::string & author = std::string());

private:
	void loadLibrary(const std::string & url);
	std::map<url, PresetHandler*> _handlers;

};

#endif