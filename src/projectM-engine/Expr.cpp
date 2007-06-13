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

#include "Expr.h"
#include "Eval.h"

float GenExpr::eval_gen_expr() {
  float l;
 	
  switch(type) {
  case VAL_T:  
    return ((ValExpr*)item)->eval_val_expr();
  case PREFUN_T:
    l = ((PrefunExpr *)item)->eval_prefun_expr();
    //if (EVAL_DEBUG) DWRITE( "eval_gen_expr: prefix function return value: %f\n", l);
    return l;		
  case TREE_T:
    return ((TreeExpr*)(item))->eval_tree_expr();
  default:
    #ifdef EVAL_DEBUG
    DWRITE( "eval_gen_expr: general expression matched no cases!\n");
    #endif
    return EVAL_ERROR;
  }  
	
}

/* Evaluates functions in prefix form */
float PrefunExpr::eval_prefun_expr() {

	int i;
	float rv;
       
	
	/* This is slightly less than safe, since
	   who knows if the passed argument is valid. For 
	   speed purposes we'll go with this */
	float *arg_list = (float *)wipemalloc( sizeof( float ) * num_args );
	
	#ifdef EVAL_DEBUG_DOUBLE
		DWRITE( "fn[");
	#endif
	/* Evaluate each argument before calling the function itself */
	for (i = 0; i < num_args; i++) {
		arg_list[i] = expr_list[i]->eval_gen_expr();
		#ifdef EVAL_DEBUG_DOUBLE 
			if (i < (num_args - 1))
				DWRITE( ", ");
		#endif
	}
	
	#ifdef EVAL_DEBUG_DOUBLE 
		DWRITE( "]");
	#endif
	
	/* Now we call the function, passing a list of 	
	   floats as its argument */
     
    rv = (func_ptr)(arg_list);	
    free( arg_list );
    arg_list = NULL;
    return rv;
}	

/* Evaluates a value expression */
float ValExpr::eval_val_expr() {

  /* Shouldn't happen */
  /* Value is a constant, return the float value */
  if (type == CONSTANT_TERM_T) {
    #ifdef EVAL_DEBUG 
		DWRITE( "%.4f", term.constant);
    #endif
    return (term.constant);
  }

  /* Value is variable, dereference it */
  if (type == PARAM_TERM_T) {
   	switch (term.param->type) {
		
	case P_TYPE_BOOL:
		#ifdef EVAL_DEBUG 
			DWRITE( "(%s:%.4f)", term.param->name, (float)(*((int*)(term.param->engine_val))));
		#endif
	       
		  
		return (float)(*((int*)(term.param->engine_val)));
	case P_TYPE_INT:
		#ifdef EVAL_DEBUG 
			DWRITE( "(%s:%.4f)", term.param->name, (float)(*((int*)(term.param->engine_val))));
		#endif

	     
		return (float)(*((int*)(term.param->engine_val)));
	case P_TYPE_DOUBLE:		
		#ifdef EVAL_DEBUG_DOUBLE 
			DWRITE( "(%s:%.4f)", term.param->name, (*((float*)term.param->engine_val)));
		#endif

		if (term.param->matrix_flag | (term.param->flags & P_FLAG_ALWAYS_MATRIX)) {
		/** Sanity check the matrix is there... */
		if ( term.param->matrix == NULL ) {
#ifdef EVAL_DEBUG_DOUBLE
		    DWRITE( "param->matrix == NULL! [%s]\tmesh: %d x %d\n", term.param->name, projectM::currentEngine->mesh_i, projectM::currentEngine->mesh_j );
#endif
		    return EVAL_ERROR;
		  }
		  if (projectM::currentEngine->mesh_i >= 0) {
			  if (projectM::currentEngine->mesh_j >= 0) {
			    return (((float**)term.param->matrix)[projectM::currentEngine->mesh_i][projectM::currentEngine->mesh_j]);
			  } else {
			    return (((float*)term.param->matrix)[projectM::currentEngine->mesh_i]);
			  }
		  }
		}
		return *((float*)(term.param->engine_val));
	default:
	  return EVAL_ERROR;	
    }
  }
  /* Unknown type, return failure */
  return PROJECTM_FAILURE;
}

/* Evaluates an expression tree */
float TreeExpr::eval_tree_expr() {
		
	float left_arg, right_arg;	
	
	/* A leaf node, evaluate the general expression. If the expression is null as well, return zero */
	if (infix_op == NULL) {
		if (gen_expr == NULL)
			return 0;
		else
	  		return gen_expr->eval_gen_expr();
	}
	
	/* Otherwise, this node is an infix operator. Evaluate
	   accordingly */
	
	#ifdef EVAL_DEBUG 
		DWRITE( "(");
	#endif
	
	left_arg = left->eval_tree_expr();

	#ifdef EVAL_DEBUG 
		
		switch (infix_op->type) {
		case INFIX_ADD:
			DWRITE( "+");
			break;		
		case INFIX_MINUS:
			DWRITE( "-");
			break;
		case INFIX_MULT:
			DWRITE( "*");
			break;
		case INFIX_MOD:
			DWRITE( "%%");
			break;
		case INFIX_OR:
			DWRITE( "|");
			break;
		case INFIX_AND:
			DWRITE( "&");
			break;
		case INFIX_DIV:
			DWRITE( "/");
			break;
		default:
			DWRITE( "?");
		}
	
	#endif
	
	right_arg = right->eval_tree_expr();
	
	#ifdef EVAL_DEBUG
		DWRITE( ")");
	#endif
	
#ifdef EVAL_DEBUG
    DWRITE( "\n" );
#endif

	switch (infix_op->type) {		
	case INFIX_ADD:
	  return (left_arg + right_arg);		
	case INFIX_MINUS:
		return (left_arg - right_arg);
	case INFIX_MULT:
		return (left_arg * right_arg);
	case INFIX_MOD:
	  if ((int)right_arg == 0) {
	    #ifdef EVAL_DEBUG 
	    DWRITE( "eval_tree_expr: modulo zero!\n");
	    #endif
	    return PROJECTM_DIV_BY_ZERO; 
	  }
	  return ((int)left_arg % (int)right_arg);
	case INFIX_OR:
		return ((int)left_arg | (int)right_arg);
	case INFIX_AND:
		return ((int)left_arg & (int)right_arg);
	case INFIX_DIV:
	  if (right_arg == 0) {
	    #ifdef EVAL_DEBUG 
	    DWRITE( "eval_tree_expr: division by zero!\n");
	    #endif
	    return MAX_DOUBLE_SIZE;
	  }		
	  return (left_arg / right_arg);
	default:
          #ifdef EVAL_DEBUG 
	    DWRITE( "eval_tree_expr: unknown infix operator!\n");
          #endif
		return EVAL_ERROR;
	}
	
	return EVAL_ERROR;
}	

/* Converts a float value to a general expression */
GenExpr * GenExpr::const_to_expr(float val) {

  GenExpr * gen_expr;
  ValExpr * val_expr;
  Term term;
  
  term.constant = val;
    
  if ((val_expr = ValExpr::new_val_expr(CONSTANT_TERM_T, &term)) == NULL)
    return NULL;

  gen_expr = GenExpr::new_gen_expr(VAL_T, (void*)val_expr);

  if (gen_expr == NULL) {
	delete val_expr;
  }
  
  return gen_expr;
}

/* Converts a regular parameter to an expression */
GenExpr * GenExpr::param_to_expr(Param * param) {

  GenExpr * gen_expr = NULL;
  ValExpr * val_expr = NULL;
  Term term;

  if (param == NULL)
    return NULL;
 
  /* This code is still a work in progress. We need
     to figure out if the initial condition is used for 
     each per frame equation or not. I am guessing that
     it isn't, and it is thusly implemented this way */
  
  /* Current guess of true behavior (08/01/03) note from carm
     First try to use the per_pixel_expr (with cloning). 
     If it is null however, use the engine variable instead. */
  
  /* 08/20/03 : Presets are now objects, as well as per pixel equations. This ends up
     making the parser handle the case where parameters are essentially per pixel equation
     substitutions */
       
  
  term.param = param;
  if ((val_expr = ValExpr::new_val_expr(PARAM_TERM_T, &term)) == NULL)
    return NULL;
  
  if ((gen_expr = GenExpr::new_gen_expr(VAL_T, (void*)val_expr)) == NULL) {
    delete val_expr;
	return NULL;	  
  } 
  return gen_expr;
}

/* Converts a prefix function to an expression */
GenExpr * GenExpr::prefun_to_expr(float (*func_ptr)(void *), GenExpr ** expr_list, int num_args) {

  GenExpr * gen_expr;
  PrefunExpr * prefun_expr;
  

  /* Malloc a new prefix function expression */
  prefun_expr = (PrefunExpr*)wipemalloc(sizeof(PrefunExpr));

  if (prefun_expr == NULL)
	  return NULL;
  
  prefun_expr->num_args = num_args;
  prefun_expr->func_ptr =(float (*)(void*)) func_ptr;
  prefun_expr->expr_list = expr_list;

  gen_expr = new_gen_expr(PREFUN_T, (void*)prefun_expr);

  if (gen_expr == NULL)
	  delete prefun_expr;
  
  return gen_expr;
}

/* Creates a new tree expression */
TreeExpr *TreeExpr::new_tree_expr(InfixOp * infix_op, GenExpr * gen_expr, TreeExpr * left, TreeExpr * right) {

		TreeExpr * tree_expr;
		tree_expr = (TreeExpr*)wipemalloc(sizeof(TreeExpr));
	
		if (tree_expr == NULL)
			return NULL;
		tree_expr->infix_op = infix_op;
		tree_expr->gen_expr = gen_expr;
		tree_expr->left = left;
		tree_expr->right = right;
		return tree_expr;
}


/* Creates a new value expression */
ValExpr *ValExpr::new_val_expr(int type, Term *term) {

  ValExpr * val_expr;
  val_expr = (ValExpr*)wipemalloc(sizeof(ValExpr));

  if (val_expr == NULL)
    return NULL;

  val_expr->type = type;
  val_expr->term.constant = term->constant;
    val_expr->term.param = term->param;

  return val_expr;
}

/* Creates a new general expression */
GenExpr * GenExpr::new_gen_expr(int type, void * item) {

	GenExpr * gen_expr;

	gen_expr = (GenExpr*)wipemalloc(sizeof(GenExpr));
	if (gen_expr == NULL)
		return NULL;
	gen_expr->type = type;
	gen_expr->item = item;	

	return gen_expr;
}

/* Frees a general expression */
GenExpr::~GenExpr() {

	switch (type) {
	case VAL_T:
		delete ((ValExpr*)item);
		break;
	case PREFUN_T:
		delete ((PrefunExpr*)item);
		break;
	case TREE_T:
		delete ((TreeExpr*)item);
		break;
	}	
}

/* Frees a function in prefix notation */
PrefunExpr::~PrefunExpr() {

	int i;
	
	/* Free every element in expression list */
	for (i = 0 ; i < num_args; i++) {
		delete expr_list[i];
	}
    free(expr_list);
}

/* Frees values of type VARIABLE and CONSTANT */
ValExpr::~ValExpr() {
  }

/* Frees a tree expression */
TreeExpr::~TreeExpr() {

	/* free left tree */
    if ( left != NULL ) {
	    delete left;
      }
	
	/* free general expression object */
    if ( gen_expr != NULL ) {
	    delete gen_expr;
      }
	
	/* Note that infix operators are always
	   stored in memory unless the program 
	   exits, so we don't remove them here */
	
	/* free right tree */
    if ( right != NULL ) {
        delete right;
      }
  }

/* Initializes an infix operator */
DLLEXPORT InfixOp::InfixOp(int type, int precedence) {

	this->type = type;
	this->precedence = precedence;
  }

/* Clones a general expression */
GenExpr *GenExpr::clone_gen_expr() {

  GenExpr * new_gen_expr;
  ValExpr * val_expr;
  TreeExpr * tree_expr;
  PrefunExpr * prefun_expr;

  /* Out of memory */
  if ((new_gen_expr = (GenExpr*)wipemalloc(sizeof(GenExpr))) == NULL)
    return NULL;

  /* Case on the type of general expression */
  switch (new_gen_expr->type = type) {

  case VAL_T: /* val expression */
    if ((val_expr = ((ValExpr*)item)->clone_val_expr()) == NULL) {
      free(new_gen_expr);
      new_gen_expr = NULL;
      return NULL;
    }
    new_gen_expr->item = (void*)val_expr;
    break;
    
  case PREFUN_T: /* prefix function expression */
    if ((prefun_expr = ((PrefunExpr*)item)->clone_prefun_expr()) == NULL) {
      free(new_gen_expr);
      new_gen_expr = NULL;
      return NULL;
    }
    new_gen_expr->item = (void*)prefun_expr;
    break;
    
  case TREE_T:  /* tree expression */
    if ((tree_expr = ((TreeExpr*)item)->clone_tree_expr()) == NULL) {
      free(new_gen_expr);
      new_gen_expr = NULL;
      return NULL;
    }
    new_gen_expr->item = (void*)tree_expr;
    break;
    
  default: /* unknown type, ut oh.. */
    free(new_gen_expr);
      new_gen_expr = NULL;
    return NULL;
  }
  
  return new_gen_expr; /* Return the new (cloned) general expression */
}


/* Clones a tree expression */
TreeExpr *TreeExpr::clone_tree_expr() {

  TreeExpr * new_tree_expr;

  /* Out of memory */
  if ((new_tree_expr = (TreeExpr*)wipemalloc(sizeof(TreeExpr))) == NULL) 
    return NULL;
  
  /* Set each argument in TreeExpr struct */
  new_tree_expr->infix_op = infix_op;  /* infix operators are in shared memory */
  new_tree_expr->gen_expr = gen_expr->clone_gen_expr(); /* clone the general expression */
  new_tree_expr->left = left->clone_tree_expr(); /* clone the left tree expression */
  new_tree_expr->right = right->clone_tree_expr(); /* clone the right tree expression */

  return new_tree_expr; /* Return the new (cloned) tree expression */
}

/* Clones a value expression, currently only passes the pointer to 
   the value that this object represents, not a pointer to a copy of the value */
ValExpr *ValExpr::clone_val_expr() {

  ValExpr * new_val_expr;

  /* Allocate space, check for out of memory */
  if ((new_val_expr = (ValExpr*)wipemalloc(sizeof(ValExpr))) == NULL) 
    return NULL;

  /* Set the values in the ValExpr struct */
  new_val_expr->type = type;
  new_val_expr->term = term;
  
  /* Return the new (cloned) value expression */
  return new_val_expr;
}

/* Clones a prefix function with its arguments */
PrefunExpr *PrefunExpr::clone_prefun_expr() {

  int i;
  PrefunExpr * new_prefun_expr;
  
  /* Out of memory */
  if ((new_prefun_expr = (PrefunExpr*)wipemalloc(sizeof(PrefunExpr))) == NULL) 
    return NULL;
  
  /* Set the function argument paired with its number of arguments */
  new_prefun_expr->num_args = num_args;
  new_prefun_expr->func_ptr = func_ptr;

  /* Allocate space for the expression list pointers */
  if ((new_prefun_expr->expr_list = (GenExpr**)wipemalloc(sizeof(GenExpr*)*new_prefun_expr->num_args)) == NULL) {
    free( new_prefun_expr );
    new_prefun_expr = NULL;
    return NULL;
  }

  /* Now copy each general expression from the argument expression list */
  for (i = 0; i < new_prefun_expr->num_args;i++) 
    new_prefun_expr->expr_list[i] = expr_list[i]->clone_gen_expr();
  
  /* Finally, return the new (cloned) prefix function expression */
  return new_prefun_expr;
}
