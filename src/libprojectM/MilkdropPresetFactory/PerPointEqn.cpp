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

#include <cassert>
#include "fatal.h"
#include "Common.hpp"

#include "CustomWave.hpp"
#include "Eval.hpp"
#include "Expr.hpp"
#include "Param.hpp"
#include "PerPixelEqn.hpp"
#include "PerPointEqn.hpp"
#include <map>
#include <iostream>
#include "wipemalloc.h"

/* Evaluates a per point equation for the current custom wave given by interface_wave ptr */
void PerPointEqn::evaluate(int i)
{
    assign_expr->eval( i, -1 );
}

PerPointEqn::PerPointEqn(int _index, Param * param, Expr * gen_expr):
    index(_index)
{
    assign_expr = Expr::create_matrix_assignment(param, gen_expr);
}


PerPointEqn::~PerPointEqn()
{
  Expr::delete_expr(assign_expr);
}
