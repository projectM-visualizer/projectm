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

/* Debug level, zero for none */
#define PARAM_DEBUG 0

#define P_CREATE 1
#define P_NONE 0

#define P_TYPE_BOOL 0
#define P_TYPE_INT 1
#define P_TYPE_DOUBLE 2
#define P_TYPE_STRING 3

#define P_FLAG_NONE 0
#define P_FLAG_READONLY 1
#define P_FLAG_USERDEF (1 << 1)
#define P_FLAG_QVAR (1 << 2)
#define P_FLAG_TVAR (1 << 3)
#define P_FLAG_ALWAYS_MATRIX (1 << 4)
#define P_FLAG_PER_PIXEL (1 << 6)
#define P_FLAG_PER_POINT (1 << 7)


#include "Expr.hpp"
#include "Common.hpp"
#include <cmath>
#include <string>
class InitCond;
class Param;
class Preset;
class Test;


#ifdef __SSE2__
#include <immintrin.h>
#endif

/* Parameter Type */
class Param : public LValue
{
protected:
    Param(const std::string &name, short int type, short int flags,
          void * eqn_val, void *matrix,
          CValue default_init_val, CValue upper_bound,
          CValue lower_bound);

    /// Create a user defined floating point parameter
    explicit Param( const std::string &name );

public:
    std::string name; /* name of the parameter, not necessary but useful neverthless */
    short int type; /* parameter number type (int, bool, or float) */
    short int flags; /* read, write, user defined, etc */
protected:
    short int matrix_flag; /* for optimization purposes */
    void * engine_val; /* pointer to the engine variable */
    void * matrix; /* per pixel / per point matrix for this variable */
public:
    CValue default_init_val; /* a default initial condition value */
protected:
    CValue upper_bound; /* this parameter's upper bound */
    CValue lower_bound; /* this parameter's lower bound */

    // for a local variable, engine_val can point here
    float local_value;

public:
    /// Create a new parameter
    static Param * create(const std::string &name, short int type, short int flags,
           void * eqn_val, void *matrix, CValue default_init_val, CValue upper_bound,
           CValue lower_bound);

    static Param * createUser(const std::string &name);

    static Test *test();

    virtual ~Param();

    static bool is_valid_param_string( const char *string );
    void set_param( float val );
    void set_param( CValue val );
    void set_param( std::string &text) { *((std::string*)engine_val) = text; }

    static Param *new_param_float( const char *name, short int flags, void *engine_val,
                             void *matrix, float upper_bound,
                             float lower_bound,
                             float init_val );

    static Param * new_param_int(const char * name, short int flags, void * engine_val,
                           int upper_bound, int lower_bound, int init_val );
    static Param * new_param_bool(const char * name, short int flags, void * engine_val,
                            bool upper_bound, bool lower_bound, bool init_val );
    static Param * new_param_string(const char * name, short int flags, void * engine_val);
#if HAVE_LLVM
    virtual llvm::Value *_llvm(JitContext &jit) = 0;
#endif
};


/* Sets the parameter engine value to value val.
	clipping occurs if necessary */
inline void Param::set_param( float val)
{
    matrix_flag = false;
    switch (type)
    {
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
	//abort();
        break;
    }
}

inline void Param::set_param( CValue val)
{
    matrix_flag = false;
    switch (type)
    {
    case P_TYPE_BOOL:
        set_param( val.bool_val );
        break;
    case P_TYPE_INT:
        set_param( val.int_val );
        break;
    case P_TYPE_DOUBLE:
        set_param( val.float_val );
        break;
    default:
        //abort();
        break;
    }
}

#endif /** !_PARAM_TYPES_H */

