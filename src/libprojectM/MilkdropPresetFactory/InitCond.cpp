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
/* Library functions to manipulate initial condition values */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.hpp"
#include "fatal.h"

#include "Expr.hpp"
#include "InitCond.hpp"
#include "Param.hpp"
#include <map>

#include "wipemalloc.h"
#include <cassert>
#include <iostream>

char InitCond::init_cond_string_buffer[STRING_BUFFER_SIZE];
int InitCond::init_cond_string_buffer_index = 0;

/* Creates a new initial condition */
InitCond::InitCond( Param * _param, CValue _init_val ):param(_param), init_val(_init_val)
{
  assert(param);
}

/* Frees initial condition structure */
InitCond::~InitCond() {}

void InitCond::evaluate() 
{
  evaluate(false);
}

/* Evaluate an initial conditon */
void InitCond::evaluate(bool evalUser)
{
    assert(this);
    assert(param);

    if (param->flags & P_FLAG_USERDEF && !evalUser)
	    return;

    /* Has side-effect of also setting matrix flag to zero. This ensures
       its constant value will be used rather than a matrix value
    */
	param->set_param(init_val);
}

/* WIP */
void InitCond::init_cond_to_string()
{
	unsigned long string_length;
	char string[MAX_TOKEN_SIZE];
	
	/* Create a string "param_name=val" */
	switch (param->type) {
		
		case P_TYPE_BOOL:
			sprintf(string, "%s=%d\n", param->name.c_str(), init_val.bool_val);
			break; 
		case P_TYPE_INT:
			sprintf(string, "%s=%d\n", param->name.c_str(), init_val.int_val);
			break;
		case P_TYPE_DOUBLE:
			sprintf(string, "%s=%f\n", param->name.c_str(), init_val.float_val);
			break;
		default:
			return;
	}
		
	/* Compute the length of the string */
	string_length = strlen(string);
	
	/* Buffer overflow check */
	if ((init_cond_string_buffer_index + string_length + 1)  > (STRING_BUFFER_SIZE - 1))
		return;
	
	/* Copy the string into the initial condition string buffer */	
    memcpy(init_cond_string_buffer + init_cond_string_buffer_index, string, string_length + 1);
	
	/* Increment the string buffer, offset by one for the null terminator, which will be
	   overwritten by the next call to this function */
	init_cond_string_buffer_index+= string_length + 1;
}

