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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include "win32-dirent.h"
#else
#include <dirent.h>
#endif /** WIN32 */
#include <time.h>

#include "Preset.hpp"
#include "Parser.hpp"
#include "ParamUtils.hpp"
#include "InitCondUtils.hpp"
#include "fatal.h"
#include <iostream>
#include <fstream>


Preset::Preset(std::istream & in, const PresetInputs & presetInputs, PresetOutputs & presetOutputs):
    builtinParams(presetInputs, presetOutputs),
    file_path("[Input Stream]"),
    m_presetOutputs(presetOutputs),
    m_presetInputs(presetInputs)
 
{

  m_presetOutputs.customWaves.clear();
  m_presetOutputs.customShapes.clear();
  clearMeshChecks();

  initialize(in);

}


Preset::Preset(const std::string & filename, const PresetInputs & presetInputs, PresetOutputs & presetOutputs):
    builtinParams(presetInputs, presetOutputs),
    file_path(filename),
    m_presetOutputs(presetOutputs),
    m_presetInputs(presetInputs)
{

  m_presetOutputs.customWaves.clear();
  m_presetOutputs.customShapes.clear();
  clearMeshChecks();

  initialize(filename);

}

Preset::~Preset()
{

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<InitCond> >(init_cond_tree);

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<InitCond> >(per_frame_init_eqn_tree);

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<PerPixelEqn> >(per_pixel_eqn_tree);

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<PerFrameEqn> >(per_frame_eqn_tree);

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<Param> >(user_param_tree);

  for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos)
  {
    	delete(*pos);
  }

  for (PresetOutputs::cshape_container::iterator pos = customShapes.begin(); pos != customShapes.end(); ++pos)
  {
    	delete(*pos);
  }

}

/* Adds a per pixel equation according to its string name. This
   will be used only by the parser */

int Preset::add_per_pixel_eqn(char * name, GenExpr * gen_expr)
{

  PerPixelEqn * per_pixel_eqn = NULL;
  int index;
  Param * param = NULL;

  assert(gen_expr);
  assert(name);

  if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: per pixel equation (name = \"%s\")\n", name);


  if (!strncmp(name, "dx", strlen("dx")))
    this->m_presetOutputs.dx_is_mesh = true;
  else if (!strncmp(name, "dy", strlen("dy")))
    this->m_presetOutputs.dy_is_mesh = true;
  else if (!strncmp(name, "cx", strlen("cx")))
    this->m_presetOutputs.cx_is_mesh = true;
  else if (!strncmp(name, "cy", strlen("cy")))
    this->m_presetOutputs.cy_is_mesh = true;
  else if (!strncmp(name, "zoom", strlen("zoom")))
    this->m_presetOutputs.zoom_is_mesh = true;
  else if (!strncmp(name, "zoomexp", strlen("zoomexp")))
    this->m_presetOutputs.zoomexp_is_mesh = true;
  else if (!strncmp(name, "rot", strlen("rot")))
    this->m_presetOutputs.rot_is_mesh= true;
  else if (!strncmp(name, "sx", strlen("sx")))
    this->m_presetOutputs.sx_is_mesh = true;
  else if (!strncmp(name, "sy", strlen("sy")))
    this->m_presetOutputs.sy_is_mesh = true;
  else if (!strncmp(name, "warp", strlen("warp")))
    this->m_presetOutputs.warp_is_mesh = true;

  /* Search for the parameter so we know what matrix the per pixel equation is referencing */

  param = ParamUtils::find(name, &this->builtinParams, &this->user_param_tree);
  if ( !param )
  {
    if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: failed to allocate a new parameter!\n");
    return PROJECTM_FAILURE;
  }

  index = per_pixel_eqn_tree.size();

  /* Create the per pixel equation given the index, parameter, and general expression */
  if ((per_pixel_eqn = new PerPixelEqn(index, param, gen_expr)) == NULL)
  {
    if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: failed to create new per pixel equation!\n");
    return PROJECTM_FAILURE;
  }




  /* Insert the per pixel equation into the preset per pixel database */
  std::pair<std::map<int, PerPixelEqn*>::iterator, bool> inserteeOption = per_pixel_eqn_tree.insert
      (std::make_pair(per_pixel_eqn->index, per_pixel_eqn));

  if (!inserteeOption.second)
  {
    printf("failed to add per pixel eqn!\n");
    delete(per_pixel_eqn);
    return PROJECTM_FAILURE;
  }

  /* Done */
  return PROJECTM_SUCCESS;
}

void Preset::evalCustomShapeInitConditions()
{

  for (PresetOutputs::cshape_container::iterator pos = customShapes.begin(); pos != customShapes.end(); ++pos) {
    assert(*pos);
    (*pos)->evalInitConds();
  }
}


void Preset::evalCustomWaveInitConditions()
{

  for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos) {
    assert(*pos);
   (*pos)->evalInitConds();
}
}


void Preset::evalCustomWavePerFrameEquations()
{

  for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos)
  {

    std::map<std::string, InitCond*> & init_cond_tree = (*pos)->init_cond_tree;
    for (std::map<std::string, InitCond*>::iterator _pos = init_cond_tree.begin(); _pos != init_cond_tree.end(); ++_pos)
    {
      assert(_pos->second);
      _pos->second->evaluate();

//	if (_pos->second->param->name =="busedots")
//		std::cerr << "busedots: " << (*(bool*)_pos->second->param->engine_val)  << std::endl;
//	else std::cerr  << "name: " << _pos->second->param->name <<std::endl;
    }

    std::map<int, PerFrameEqn*> & per_frame_eqn_tree = (*pos)->per_frame_eqn_tree;
    for (std::map<int, PerFrameEqn*>::iterator _pos = per_frame_eqn_tree.begin(); _pos != per_frame_eqn_tree.end(); ++_pos)
    {
      assert(_pos->second);
      _pos->second->evaluate();
    }
  }

}

void Preset::evalCustomShapePerFrameEquations()
{

  for (PresetOutputs::cshape_container::iterator pos = customShapes.begin(); pos != customShapes.end(); ++pos)
  {

    std::map<std::string, InitCond*> & init_cond_tree = (*pos)->init_cond_tree;
    for (std::map<std::string, InitCond*>::iterator _pos = init_cond_tree.begin(); _pos != init_cond_tree.end(); ++_pos)
    {
      assert(_pos->second);
      _pos->second->evaluate();
    }
    std::map<int, PerFrameEqn*> & per_frame_eqn_tree = (*pos)->per_frame_eqn_tree;
    for (std::map<int, PerFrameEqn*>::iterator _pos = per_frame_eqn_tree.begin(); _pos != per_frame_eqn_tree.end(); ++_pos)
    {
      assert(_pos->second);
      _pos->second->evaluate();
    }
  }

}

void Preset::evalPerFrameInitEquations()
{

  /// @bug not sure if necessary
  for (std::map<std::string, InitCond*>::iterator pos = init_cond_tree.begin(); pos != init_cond_tree.end(); ++pos)
  {
    assert(pos->second);
    pos->second->evaluate();
  }

  for (std::map<std::string, InitCond*>::iterator pos = per_frame_init_eqn_tree.begin(); pos != per_frame_init_eqn_tree.end(); ++pos)
  {
    assert(pos->second);
    pos->second->evaluate();
  }

}

void Preset::evalPerFrameEquations()
{

  for (std::map<std::string, InitCond*>::iterator pos = init_cond_tree.begin(); pos != init_cond_tree.end(); ++pos)
  {
    assert(pos->second);
    pos->second->evaluate();
  }

  for (std::map<int, PerFrameEqn*>::iterator pos = per_frame_eqn_tree.begin(); pos != per_frame_eqn_tree.end(); ++pos)
  {
    assert(pos->second);
    pos->second->evaluate();
  }

}

void Preset::preloadInitialize() {
 
  /// @note commented this out because it should be unnecessary
  // Clear equation trees
  //init_cond_tree.clear();
  //user_param_tree.clear();
  //per_frame_eqn_tree.clear();
  //per_pixel_eqn_tree.clear();
  //per_frame_init_eqn_tree.clear();

  /* Set initial index values */
  this->per_pixel_eqn_string_index = 0;
  this->per_frame_eqn_string_index = 0;
  this->per_frame_init_eqn_string_index = 0;

  /* Clear string buffers */
  /// @bug replace with ostringstream?
  memset(this->per_pixel_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  memset(this->per_frame_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  memset(this->per_frame_init_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
}

void Preset::postloadInitialize() {

  /* It's kind of ugly to reset these values here. Should definitely be placed in the parser somewhere */
  this->per_frame_eqn_count = 0;
  this->per_frame_init_eqn_count = 0;

  this->loadBuiltinParamsUnspecInitConds();
  this->loadCustomWaveUnspecInitConds();
  this->loadCustomShapeUnspecInitConds();

  /// @note Recently moved from evaluateFrame() - is this more correct? not sure
  evalPerFrameInitEquations();
  evalCustomWaveInitConditions();
  evalCustomShapeInitConditions();

}

void Preset::initialize(const std::string & pathname)
{
  int retval;

  preloadInitialize();
  std::cerr << "[Preset] loading file \"" << pathname << "\"..." << std::endl;

  if ((retval = loadPresetFile(pathname)) < 0)
  {

     std::cerr << "[Preset] failed to load file \"" <<
      pathname << "\"!" << std::endl;

    /// @bug how should we handle this problem? a well define exception?
    throw retval;
  }

  postloadInitialize();
}

void Preset::initialize(std::istream & in)
{
  int retval;

  preloadInitialize();

  if ((retval = readIn(in)) < 0)
  {

     std::cerr << "[Preset] failed to load from stream " << std::endl; 

    /// @bug how should we handle this problem? a well define exception?
    throw retval;
  }

  postloadInitialize();
}

void Preset::loadBuiltinParamsUnspecInitConds() {

  InitCondUtils::LoadUnspecInitCond loadUnspecInitCond(this->init_cond_tree, this->per_frame_init_eqn_tree);

  this->builtinParams.traverse(loadUnspecInitCond);

}

void Preset::loadCustomWaveUnspecInitConds()
{


  for (PresetOutputs::cwave_container::iterator pos = customWaves.begin(); pos != customWaves.end(); ++pos)
  {
    assert(*pos);
    (*pos)->loadUnspecInitConds();
  }

}

void Preset::loadCustomShapeUnspecInitConds()
{

  for (PresetOutputs::cshape_container::iterator pos = customShapes.begin(); pos != customShapes.end(); ++pos)
  {
    assert(*pos);
    (*pos)->loadUnspecInitConds();
  }
}


void Preset::evaluateFrame()
{

  /* Evaluate all equation objects according to milkdrop flow diagram */

  evalPerFrameEquations();
  evalPerPixelEqns();
  evalCustomWavePerFrameEquations();
  evalCustomShapePerFrameEquations();

  // Setup pointers of the custom waves and shapes to the preset outputs instance
  /// @slow an extra O(N) per frame, could do this during eval
  m_presetOutputs.customWaves = PresetOutputs::cwave_container(customWaves); 
  m_presetOutputs.customShapes = PresetOutputs::cshape_container(customShapes);

}


// Evaluates all per-pixel equations 
void Preset::evalPerPixelEqns()
{

  /* Evaluate all per pixel equations in the tree datastructure */
  for (int mesh_x = 0; mesh_x < m_presetInputs.gx; mesh_x++)
	  for (int mesh_y = 0; mesh_y < m_presetInputs.gy; mesh_y++)
  for (std::map<int, PerPixelEqn*>::iterator pos = per_pixel_eqn_tree.begin();
       pos != per_pixel_eqn_tree.end(); ++pos)
    pos->second->evaluate(mesh_x, mesh_y);

}

int Preset::readIn(std::istream & fs) {

  line_mode_t line_mode;

  /* Parse any comments */
  if (Parser::parse_top_comment(fs) < 0)
  {
    std::cerr << "[Preset::readIn] no left bracket found..." << std::endl;
    return PROJECTM_FAILURE;
  }

  /* Parse the preset name and a left bracket */
  char tmp_name[MAX_TOKEN_SIZE];

  if (Parser::parse_preset_name(fs, tmp_name) < 0)
  {
    std::cerr <<  "[Preset::readIn] loading of preset name failed" << std::endl;
    return PROJECTM_ERROR;
  }

  this->name = std::string(tmp_name);

  std::cerr << "[Preset::readIn] preset \"" << this->name << "\" parsed." << std::endl;;

  // Loop through each line in file, trying to succesfully parse the file. 
  // If a line does not parse correctly, keep trucking along to next line.
  int retval;
  while ((retval = Parser::parse_line(fs, this)) != EOF)
  {
    if (retval == PROJECTM_PARSE_ERROR)
    {
      line_mode = NORMAL_LINE_MODE;
      std::cerr << "[Preset::readIn()] parse error in file \"" << this->absoluteFilePath() << "\"" << std::endl;
    }
  }

  std::cerr << "loadPresetFile: finished line parsing successfully" << std::endl;

  /* Now the preset has been loaded.
     Evaluation calls can be made at appropiate
     times in the frame loop */

return PROJECTM_SUCCESS;
}

/* loadPresetFile: private function that loads a specific preset denoted
   by the given pathname */
int Preset::loadPresetFile(const std::string & pathname)
{


  /* Open the file corresponding to pathname */
  std::ifstream fs(pathname.c_str());
  if (!fs || fs.eof()) {
    if (PRESET_DEBUG)
    	std::cerr << "loadPresetFile: loading of file \"" << pathname << "\" failed!\n";
    return PROJECTM_ERROR;
  }

 return readIn(fs);

}

