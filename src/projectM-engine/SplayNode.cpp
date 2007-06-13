/**
 * $Id$
 *
 * $Log$
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "projectM.h"

#include "common.h"
#include "fatal.h"

#include "Param.h"
#include "SplayNode.h"
#include "SplayTree.h"

#include "wipemalloc.h"

/** Create a new default splaynode */
SplayNode::SplayNode() {
    this->data = NULL;
    this->type = -1;
    this->key = NULL;
    this->tree = NULL;
  }

/* Create a new splay node type */
SplayNode::SplayNode(int type, void * key, void * data, SplayTree *tree) {

	/* Creates the new splay node struct */
	this->data = data;
	this->type = type;
	this->key = key;
    this->tree = tree;
  }

/* Recursively free all the splaynodes */
SplayNode::~SplayNode() {

    if ( tree == NULL ) {
        if ( key != NULL || data != NULL ) {
            /** This shouldn't happen */
            printf( "~SplayNode: tree is NULL with non-NULL key/data!\n" );
            printf( "\tleft: %X\tright: %X\tdata: %X\tkey: %X\n",
                    left, right, data, key );
          }
        return;
      }

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
    tree->free_key(key);
  
  /* Note that the data pointers are not freed here.
     Should be freed with a splay traversal function */
  }
