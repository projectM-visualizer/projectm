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

/* Basic Parameter Functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "projectM.h"

#include "fatal.h"
#include "common.h"

#include "CustomShape.h"
#include "Eval.h"
#include "Expr.h"
#include "InitCond.h"
#include "Param.h"
#include "Preset.hpp"
#include "SplayTree.h"

#include "wipemalloc.h"

/** Constructor */
Param::Param( char * name, short int type, short int flags, void * engine_val, void * matrix,
              CValue default_init_val, CValue upper_bound, CValue lower_bound):
        type(type),
        flags (flags),
        matrix_flag (0),
        matrix (matrix),
        engine_val( engine_val),
        default_init_val (default_init_val),
        upper_bound (upper_bound),
        lower_bound (lower_bound)
    {

    strncpy(this->name, name, MAX_TOKEN_SIZE-1);

    /** @@FIX THIS */
    //this->gx = projectM::currentEngine->gx;
    //this->gy = projectM::currentEngine->gy;
}


/* Creates a user defined parameter */
Param::Param(char * name) :
        type(P_TYPE_DOUBLE),
        flags(P_FLAG_USERDEF),
        matrix_flag(0),
        matrix(0)
        {

	engine_val = new float();

	default_init_val.float_val = DEFAULT_DOUBLE_IV;
        upper_bound.float_val = DEFAULT_DOUBLE_UB;
        lower_bound.float_val = DEFAULT_DOUBLE_LB;

    /* Copy given name into parameter structure */
    strncpy(this->name, name, MAX_TOKEN_SIZE-1);

    (*(float*)this->engine_val) = DEFAULT_DOUBLE_IV;


    /** @@FIX THIS */
    //this->gx = projectM::currentEngine->gx;
    //this->gy = projectM::currentEngine->gy;
}

/* Free's a parameter type */
Param::~Param() {

    int x;
    if (flags & P_FLAG_USERDEF) {
        free(engine_val);
    }

    //if (!(param->flags & P_FLAG_DONT_FREE_MATRIX)) {
    if (matrix && !(flags & P_FLAG_DONT_FREE_MATRIX)) {

        if (flags & P_FLAG_PER_POINT) {
            free(matrix);
        } //FIX THIS NOW XMAS05
        else if (flags & P_FLAG_PER_PIXEL) {
            for (x = 0; x < gx; x++)
                free(((float**)matrix)[x]);
            free(matrix);
            matrix = NULL;
        }
    }

    if (PARAM_DEBUG) printf("free_param: freeing \"%s\".\n", name);
}

/* Compare string name with parameter name */
int Param::compare_param( char * name, char * name2 ) {

    int cmpval;
    printf("am i used\n");
    /* Uses string comparison function */
    cmpval = strncmp(name, name2, MAX_TOKEN_SIZE-1);

    return cmpval;
}

/* Returns nonzero if the string is valid parameter name */
int Param::is_valid_param_string( char * string ) {

    if (string == NULL)
        return FALSE;

    /* This ensures the first character is non numeric */
    if ( ((*string) >= 48) && ((*string) <= 57))
        return FALSE;

    /* These probably should never happen */
    if (*string == '.')
        return FALSE;

    if (*string == '+')
        return FALSE;

    if (*string == '-')
        return FALSE;

    /* Could also add checks for other symbols. May do later */

    return TRUE;

}

/* Sets the parameter engine value to value val.
	clipping occurs if necessary */
void Param::set_param( float val) {

    switch (type) {

    case P_TYPE_BOOL:
        if (val < 0)
            *((int*)engine_val) = 0;
        else if (val > 0)
            *((int*)engine_val) = 1;
        else
            *((int*)engine_val) = 0;
        break;
    case P_TYPE_INT:
        /* Make sure value is an integer */
        val = floor(val);
        if (val < lower_bound.int_val)
            *((int*)engine_val) = lower_bound.int_val;
        else if (val > upper_bound.int_val)
            *((int*)engine_val) = upper_bound.int_val;
        else
            *((int*)engine_val) = (int)val;
        break;
    case P_TYPE_DOUBLE:
        /* Make sure value is an integer */


        if (val < lower_bound.float_val)
            *((float*)engine_val) = lower_bound.float_val;
        else if (val > upper_bound.float_val)
            *((float*)engine_val) = upper_bound.float_val;
        else
            *((float*)engine_val) = val;
        break;
    default:
        break;

    }

    return;
}


/* Loads a float parameter into the builtin database */
Param * Param::new_param_float(char * name, short int flags, void * engine_val, void * matrix,
                               float upper_bound, float lower_bound, float init_val) {

    Param * param;
    CValue iv, ub, lb;

    iv.float_val = init_val;
    ub.float_val = upper_bound;
    lb.float_val = lower_bound;

    if ((param = new Param(name, P_TYPE_DOUBLE, flags, engine_val, matrix,iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}


/* Creates a new parameter of type int */
Param * Param::new_param_int(char * name, short int flags, void * engine_val,
                             int upper_bound, int lower_bound, int init_val) {

    Param * param;
    CValue iv, ub, lb;

    iv.int_val = init_val;
    ub.int_val = upper_bound;
    lb.int_val = lower_bound;

    if ((param = new Param(name, P_TYPE_INT, flags, engine_val, NULL, iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}

/* Creates a new parameter of type bool */
Param * Param::new_param_bool(char * name, short int flags, void * engine_val,
                              int upper_bound, int lower_bound, int init_val) {

    Param * param;
    CValue iv, ub, lb;

    iv.bool_val = init_val;
    ub.bool_val = upper_bound;
    lb.bool_val = lower_bound;

    if ((param = new Param(name, P_TYPE_BOOL, flags, engine_val, NULL, iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}
	


/// @bug this is fucked
void Param::load_init_cond() {

    // bullshit
    int type;
    void *engine_val;
    InitCond * init_cond;
    CValue init_val;

    /* Don't count read only parameters as initial conditions */
    if (flags & P_FLAG_READONLY)
        return;

    /* If initial condition was not defined by the preset file, force a default one
       with the following code */
abort();

//    if ((init_cond = (InitCond*)(init_cond_tree->splay_find(name))) == NULL) {

        /* Make sure initial condition does not exist in the set of per frame initial equations */
//        if ((init_cond = (InitCond*)(per_frame_init_eqn_tree->splay_find(name))) != NULL)
            return;

        if (type == P_TYPE_BOOL)
            init_val.bool_val = 0;

        else if (type == P_TYPE_INT)
            init_val.int_val = *(int*)engine_val;

        else if (type == P_TYPE_DOUBLE)
            init_val.float_val = *(float*)engine_val;

        /* Create new initial condition */
        if ((init_cond = new InitCond(this, init_val)) == NULL)
            return;

        /* Insert the initial condition into this presets tree */
//        if (this->init_cond_tree->splay_insert(init_cond, init_cond->param->name) < 0) {
            delete init_cond;
            return;
  //      }

//    }

}
