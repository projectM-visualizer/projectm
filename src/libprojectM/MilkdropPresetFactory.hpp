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

#ifndef __MILKDROP_PRESET_FACTORY_HPP
#define __MILKDROP_PRESET_FACTORY_HPP

#include "MilkdropPreset.hpp"
#include <memory>
#include "PresetFactory.hpp"

class MilkdropPresetFactory : public PresetFactory {

public:


 MilkdropPresetFactory();

 virtual ~MilkdropPresetFactory();

 std::auto_ptr<Preset> allocate(const std::string & url, const std::string & name = std::string(), 	const std::string & author = std::string());

private:
	PresetOutputs _presetOutputs;
	PresetInputs _presetInputs;
};

#endif