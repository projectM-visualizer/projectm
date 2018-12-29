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

#include "fatal.h"
#include "Common.hpp"

#include "CustomShape.hpp"
#include "Eval.hpp"
#include "Expr.hpp"
#include "InitCond.hpp"
#include "Param.hpp"
#include "Preset.hpp"
#include <map>
#include <iostream>
#include <cassert>

/** Constructor */
Param::Param( std::string _name, short int _type, short int _flags, void * _engine_val, void * _matrix,
              CValue _default_init_val, CValue _upper_bound, CValue _lower_bound):
	name(_name),
        type(_type),
        flags (_flags),
        matrix_flag (0),
        engine_val(_engine_val),
        matrix (_matrix),
        default_init_val (_default_init_val),
        upper_bound (_upper_bound),
        lower_bound (_lower_bound),
        local_value(0.0)
    {

}


/* Creates a user defined parameter */
Param::Param(std::string _name) :
	name(_name),
        type(P_TYPE_DOUBLE),
	flags(P_FLAG_USERDEF),
        matrix_flag(0),
        matrix(0),
      local_value(0.0)
        {

	engine_val = (float *)&local_value;

	default_init_val = CValue(DEFAULT_DOUBLE_IV);
        upper_bound= CValue(DEFAULT_DOUBLE_UB);
        lower_bound= CValue(DEFAULT_DOUBLE_LB);

    /// @note may have fixed a recent bug. testing
    *((float*)engine_val) = to_float(default_init_val.float_val());
 }

/* Free's a parameter type */
Param::~Param() {
    if (PARAM_DEBUG) printf("~Param: freeing \"%s\".\n", name.c_str());
}


/* Sets the parameter engine value to value val.
	clipping occurs if necessary */
void Param::set_param( double val) {

    switch (type) {

        case P_TYPE_BOOL:
            if (val < 0)
                *((bool*)engine_val) = false;
            else if (val > 0)
                *((bool*)engine_val) = true;
            else
                *((bool*)engine_val) = false;
            break;
        case P_TYPE_INT:
            /* Make sure value is an integer */
            val = floor(val);
            if (val < lower_bound.int_val())
                *((int*)engine_val) = lower_bound.int_val();
            else if (val > upper_bound.int_val())
                *((int*)engine_val) = upper_bound.int_val();
            else
                *((int*)engine_val) = (int)val;
            break;
        case P_TYPE_DOUBLE:
            /* Make sure value is an integer */


            if (val < lower_bound.float_val())
                *((float*)engine_val) = to_float(lower_bound.float_val());
            else if (val > upper_bound.float_val())
                *((float*)engine_val) = to_float(upper_bound.float_val());
            else
                *((float*)engine_val) = to_float(val);
            break;
        default:
            //abort();
            break;

    }

    return;
}


void Param::set_param( CValue val )
{
    assert( type == val.type || val.int_val() == 0 || val.int_val() == 1);

    switch (type) {

        case P_TYPE_BOOL:
            *((bool *)engine_val) = val.bool_val();
            break;
        case P_TYPE_INT:
            *((int *)engine_val) = val.int_val();
            break;
        case P_TYPE_DOUBLE:
            *((float *)engine_val) = to_float(val.float_val());
            break;
        default:
            //abort();
            break;
    }
}


void Param::set_param( const std::string &val )
{
    assert( type == P_TYPE_STRING );
    *((std::string*)engine_val) = val;
}


void ParamRGBA::set_param( double val )
{
    float r, g, b, a;
    unpackRGBA(val, r, g, b, a);
    if (nullptr != param_a)
    {
        param_a->set_param( a );
    }
    param_b->set_param( b );
    param_g->set_param( g );
    param_r->set_param( r );
}


void ParamRGBA::set_param( CValue val )
{
    set_param( val.float_val() );
}


/* Returns nonzero if the string is valid parameter name */
bool Param::is_valid_param_string( const char * string ) {

    if (string == NULL)
        return false;

    /* This ensures the first character is non numeric */
    if ( ((*string) >= 48) && ((*string) <= 57))
        return false;

    /* These probably should never happen */
    if (*string == '.')
        return false;

    if (*string == '+')
        return false;

    if (*string == '-')
        return false;

    /* Could also add checks for other symbols. May do later */

    return true;

}



/* Loads a float parameter into the builtin database */
Param * Param::new_param_float(const char * name, short int flags, void * engine_val, void * matrix,
                               float upper_bound, float lower_bound, float init_val) {

    Param * param;
    CValue iv, ub, lb;
    assert(engine_val);

    iv = CValue(init_val);
    ub = CValue(upper_bound);
    lb = CValue(lower_bound);

    if ((param = new Param(name, P_TYPE_DOUBLE, flags, engine_val, matrix,iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}

/* Creates a new parameter of type int */
Param * Param::new_param_int(const char * name, short int flags, void * engine_val,	
                             int upper_bound, int lower_bound, int init_val) {

    Param * param;
    CValue iv, ub, lb;
    assert(engine_val);

    iv = CValue(init_val);
    ub = CValue(upper_bound);
    lb = CValue(lower_bound);

    if ((param = new Param(name, P_TYPE_INT, flags, engine_val, NULL, iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}

/* Creates a new parameter of type bool */
Param * Param::new_param_bool(const char * name, short int flags, void * engine_val,
                              bool upper_bound, bool lower_bound, bool init_val) {

    Param * param;
    CValue iv, ub, lb;
    assert(engine_val);

    iv = CValue(init_val);
    ub = CValue(upper_bound);
    lb = CValue(lower_bound);

    if ((param = new Param(name, P_TYPE_BOOL, flags, engine_val, NULL, iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}

/* Creates a new parameter of type string */
Param * Param::new_param_string(const char * name, short int flags, void * engine_val) {

    Param * param;
    CValue iv, ub, lb;
    assert(engine_val);

    iv = CValue(false);
    ub = CValue(false);
    lb = CValue(false);

    if ((param = new Param(name, P_TYPE_STRING, flags, engine_val, NULL, iv, ub, lb)) == NULL)
        return NULL;


    /* Finished, return success */
    return param;
}


