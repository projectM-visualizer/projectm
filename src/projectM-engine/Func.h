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
 * Function
 *
 * $Log$
 */

#ifndef _FUNC_H
#define _FUNC_H

#include "common.h"

class Func;
class SplayTree;

/* Function Type */
class Func {
public:
    char name[MAX_TOKEN_SIZE];  
    float (*func_ptr)(float*);
    int num_args;
    static SplayTree *builtin_func_tree;

    /* Public Prototypes */
    DLLEXPORT ~Func();
    static Func *create_func ( char *name, float (*func_ptr)(float*), int num_args );
  };

/** Splay traversal */
inline void free_func_helper( void *func ) {
    delete (Func *)func;
  }
    
#endif /** !_FUNC_H */
