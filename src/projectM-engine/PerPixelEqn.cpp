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

#include "fatal.h"
#include "Common.hpp"

#include "Expr.hpp"
#include "Eval.hpp"
#include "Param.hpp"
#include "PerPixelEqn.hpp"
#include "SplayTree.hpp"

#include "wipemalloc.h"

/* Evaluates a per pixel equation */
void PerPixelEqn::evalPerPixelEqn() {

  float ** param_matrix = NULL;
  GenExpr * eqn_ptr = NULL;
  int x,y;

  eqn_ptr = gen_expr; 
 if (param->matrix == NULL) {
    if (PER_PIXEL_EQN_DEBUG) printf("evalPerPixelEqn: [begin initializing matrix] (index = %d) (name = %s)\n", 
  			  index, param->name);
    
    param_matrix = (float**)wipemalloc(param->gx*sizeof(float*));
    param->matrix = param_matrix;

    for(x = 0; x < param->gx; x++)
      param_matrix[x] = (float *)wipemalloc(param->gy * sizeof(float));

    for (x = 0; x < param->gx; x++)
      for (y = 0; y < param->gy; y++)
	    param_matrix[x][y] = 0.0;

    if (param->name == NULL)
      printf("null parameter?\n");

    //    printf("PARAM MATRIX: \"%s\" initialized.\n", per_pixel_eqn->param->name);
  }
  else 
    param_matrix = (float**)param->matrix;
 
  if (eqn_ptr == NULL || param_matrix == NULL )
    printf("something is seriously wrong...\n");

//    param->matrix_flag = 0;         /** Force matrix ignore to update time */
  for (projectM::currentEngine->mesh_i = 0; projectM::currentEngine->mesh_i < param->gx; projectM::currentEngine->mesh_i++) {    
    for (projectM::currentEngine->mesh_j = 0; projectM::currentEngine->mesh_j < param->gy; projectM::currentEngine->mesh_j++) {     
      param_matrix[projectM::currentEngine->mesh_i][projectM::currentEngine->mesh_j] = eqn_ptr->eval_gen_expr();
    }
  }
  
  /* Now that this parameter has been referenced with a per
     pixel equation, we let the evaluator know by setting
     this flag */
  param->matrix_flag = 1; 
   param->flags |= P_FLAG_PER_PIXEL;
}

PerPixelEqn *PerPixelEqn::new_per_pixel_eqn( int index, Param * param, 
                                             GenExpr * gen_expr) {

	PerPixelEqn * per_pixel_eqn;
	
	if (index < 0)
	  return NULL;
	if (param == NULL)
	  return NULL;
	if (gen_expr == NULL)
	  return NULL;
	
	if ((per_pixel_eqn = (PerPixelEqn*)wipemalloc(sizeof(PerPixelEqn))) == NULL)
	  return NULL;
	
	per_pixel_eqn->index = index;
	per_pixel_eqn->param = param;
	per_pixel_eqn->gen_expr = gen_expr;
	
	return per_pixel_eqn;	
}

void PerPixelEqn::free_per_pixel_eqn() {

	delete gen_expr;
	
	return;
}
