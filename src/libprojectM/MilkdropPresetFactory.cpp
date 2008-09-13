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

#include "BuiltinFuncs.hpp"
#include "Eval.hpp"

MilkdropPresetFactory::MilkdropPresetFactory(int gx, int gy) {
	/* Initializes the builtin function database */
	BuiltinFuncs::init_builtin_func_db();

	/* Initializes all infix operators */
	Eval::init_infix_ops();	

	_presetOutputs.Initialize(gx,gy);

}

MilkdropPresetFactory::~MilkdropPresetFactory() {

	Eval::destroy_infix_ops();
	BuiltinFuncs::destroy_builtin_func_db();

}

std::auto_ptr<Preset> MilkdropPresetFactory::allocate(const std::string & url, const std::string & name, const std::string & author) {
	_presetOutputs.customWaves.clear();
	_presetOutputs.customShapes.clear();

	return std::auto_ptr<Preset>(new MilkdropPreset(url, name, _presetOutputs));
}
