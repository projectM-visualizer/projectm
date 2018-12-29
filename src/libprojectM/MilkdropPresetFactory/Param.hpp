/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
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
/**
 * $Id$
 *
 * Parameter used within a preset
 *
 * $Log$
 */

#ifndef _PARAM_H
#define _PARAM_H

#include "ParamDef.hpp"
#include "Expr.hpp"
#include "Common.hpp"
#include <cmath>
#include <string>
class InitCond;
class Param;
class Preset;

#ifdef __SSE2__
#include <immintrin.h>
#endif

/* Parameter Type */
class Param {
public:
    std::string name; /* name of the parameter, not necessary but useful neverthless */
    short int type; /* parameter number type (int, bool, or float) */
    short int flags; /* read, write, user defined, etc */
    short int matrix_flag; /* for optimization purposes */
private:
    void * engine_val; /* pointer to the engine variable */
public:
    void * matrix; /* per pixel / per point matrix for this variable */
    CValue default_init_val; /* a default initial condition value */
    CValue upper_bound; /* this parameter's upper bound */
    CValue lower_bound; /* this parameter's lower bound */

    // for a local variable, engine_val can point here
    float local_value;

    /// Create a new parameter
    Param(std::string name, short int type, short int flags,
           void * eqn_val, void *matrix,
           CValue default_init_val, CValue upper_bound,
           CValue lower_bound);
virtual
    ~Param();

    /// Create a user defined floating point parameter
    explicit Param( std::string name );

    virtual double eval()
    {
        if ( type == P_TYPE_DOUBLE )
            return *(float *)engine_val;
        else if ( type == P_TYPE_INT )
            return (float)*(int *)engine_val;
        else if ( type == P_TYPE_BOOL )
            return (float)*(bool *)engine_val;
        assert(false);
    }

    static bool is_valid_param_string( const char *string );
    virtual void set_param( const std::string &val ); // does not convert
    virtual void set_param( double val );
    virtual void set_param( CValue val );

    static Param *new_param_float( const char *name, short int flags, void *engine_val,
                             void *matrix, float upper_bound,
                             float lower_bound,
                             float init_val );
    static Param * new_param_int(const char * name, short int flags, void * engine_val,
                           int upper_bound, int lower_bound, int init_val );
    static Param * new_param_bool(const char * name, short int flags, void * engine_val,
                            bool upper_bound, bool lower_bound, bool init_val );
    static Param * new_param_string(const char * name, short int flags, void * engine_val);

};


class ParamRGBA : public Param
{
public:
    ParamRGBA(std::string name_, Param *r, Param *g, Param *b, Param *a):
            Param(name_, P_TYPE_DOUBLE, P_FLAG_RGB, nullptr, nullptr, 0, CValue(0x00ffffff), CValue(0)),
            param_r(r), param_g(g), param_b(b), param_a(a)
    {
    }

    virtual void set_param( double val ) override;
    virtual void set_param( CValue val ) override;

private:
    Param *param_r, *param_g, *param_b, *param_a;
};

#endif /** !_PARAM_TYPES_H */

