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
#include <iostream>

#include "fatal.h"
#include "Common.hpp"

#include "Expr.hpp"
#include "Eval.hpp"
#include "Param.hpp"
#include "PerPixelEqn.hpp"
#include <map>

#include "wipemalloc.h"
#include <cassert>

/* Evaluates a per pixel equation */
void PerPixelEqn::evaluate(int mesh_i, int mesh_j)
{
    float v = gen_expr->eval( mesh_i, mesh_j );
    param->set_matrix( mesh_i, mesh_j, v );
}

PerPixelEqn::PerPixelEqn(int _index, Param * _param, Expr * _gen_expr):index(_index), param(_param), gen_expr(_gen_expr) {

	assert(index >= 0);
	assert(param != 0);
	assert(gen_expr != 0);
}


PerPixelEqn::~PerPixelEqn()
{
    Expr::delete_expr(gen_expr);
}
