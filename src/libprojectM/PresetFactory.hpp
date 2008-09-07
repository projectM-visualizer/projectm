//
// C++ Interface: PresetFactory
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "Preset.hpp"
#include <memory>

#ifndef __PRESET_FACTORY_HPP
#define __PRESET_FACTORY_HPP

class PresetFactory {

public:


 PresetFactory() {}

 virtual ~PresetFactory() {}

 /// Constructs a new preset given an url
 /// \param url a locational identifier referencing the preset
 /// \returns a valid preset object- otherwise
 virtual std::auto_ptr<Preset> allocate(std::string url, std::string name);



};

#endif
