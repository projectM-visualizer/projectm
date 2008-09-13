//
// C++ Implementation: MilkdropPresetFactory
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

#include "MilkdropPresetFactory.hpp"


MilkdropPresetFactory::MilkdropPresetFactory(int gx, int gy) 
{	
	/// @bug fix with correct gx gy
	_presetOutputs.Initialize(gx,gy);
}

MilkdropPresetFactory::~MilkdropPresetFactory() {}

std::auto_ptr<Preset> MilkdropPresetFactory::allocate(const std::string & url, const std::string & name, const std::string & author) {
	_presetOutputs.customWaves.clear();
	_presetOutputs.customShapes.clear();

	return std::auto_ptr<Preset>(new MilkdropPreset(url, name, _presetOutputs));
}
