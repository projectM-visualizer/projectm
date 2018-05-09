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

#include "wipemalloc.h"

#include "Expr.hpp"
#include <cassert>

#include <iostream>
#include "Eval.hpp"


/* Evaluates functions in prefix form */
float PrefunExpr::eval ( int mesh_i, int mesh_j )
{
	assert ( func_ptr );
	float arg_list_stk[10];

	float * arg_list;
	float * argp;
	Expr **expr_listp = expr_list;


	if (this->num_args > 10) {
		arg_list = new float[this->num_args];
	} else {
		arg_list = arg_list_stk;
	}
	argp = arg_list;

	assert(arg_list);

	//printf("numargs %d", num_args);

	/* Evaluate each argument before calling the function itself */
	for ( int i = 0; i < num_args; i++ )
	{
		*(argp++) = (*(expr_listp++))->eval ( mesh_i, mesh_j );
		//printf("numargs %x", arg_list[i]);
	}
	/* Now we call the function, passing a list of
	   floats as its argument */

	const float value = ( func_ptr ) ( arg_list );

	if (arg_list != arg_list_stk) {
		delete[](arg_list);
	}
	return value;
}


// private subclasses of ValExpr -- ConstantExpr, ParameterExpr
class ConstantExpr : public ValExpr
{
public:
	ConstantExpr( int type, Term *term ) : ValExpr(type,term) {}
	float eval(int mesh_i, int mesh_j ) { return term.constant; }
};

class ParameterExpr : public ValExpr
{
public:
	ParameterExpr( int type, Term *term ) : ValExpr(type,term) {}
	float eval(int mesh_i, int mesh_j );
};

/* Evaluates a value expression */
float ParameterExpr::eval ( int mesh_i, int mesh_j )
{
	switch ( term.param->type )
	{

		case P_TYPE_BOOL:


			return ( float ) ( * ( ( bool* ) ( term.param->engine_val ) ) );
		case P_TYPE_INT:


			return ( float ) ( * ( ( int* ) ( term.param->engine_val ) ) );
		case P_TYPE_DOUBLE:


			if ( term.param->matrix_flag | ( term.param->flags & P_FLAG_ALWAYS_MATRIX ) )
			{

				/* Sanity check the matrix is there... */
				assert ( term.param->matrix != NULL );

				/// @slow boolean check could be expensive in this critical (and common) step of evaluation
				if ( mesh_i >= 0 )
				{
					if ( mesh_j >= 0 )
					{
						return ( ( ( float** ) term.param->matrix ) [mesh_i][mesh_j] );
					}
					else
					{
						return ( ( ( float* ) term.param->matrix ) [mesh_i] );
					}
				}
				//assert(mesh_i >=0);
			}
			//std::cout << term.param->name << ": " << (*((float*)term.param->engine_val)) << std::endl;
			return * ( ( float* ) ( term.param->engine_val ) );
		default:
			return EVAL_ERROR;
	}
}


/* This could be optimized a lot more if we could return an Expr instead of a TreeExpr */
TreeExpr * TreeExpr::create( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right )
{
	float defautFloat = _infix_op == Eval::infix_mult ? 1.0f : 0.0f;

	if (_infix_op == NULL)
	{
		if (_gen_expr == NULL)
			_gen_expr = Expr::const_to_expr(0.0f);
	}
	else
	{
		if (_left == NULL)
			_left = new TreeExpr(NULL, Expr::const_to_expr(defautFloat), NULL, NULL);
		if (_right == NULL)
			_right = new TreeExpr(NULL, Expr::const_to_expr(defautFloat), NULL, NULL);
	}
	return new TreeExpr( _infix_op, _gen_expr, _left, _right);
}

/* Evaluates an expression tree */
float TreeExpr::eval ( int mesh_i, int mesh_j )
{
	float left_arg, right_arg;

	/* A leaf node, evaluate the general expression. If the expression is null as well, return zero */
	if ( infix_op == NULL )
	{
		return gen_expr->eval( mesh_i, mesh_j );
	}

	/* Otherwise, this node is an infix operator. Evaluate
	   accordingly */

	left_arg = left->eval ( mesh_i, mesh_j );
	right_arg = right->eval ( mesh_i, mesh_j );

	switch ( infix_op->type )
	{
		case INFIX_ADD:
			return ( left_arg + right_arg );
		case INFIX_MINUS:
			return ( left_arg - right_arg );
		case INFIX_MULT:
			return ( left_arg * right_arg );
		case INFIX_MOD:
			if ( ( int ) right_arg == 0 )
			{
				return PROJECTM_DIV_BY_ZERO;
			}
			return ( ( int ) left_arg % ( int ) right_arg );
		case INFIX_OR:
			return ( ( int ) left_arg | ( int ) right_arg );
		case INFIX_AND:
			return ( ( int ) left_arg & ( int ) right_arg );
		case INFIX_DIV:
			if ( right_arg == 0 )
			{
				return MAX_DOUBLE_SIZE;
			}
			return ( left_arg / right_arg );
		default:
			return EVAL_ERROR;
	}

	return EVAL_ERROR;
}

/* Converts a float value to a general expression */
Expr * Expr::const_to_expr ( float val )
{
	Term term;
	term.constant = val;
	return ValExpr::create( CONSTANT_TERM_T, &term );
}

/* Converts a regular parameter to an expression */
Expr * Expr::param_to_expr ( Param * param )
{
	ValExpr * val_expr = NULL;
	Term term;

	if ( param == NULL )
		return NULL;

	/* This code is still a work in progress. We need
	   to figure out if the initial condition is used for
	   each per frame equation or not. I am guessing that
	   it isn't, and it is thusly implemented this way */

	/* Current guess of true behavior (08/01/03) note from carm
	   First try to use the per_pixel_expr (with cloning)
	   If it is null however, use the engine variable instead. */

	/* 08/20/03 : Presets are now objects, as well as per pixel equations. This ends up
	   making the parser handle the case where parameters are essentially per pixel equation
	   substitutions */


	term.param = param;
	return ValExpr::create( PARAM_TERM_T, &term );
}

/* Converts a prefix function to an expression */
Expr * Expr::prefun_to_expr ( float ( *func_ptr ) ( void * ), Expr ** expr_list, int num_args )
{
	PrefunExpr * prefun_expr;

	prefun_expr = new PrefunExpr();
	prefun_expr->num_args = num_args;
	prefun_expr->func_ptr = ( float ( * ) ( void* ) ) func_ptr;
	prefun_expr->expr_list = expr_list;
	return prefun_expr;
}

/* Creates a new tree expression */
TreeExpr::TreeExpr ( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right ) :
		infix_op ( _infix_op ), gen_expr ( _gen_expr ),
	left ( _left ), right ( _right ) {}


/* Creates a new value expression */
ValExpr::ValExpr ( int _type, Term * _term ) :type ( _type )
{
	term.constant = _term->constant;
	term.param = _term->param;
}

ValExpr *ValExpr::create ( int _type , Term * _term )
{
	if (_type == CONSTANT_TERM_T)
		return new ConstantExpr( _type, _term );
	else
		return new ParameterExpr( _type, _term );	
}

/* Frees a function in prefix notation */
PrefunExpr::~PrefunExpr()
{
	int i;

	/* Free every element in expression list */
	for ( i = 0 ; i < num_args; i++ )
	{
		delete expr_list[i];
	}
	free ( expr_list );
}

/* Frees values of type VARIABLE and CONSTANT */
ValExpr::~ValExpr()
{}

/* Frees a tree expression */
TreeExpr::~TreeExpr()
{

	/* free left tree */
	if ( left != NULL )
	{
		delete left;
	}

	/* free general expression object */
	if ( gen_expr != NULL )
	{
		delete gen_expr;
	}

	/* Note that infix operators are always
	   stored in memory unless the program
	   exits, so we don't remove them here */

	/* free right tree */
	if ( right != NULL )
	{
		delete right;
	}
}

/* Initializes an infix operator */
InfixOp::InfixOp ( int type, int precedence )
{

	this->type = type;
	this->precedence = precedence;
}



PrefunExpr::PrefunExpr() {}
