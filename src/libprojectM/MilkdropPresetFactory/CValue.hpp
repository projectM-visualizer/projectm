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
 * Value
 *
 * $Log$
 */

#ifndef _CVALUE_H
#define _CVALUE_H

#include "ParamDef.hpp"
#include <assert.h>

typedef struct CValue_t
{
private:
    union
    {
        int int_value;
        double double_value;
    };

public:
#ifndef NDEBUG
    int type;
#endif

#ifndef NDEBUG
    CValue_t()         : double_value(0.0), type(P_TYPE_DOUBLE) {}
    CValue_t(bool b)   : int_value(b), type(P_TYPE_BOOL)       {}
    CValue_t(int v)    : int_value(v), type(P_TYPE_INT)        {}
    CValue_t(double d) : double_value(d), type(P_TYPE_DOUBLE)   {}
#else
    CValue_t()         : double_value(0.0) {}
    CValue_t(bool b)   : int_value(b)     {}
    CValue_t(int v)    : int_value(v)     {}
    CValue_t(double d) : double_value(d)   {}
#endif
    bool bool_val()    { return int_value != 0; }
    int int_val()      { return int_value; }
    double float_val() { return double_value; }
} CValue;

#endif /** _CVALUE_H */
