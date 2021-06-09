/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef WIN32
#include "dirent.h"
#else

#include <dirent.h>

#endif /** WIN32 */

#include <time.h>

#include "MilkdropPreset.hpp"
#include "Parser.hpp"
#include "ParamUtils.hpp"
#include "InitCondUtils.hpp"
#include "fatal.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "PresetFrameIO.hpp"

#include "PresetFactoryManager.hpp"
#include "MilkdropPresetFactory.hpp"

#ifdef __SSE2__

#include <immintrin.h>

#endif


MilkdropPreset::MilkdropPreset(MilkdropPresetFactory* factory, std::istream& in, const std::string& presetName,
                               PresetOutputs* presetOutputs)
    : Preset(presetName)
    , builtinParams(_presetInputs, presetOutputs)
    , per_pixel_program(nullptr)
    , _factory(factory)
    , _presetOutputs(presetOutputs)
{
    initialize(in);
}


MilkdropPreset::MilkdropPreset(MilkdropPresetFactory* factory, const std::string& absoluteFilePath,
                               const std::string& presetName, PresetOutputs* presetOutputs)
    : Preset(presetName)
    , builtinParams(_presetInputs, presetOutputs)
    , per_pixel_program(nullptr)
    , _filename(parseFilename(absoluteFilePath))
    , _absoluteFilePath(absoluteFilePath)
    , _factory(factory)
    , _presetOutputs(presetOutputs)
{

    initialize(absoluteFilePath);
}


MilkdropPreset::~MilkdropPreset()
{

    traverse<TraverseFunctors::Delete<InitCond> >(init_cond_tree);

    traverse<TraverseFunctors::Delete<InitCond> >(per_frame_init_eqn_tree);

    traverse<TraverseFunctors::Delete<PerPixelEqn> >(per_pixel_eqn_tree);
    Expr::delete_expr(per_pixel_program);

    traverseVector<TraverseFunctors::Delete<PerFrameEqn> >(per_frame_eqn_tree);

    traverse<TraverseFunctors::Delete<Param> >(user_param_tree);

    /// Testing deletion of render items by the preset. would be nice if it worked,
    /// and seems to be working if you use a mutex on the preset switching.

    for (PresetOutputs::cwave_container::iterator pos = customWaves.begin();
         pos != customWaves.end(); ++pos)
    {
        //  __android_log_print(ANDROID_LOG_ERROR, "projectM", "not freeing wave %x", *pos);
        delete (*pos);
    }

    for (PresetOutputs::cshape_container::iterator pos = customShapes.begin();
         pos != customShapes.end(); ++pos)
    {
//__android_log_print(ANDROID_LOG_ERROR, "projectM", "not freeing shape %x", *pos);

        delete (*pos);
    }
    customWaves.clear();
    customShapes.clear();

    if (_factory)
    {
        _factory->releasePreset(this);
    }
}

/* Adds a per pixel equation according to its string name. This
   will be used only by the parser */

int MilkdropPreset::add_per_pixel_eqn(char* name, Expr* gen_expr)
{
    PerPixelEqn* per_pixel_eqn = NULL;
    Param* param = NULL;

    assert(gen_expr);
    assert(name);

    if (PER_PIXEL_EQN_DEBUG)
    {
        printf("add_per_pixel_eqn: per pixel equation (name = \"%s\")\n", name);
    }

    /* Search for the parameter so we know what matrix the per pixel equation is referencing */

    param = ParamUtils::find(name, &this->builtinParams, &this->user_param_tree);
    if (!param)
    {
        if (PER_PIXEL_EQN_DEBUG)
        {
            printf("add_per_pixel_eqn: failed to allocate a new parameter!\n");
        }
        return PROJECTM_FAILURE;
    }

    auto index = per_pixel_eqn_tree.size();

    /* Create the per pixel equation given the index, parameter, and general expression */
    if ((per_pixel_eqn = new PerPixelEqn(index, param, gen_expr)) == NULL)
    {
        if (PER_PIXEL_EQN_DEBUG)
        {
            printf("add_per_pixel_eqn: failed to create new per pixel equation!\n");
        }
        return PROJECTM_FAILURE;
    }

    /* Insert the per pixel equation into the preset per pixel database */
    std::pair<std::map<int, PerPixelEqn*>::iterator, bool> inserteeOption = per_pixel_eqn_tree.insert
        (std::make_pair(per_pixel_eqn->index, per_pixel_eqn));

    if (!inserteeOption.second)
    {
        printf("failed to add per pixel eqn!\n");
        delete (per_pixel_eqn);
        return PROJECTM_FAILURE;
    }

    /* Done */
    return PROJECTM_SUCCESS;
}

void MilkdropPreset::evalCustomShapeInitConditions()
{

    for (PresetOutputs::cshape_container::iterator pos = customShapes.begin(); pos != customShapes.end(); ++pos)
    {
        assert(*pos);
        (*pos)->evalInitConds();
    }
}


void MilkdropPreset::evalCustomWaveInitConditions()
{

    for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos)
    {
        assert(*pos);
        (*pos)->evalInitConds();
    }
}


void MilkdropPreset::evalCustomWavePerFrameEquations()
{

    for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos)
    {

        std::map<std::string, InitCond*>& init_cond_tree2 = (*pos)->init_cond_tree;
        for (std::map<std::string, InitCond*>::iterator _pos = init_cond_tree2.begin();
             _pos != init_cond_tree2.end(); ++_pos)
        {
            assert(_pos->second);
            _pos->second->evaluate();
        }

        std::vector<PerFrameEqn*>& per_frame_eqn_tree2 = (*pos)->per_frame_eqn_tree;
        for (std::vector<PerFrameEqn*>::iterator _pos = per_frame_eqn_tree2.begin();
             _pos != per_frame_eqn_tree2.end(); ++_pos)
        {
            (*_pos)->evaluate();
        }
    }

}

void MilkdropPreset::evalCustomShapePerFrameEquations()
{

    for (PresetOutputs::cshape_container::iterator pos = customShapes.begin(); pos != customShapes.end(); ++pos)
    {

        std::map<std::string, InitCond*>& init_cond_tree2 = (*pos)->init_cond_tree;
        for (std::map<std::string, InitCond*>::iterator _pos = init_cond_tree2.begin();
             _pos != init_cond_tree2.end(); ++_pos)
        {
            assert(_pos->second);
            _pos->second->evaluate();
        }

        std::vector<PerFrameEqn*>& per_frame_eqn_tree2 = (*pos)->per_frame_eqn_tree;
        for (std::vector<PerFrameEqn*>::iterator _pos = per_frame_eqn_tree2.begin();
             _pos != per_frame_eqn_tree2.end(); ++_pos)
        {
            (*_pos)->evaluate();
        }
    }

}

void MilkdropPreset::evalPerFrameInitEquations()
{

    for (std::map<std::string, InitCond*>::iterator pos = per_frame_init_eqn_tree.begin();
         pos != per_frame_init_eqn_tree.end(); ++pos)
    {
        assert(pos->second);
        pos->second->evaluate();
    }

}

void MilkdropPreset::evalPerFrameEquations()
{

    for (std::map<std::string, InitCond*>::iterator pos = init_cond_tree.begin(); pos != init_cond_tree.end(); ++pos)
    {
        assert(pos->second);
        pos->second->evaluate();
    }

    for (std::vector<PerFrameEqn*>::iterator pos = per_frame_eqn_tree.begin(); pos != per_frame_eqn_tree.end(); ++pos)
    {
        (*pos)->evaluate();
    }

}

void MilkdropPreset::preloadInitialize()
{

    /// @note commented this out because it should be unnecessary
    // Clear equation trees
    //init_cond_tree.clear();
    //user_param_tree.clear();
    //per_frame_eqn_tree.clear();
    //per_pixel_eqn_tree.clear();
    //per_frame_init_eqn_tree.clear();


}

void MilkdropPreset::postloadInitialize()
{
    /* It's kind of ugly to reset these values here. Should definitely be placed in the parser somewhere */
    this->per_frame_eqn_count = 0;
    this->per_frame_init_eqn_count = 0;

    this->loadBuiltinParamsUnspecInitConds();
    this->loadCustomWaveUnspecInitConds();
    this->loadCustomShapeUnspecInitConds();


/// @bug are you handling all the q variables conditions? in particular, the un-init case?
//    if (_presetOutputs)
//    {
//        _presetOutputs->q1 = 0;
//        _presetOutputs->q2 = 0;
//        _presetOutputs->q3 = 0;
//        _presetOutputs->q4 = 0;
//        _presetOutputs->q5 = 0;
//        _presetOutputs->q6 = 0;
//        _presetOutputs->q7 = 0;
//        _presetOutputs->q8 = 0;
//    }

}

void MilkdropPreset::Render(const BeatDetect& music, const PipelineContext& context)
{
    _presetInputs.update(music, context);

    evaluateFrame();
    pipeline().Render(music, context);

}

void MilkdropPreset::initialize(const std::string& pathname)
{
    preloadInitialize();

    if (MILKDROP_PRESET_DEBUG)
    {
        std::cerr << "[Preset] loading file \"" << pathname << "\"..." << std::endl;
    }

    loadPresetFile(pathname);

    postloadInitialize();

    if (!presetOutputs().compositeShader.programSource.empty())
    {
        pipeline().compositeShaderFilename = pathname;
    }
    if (!presetOutputs().warpShader.programSource.empty())
    {
        pipeline().warpShaderFilename = pathname;
    }
}

void MilkdropPreset::initialize(std::istream& in)
{
    int retval;

    preloadInitialize();

    if ((retval = readIn(in)) < 0)
    {

        if (MILKDROP_PRESET_DEBUG)
        {
            std::cerr << "[Preset] failed to load from stream " << std::endl;
        }

        /// @bug how should we handle this problem? a well define exception?
        throw PresetFactoryException("failed to read from input stream");
    }

    postloadInitialize();
}

void MilkdropPreset::loadBuiltinParamsUnspecInitConds()
{

    InitCondUtils::LoadUnspecInitCond loadUnspecInitCond(this->init_cond_tree, this->per_frame_init_eqn_tree);

    this->builtinParams.apply(loadUnspecInitCond);
    traverse(user_param_tree, loadUnspecInitCond);

}

void MilkdropPreset::loadCustomWaveUnspecInitConds()
{

    for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos)
    {
        assert(*pos);
        (*pos)->loadUnspecInitConds();
    }

}

void MilkdropPreset::loadCustomShapeUnspecInitConds()
{

    for (PresetOutputs::cshape_container::iterator pos = customShapes.begin();
         pos != customShapes.end(); ++pos)
    {
        assert(*pos);
        (*pos)->loadUnspecInitConds();
    }
}


void MilkdropPreset::evaluateFrame()
{

    // Evaluate all equation objects according to milkdrop flow diagram

    evalPerFrameInitEquations();
    evalPerFrameEquations();

    // Important step to ensure custom shapes and waves don't stamp on the q variable values
    // calculated by the per frame (init) and per pixel equations.
    transfer_q_variables(customWaves);
    transfer_q_variables(customShapes);

    initialize_PerPixelMeshes();

    evalPerPixelEqns();

    evalCustomWaveInitConditions();
    evalCustomWavePerFrameEquations();

    evalCustomShapeInitConditions();
    evalCustomShapePerFrameEquations();

    // Setup pointers of the custom waves and shapes to the preset outputs instance
    if (_presetOutputs)
    {
        /// @slow an extra O(N) per frame, could do this during eval
        _presetOutputs->customWaves = PresetOutputs::cwave_container(customWaves);
        _presetOutputs->customShapes = PresetOutputs::cshape_container(customShapes);
    }

}


#ifdef __SSE2__

inline void init_mesh(float** mesh, const float value, const int gx, const int gy)
{
    __m128 mvalue = _mm_set_ps1(value);
    for (int x = 0; x < gx; x++)
    {
        for (int y = 0; y < gy; y += 4)
        {
            _mm_store_ps(&mesh[x][y], mvalue);
        }
    }
}

#else
inline void init_mesh(float **mesh, const float value, const int gx, const int gy)
{
    for (int x=0; x<gx; x++)
        for (int y=0; y<gy; y++)
            mesh[x][y] = value;
}
#endif

void MilkdropPreset::initialize_PerPixelMeshes()
{
    int gx = presetInputs().gx;
    int gy = presetInputs().gy;

    if (!_presetOutputs)
    {
        return;
    }

    init_mesh(_presetOutputs->cx_mesh, _presetOutputs->cx, gx, gy);
    init_mesh(_presetOutputs->cy_mesh, _presetOutputs->cy, gx, gy);
    init_mesh(_presetOutputs->sx_mesh, _presetOutputs->sx, gx, gy);
    init_mesh(_presetOutputs->sy_mesh, _presetOutputs->sy, gx, gy);
    init_mesh(_presetOutputs->dx_mesh, _presetOutputs->dx, gx, gy);
    init_mesh(_presetOutputs->dy_mesh, _presetOutputs->dy, gx, gy);
    init_mesh(_presetOutputs->zoom_mesh, _presetOutputs->zoom, gx, gy);
    init_mesh(_presetOutputs->zoomexp_mesh, _presetOutputs->zoomexp, gx, gy);
    init_mesh(_presetOutputs->rot_mesh, _presetOutputs->rot, gx, gy);
    init_mesh(_presetOutputs->warp_mesh, _presetOutputs->warp, gx, gy);
}


// Evaluates all per-pixel equations
void MilkdropPreset::evalPerPixelEqns()
{
    // Quick bail out if there is nothing to do.
    if (per_pixel_eqn_tree.empty())
    {
        return;
    }

    if (nullptr == per_pixel_program)
    {
        // This is a little forward looking, but if we want to JIT assignments expressions, we might
        // as well JIT the batch all together rather than one at a time.  At the moment ProgramExpr is
        // just a different place to loop over the individual steps, but the idea is that this encapsulates
        // an optimizable chunk of work.
        // See also CustomWave which does the same for PerPointEqn
        std::vector<Expr*> steps;
        for (std::map<int, PerPixelEqn*>::iterator pos = per_pixel_eqn_tree.begin();
             pos != per_pixel_eqn_tree.end(); ++pos)
        {
            steps.push_back(pos->second->assign_expr);
        }
        Expr* program_expr = Expr::create_program_expr(steps, false);
        Expr* jit = nullptr;
#if HAVE_LLVM
        std::string module_name = this->_filename + "_per_pixel";
        if (!steps.empty())
            jit = Expr::jit(program_expr, module_name);
#endif
        per_pixel_program = jit ? jit : program_expr;
    }

    for (int mesh_x = 0; mesh_x < presetInputs().gx; mesh_x++)
    {
        for (int mesh_y = 0; mesh_y < presetInputs().gy; mesh_y++)
        {
            per_pixel_program->eval(mesh_x, mesh_y);
        }
    }
}

int MilkdropPreset::readIn(std::istream& fs)
{
    if (_presetOutputs)
    {
        _presetOutputs->compositeShader.programSource.clear();
        _presetOutputs->warpShader.programSource.clear();
    }

    /* Parse any comments (aka "[preset00]") */
    /* We don't do anything with this info so it's okay if it's missing */
    if (Parser::parse_top_comment(fs) == PROJECTM_SUCCESS)
    {
        /* Parse the preset name and a left bracket */
        char tmp_name[MAX_TOKEN_SIZE];

        if (Parser::parse_preset_name(fs, tmp_name) < 0)
        {
            std::cerr << "[Preset::readIn] loading of preset name failed" << std::endl;
            fs.seekg(0);
        }
        /// @note  We ignore the preset name because [preset00] is just not so useful
    }
    else
    {
        // no comment found. whatever
        if (MILKDROP_PRESET_DEBUG)
        {
            std::cerr << "[Preset::readIn] no left bracket found..." << std::endl;
        }
        fs.seekg(0);
    }

    // Loop through each line in file, trying to successfully parse the file.
    // If a line does not parse correctly, keep trucking along to next line.
    int retval;
    while ((retval = Parser::parse_line(fs, this)) != EOF)
    {
        if (retval == PROJECTM_PARSE_ERROR)
        {
            // std::cerr << "[Preset::readIn()] parse error in file \"" << this->absoluteFilePath() << "\"" << std::endl;
        }
    }

//  std::cerr << "loadPresetFile: finished line parsing successfully" << std::endl;

    /* Now the preset has been loaded.
       Evaluation calls can be made at appropiate
       times in the frame loop */

    return PROJECTM_SUCCESS;
}

/* loadPresetFile: private function that loads a specific preset denoted
   by the given pathname */
int MilkdropPreset::loadPresetFile(const std::string& pathname)
{


    /* Open the file corresponding to pathname */
    std::ifstream fs(pathname.c_str());
    if (!fs || fs.eof())
    {

        std::ostringstream oss;
        oss << "Problem reading file from path: \"" << pathname << "\"";

        throw PresetFactoryException(oss.str());

    }

    return readIn(fs);

}

const std::string& MilkdropPreset::name() const
{

    return filename();
}

