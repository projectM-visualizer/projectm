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
#include "PresetFrameIO.hpp"

MilkdropPresetFactory::MilkdropPresetFactory(int gx_, int gy_)
    : gx(gx_)
    , gy(gy_)
    , _presetOutputsCache(nullptr)
{
    /* Initializes the builtin function database */
    BuiltinFuncs::init_builtin_func_db();

    /* Initializes all infix operators */
    Eval::init_infix_ops();
}

MilkdropPresetFactory::~MilkdropPresetFactory()
{
    Eval::destroy_infix_ops();
    BuiltinFuncs::destroy_builtin_func_db();

    if (_presetOutputsCache)
    {
        delete (_presetOutputsCache);
        _presetOutputsCache = nullptr;
    }
}

/* Reinitializes the engine variables to a default (conservative and sane) value */
void resetPresetOutputs(PresetOutputs* presetOutputs)
{
    if (!presetOutputs)
    {
        return;
    }

    presetOutputs->zoom = 1.0;
    presetOutputs->zoomexp = 1.0;
    presetOutputs->rot = 0.0;
    presetOutputs->warp = 0.0;

    presetOutputs->sx = 1.0;
    presetOutputs->sy = 1.0;
    presetOutputs->dx = 0.0;
    presetOutputs->dy = 0.0;
    presetOutputs->cx = 0.5;
    presetOutputs->cy = 0.5;

    presetOutputs->screenDecay = .98;

    presetOutputs->wave.r = 1.0;
    presetOutputs->wave.g = 0.2;
    presetOutputs->wave.b = 0.0;
    presetOutputs->wave.x = 0.5;
    presetOutputs->wave.y = 0.5;
    presetOutputs->wave.mystery = 0.0;

    presetOutputs->border.outer_size = 0.0;
    presetOutputs->border.outer_r = 0.0;
    presetOutputs->border.outer_g = 0.0;
    presetOutputs->border.outer_b = 0.0;
    presetOutputs->border.outer_a = 0.0;

    presetOutputs->border.inner_size = 0.0;
    presetOutputs->border.inner_r = 0.0;
    presetOutputs->border.inner_g = 0.0;
    presetOutputs->border.inner_b = 0.0;
    presetOutputs->border.inner_a = 0.0;

    presetOutputs->mv.a = 0.0;
    presetOutputs->mv.r = 0.0;
    presetOutputs->mv.g = 0.0;
    presetOutputs->mv.b = 0.0;
    presetOutputs->mv.length = 1.0;
    presetOutputs->mv.x_num = 16.0;
    presetOutputs->mv.y_num = 12.0;
    presetOutputs->mv.x_offset = 0.02;
    presetOutputs->mv.y_offset = 0.02;


    /* PER_FRAME CONSTANTS END */
    presetOutputs->fRating = 0;
    presetOutputs->fGammaAdj = 1.0;
    presetOutputs->videoEcho.zoom = 1.0;
    presetOutputs->videoEcho.a = 0;
    presetOutputs->videoEcho.orientation = Normal;

    presetOutputs->wave.additive = false;
    presetOutputs->wave.dots = false;
    presetOutputs->wave.thick = false;
    presetOutputs->wave.modulateAlphaByVolume = 0;
    presetOutputs->wave.maximizeColors = 0;
    presetOutputs->textureWrap = 0;
    presetOutputs->bDarkenCenter = 0;
    presetOutputs->bRedBlueStereo = 0;
    presetOutputs->bBrighten = 0;
    presetOutputs->bDarken = 0;
    presetOutputs->bSolarize = 0;
    presetOutputs->bInvert = 0;
    presetOutputs->bMotionVectorsOn = 1;

    presetOutputs->wave.a = 1.0;
    presetOutputs->wave.scale = 1.0;
    presetOutputs->wave.smoothing = 0;
    presetOutputs->wave.mystery = 0;
    presetOutputs->wave.modOpacityEnd = 0;
    presetOutputs->wave.modOpacityStart = 0;
    presetOutputs->fWarpAnimSpeed = 0;
    presetOutputs->fWarpScale = 0;
    presetOutputs->fShader = 0;

    /* PER_PIXEL CONSTANT END */
    /* Q VARIABLES START */

    for (int i = 0; i < 32; i++)
    {
        presetOutputs->q[i] = 0;
    }

//	for ( std::vector<CustomWave*>::iterator pos = presetOutputs->customWaves.begin();
//	        pos != presetOutputs->customWaves.end(); ++pos )
//		if ( *pos != 0 ) delete ( *pos );

//	for ( std::vector<CustomShape*>::iterator pos = presetOutputs->customShapes.begin();
//	        pos != presetOutputs->customShapes.end(); ++pos )
//		if ( *pos != 0 ) delete ( *pos );

    presetOutputs->customWaves.clear();
    presetOutputs->customShapes.clear();

    /* Q VARIABLES END */

}


/* Reinitializes the engine variables to a default (conservative and sane) value */
void MilkdropPresetFactory::reset()
{
    if (_presetOutputsCache)
    {
        resetPresetOutputs(_presetOutputsCache);
    }
}

PresetOutputs* MilkdropPresetFactory::createPresetOutputs(int gx, int gy)
{

    PresetOutputs* presetOutputs = new PresetOutputs();

    presetOutputs->Initialize(gx, gy);

    /* PER FRAME CONSTANTS BEGIN */
    presetOutputs->zoom = 1.0;
    presetOutputs->zoomexp = 1.0;
    presetOutputs->rot = 0.0;
    presetOutputs->warp = 0.0;

    presetOutputs->sx = 1.0;
    presetOutputs->sy = 1.0;
    presetOutputs->dx = 0.0;
    presetOutputs->dy = 0.0;
    presetOutputs->cx = 0.5;
    presetOutputs->cy = 0.5;

    presetOutputs->screenDecay = .98;


//_presetInputs.meshx = 0;
//_presetInputs.meshy = 0;


    /* PER_FRAME CONSTANTS END */
    presetOutputs->fRating = 0;
    presetOutputs->fGammaAdj = 1.0;
    presetOutputs->videoEcho.zoom = 1.0;
    presetOutputs->videoEcho.a = 0;
    presetOutputs->videoEcho.orientation = Normal;

    presetOutputs->textureWrap = 0;
    presetOutputs->bDarkenCenter = 0;
    presetOutputs->bRedBlueStereo = 0;
    presetOutputs->bBrighten = 0;
    presetOutputs->bDarken = 0;
    presetOutputs->bSolarize = 0;
    presetOutputs->bInvert = 0;
    presetOutputs->bMotionVectorsOn = 1;
    presetOutputs->fWarpAnimSpeed = 0;
    presetOutputs->fWarpScale = 0;
    presetOutputs->fShader = 0;

    /* PER_PIXEL CONSTANTS BEGIN */

    /* PER_PIXEL CONSTANT END */

    /* Q AND T VARIABLES START */

    for (unsigned int i = 0; i < NUM_Q_VARIABLES; i++)
    {
        presetOutputs->q[i] = 0;
    }

    /* Q AND T VARIABLES END */
    return presetOutputs;
}


std::unique_ptr<Preset>
MilkdropPresetFactory::allocate(const std::string& url, const std::string& name, const std::string& author)
{

    PresetOutputs* presetOutputs;
    // use cached PresetOutputs if there is one, otherwise allocate
    if (_presetOutputsCache)
    {
        presetOutputs = _presetOutputsCache;
        _presetOutputsCache = nullptr;
    }
    else
    {
        presetOutputs = createPresetOutputs(gx, gy);
    }

    resetPresetOutputs(presetOutputs);

    std::string path;
    if (PresetFactory::protocol(url, path) == PresetFactory::IDLE_PRESET_PROTOCOL)
    {
        return IdlePresets::allocate(this, path, presetOutputs);
    }
    else
    {
        return std::unique_ptr<Preset>(new MilkdropPreset(this, url, name, presetOutputs));
    }
}

// this gives the preset a way to return the PresetOutput w/o dependency on class projectM behavior
void MilkdropPresetFactory::releasePreset(Preset* preset)
{
    auto milkdropPreset = dynamic_cast<MilkdropPreset*>(preset);

    if (!milkdropPreset || !milkdropPreset->_presetOutputs)
    {
        // Instance is not a MilkdropPreset or has no PresetOutput object.
        return;
    }

    // return PresetOutputs to the cache
    if (!_presetOutputsCache)
    {
        _presetOutputsCache = milkdropPreset->_presetOutputs;
    }
    else
    {
        delete milkdropPreset->_presetOutputs;
    }
}
