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


class PrefunExprOne : public PrefunExpr
{
	float eval ( int mesh_i, int mesh_j )
	{
		float val = expr_list[0]->eval ( mesh_i, mesh_j );
		return (func_ptr)(&val);
	}
};


class ConstantExpr : public Expr
{
	float constant;
public:
	ConstantExpr( float value ) : Expr(CONSTANT), constant(value) {}
	ConstantExpr( int type, Term *term ) : Expr(CONSTANT), constant(term->constant) {}
	bool isConstant() 
	{
		return true; 
	}
	float eval(int mesh_i, int mesh_j ) 
	{ 
		return constant; 
	}
	std::ostream &to_string(std::ostream &out)
	{
		out << constant; return out;
	}
};

class ParameterExpr : public Expr
{
protected:
	Term term;
public:
	ParameterExpr( int _type, Term *_term ) : Expr(PARAMETER), term(*_term) {}
	float eval(int mesh_i, int mesh_j ) = 0;
	std::ostream& to_string(std::ostream& out)
	{
		out << term.param->name;
		return out;
	}
};

class BoolParameterExpr : public ParameterExpr
{
public:
	BoolParameterExpr( int _type, Term *_term ) : ParameterExpr(_type,_term) {}
	float eval ( int mesh_i, int mesh_j )
	{
		assert( term.param->type == P_TYPE_BOOL );
		return ( float ) ( * ( ( bool* ) ( term.param->engine_val ) ) );
	}
};
class IntParameterExpr : public ParameterExpr
{
public:
	IntParameterExpr( int _type, Term *_term ) : ParameterExpr(_type,_term) {}
	float eval ( int mesh_i, int mesh_j )
	{
		assert( term.param->type == P_TYPE_INT );
		return ( float ) ( * ( ( int* ) ( term.param->engine_val ) ) );
	}
};
class FloatParameterExpr : public ParameterExpr
{
public:
	FloatParameterExpr( int _type, Term *_term ) : ParameterExpr(_type,_term) {}
	float eval ( int mesh_i, int mesh_j );
};
/* TODO optimize FloatParameterExpr::eval() further.
//  - flag "never matrix" parameters
//  - always pass in 2d matrix. instead of using  (i, -1) for 1d matrix, we could just use (0, i) and avoid check
//  - instead of using matrix_flag to give "copy on write" behavior, maybe pre-copy from engine_val to matrix[]
*/
float FloatParameterExpr::eval ( int mesh_i, int mesh_j )
{
	assert( term.param->type == P_TYPE_DOUBLE );
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
}


class MultAndAddExpr : public Expr
{
	Expr *a, *b, *c;
public:
	MultAndAddExpr(Expr *_a, Expr *_b, Expr *_c) : Expr(OTHER),
		a(_a), b(_b), c(_c)
	{
	}
	float eval(int mesh_i, int mesh_j)
	{
		float a_value = a->eval(mesh_i,mesh_j);
		float b_value = b->eval(mesh_i,mesh_j);
		float c_value = c->eval(mesh_i,mesh_j);
		return a_value * b_value + c_value;
	}
	std::ostream &to_string(std::ostream &out)
	{
		out << "(" << a << " * " << b << ") + " << c;
		return out;
	}
};

std::ostream &TreeExpr::to_string(std::ostream &out)
{
	if (NULL == infix_op)
	{
		out << gen_expr;
	}
	else
	{
		out << "(" << left << " ";
		switch ( infix_op->type )
		{
		case INFIX_ADD:
			out << "+"; break;
		case INFIX_MINUS:
			out << "-"; break;
		case INFIX_MULT:
			out << "*"; break;
		case INFIX_MOD:
			out << "%"; break;
		case INFIX_OR:
			out << "|"; break;
		case INFIX_AND:
			out << "&"; break;
		case INFIX_DIV:
			out << "/"; break;
		default:
			out << "infix_op_ERROR"; break;
		}
		out << " " << right << ")";
	}
	return out;
}

/* NOTE: Parser.cpp directly manipulates TreeExpr, so it is easier to optimizer AFTER parsing
 * than while building up the tree initially 
 */
Expr *TreeExpr::optimize()
{
	if (infix_op == NULL)
	{
		Expr *opt = gen_expr->optimize();
		if (opt != gen_expr)
			delete gen_expr;
		gen_expr = NULL;
		return opt;
	}
	if (left != NULL)
	{
		Expr *l = left->optimize();
		if (l != left)
			delete left;
		left = l;
	}
	if (right != NULL)
	{
		Expr *r = right->optimize();
		if (r != right)
			delete right;
		right = r;
	}
	if (left == NULL)
	{
		Expr *opt = right;
		right = NULL;
		return opt;
	}
	if (right == NULL)
	{
		Expr *opt = left;
		left = NULL;
		return opt;
	}
	if (left->isConstant() && right->isConstant())
		return Expr::const_to_expr(eval(-1, -1));

	// this is gratuitious, but a*b+c is super common, so as proof-of-concept, let's make a special Expr
	if (infix_op->type == INFIX_ADD && 
		((left->clazz == TREE && ((TreeExpr *)left)->infix_op->type == INFIX_MULT) ||
		(right->clazz == TREE && ((TreeExpr *)right)->infix_op->type == INFIX_MULT)))
	{
		Expr *a, *b, *c;
		if (left->clazz == TREE && ((TreeExpr *)left)->infix_op->type == INFIX_MULT)
		{
			a = ((TreeExpr *)left)->left;
			b = ((TreeExpr *)left)->right;
			c = right;
			((TreeExpr *)left)->left = NULL;
			((TreeExpr *)left)->right = NULL;
			right = NULL;
		}
		else
		{
			a = ((TreeExpr *)right)->left;
			b = ((TreeExpr *)right)->right;
			c = left;
			((TreeExpr *)right)->left = NULL;
			((TreeExpr *)right)->right = NULL;
			left = NULL;
		}
		return new MultAndAddExpr(a,b,c);
	}
	return this;
}

/* Evaluates an expression tree */
float TreeExpr::eval ( int mesh_i, int mesh_j )
{
	float left_arg, right_arg;

	/* shouldn't be null if we've called optimize() */
	assert(NULL != infix_op);

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
	return new ConstantExpr( CONSTANT_TERM_T, &term );
}

/* Converts a regular parameter to an expression */
Expr * Expr::param_to_expr ( Param * param )
{
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

	switch ( param->type )
	{
		case P_TYPE_BOOL:
			return new BoolParameterExpr( PARAM_TERM_T, &term );
		case P_TYPE_INT:
			return new IntParameterExpr( PARAM_TERM_T, &term );
		case P_TYPE_DOUBLE:
			return new FloatParameterExpr( PARAM_TERM_T, &term );
	}
	return NULL;
}

/* Converts a prefix function to an expression */
Expr * Expr::prefun_to_expr ( float ( *func_ptr ) ( void * ), Expr ** expr_list, int num_args )
{
	PrefunExpr * prefun_expr;
	if (num_args == 1)
		prefun_expr = new PrefunExprOne();
	else
		prefun_expr = new PrefunExpr();
	prefun_expr->num_args = num_args;
	prefun_expr->func_ptr = ( float ( * ) ( void* ) ) func_ptr;
	prefun_expr->expr_list = expr_list;
	return prefun_expr;
}

/* Creates a new tree expression */
TreeExpr::TreeExpr ( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right ) :
		Expr( TREE ),
		infix_op ( _infix_op ), gen_expr ( _gen_expr ),
	left ( _left ), right ( _right ) {}

class TreeExprAdd : public TreeExpr
{
public:
	TreeExprAdd( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right ) :
	 	TreeExpr( _infix_op, _gen_expr, _left, _right) {}
	float eval( int mesh_i, int mesh_j)
	{
		return left->eval(mesh_i, mesh_j) + right->eval(mesh_i, mesh_j);
	}
};

class TreeExprMinus : public TreeExpr
{
public:
	TreeExprMinus( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right ) :
	 	TreeExpr( _infix_op, _gen_expr, _left, _right) {}
	float eval( int mesh_i, int mesh_j)
	{
		return left->eval(mesh_i, mesh_j) - right->eval(mesh_i, mesh_j);
	}
};

class TreeExprMult : public TreeExpr
{
public:
	TreeExprMult( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right ) :
	 	TreeExpr( _infix_op, _gen_expr, _left, _right) {}
	float eval( int mesh_i, int mesh_j)
	{
		return left->eval(mesh_i, mesh_j) * right->eval(mesh_i, mesh_j);
	}
};

TreeExpr * TreeExpr::create( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right )
{
	if ( NULL != _infix_op )
	{
		if ( _infix_op->type == INFIX_ADD )
			return new TreeExprAdd( _infix_op, _gen_expr, _left, _right);
		if ( _infix_op->type == INFIX_MINUS )
			return new TreeExprMinus( _infix_op, _gen_expr, _left, _right);
		if ( _infix_op->type == INFIX_MULT )
			return new TreeExprMult( _infix_op, _gen_expr, _left, _right);
	}
	return new TreeExpr( _infix_op, _gen_expr, _left, _right );
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
InfixOp::InfixOp ( int _type, int _precedence )
{
	this->type = _type;
	this->precedence = _precedence;
}

PrefunExpr::PrefunExpr() : Expr(FUNCTION)
{
}

Expr *PrefunExpr::optimize()
{
	bool constant_args = true;
	for (int i=0 ; i < num_args ; i++)
	{
		Expr *orig = expr_list[i];
		expr_list[i] = orig->optimize();
		if (orig != expr_list[i])
			delete orig;
		constant_args &= expr_list[i]->isConstant();
	}
    // TODO most functions can be pre-evaluated if inputs are constant, but not all
	return this;
}

std::ostream& PrefunExpr::to_string(std::ostream& out)
{
	char comma = ' ';
	out << "<function>(";
	for (int i=0 ; i < num_args ; i++)
	{
		out << comma;
		out << expr_list[i];
		comma = ',';
	}
	out << ")";
	return out;
}
