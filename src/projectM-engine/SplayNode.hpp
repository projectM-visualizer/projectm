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
 * Node of a splay tree
 *
 * $Log$
 */

#ifndef _SPLAYNODE_HPP
#define _SPLAYNODE_HPP

//#include "projectM.h"

#include "compare.h"

typedef void Object;
template <class Data = Object>
class SplayNode {
public:
    SplayNode *left, *right;
    Data *data;
    void *key;
   void (*free_key)(void*);
    SplayNode();
    SplayNode(void *key, Data *data, void (*free_key)(void*));
    ~SplayNode();
  };

/** Create a new default splaynode */
template <class Data>
SplayNode<Data>::SplayNode() {
    this->data = NULL;
    this->key = NULL;
    this->free_key = free_key;
  }

/* Create a new splay node type */
template <class Data>
SplayNode<Data>::SplayNode(void * key, Data * data, void (*free_key)(void*)) {

	/* Creates the new splay node struct */
	this->data = data;	
	this->key = key;
        this->free_key = free_key;
  }

/* Recursively free all the splaynodes */
template <class Data>
SplayNode<Data>::~SplayNode() {

  /* Ok if this happens, a recursive base case */
  /* Free left node */
    if ( left != NULL ) {
        delete left;
      }
  
  /* Free right node */
    if ( right != NULL ) {
        delete right;
      }
  
    /* Free this node's key */
//    printf( "~SplayNode: %X\t%X\n", key, tree->free_key );

    this->free_key(key);
  
  /* Note that the data pointers are not freed here.
     Should be freed with a splay traversal function */
  }

#endif /** !_SPLAYNODE_HPP */
