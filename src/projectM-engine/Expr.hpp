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
 * Expression
 *
 * $Log$
 */

#ifndef _EXPR_H
#define _EXPR_H

#include "dlldefs.h"
#include "CValue.hpp"

class Param;

#define CONST_STACK_ELEMENT 0
#define EXPR_STACK_ELEMENT 1

#define EVAL_ERROR -1

/* Infix Operator Function */
class InfixOp {
public:
    int type;
    int precedence;  

    DLLEXPORT InfixOp( int type, int precedence );
  };

/** Term */
class Term {
public:
    float constant; /* static variable */
    Param *param; /* pointer to a changing variable */

    Term() { this->constant = 0; this->param = 0; }
  };

/* General Expression Type */
class GenExpr {
public:
    int type;
    void * item;

    ~GenExpr();

    static GenExpr *new_gen_expr( int type, void *item );
    GenExpr *clone_gen_expr();
    float eval_gen_expr();

    static GenExpr *const_to_expr( float val );
    static GenExpr *param_to_expr( Param *param );
    static GenExpr *prefun_to_expr( float (*func_ptr)(void *), GenExpr **expr_list, int num_args );
  };

/* Value expression, contains a term union */
class ValExpr {
public:
    int type;
    Term term;

    ~ValExpr();
    static ValExpr *new_val_expr( int type, Term *term );
    ValExpr *clone_val_expr();

    float eval_val_expr();
  };

/* A binary expression tree ordered by operator precedence */
class TreeExpr {
public:
    InfixOp * infix_op; /* null if leaf */
    GenExpr * gen_expr;
    TreeExpr *left, *right;

    ~TreeExpr();
    static TreeExpr *new_tree_expr( InfixOp *infix_op, GenExpr *gen_expr, 
                                    TreeExpr *left, TreeExpr *right );
    TreeExpr *clone_tree_expr();
    float eval_tree_expr();
  };

/* A function expression in prefix form */
class PrefunExpr {
public:
    float (*func_ptr)(void*);
    int num_args;
    GenExpr **expr_list;

    ~PrefunExpr();
    PrefunExpr *clone_prefun_expr();
    float eval_prefun_expr();
  };

#endif /** _EXPR_H */
