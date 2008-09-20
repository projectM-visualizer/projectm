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
#include "MilkdropPresetFactory.hpp"
#include "MilkdropPreset.hpp"
#include "BuiltinFuncs.hpp"
#include "Eval.hpp"
#include "IdlePreset.hpp"

MilkdropPresetFactory::MilkdropPresetFactory(int gx, int gy) {
	/* Initializes the builtin function database */
	BuiltinFuncs::init_builtin_func_db();

	/* Initializes all infix operators */
	Eval::init_infix_ops();	
	
	initializePresetOutputs(gx,gy);
}

MilkdropPresetFactory::~MilkdropPresetFactory() {

	Eval::destroy_infix_ops();
	BuiltinFuncs::destroy_builtin_func_db();

}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void MilkdropPresetFactory::reset()
{

	_presetOutputs.zoom=1.0;
	_presetOutputs.zoomexp= 1.0;
	_presetOutputs.rot= 0.0;
	_presetOutputs.warp= 0.0;

	_presetOutputs.sx= 1.0;
	_presetOutputs.sy= 1.0;
	_presetOutputs.dx= 0.0;
	_presetOutputs.dy= 0.0;
	_presetOutputs.cx= 0.5;
	_presetOutputs.cy= 0.5;

	_presetOutputs.screenDecay=.98;

	_presetOutputs.wave.r= 1.0;
	_presetOutputs.wave.g= 0.2;
	_presetOutputs.wave.b= 0.0;
	_presetOutputs.wave.x= 0.5;
	_presetOutputs.wave.y= 0.5;
	_presetOutputs.wave.mystery= 0.0;

	_presetOutputs.border.outer_size= 0.0;
	_presetOutputs.border.outer_r= 0.0;
	_presetOutputs.border.outer_g= 0.0;
	_presetOutputs.border.outer_b= 0.0;
	_presetOutputs.border.outer_a= 0.0;

	_presetOutputs.border.inner_size = 0.0;
	_presetOutputs.border.inner_r = 0.0;
	_presetOutputs.border.inner_g = 0.0;
	_presetOutputs.border.inner_b = 0.0;
	_presetOutputs.border.inner_a = 0.0;

	_presetOutputs.mv.a = 0.0;
	_presetOutputs.mv.r = 0.0;
	_presetOutputs.mv.g = 0.0;
	_presetOutputs.mv.b = 0.0;
	_presetOutputs.mv.length = 1.0;
	_presetOutputs.mv.x_num = 16.0;
	_presetOutputs.mv.y_num = 12.0;
	_presetOutputs.mv.x_offset = 0.02;
	_presetOutputs.mv.y_offset = 0.02;

	
	/* PER_FRAME CONSTANTS END */
	_presetOutputs.fRating = 0;
	_presetOutputs.fGammaAdj = 1.0;
	_presetOutputs.videoEcho.zoom = 1.0;
	_presetOutputs.videoEcho.a = 0;
	_presetOutputs.videoEcho.orientation = Normal;

	_presetOutputs.wave.additive = false;
	_presetOutputs.wave.dots = false;
	_presetOutputs.wave.thick = false;
	_presetOutputs.wave.modulateAlphaByVolume = 0;
	_presetOutputs.wave.maximizeColors = 0;
	_presetOutputs.textureWrap = 0;
	_presetOutputs.bDarkenCenter = 0;
	_presetOutputs.bRedBlueStereo = 0;
	_presetOutputs.bBrighten = 0;
	_presetOutputs.bDarken = 0;
	_presetOutputs.bSolarize = 0;
	_presetOutputs.bInvert = 0;
	_presetOutputs.bMotionVectorsOn = 1;

	_presetOutputs.wave.a =1.0;
	_presetOutputs.wave.scale = 1.0;
	_presetOutputs.wave.smoothing = 0;
	_presetOutputs.wave.mystery = 0;
	_presetOutputs.wave.modOpacityEnd = 0;
	_presetOutputs.wave.modOpacityStart = 0;
	_presetOutputs.fWarpAnimSpeed = 0;
	_presetOutputs.fWarpScale = 0;
	_presetOutputs.fShader = 0;

	/* PER_PIXEL CONSTANT END */
	/* Q VARIABLES START */

	for (int i = 0;i<32;i++)
		_presetOutputs.q[i] = 0;

	/* Q VARIABLES END */

}

void MilkdropPresetFactory::initializePresetOutputs(int gx, int gy)
{

	_presetOutputs.Initialize(gx,gy);

	/* PER FRAME CONSTANTS BEGIN */
	_presetOutputs.zoom=1.0;
	_presetOutputs.zoomexp= 1.0;
	_presetOutputs.rot= 0.0;
	_presetOutputs.warp= 0.0;

	_presetOutputs.sx= 1.0;
	_presetOutputs.sy= 1.0;
	_presetOutputs.dx= 0.0;
	_presetOutputs.dy= 0.0;
	_presetOutputs.cx= 0.5;
	_presetOutputs.cy= 0.5;

	_presetOutputs.screenDecay=.98;


//_presetInputs.meshx = 0;
//_presetInputs.meshy = 0;


	/* PER_FRAME CONSTANTS END */
	_presetOutputs.fRating = 0;
	_presetOutputs.fGammaAdj = 1.0;
	_presetOutputs.videoEcho.zoom = 1.0;
	_presetOutputs.videoEcho.a = 0;
	_presetOutputs.videoEcho.orientation = Normal;

	_presetOutputs.textureWrap = 0;
	_presetOutputs.bDarkenCenter = 0;
	_presetOutputs.bRedBlueStereo = 0;
	_presetOutputs.bBrighten = 0;
	_presetOutputs.bDarken = 0;
	_presetOutputs.bSolarize = 0;
	_presetOutputs.bInvert = 0;
	_presetOutputs.bMotionVectorsOn = 1;

	_presetOutputs.fWarpAnimSpeed = 0;
	_presetOutputs.fWarpScale = 0;
	_presetOutputs.fShader = 0;

	/* PER_PIXEL CONSTANTS BEGIN */

	/* PER_PIXEL CONSTANT END */

	/* Q AND T VARIABLES START */

	for (int i = 0;i<NUM_Q_VARIABLES;i++)
		_presetOutputs.q[i] = 0;


	/* Q AND T VARIABLES END */

}


std::auto_ptr<Preset> MilkdropPresetFactory::allocate(const std::string & url, const std::string & name, const std::string & author) {

	_presetOutputs.customWaves.clear();
	_presetOutputs.customShapes.clear();

	std::string path;
	if (PresetFactory::protocol(url, path) == PresetFactory::IDLE_PRESET_PROTOCOL) {
		return IdlePresets::allocate(path, _presetOutputs);	
	} else
		return std::auto_ptr<Preset>(new MilkdropPreset(url, name, _presetOutputs));
}
