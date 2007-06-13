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
#include <stdlib.h>
#include <string.h>

#include "fatal.h"
#include "common.h"

#include "Param.h"
#include "PerFrameEqn.h"

#include "Eval.h"
#include "Expr.h"

#include "wipemalloc.h"

/* Evaluate an equation */
void PerFrameEqn::eval_per_frame_eqn() {

     if (PER_FRAME_EQN_DEBUG) { 
		 printf("per_frame_%d=%s= ", index, param->name);
		 fflush(stdout);
	 }
	 
    //*((float*)per_frame_eqn->param->engine_val) = eval_gen_expr(per_frame_eqn->gen_expr);
	param->set_param(gen_expr->eval_gen_expr());
     if (PER_FRAME_EQN_DEBUG) printf(" = %.4f\n", *((float*)param->engine_val)); 
		 
}

/*
void eval_per_frame_init_eqn(PerFrameEqn * per_frame_eqn) {

   float val;
   init_cond_t * init_cond;
   if (per_frame_eqn == NULL)
     return;

     if (PER_FRAME_EQN_DEBUG) { 
		 printf("per_frame_init: %s = ", per_frame_eqn->param->name);
		 fflush(stdout);
	 }
	 		
	
    val = *((float*)per_frame_eqn->param->engine_val) = eval_gen_expr(per_frame_eqn->gen_expr);
    if (PER_FRAME_EQN_DEBUG) printf(" = %f\n", *((float*)per_frame_eqn->param->engine_val)); 
     
	if (per_frame_eqn->param->flags & P_FLAG_QVAR) {
		
		per_frame_eqn->param->init_val.float_val = val;
		if ((init_cond = new_init_cond(per_frame_eqn->param)) == NULL)
			return;
		
		if ((list_append(init_cond_list, init_cond)) < 0) {
			free_init_cond(init_cond);
			return;
		}
    }
}
*/

/* Frees perframe equation structure */
PerFrameEqn::~PerFrameEqn() {

  delete gen_expr;
  /** Destroy param? */
}

/* Create a new per frame equation */
PerFrameEqn * PerFrameEqn::new_per_frame_eqn(int index, Param * param, GenExpr * gen_expr) {

  PerFrameEqn * per_frame_eqn;

  per_frame_eqn = (PerFrameEqn*)wipemalloc(sizeof(PerFrameEqn));

  if (per_frame_eqn == NULL)
    return NULL;

  per_frame_eqn->param = param;
  per_frame_eqn->gen_expr = gen_expr;
  per_frame_eqn->index = index;
  /* Set per frame eqn name */
  //  memset(per_frame_eqn->name, 0, MAX_TOKEN_SIZE);
  //strncpy(per_frame_eqn->name, name, MAX_TOKEN_SIZE-1);
  return per_frame_eqn;

}
