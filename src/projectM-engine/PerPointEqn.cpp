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

#include "CustomWave.hpp"
#include "Eval.hpp"
#include "Expr.hpp"
#include "Param.hpp"
#include "PerPixelEqn.hpp"
#include "PerPointEqn.hpp"
#include <map>

#include "wipemalloc.h"

/* Evaluates a per point equation for the current custom wave given by interface_wave ptr */
void PerPointEqn::evalPerPointEqn() {
  
  int samples, size;
  float * param_matrix;
  GenExpr * eqn_ptr;
abort();
//  samples = CustomWave::interface_wave->samples;
  eqn_ptr = gen_expr;
 
  if (param->matrix == NULL) {
    if ((param_matrix = (float*) (param->matrix = wipemalloc(size = samples*sizeof(float)))) == NULL)
      return;
    memset(param_matrix, 0, size);
  }
  else 
    param_matrix = (float*)param->matrix;
  
  for (projectM::currentEngine->mesh_i = 0; projectM::currentEngine->mesh_i < samples; projectM::currentEngine->mesh_i++) {    
      param_matrix[projectM::currentEngine->mesh_i] = eqn_ptr->eval_gen_expr();
  }
  
  /* Now that this parameter has been referenced with a per
     point equation, we let the evaluator know by setting
     this flag */
  param->matrix_flag = 1; 
}

PerPointEqn * PerPointEqn::new_per_point_eqn(int index, Param * param, GenExpr * gen_expr) {
    PerPointEqn * per_point_eqn = NULL;
    if (param == NULL)
        return NULL;
    if (gen_expr == NULL)
        return NULL;
    if ((per_point_eqn = (PerPointEqn*)wipemalloc(sizeof(PerPointEqn))) == NULL)
        return NULL;

     per_point_eqn->index = index;
     per_point_eqn->gen_expr = gen_expr;
     per_point_eqn->param = param;
     return per_point_eqn;
}


PerPointEqn::~PerPointEqn() {
    delete gen_expr;
  }
