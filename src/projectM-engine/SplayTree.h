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
 * Splay tree
 *
 * $Log$
 */

#ifndef _SPLAYTREE_H
#define _SPLAYTREE_H

#define REGULAR_NODE_TYPE 0
#define SYMBOLIC_NODE_TYPE 1

#define PERFECT_MATCH 0
#define CLOSEST_MATCH 1

#include "projectM.h"
#include "SplayNode.h"
#include "compare.h"
#include "Param.h"

class SplayTree {
public:
    SplayNode *root;
    int (*compare)(void*,void*);
    void * (*copy_key)(void *);
    void (*free_key)(void*);

    static SplayTree *create_splaytree(int (*compare)(void*,void*), void * (*copy_key)(void*), void (*free_key)(void*));
    ~SplayTree();

    void *splay_find(void * key);
    int splay_insert(void * data, void * key);
    int splay_insert_node( SplayNode *node );
    int splay_insert_link(void * alias_key, void * orig_key);
    int splay_delete(void * key);
    SplayNode *splay_delete_helper( void *key, SplayNode *node,
                                     int (*compare)(void*,void*), 
                                     void (*free_key)(void*) );
    int splay_size();
    int splay_rec_size( SplayNode *node );

    SplayNode *splay( void *key, SplayNode *t, int *match_type, int (*compare)(void *,void *) );
    void splay_traverse(void (*func_ptr)(void*));
    void splay_traverse_helper (void (*func_ptr)(void*), SplayNode * splaynode);

    SplayNode *get_splaynode_of(void * key);
    void *splay_find_above_min(void * key);
    void splay_find_above_min_helper( void *max_key, void **closest_key,
                                       SplayNode *root, int (*compare)(void *,void *));
    void *splay_find_below_max(void * key);
    void splay_find_below_max_helper( void *min_key, void **closest_key,
                                       SplayNode *root, int (*compare)(void *,void *));
    void *splay_find_min();
    void *splay_find_max();

    /** Helper functions */
    int insert_param( Param *param );
    Param *find_param_db( char *name, int create_flag );
  };

#endif /** !_SPLAYTREE_H */
