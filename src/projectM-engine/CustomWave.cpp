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

#include "projectM.h"

#include "common.h"
#include "fatal.h"

#include "CustomWave.h"
#include "Eval.h"
#include "Expr.h"
#include "InitCond.h"
#include "Param.h"
#include "PerFrameEqn.h"
#include "PerPointEqn.h"
#include "Preset.hpp"
#include "SplayTree.h"

#include "wipemalloc.h"

#define MAX_SAMPLE_SIZE 4096


int interface_id = 0;

CustomWave * CustomWave::new_custom_wave(int id) {

  CustomWave * custom_wave;
  Param * param;
  
  if ((custom_wave = (CustomWave*)wipemalloc(sizeof(CustomWave))) == NULL)
    return NULL;

  custom_wave->id = id;
  custom_wave->per_frame_count = 0;

  custom_wave->samples = 512;
  custom_wave->bSpectrum = 0;
  custom_wave->enabled = 1;
  custom_wave->sep = 1;
  custom_wave->smoothing = 0.0;
  custom_wave->bUseDots = 0;
  custom_wave->bAdditive = 0;
  custom_wave->r = custom_wave->g = custom_wave->b = custom_wave->a = 0.0;
  custom_wave->scaling = 1.0;
  custom_wave->per_frame_eqn_string_index = 0;
  custom_wave->per_frame_init_eqn_string_index = 0;
  custom_wave->per_point_eqn_string_index = 0;

  custom_wave->r_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->g_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->b_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->a_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->x_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->y_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->value1 = (float*) wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->value2 = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));
  custom_wave->sample_mesh = (float*)wipemalloc(MAX_SAMPLE_SIZE*sizeof(float));

  /* Initialize tree data structures */
  
  if ((custom_wave->param_tree = 
       SplayTree::create_splaytree((int (*)(void*, void*))compare_string, (void* (*)(void*))copy_string, (void (*)(void*))free_string)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((custom_wave->per_point_eqn_tree = 
       SplayTree::create_splaytree((int (*)(void*, void*))compare_int, (void* (*)(void*))copy_int, (void (*)(void*))free_int)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((custom_wave->per_frame_eqn_tree = 
       SplayTree::create_splaytree((int (*)(void*, void*))compare_int,(void* (*)(void*)) copy_int,(void (*)(void*)) free_int)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((custom_wave->init_cond_tree = 
       SplayTree::create_splaytree((int (*)(void*, void*))compare_string, (void*(*)(void*))copy_string,(void (*)(void*)) free_string)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }
  
  if ((custom_wave->per_frame_init_eqn_tree = 
       SplayTree::create_splaytree((int (*)(void*, void*))compare_string, (void*(*)(void*))copy_string, (void (*)(void*))free_string)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  
  /* Start: Load custom wave parameters */

  if ((param = Param::new_param_float("r", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &custom_wave->r, custom_wave->r_mesh, 1.0, 0.0, .5)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
 
  if ((param = Param::new_param_float("g", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &custom_wave->g,  custom_wave->g_mesh, 1.0, 0.0, .5)) == NULL){
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("b", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &custom_wave->b,  custom_wave->b_mesh, 1.0, 0.0, .5)) == NULL){
    custom_wave->free_custom_wave();
    return NULL;				       
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("a", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &custom_wave->a,  custom_wave->a_mesh, 1.0, 0.0, .5)) == NULL){
    custom_wave->free_custom_wave();
    return NULL;
  }
  
  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("x", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &custom_wave->x,  custom_wave->x_mesh, 1.0, 0.0, .5)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("y", P_FLAG_DONT_FREE_MATRIX | P_FLAG_PER_POINT, &custom_wave->y,  custom_wave->y_mesh, 1.0, 0.0, .5)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_bool("enabled", P_FLAG_NONE, &custom_wave->enabled, 1, 0, 0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_int("sep", P_FLAG_NONE, &custom_wave->sep, 100, -100, 0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_bool("bSpectrum", P_FLAG_NONE, &custom_wave->bSpectrum, 1, 0, 0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_bool("bDrawThick", P_FLAG_NONE, &custom_wave->bDrawThick, 1, 0, 0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_bool("bUseDots", P_FLAG_NONE, &custom_wave->bUseDots, 1, 0, 0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
 
  if ((param = Param::new_param_bool("bAdditive", P_FLAG_NONE, &custom_wave->bAdditive, 1, 0, 0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_int("samples", P_FLAG_NONE, &custom_wave->samples, 2048, 1, 512)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }
 
  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("sample", P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX | P_FLAG_ALWAYS_MATRIX | P_FLAG_PER_POINT,
				&custom_wave->sample, custom_wave->sample_mesh, 1.0, 0.0, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }
 
 if (custom_wave->param_tree->insert_param(param) < 0) {
    printf("failed to insert sample\n");
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("value1", P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX | P_FLAG_ALWAYS_MATRIX | P_FLAG_PER_POINT, &custom_wave->v1, custom_wave->value1, 1.0, -1.0, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("value2", P_FLAG_READONLY | P_FLAG_DONT_FREE_MATRIX | P_FLAG_ALWAYS_MATRIX | P_FLAG_PER_POINT, &custom_wave->v2, custom_wave->value2, 1.0, -1.0, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("smoothing", P_FLAG_NONE, &custom_wave->smoothing, NULL, 1.0, 0.0, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("scaling", P_FLAG_NONE, &custom_wave->scaling, NULL, MAX_DOUBLE_SIZE, 0.0, 1.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
 
  if ((param = Param::new_param_float("t1", P_FLAG_PER_POINT | P_FLAG_TVAR, &custom_wave->t1, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("t2",  P_FLAG_PER_POINT |P_FLAG_TVAR, &custom_wave->t2, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("t3",  P_FLAG_PER_POINT |P_FLAG_TVAR, &custom_wave->t3, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
  if ((param = Param::new_param_float("t4",  P_FLAG_PER_POINT |P_FLAG_TVAR, &custom_wave->t4, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
  if ((param = Param::new_param_float("t5", P_FLAG_TVAR, &custom_wave->t5, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }
 
  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
  if ((param = Param::new_param_float("t6", P_FLAG_TVAR | P_FLAG_PER_POINT, &custom_wave->t6, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
  if ((param = Param::new_param_float("t7", P_FLAG_TVAR | P_FLAG_PER_POINT, &custom_wave->t7, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if ((param = Param::new_param_float("t8", P_FLAG_TVAR | P_FLAG_PER_POINT, &custom_wave->t8, NULL, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0)) == NULL) {
    custom_wave->free_custom_wave();
    return NULL;
  }

  if (custom_wave->param_tree->insert_param(param) < 0) {
    custom_wave->free_custom_wave();
    return NULL;
  }
  
  /* End of parameter loading. Note that the read only parameters associated
     with custom waves (ie, sample) are global variables, and not specific to 
     the custom wave datastructure. */


  return custom_wave;

}

void CustomWave::destroy_per_frame_init_eqn_tree() {

  if (!per_frame_init_eqn_tree)
    return;

  per_frame_init_eqn_tree->splay_traverse((void (*)(void*))free_init_cond_helper);
  delete per_frame_init_eqn_tree;

}


void CustomWave::destroy_per_point_eqn_tree(SplayTree * tree) {

  if (!tree)
    return;

  tree->splay_traverse((void (*)(void*))free_per_point_eqn_helper);
  delete tree;

}

void CustomWave::destroy_init_cond_tree(SplayTree * tree) {

  if (!tree)
    return;

  tree->splay_traverse((void (*)(void*))free_init_cond_helper);
  delete tree;

}

void CustomWave::destroy_per_frame_eqn_tree(SplayTree * tree) {


  if (!tree)
    return;

  tree->splay_traverse((void (*)(void*))free_per_frame_eqn_helper);
  delete tree;

}


void CustomWave::destroy_param_db_tree(SplayTree * tree) {

  if (!tree)
    return;

  tree->splay_traverse((void (*)(void*))free_param_helper);
  delete tree;

}

/* Frees a custom wave form object */
void CustomWave::free_custom_wave() {

  if (param_tree == NULL)
    return;

  destroy_per_point_eqn_tree(per_point_eqn_tree);
  destroy_per_frame_eqn_tree(per_frame_eqn_tree);
  destroy_init_cond_tree(init_cond_tree);
  destroy_param_db_tree(param_tree);
  destroy_per_frame_init_eqn_tree();

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

//  free(custom_wave);
//  custom_wave = NULL;

  return;

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

 if ((param = param_tree->find_param_db(name, TRUE)) == NULL) {
   if (CUSTOM_WAVE_DEBUG) printf("add_per_point_eqn: failed to allocate a new parameter!\n");
   return PROJECTM_FAILURE;
 
 } 	 

 /* Find most largest index in the splaytree */
 if ((per_point_eqn = (PerPointEqn*)per_point_eqn_tree->splay_find_max()) == NULL)
   index = 0;
 else
   index = per_point_eqn->index+1;

 /* Create the per pixel equation given the index, parameter, and general expression */
 if ((per_point_eqn = PerPointEqn::new_per_point_eqn(index, param, gen_expr)) == NULL)
	 return PROJECTM_FAILURE;
 if (CUSTOM_WAVE_DEBUG) 
   printf("add_per_point_eqn: created new equation (index = %d) (name = \"%s\")\n", per_point_eqn->index, per_point_eqn->param->name);
 /* Insert the per pixel equation into the preset per pixel database */
 if (per_point_eqn_tree->splay_insert(per_point_eqn, &per_point_eqn->index) < 0) {
	delete per_point_eqn;
	return PROJECTM_FAILURE;	 
 }
	 
 /* Done */ 
 return PROJECTM_SUCCESS;
}

CustomWave * CustomWave::find_custom_wave(int id, Preset * preset, int create_flag) {

  CustomWave * custom_wave = NULL;

  if (preset == NULL)
    return NULL;
  
  if ((custom_wave = (CustomWave*)preset->custom_wave_tree->splay_find(&id)) == NULL) {

    if (CUSTOM_WAVE_DEBUG) { printf("find_custom_wave: creating custom wave (id = %d)...", id);fflush(stdout);}

    if (create_flag == FALSE) {
      if (CUSTOM_WAVE_DEBUG) printf("you specified not to (create flag = false), returning null\n");
       return NULL;
    }

    if ((custom_wave = new_custom_wave(id)) == NULL) {
      if (CUSTOM_WAVE_DEBUG) printf("failed...out of memory?\n");
      return NULL;
    }

    if  (CUSTOM_WAVE_DEBUG) {printf("success.Inserting..."); fflush(stdout);}

   if (preset->custom_wave_tree->splay_insert(custom_wave, &custom_wave->id) < 0) {
     if (CUSTOM_WAVE_DEBUG) printf("failed!\n");
     custom_wave->free_custom_wave();
     return NULL;
    }
 
   if (CUSTOM_WAVE_DEBUG) printf("done.\n");
  }
 
  return custom_wave;
}

void CustomWave::eval_custom_wave_init_conds() {
  init_cond_tree->splay_traverse((void (*)(void*))eval_init_cond_helper );
  per_frame_init_eqn_tree->splay_traverse((void (*)(void*))eval_init_cond_helper );
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
    abort(); 
    per_point_eqn_tree->splay_traverse((void (*)(void*))eval_per_point_eqn_helper); 


  /* Reset index */
  projectM::currentEngine->mesh_i = -1;
}

void CustomWave::load_unspecified_init_conds_wave() {
//    interface_wave = this;
    param_tree->splay_traverse((void (*)(void*))load_unspec_init_cond_helper);
//    interface_wave = NULL;
  }


void CustomWave::load_unspec_init_cond() {
abort();
#if 0
    InitCond * init_cond;
    CValue init_val;

    /* Don't count these parameters as initial conditions */
    if (flags & P_FLAG_READONLY)
        return;
    if (flags & P_FLAG_QVAR)
        return;
    if (flags & P_FLAG_TVAR)
        return;
    if (flags & P_FLAG_USERDEF)
        return;

    /* If initial condition was not defined by the preset file, force a default one
       with the following code */
    if ((init_cond = (InitCond*)init_cond_tree->splay_find(name)) == NULL) {

        /* Make sure initial condition does not exist in the set of per frame initial equations */
        if ((init_cond = (InitCond*)per_frame_init_eqn_tree->splay_find(name)) != NULL)
            return;

        if (type == P_TYPE_BOOL)
            init_val.bool_val = 0;

        else if (type == P_TYPE_INT)
            init_val.int_val = *(int*)engine_val;

        else if (type == P_TYPE_DOUBLE)
            init_val.float_val = *(float*)engine_val;

        //printf("%s\n", param->name);
        /* Create new initial condition */
        if ((init_cond = new InitCond(this, init_val)) == NULL)
            return;

        /* Insert the initial condition into this presets tree */
        if (init_cond_tree->splay_insert(init_cond, init_cond->param->name) < 0) {
            delete init_cond;
            return;
        }

    }
#endif
}
