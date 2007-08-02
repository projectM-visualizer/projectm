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
#include "fatal.h"
#include <iostream>

Preset::Preset(const std::string & filename, const PresetInputs & presetInputs, PresetOutputs & presetOutputs):
    file_path(filename),
    builtinParams(presetInputs, presetOutputs),
    customWaves(&presetOutputs.customWaves),
    customShapes(&presetOutputs.customShapes),
    m_presetOutputs(presetOutputs)

{


  initialize(filename);

}

Preset::~Preset()
{

#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE( "~preset(): in\n" );
#endif



  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<InitCond> >(init_cond_tree);

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<InitCond> >(per_frame_init_eqn_tree);

  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<PerPixelEqn> >(per_pixel_eqn_tree);
 
  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<PerFrameEqn> >(per_frame_eqn_tree);
 
  Algorithms::traverse<Algorithms::TraverseFunctors::DeleteFunctor<Param> >(user_param_tree);
 
  /// @note no need to clear the actual container itself
  for (PresetOutputs::cwave_container::iterator pos = customWaves->begin(); pos != customWaves->end(); ++pos)
    delete(*pos);

  for (PresetOutputs::cshape_container::iterator pos = customShapes->begin(); pos != customShapes->end(); ++pos)
    delete(*pos);


#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE( "~Preset(): out\n" );
#endif


}
/* Adds a per pixel equation according to its string name. This
   will be used only by the parser */

int Preset::add_per_pixel_eqn(char * name, GenExpr * gen_expr)
{

  PerPixelEqn * per_pixel_eqn = NULL;
  int index;
  Param * param = NULL;

  /* Argument checks */
  if (gen_expr == NULL)
    return PROJECTM_FAILURE;
  if (name == NULL)
    return PROJECTM_FAILURE;

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

  /* Search for the parameter so we know what matrix the per pixel equation is referencing */

  param = ParamUtils::find(name, &this->builtinParams, &this->user_param_tree);
  if ( !param )
  {
    if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: failed to allocate a new parameter!\n");
    return PROJECTM_FAILURE;
  }

  if (per_pixel_eqn_tree.empty())
  {
    index = 0;
  }
  else
  {
    std::map<int, PerPixelEqn*>::iterator lastPos = per_pixel_eqn_tree.end();
    index = per_pixel_eqn_tree.size();
  }

  /* Create the per pixel equation given the index, parameter, and general expression */
  if ((per_pixel_eqn = new PerPixelEqn(index, param, gen_expr)) == NULL)
  {
    if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: failed to create new per pixel equation!\n");
    return PROJECTM_FAILURE;

  }

  if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: new equation (index = %d,matrix=%X) (param = \"%s\")\n",
                                    per_pixel_eqn->index, per_pixel_eqn->param->matrix, per_pixel_eqn->param->name.c_str());

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

  for (PresetOutputs::cshape_container::iterator pos = customShapes->begin(); pos != customShapes->end(); ++pos)
    (*pos)->eval_custom_shape_init_conds();
}


void Preset::evalCustomWaveInitConditions()
{

  for (PresetOutputs::cwave_container::iterator pos = customWaves->begin(); pos != customWaves->end(); ++pos)
    (*pos)->eval_custom_wave_init_conds();
}


void Preset::evalCustomWavePerFrameEquations()
{

  /// @bug see above
  for (PresetOutputs::cwave_container::iterator pos = customWaves->begin(); pos != customWaves->end(); ++pos)
  {

    std::map<std::string, InitCond*> & init_cond_tree = (*pos)->init_cond_tree;
    for (std::map<std::string, InitCond*>::iterator _pos = init_cond_tree.begin(); _pos != init_cond_tree.end(); ++_pos)
      _pos->second->evaluate();

    std::map<int, PerFrameEqn*> & per_frame_eqn_tree = (*pos)->per_frame_eqn_tree;
    for (std::map<int, PerFrameEqn*>::iterator _pos = per_frame_eqn_tree.begin(); _pos != per_frame_eqn_tree.end(); ++_pos)
      _pos->second->evaluate();
  }

}

void Preset::evalCustomShapePerFrameEquations()
{

  for (PresetOutputs::cshape_container::iterator pos = customShapes->begin(); pos != customShapes->end(); ++pos)
  {

    std::map<std::string, InitCond*> & init_cond_tree = (*pos)->init_cond_tree;
    for (std::map<std::string, InitCond*>::iterator _pos = init_cond_tree.begin(); _pos != init_cond_tree.end(); ++_pos)
      _pos->second->evaluate();

    std::map<int, PerFrameEqn*> & per_frame_eqn_tree = (*pos)->per_frame_eqn_tree;
    for (std::map<int, PerFrameEqn*>::iterator _pos = per_frame_eqn_tree.begin(); _pos != per_frame_eqn_tree.end(); ++_pos)
      _pos->second->evaluate();
  }

}

void Preset::evalInitConditions()
{

  for (std::map<std::string, InitCond*>::iterator pos = per_frame_init_eqn_tree.begin(); pos != per_frame_init_eqn_tree.end(); ++pos)
    pos->second->evaluate();

}

void Preset::evalPerFrameEquations()
{

  for (std::map<std::string, InitCond*>::iterator pos = init_cond_tree.begin(); pos != init_cond_tree.end(); ++pos)
    pos->second->evaluate();

  for (std::map<int, PerFrameEqn*>::iterator pos = per_frame_eqn_tree.begin(); pos != per_frame_eqn_tree.end(); ++pos)
    pos->second->evaluate();

}


void Preset::initialize(const std::string & pathname)
{

  // Clear equation trees
  init_cond_tree.clear();
  user_param_tree.clear();
  per_frame_eqn_tree.clear();
  per_pixel_eqn_tree.clear();
  per_frame_init_eqn_tree.clear();

  /* Set initial index values */
  this->per_pixel_eqn_string_index = 0;
  this->per_frame_eqn_string_index = 0;
  this->per_frame_init_eqn_string_index = 0;

  /* Clear string buffers */
  memset(this->per_pixel_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  memset(this->per_frame_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  memset(this->per_frame_init_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  int retval;
  
  if ((retval = load_preset_file(pathname.c_str())) < 0)
  {

#ifdef PRESET_DEBUG
    if (PRESET_DEBUG) std::cerr << "[Preset] failed to load file \"" << 
	pathname << "\"!" << std::endl;
#endif
    //this->close_preset();
    /// @bug how should we handle this problem? a well define exception?
    throw retval;
  }

  /* It's kind of ugly to reset these values here. Should definitely be placed in the parser somewhere */
  this->per_frame_eqn_count = 0;
  this->per_frame_init_eqn_count = 0;

}

/// @bug broken / unimplemented
void Preset::savePreset(char * filename)
{

  return;

#ifdef PANTS
  FILE * fs;

  if (filename == NULL)
    return;

  /* Open the file corresponding to pathname */
  if ((fs = fopen(filename, "w+")) == 0)
  {
#ifdef PRESET_DEBUG
    if (PRESET_DEBUG) printf("savePreset: failed to create filename \"%s\"!\n", filename);
#endif
    return;
  }

  write_stream = fs;

  if (write_preset_name(fs) < 0)
  {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_init_conditions(fs) < 0)
  {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_per_frame_init_equations(fs) < 0)
  {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_per_frame_equations(fs) < 0)
  {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_per_pixel_equations(fs) < 0)
  {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  write_stream = NULL;
  fclose(fs);
#endif
}

int Preset::write_preset_name(FILE * fs)
{

  char s[256];
  int len;

  memset(s, 0, 256);

  if (fs == NULL)
    return PROJECTM_FAILURE;

  /* Format the preset name in a string */
  /// @bug removed, code is dead
  //   sprintf(s, "[%s]\n", name);

  len = strlen(s);

  /* Write preset name to file stream */
  if (fwrite(s, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;

}

#ifdef NEEDS_MOVED
int Preset::write_init_conditions(FILE * fs)
{

  if (fs == NULL)
    return PROJECTM_FAILURE;

  init_cond_tree.splay_traverse( (void (*)(void*))write_init);

  return PROJECTM_SUCCESS;
}

void Preset::write_init( InitCond * init_cond )
{

  char s[512];
  int len;

  if (write_stream == NULL)
    return;

  memset(s, 0, 512);

  if (init_cond->param->type == P_TYPE_BOOL)
    sprintf(s, "%s=%d\n", init_cond->param->name, init_cond->init_val.bool_val);

  else if (init_cond->param->type == P_TYPE_INT)
    sprintf(s, "%s=%d\n", init_cond->param->name, init_cond->init_val.int_val);

  else if (init_cond->param->type == P_TYPE_DOUBLE)
    sprintf(s, "%s=%f\n", init_cond->param->name, init_cond->init_val.float_val);

else { printf("write_init: unknown parameter type!\n"); return; }

  len = strlen(s);

  if ((fwrite(s, 1, len, write_stream)) != len)
    printf("write_init: failed writing to file stream! Out of disk space?\n");

}


int Preset::write_per_frame_init_equations(FILE * fs)
{

  int len;

  if (fs == NULL)
    return PROJECTM_FAILURE;

  len = strlen(per_frame_init_eqn_string_buffer);

  if (fwrite(per_frame_init_eqn_string_buffer, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;
}


int Preset::write_per_frame_equations(FILE * fs)
{

  int len;

  if (fs == NULL)
    return PROJECTM_FAILURE;

  len = strlen(per_frame_eqn_string_buffer);

  if (fwrite(per_frame_eqn_string_buffer, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;
}


int Preset::write_per_pixel_equations(FILE * fs)
{

  int len;

  if (fs == NULL)
    return PROJECTM_FAILURE;

  len = strlen(per_pixel_eqn_string_buffer);

  if (fwrite(per_pixel_eqn_string_buffer, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;
}
#endif /** NEEDS_MOVED */

void Preset::load_custom_wave_init_conditions()
{
  
  for (PresetOutputs::cwave_container::iterator pos = customWaves->begin(); pos != customWaves->end(); ++pos)
    (*pos)->load_unspecified_init_conds();

}

void Preset::load_custom_shape_init_conditions()
{

  //     void eval_custom_shape_init_conds();

  for (PresetOutputs::cshape_container::iterator pos = customShapes->begin(); pos != customShapes->end(); ++pos)
    (*pos)->load_custom_shape_init();
}




void Preset::evaluateFrame()
{

  /* Evaluate all equation objects in same order as the renderer */

  evalInitConditions();
  evalPerFrameEquations();
  evalPerPixelEqns();
  evalInitConditions();
  evalCustomShapeInitConditions();
  evalCustomWavePerFrameEquations();
  evalCustomShapePerFrameEquations();

}

/** Evaluates all per-pixel equations */
void Preset::evalPerPixelEqns()
{

  /* Evaluate all per pixel equations using splay traversal */
  for (std::map<int, PerPixelEqn*>::iterator pos = per_pixel_eqn_tree.begin();
       pos != per_pixel_eqn_tree.end(); ++pos)
    pos->second->evaluate();


}

/** Finds / Creates (if necessary) initial condition associated with passed parameter */
InitCond * Preset::get_init_cond( Param *param )
{

  InitCond * init_cond;
  CValue init_val;

  assert(param);

  std::map<std::string, InitCond*>::iterator pos = init_cond_tree.find(param->name);

  init_cond = pos == init_cond_tree.end() ? 0 : pos->second;

  if (init_cond  == NULL)
  {

    if (param->type == P_TYPE_BOOL)
      init_val.bool_val = 0;

    else if (param->type == P_TYPE_INT)
      init_val.int_val = *(int*)param->engine_val;

    else if (param->type == P_TYPE_DOUBLE)
      init_val.float_val = *(float*)param->engine_val;

    /* Create new initial condition */
    if ((init_cond = new InitCond(param, init_val)) == NULL)
      return NULL;

    /* Insert the initial condition into this presets tree */
    std::pair<std::map<std::string, InitCond*>::iterator, bool> inserteePair =
      init_cond_tree.insert(std::make_pair(init_cond->param->name, init_cond));

    if (!inserteePair.second)
    {
      delete init_cond;
      return NULL;
    }
  }

  return init_cond;

}

/* load_preset_file: private function that loads a specific preset denoted
   by the given pathname */
int Preset::load_preset_file(const char * pathname)
{

  FILE * fs;
  int retval;
  int lineno;
  line_mode_t line_mode;

  if (pathname == NULL)
    return PROJECTM_FAILURE;

  /* Open the file corresponding to pathname */
  if ((fs = fopen(pathname, "rb")) == 0)
  {
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: loading of file %s failed!\n", pathname);
#endif
    return PROJECTM_ERROR;
  }

#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE( "load_preset_file: file stream \"%s\" opened successfully\n", pathname);
#endif

  /* Parse any comments */
  if (Parser::parse_top_comment(fs) < 0)
  {
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: no left bracket found...\n");
#endif
    fclose(fs);
    return PROJECTM_FAILURE;
  }

  /* Parse the preset name and a left bracket */
  char tmp_name[MAX_TOKEN_SIZE];

  if (Parser::parse_preset_name(fs, tmp_name) < 0)
  {
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: loading of preset name in file \"%s\" failed\n", pathname);
#endif
    fclose(fs);
    return PROJECTM_ERROR;
  }
  name = std::string(tmp_name);

#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE( "load_preset_file: preset \"%s\" parsed\n", this->name);
#endif

  /* Parse each line until end of file */
  lineno = 0;
#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE( "load_preset_file: beginning line parsing...\n");
#endif
  while ((retval = Parser::parse_line(fs, this)) != EOF)
  {
    if (retval == PROJECTM_PARSE_ERROR)
    {
      line_mode = NORMAL_LINE_MODE;
#if defined(PRESET_DEBUG) && defined(DEBUG)
      DWRITE( "load_preset_file: parse error in file \"%s\": line %d\n", pathname,lineno);
#endif

    }
    lineno++;
  }

#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE("load_preset_file: finished line parsing successfully\n");
#endif

  /* Now the preset has been loaded.
     Evaluation calls can be made at appropiate
     times in the frame loop */

  fclose(fs);
#if defined(PRESET_DEBUG) && defined(DEBUG)
  DWRITE("load_preset_file: file \"%s\" closed, preset ready\n", pathname);
#endif
  return PROJECTM_SUCCESS;
}


void Preset::load_init_conditions()
{

  ParamUtils::LoadInitCondFunctor functor(this);
  builtinParams.traverse(functor);

}



CustomWave * Preset::find_custom_wave(int id, bool create_flag)
{
  CustomWave * custom_wave = NULL;

  assert(customWaves);

  if ((custom_wave = (*customWaves)[id]) == NULL)
  {

    if (CUSTOM_WAVE_DEBUG) { printf("find_custom_wave: creating custom wave (id = %d)...", id);fflush(stdout);}

    if (create_flag == FALSE)
    {
      if (CUSTOM_WAVE_DEBUG) printf("you specified not to (create flag = false), returning null\n");
      return NULL;
    }

    if ((custom_wave = new CustomWave(id)) == NULL)
    {
      if (CUSTOM_WAVE_DEBUG) printf("failed...out of memory?\n");
      return NULL;
    }

    customWaves->push_back(custom_wave);
  }

  return custom_wave;

}


CustomShape * Preset::find_custom_shape(int id, bool create_flag)
{

  CustomShape * custom_shape = NULL;
  assert(customShapes);
  if ((custom_shape = (*customShapes)[id]) == NULL)
  {

    if (CUSTOM_SHAPE_DEBUG) { printf("find_custom_shape: creating custom shape (id = %d)...", id);fflush(stdout);}

    if (create_flag == FALSE)
    {
      if (CUSTOM_SHAPE_DEBUG) printf("you specified not to (create flag = false), returning null\n");
      return NULL;
    }

    if ((custom_shape = new CustomShape(id)) == NULL)
    {
      if (CUSTOM_SHAPE_DEBUG) printf("failed...out of memory?\n");
      return NULL;
    }

    customShapes->push_back(custom_shape);

  }

  return custom_shape;
}

/* Find a parameter given its name, will create one if not found */
Param * Preset::find(char * name, int flags)
{

  Param * param = NULL;

  /* Null argument checks */
  assert(name);

  /* First look in the builtin database */
  param = (Param *)this->builtinParams.find_builtin_param(name);

  /* If the search failed, check the user database */
  if (param == NULL)
  {
    std::map<std::string, Param*>::iterator pos = user_param_tree.find(name);

    if (pos == user_param_tree.end())
      param = 0;
    else
      param = pos->second;
  }

  /* If it doesn't exist in the user (or builtin) database and
  	  create_flag is set, then make it and insert into the database 
  */

  if ((param == NULL) && (flags & P_CREATE))
  {

    /* Check if string is valid */
    if (!Param::is_valid_param_string(name))
    {
      if (PARAM_DEBUG) printf("find: invalid parameter name:\"%s\"\n", name);
      return NULL;
    }
    /* Now, create the user defined parameter given the passed name */
    if ((param = new Param(name)) == NULL)
    {
      if (PARAM_DEBUG) printf("find: failed to create a new user parameter!\n");
      return NULL;
    }
    /* Finally, insert the new parameter into this preset's proper splaytree */
    std::pair<std::map<std::string, Param*>::iterator, bool> inserteePair
    = user_param_tree.insert(std::make_pair(param->name, param));

    if (!inserteePair.second)
    {
      if (PARAM_DEBUG) printf("PARAM \"%s\" already exists in user parameter tree!\n", param->name.c_str());
      delete param;
      return NULL;
    }

  }

  /* Return the found (or created) parameter. Note that if P_CREATE is not set, this could be null */
  return param;

}
