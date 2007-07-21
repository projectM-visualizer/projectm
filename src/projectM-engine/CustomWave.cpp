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

#include "projectM.hpp"

#include "Common.hpp"
#include "fatal.h"

#include "CustomWave.hpp"
#include "Eval.hpp"
#include "Expr.hpp"
#include "InitCond.hpp"
#include "Param.hpp"
#include "PerFrameEqn.hpp"
#include "PerPointEqn.hpp"
#include "Preset.hpp"
#include <map>
#include "ParamUtils.hpp"
#include "InitCondUtils.hpp"
#include "wipemalloc.h"

#define MAX_SAMPLE_SIZE 4096


int interface_id = 0;

CustomWave::CustomWave(int id):id(id)
{
	
  Param * param;
  
/// @bug: should put these in member init list
  this->per_frame_count = 0;

  this->samples = 512;
  this->bSpectrum = 0;
  this->enabled = 1;
  this->sep = 1;
  this->smoothing = 0.0;
  this->bUseDots = 0;
  this->bAdditive = 0;
  this->r = this->g = this->b = this->a = 0.0;
  this->scaling = 1.0;
  this->per_frame_eqn_string_index = 0;
  this->per_frame_init_eqn_string_index = 0;
  this->per_point_eqn_string_index = 0;

  this->r_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->g_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->b_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->a_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->x_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->y_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->value1 = (float*) wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->value2 = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  this->sample_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));

  
  /* Start: Load custom wave parameters */

  if ((param = Param::new_param_float("r", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &this->r, this->r_mesh, 1.0, 0.0, .5)) == NULL) {
    delete(this);
    /// @bug make exception
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    /// @bug make exception
    abort();
  }
 
  if ((param = Param::new_param_float("g", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &this->g,  this->g_mesh, 1.0, 0.0, .5)) == NULL){
    delete(this);
/// @bug make exception
    abort();
    
  }

  if (ParamUtils::insert(param, param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();
  }

  if ((param = Param::new_param_float("b", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &this->b,  this->b_mesh, 1.0, 0.0, .5)) == NULL){
    delete(this);
/// @bug make exception
    abort();

  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_float("a", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &this->a,  this->a_mesh, 1.0, 0.0, .5)) == NULL){
    delete(this);
/// @bug make exception
    abort();


  }
  
  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_float("x", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &this->x,  this->x_mesh, 1.0, 0.0, .5)) == NULL) {
    delete(this);
/// @bug make exception
    abort();


  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_float("y", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &this->y,  this->y_mesh, 1.0, 0.0, .5)) == NULL) {
    delete(this);
/// @bug make exception
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);

/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_bool("enabled", P_FLAG_NONE, &this->enabled, 1, 0, 0)) == NULL) {
    delete(this);
/// @bug make exception
    abort();


  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);

/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_int("sep", P_FLAG_NONE, &this->sep, 100, -100, 0)) == NULL) {
    delete(this);
/// @bug make exception
    abort();


  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();


  }

  if ((param = Param::new_param_bool("bSpectrum", P_FLAG_NONE, &this->bSpectrum, 1, 0, 0)) == NULL) {
    delete(this);
/// @bug make exception
    abort();


  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_bool("bDrawThick", P_FLAG_NONE, &this->bDrawThick, 1, 0, 0)) == NULL) {
    delete(this);
/// @bug make exception
    abort();


  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
/// @bug make exception
    abort();

  }

  if ((param = Param::new_param_bool("bUseDots", P_FLAG_NONE, &this->bUseDots, 1, 0, 0)) == NULL) {
    delete(this);
/// @bug make exception
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
 
  if ((param = Param::new_param_bool("bAdditive", P_FLAG_NONE, &this->bAdditive, 1, 0, 0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_int("samples", P_FLAG_NONE, &this->samples, 2048, 1, 512)) == NULL) {
    delete(this);
    abort();
  }
 
  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("sample", P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX | P_FLAG_ALWAYS_MATRIX | P_FLAG_PER_POINT,
				&this->sample, this->sample_mesh, 1.0, 0.0, 0.0)) == NULL) {
    delete(this);
    abort();
  }
 
 if (ParamUtils::insert(param, this->param_tree) < 0) {
    printf("failed to insert sample\n");
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("value1", P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX | P_FLAG_ALWAYS_MATRIX | P_FLAG_PER_POINT, &this->v1, this->value1, 1.0, -1.0, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("value2", P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX | P_FLAG_ALWAYS_MATRIX | P_FLAG_PER_POINT, &this->v2, this->value2, 1.0, -1.0, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("smoothing", P_FLAG_NONE, &this->smoothing, NULL, 1.0, 0.0, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("scaling", P_FLAG_NONE, &this->scaling, NULL, MAX_DOUBLE_SIZE, 0.0, 1.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
 
  if ((param = Param::new_param_float("t1", P_FLAG_PER_POINT | P_FLAG_TVAR, &this->t1, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("t2",  P_FLAG_PER_POINT |P_FLAG_TVAR, &this->t2, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("t3",  P_FLAG_PER_POINT |P_FLAG_TVAR, &this->t3, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
  if ((param = Param::new_param_float("t4",  P_FLAG_PER_POINT |P_FLAG_TVAR, &this->t4, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
  if ((param = Param::new_param_float("t5", P_FLAG_TVAR, &this->t5, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }
 
  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
  if ((param = Param::new_param_float("t6", P_FLAG_TVAR | P_FLAG_PER_POINT, &this->t6, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
  if ((param = Param::new_param_float("t7", P_FLAG_TVAR | P_FLAG_PER_POINT, &this->t7, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }

  if ((param = Param::new_param_float("t8", P_FLAG_TVAR | P_FLAG_PER_POINT, &this->t8, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    delete(this);
    abort();
  }

  if (ParamUtils::insert(param, this->param_tree) < 0) {
    delete(this);
    abort();
  }
  
  /* End of parameter loading. Note that the read only parameters associated
     with custom waves (ie, sample) are variables stored in PresetFrameIO.hpp,
     and not specific to the custom wave datastructure. */

}

CustomWave::~CustomWave() {

  if (param_tree == NULL)
    return;

  
  for (std::map<int, PerPointEqn*>::iterator pos = per_point_eqn_tree.begin(); pos != per_point_eqn_tree.end(); ++pos)
  	delete(pos->second);

  for (std::map<int, PerFrameEqn*>::iterator pos = per_frame_eqn_tree.begin(); pos != per_frame_eqn_tree.end(); ++pos)
  	delete(pos->second);

  for (std::map<std::string, InitCond*>::iterator pos = init_cond_tree.begin(); pos != init_cond_tree.end(); ++pos)
  	delete(pos->second);

  for (std::map<std::string, InitCond*>::iterator pos = per_frame_init_eqn_tree.begin(); pos != per_frame_init_eqn_tree.end(); ++pos)
  	delete(pos->second);

  for (std::map<std::string, Param*>::iterator pos = param_tree->begin(); pos != param_tree->end(); ++pos)
  	delete(pos->second);


  free(r_mesh);
  free(g_mesh);
  free(b_mesh);
  free(a_mesh);
  free(x_mesh);
  free(y_mesh);
  free(value1);
  free(value2);
  free(sample_mesh);

  r_mesh = NULL;
  g_mesh = NULL;
  b_mesh = NULL;
  a_mesh = NULL;
  x_mesh = NULL;
  y_mesh = NULL;
  value1 = NULL;
  value2 = NULL;
  sample_mesh = NULL;


}





int CustomWave::add_per_point_eqn(char * name, GenExpr * gen_expr) {

  PerPointEqn * per_point_eqn;
  int index;
  Param * param = NULL;

  /* Argument checks */
  if (gen_expr == NULL)
	  return PROJECTM_FAILURE;
  if (name == NULL)
	  return PROJECTM_FAILURE;
  
 if (CUSTOM_WAVE_DEBUG) printf("add_per_point_eqn: per pixel equation (name = \"%s\")\n", name);

 /* Search for the parameter so we know what matrix the per pixel equation is referencing */

 if ((param = ParamUtils::find<ParamUtils::AUTO_CREATE>(name,param_tree)) == NULL) {
   if (CUSTOM_WAVE_DEBUG) printf("add_per_point_eqn: failed to allocate a new parameter!\n");
   return PROJECTM_FAILURE;
 
 } 	 

 /* Find most largest index in the splaytree */
 
 std::map<int, PerPointEqn*>::iterator pos = --per_point_eqn_tree.end();
 
 if (pos == per_point_eqn_tree.end())
   index = 0;
 else
   index = pos->second->index+1;

 /* Create the per pixel equation given the index, parameter, and general expression */
	
 if ((per_point_eqn = new PerPointEqn(index, param, gen_expr, samples)) == NULL)
	 return PROJECTM_FAILURE;
 if (CUSTOM_WAVE_DEBUG) 
   printf("add_per_point_eqn: created new equation (index = %d) (name = \"%s\")\n", per_point_eqn->index, per_point_eqn->param->name.c_str());

 /* Insert the per pixel equation into the preset per pixel database */
  
 per_point_eqn_tree.insert(std::make_pair(per_point_eqn->index, per_point_eqn)); 

 /* Done */ 
 return PROJECTM_SUCCESS;
}


void CustomWave::eval_custom_wave_init_conds() {

  for (std::map<std::string, InitCond*>::iterator pos = init_cond_tree.begin(); pos != init_cond_tree.end(); ++pos)
	pos->second->evaluate();

  for (std::map<std::string, InitCond*>::iterator pos = per_frame_init_eqn_tree.begin(); pos != per_frame_init_eqn_tree.end(); ++pos)
   	pos->second->evaluate();
}

/** Evaluate per-point equations */
void CustomWave::evalPerPointEqns() { 

  int x;

  for (x = 0; x < samples; x++)
    r_mesh[x] = r;
  for (x = 0; x < samples; x++)
    g_mesh[x] = g;
  for (x = 0; x < samples; x++)
    b_mesh[x] = b;
  for (x = 0; x < samples; x++)
    a_mesh[x] = a;
  for (x = 0; x < samples; x++)
    x_mesh[x] = x;
  for (x = 0; x < samples; x++)
    y_mesh[x] = y;

  /* Evaluate per pixel equations */
    for (std::map<int, PerPointEqn*>::iterator pos = per_point_eqn_tree.begin(); pos != per_point_eqn_tree.end();++pos)
	pos->second->evaluate();

  /* Reset index */
  projectM::currentEngine->mesh_i = -1;
}

void CustomWave::load_unspecified_init_conds() {

	InitCondUtils::LoadUnspecInitCond fun(this->init_cond_tree, this->per_frame_init_eqn_tree);
	Algorithms::traverse(*param_tree, fun);
}




