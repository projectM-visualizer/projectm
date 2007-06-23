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
 * Initial condition
 *
 * $Log$
 */

#ifndef _INIT_COND_H
#define _INIT_COND_H

//#define INIT_COND_DEBUG 2
#define INIT_COND_DEBUG 0

#include "Param.h"

class InitCond;
class Param;
#include "SplayTree.hpp"

class InitCond {
public:
    Param *param;
    CValue init_val;

    static char init_cond_string_buffer[STRING_BUFFER_SIZE];
    static int init_cond_string_buffer_index;

    InitCond( Param * param, CValue init_val );
    ~InitCond();
    void eval_init_cond();

    void init_cond_to_string();
    void write_init();
  };

/** Splaytree traversal helpers */
inline void eval_init_cond_helper( void *init_cond ) {
    ((InitCond *)init_cond)->eval_init_cond();
  }

inline void free_init_cond_helper( void *init_cond ) {
    delete ((InitCond *)init_cond);
  }

inline void write_init_helper( void *init_cond ) {
    ((InitCond *)init_cond)->write_init();
  }

inline void init_cond_to_string_helper( void *init_cond ) {
    ((InitCond *)init_cond)->init_cond_to_string();
  }

#endif /** !_INIT_COND_H */
