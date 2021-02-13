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
class DLLEXPORT PresetOutputs;
class DLLEXPORT PresetInputs;

class MilkdropPresetFactory : public PresetFactory {

public:
 MilkdropPresetFactory(int gx, int gy);

 virtual ~MilkdropPresetFactory();
 // called by ~MilkdropPreset
 void releasePreset(Preset *preset);

 std::unique_ptr<Preset> allocate(const std::string & url, const std::string & name = std::string(),
	const std::string & author = std::string());

 std::string supportedExtensions() const { return ".milk .prjm"; }

private:
    static PresetOutputs* createPresetOutputs(int gx, int gy);
	void reset();
	int gx;
	int gy;
	PresetOutputs * _presetOutputsCache;
};

#endif
