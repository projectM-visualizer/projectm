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
/* Function management */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Common.hpp"
#include "fatal.h"

#include "Func.hpp"
#include <map>

#include "wipemalloc.h"




/* Private function prototypes */
void *Func::copy_func_key(char * string) {
	
	char * clone_string;
	
	if ((clone_string = (char*)wipemalloc(MAX_TOKEN_SIZE)) == NULL)
		return NULL;
	
	strncpy(clone_string, string, MAX_TOKEN_SIZE-1);
	
	return (void*)clone_string;
}	

/* Compare string name with function name */
int Func::compare_func(char * name, char * name2) {

  int cmpval;

  /* Uses string comparison function */
  cmpval = strncmp(name, name2, MAX_TOKEN_SIZE-1);
  
  return cmpval;
}

Func * Func::create_func (char * name, float (*func_ptr)(float*), int num_args) {

  Func * func;
  func = (Func*)wipemalloc(sizeof(Func));
 
  if (func == NULL)
    return NULL;

  
  /* Clear name space */
  memset(func->name, 0, MAX_TOKEN_SIZE);

  /* Copy given name into function structure */
  strncpy(func->name, name, MAX_TOKEN_SIZE); 

  /* Assign value pointer */
  func->func_ptr = func_ptr;
  func->num_args = num_args;
  /* Return instantiated function */
  return func;

}

/* Frees a function type, real complicated... */
Func::~Func() {
  }
