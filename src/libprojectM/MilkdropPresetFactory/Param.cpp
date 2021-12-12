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

#include "Common.hpp"
#include "Expr.hpp"
#include "Param.hpp"

#include <cassert>
#include <cstdio>
#include <map>
#include <stdexcept>

/** Constructor */
Param::Param( const std::string &_name, short int _type, short int _flags, void * _engine_val, void * _matrix,
              CValue _default_init_val, CValue _upper_bound, CValue _lower_bound):
        LValue(PARAMETER),
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
Param::Param(const std::string &_name) : LValue(PARAMETER),
        name(_name),
        type(P_TYPE_DOUBLE),
        flags(P_FLAG_USERDEF),
        matrix_flag(0),
        matrix(0),
        local_value(0.0)
{
        engine_val = (float *)&local_value;

        default_init_val.float_val = DEFAULT_DOUBLE_IV;
        upper_bound.float_val = DEFAULT_DOUBLE_UB;
        lower_bound.float_val = DEFAULT_DOUBLE_LB;

    /// @note may have fixed a recent bug. testing
    *((float*)engine_val) = default_init_val.float_val;
 }

/* Free's a parameter type */
Param::~Param() {
    if (PARAM_DEBUG) printf("~Param: freeing \"%s\".\n", name.c_str());
}


/* Returns nonzero if the string is valid parameter name */
bool Param::is_valid_param_string(const char* string) {

    if (!string)
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
Param* Param::new_param_float(const char* name, short int flags, void* engine_val, void* matrix,
                              float upper_bound, float lower_bound, float init_val)
{
    assert(engine_val);

    CValue iv;
    CValue ub;
    CValue lb;

    iv.float_val = init_val;
    ub.float_val = upper_bound;
    lb.float_val = lower_bound;

    return Param::create(name, P_TYPE_DOUBLE, flags, engine_val, matrix,iv, ub, lb);
}

/* Creates a new parameter of type int */
Param* Param::new_param_int(const char* name, short int flags, void* engine_val,
                            int upper_bound, int lower_bound, int init_val)
{
    assert(engine_val);

    CValue iv;
    CValue ub;
    CValue lb;

    iv.int_val = init_val;
    ub.int_val = upper_bound;
    lb.int_val = lower_bound;

    return Param::create(name, P_TYPE_INT, flags, engine_val, nullptr, iv, ub, lb);
}

/* Creates a new parameter of type bool */
Param* Param::new_param_bool(const char* name, short int flags, void* engine_val,
                             bool upper_bound, bool lower_bound, bool init_val)
{
    assert(engine_val);

    CValue iv;
    CValue ub;
    CValue lb;

    iv.bool_val = init_val;
    ub.bool_val = upper_bound;
    lb.bool_val = lower_bound;

    return Param::create(name, P_TYPE_BOOL, flags, engine_val, nullptr, iv, ub, lb);
}

/* Creates a new parameter of type string */
Param* Param::new_param_string(const char* name, short int flags, void* engine_val)
{
    assert(engine_val);

    CValue iv;
    CValue ub;
    CValue lb;

    iv.bool_val = false;
    ub.bool_val = false;
    lb.bool_val = false;

    return Param::create(name, P_TYPE_STRING, flags, engine_val, nullptr, iv, ub, lb);
}


/*
 * I made Param extend Expr just to avoid inheritence of multiple virtual classes,
 * however, this is really the subclass that knows how to interact with Expr
 */

struct _Param : public Param
{
    _Param( const std::string &name_, short int type_, short int flags_,
                void * eqn_val_, void *matrix_,
                CValue default_init_val_, CValue upper_bound_,
                CValue lower_bound_) :
            Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_)
    {
        if (flags & P_FLAG_ALWAYS_MATRIX)
            matrix_flag = true;
    }
    explicit _Param( const std::string &name_) : Param(name_) {}

    void _delete_from_tree() override
    {
        /* do nothing, as the param isn't owned by the expresion tree */
    }

    void set(float value) override
    {
        set_param(value);
    }
    void set_matrix(int mesh_i, int mesh_j, float value) override
    {
        set_param(value);
    }
#if HAVE_LLVM
    llvm::Value *_llvm(JitContext &jit) override
    {
        return Expr::generate_eval_call(jit, this, name.c_str());
    }
#endif
};


class _BoolParam : public _Param
{
public:
    _BoolParam( const std::string &name_, short int type_, short int flags_,
                void * eqn_val_, void *matrix_,
                CValue default_init_val_, CValue upper_bound_,
                CValue lower_bound_) :
            _Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_) {}

    float eval(int mesh_i, int mesh_j) override
    {
        return *(bool *)engine_val ? 1 : 0;
    }
};

class _IntParam : public _Param
{
public:
    _IntParam( const std::string &name_, short int type_, short int flags_,
                void * eqn_val_, void *matrix_,
                CValue default_init_val_, CValue upper_bound_,
                CValue lower_bound_) :
            _Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_) {}
    float eval(int mesh_i, int mesh_j) override
    {
        return *(int *)engine_val;
    }
};

class _StringParam : public _Param
{
public:
    _StringParam( const std::string &name_, short int type_, short int flags_,
               void * eqn_val_, void *matrix_,
               CValue default_init_val_, CValue upper_bound_,
               CValue lower_bound_) :
            _Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_) {}
    float eval(int mesh_i, int mesh_j) override
    {
        return 0;
    }
};

class _FloatParam : public _Param
{
public:
    _FloatParam( const std::string &name_, short int type_, short int flags_,
               void * eqn_val_, void *matrix_,
               CValue default_init_val_, CValue upper_bound_,
               CValue lower_bound_) :
            _Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_) {}
    explicit _FloatParam( const std::string &name_) : _Param(name_) {}
    float eval(int mesh_i, int mesh_j) override
    {
        return *(float *)engine_val;
    }
#if HAVE_LLVM
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Constant *ptr = jitx.CreateFloatPtr((float *)engine_val);
        return jitx.builder.CreateLoad(ptr, name);
    }
    virtual llvm::Value *_llvm_set_matrix(JitContext &jitx, llvm::Value *rhs)
    {
        llvm::Constant *ptr = jitx.CreateFloatPtr((float *)engine_val);
        jitx.builder.CreateStore(rhs, ptr, false);
        return rhs;
    }
#endif
};

class _MeshParam : public _Param
{
public:
    _MeshParam( const std::string &name_, short int type_, short int flags_,
                 void * eqn_val_, void *matrix_,
                 CValue default_init_val_, CValue upper_bound_,
                 CValue lower_bound_) :
            _Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_) {}
    float eval(int mesh_i, int mesh_j) override
    {
        assert( type == P_TYPE_DOUBLE );
        // see issue 64: There are presets with per_point equations that read from pre_frame/per_pixel parameters,
        //    e.g. per_point1=dx=dx*1.01
        // any this means that we get called with (i>=0,j==-1)
        if ( matrix_flag && mesh_i >= 0 && mesh_j >= 0)
            return ( ( ( float** ) matrix ) [mesh_i][mesh_j] );
        return * ( ( float* ) ( engine_val ) );
    }
    void set_matrix(int mesh_i, int mesh_j, float value) override
    {
        if (nullptr == matrix)
        {
            *(float *)engine_val = value;
        }
        else
        {
            ((float **) matrix)[mesh_i][mesh_j] = value;
            matrix_flag = true;
        }
    }
};


// TODO merge PointsParam/MeshParam
class _PointsParam : public _Param
{
public:
    _PointsParam( const std::string &name_, short int type_, short int flags_,
                 void * eqn_val_, void *matrix_,
                 CValue default_init_val_, CValue upper_bound_,
                 CValue lower_bound_) :
            _Param(name_, type_, flags_, eqn_val_, matrix_, default_init_val_, upper_bound_, lower_bound_) {}
    float eval(int mesh_i, int mesh_j) override
    {
        assert( mesh_j == -1 );
        assert( type == P_TYPE_DOUBLE );
        if ( matrix_flag && mesh_i >= 0 )
            return ( ( ( float* ) matrix ) [mesh_i] );
        return * ( ( float* ) ( engine_val ) );
    }
    void set_matrix(int mesh_i, int mesh_j, float value) override
    {
        if (nullptr == matrix)
        {
            *(float *)engine_val = value;
        }
        else
        {
            ((float *) matrix)[mesh_i] = value;
            matrix_flag = true;
        }
    }
};


Param * Param::create( const std::string &name, short int type, short int flags,
    void * eqn_val, void *matrix,
    CValue default_init_val, CValue upper_bound,
    CValue lower_bound)
{
    if (type == P_TYPE_BOOL)
    {
        assert(nullptr == matrix);
        assert(0 == (flags & (P_FLAG_PER_PIXEL|P_FLAG_PER_POINT)));
        return new _BoolParam( name, type, flags, eqn_val, matrix, default_init_val, upper_bound, lower_bound );
    }
    if (type == P_TYPE_INT)
    {
        assert(nullptr == matrix);
        assert(0 ==(flags & (P_FLAG_PER_PIXEL|P_FLAG_PER_POINT)));
        return new _IntParam( name, type, flags, eqn_val, matrix, default_init_val, upper_bound, lower_bound );
    }
    if (type == P_TYPE_STRING)
    {
        assert(0 == (flags & (P_FLAG_PER_PIXEL|P_FLAG_PER_POINT)));
        return new _StringParam( name, type, flags, eqn_val, matrix, default_init_val, upper_bound, lower_bound );
    }
    assert(type == P_TYPE_DOUBLE);
    if (matrix == nullptr)
    {
        assert(0 == (flags & (P_FLAG_PER_PIXEL|P_FLAG_PER_POINT)));
        return new _FloatParam( name, type, flags, eqn_val, matrix, default_init_val, upper_bound, lower_bound );
    }
    assert( flags & (P_FLAG_PER_PIXEL|P_FLAG_PER_POINT) );
    if (flags & P_FLAG_PER_PIXEL)
    {
        return new _MeshParam( name, type, flags, eqn_val, matrix, default_init_val, upper_bound, lower_bound );
    }
    else
    {
        return new _PointsParam( name, type, flags, eqn_val, matrix, default_init_val, upper_bound, lower_bound );
    }
}

Param * Param::createUser( const std::string &name )
{
    return new _FloatParam( name );
}


// TESTS


#include <TestRunner.hpp>

#ifndef NDEBUG

struct ParamTest : public Test
{
    ParamTest() : Test("ParamTest")
    {}

public:
    bool test() override
    {
        return true;
    }
};

Test* Param::test()
{
    return new ParamTest();
}

#else

Test* Param::test()
{
    return nullptr;
}

#endif