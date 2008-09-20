//
// C++ Interface: MilkdropPresetFactory
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

#include <memory>
#include "../PresetFactory.hpp"
class PresetOutputs;
class PresetInputs;

class MilkdropPresetFactory : public PresetFactory {

public:
 
 MilkdropPresetFactory(int gx, int gy);

 virtual ~MilkdropPresetFactory();

 std::auto_ptr<Preset> allocate(const std::string & url, const std::string & name = std::string(),
	const std::string & author = std::string());

 std::string supportedExtensions() const { return "milk prjm"; }

private:
	void initializePresetOutputs(int gx, int gy);
	void reset();
	PresetOutputs * _presetOutputs;
	//PresetInputs _presetInputs;
};

#endif
