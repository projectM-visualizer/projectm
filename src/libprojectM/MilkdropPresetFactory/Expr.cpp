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
#include "BuiltinFuncs.hpp"

// #ifdef USE_LLVM
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>


struct JitContext
{
	JitContext(llvm::LLVMContext &context_, llvm::IRBuilder<llvm::ConstantFolder,llvm::IRBuilderDefaultInserter> &builder_, llvm::Value *i, llvm::Value *j) :
	    context(context_), builder(builder_), mesh_i(i), mesh_j(j)
	{
		floatType = llvm::Type::getFloatTy(context);
	}

	llvm::LLVMContext &context;
	llvm::IRBuilder<llvm::ConstantFolder,llvm::IRBuilderDefaultInserter> builder;
	llvm::Type *floatType;
	llvm::Value *mesh_i;
	llvm::Value *mesh_j;
};


// #endif


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

class SinExpr : public PrefunExpr
{
	float eval ( int mesh_i, int mesh_j ) override
	{
		float val = expr_list[0]->eval ( mesh_i, mesh_j );
		return sinf(val);
	}
};

class CosExpr : public PrefunExpr
{
	float eval ( int mesh_i, int mesh_j ) override
	{
		float val = expr_list[0]->eval ( mesh_i, mesh_j );
		return cosf(val);
	}
};

class LogExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j ) override
    {
        float val = expr_list[0]->eval ( mesh_i, mesh_j );
        return logf(val);
    }
};

class PowExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j ) override
    {
        float x = expr_list[0]->eval ( mesh_i, mesh_j );
        float y = expr_list[1]->eval ( mesh_i, mesh_j );
        return powf(x, y);
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

    llvm::Value *_llvm(JitContext &jitx) override
    {
        return llvm::ConstantFP::get(jitx.floatType, constant);
    }
};


class MultAndAddExpr : public Expr
{
	Expr *a, *b, *c;
public:
	MultAndAddExpr(Expr *_a, Expr *_b, Expr *_c) : Expr(OTHER),
		a(_a), b(_b), c(_c)
	{
	}
    ~MultAndAddExpr() override {
        Expr::delete_expr(a);
        Expr::delete_expr(b);
        Expr::delete_expr(c);
    }
	float eval(int mesh_i, int mesh_j) override
	{
		float a_value = a->eval(mesh_i,mesh_j);
		float b_value = b->eval(mesh_i,mesh_j);
		float c_value = c->eval(mesh_i,mesh_j);
		return a_value * b_value + c_value;
	}
	std::ostream &to_string(std::ostream &out) override
	{
		out << "(" << a << " * " << b << ") + " << c;
		return out;
	}
};

class MultConstExpr : public Expr
{
    Expr *expr;
    float c;
public:
    MultConstExpr(Expr *_expr, float _c) : Expr(OTHER),
        expr(_expr), c(_c)
    {
    }
    ~MultConstExpr() override
    {
        Expr::delete_expr(expr);
    }
    float eval(int mesh_i, int mesh_j) override
    {
        float value = expr->eval(mesh_i,mesh_j);
        return value * c;
    }
    std::ostream &to_string(std::ostream &out) override
    {
        out << "(" << expr << " * " << c << ") + " << c;
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

/* NOTE: Parser.cpp directly manipulates TreeExpr, so it is easier to _optimizer AFTER parsing
 * than while building up the tree initially 
 */
Expr *TreeExpr::_optimize()
{
	if (infix_op == NULL)
	{
		Expr *opt = gen_expr->_optimize();
		if (opt != gen_expr)
			Expr::delete_expr(gen_expr);
		gen_expr = NULL;
		return opt;
	}
	if (left != NULL)
	{
		Expr *l = left->_optimize();
		if (l != left)
		    Expr::delete_expr(left);
		left = l;
	}
	if (right != NULL)
	{
		Expr *r = right->_optimize();
		if (r != right)
			Expr::delete_expr(right);
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
		    std::swap(left,right);
        a = ((TreeExpr *)right)->left;
        b = ((TreeExpr *)right)->right;
        c = left;
        ((TreeExpr *)right)->left = NULL;
        ((TreeExpr *)right)->right = NULL;
        left = NULL;
		return new MultAndAddExpr(a,b,c);
	}

	if (infix_op->type == INFIX_MULT && (left->isConstant() || right->isConstant()))
    {
	    if (right->isConstant())
	        std::swap(left, right);
        float c = left->eval(-1,-1);
        Expr *expr = right;
        right = left = nullptr;
        return new MultConstExpr(expr, c);
    }

	return this;
}

/* Evaluates an expression tree */
float TreeExpr::eval ( int mesh_i, int mesh_j )
{
	float left_arg, right_arg;

	/* shouldn't be null if we've called _optimize() */
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
}

llvm::Value *TreeExpr::_llvm(JitContext &jitx)
{
	llvm::Value *lhs = left->_llvm(jitx);
	llvm::Value *rhs = right->_llvm(jitx);
	if (nullptr == lhs || nullptr == rhs)
		return nullptr;
	switch ( infix_op->type )
	{
	case INFIX_ADD:
		return jitx.builder.CreateFAdd(lhs, rhs);
	case INFIX_MINUS:
		return jitx.builder.CreateFSub(lhs, rhs);
	case INFIX_MULT:
		return jitx.builder.CreateFMul(lhs, rhs);
	case INFIX_MOD:
		return Expr::_llvm(jitx);
	case INFIX_OR:
		return Expr::_llvm(jitx);
	case INFIX_AND:
		return Expr::_llvm(jitx);
	case INFIX_DIV:
		return Expr::_llvm(jitx);
	default:
		return nullptr;
	}
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
	if ( param == NULL )
		return NULL;

	switch ( param->type )
	{
		case P_TYPE_BOOL:
		    return param->getExpr();
			//return new BoolParameterExpr( PARAM_TERM_T, &term );
		case P_TYPE_INT:
            return param->getExpr();
			//return new IntParameterExpr( PARAM_TERM_T, &term );
		case P_TYPE_DOUBLE:
            return param->getExpr();
            //return new FloatParameterExpr( PARAM_TERM_T, &term );
		default:
			return NULL;
	}
}

/* Converts a prefix function to an expression */
Expr * Expr::prefun_to_expr ( float ( *func_ptr ) ( void * ), Expr ** expr_list, int num_args )
{
    PrefunExpr *prefun_expr;
    if (num_args == 1)
    {
        if (func_ptr == (float (*)(void *)) FuncWrappers::sin_wrapper)
            prefun_expr = new SinExpr();
        else if (func_ptr == (float (*)(void *)) FuncWrappers::cos_wrapper)
            prefun_expr = new CosExpr();
        else if (func_ptr == (float (*)(void *)) FuncWrappers::log_wrapper)
            prefun_expr = new LogExpr();
        else
            prefun_expr = new PrefunExprOne();
    }
    else if (num_args == 2)
    {
        if (func_ptr == (float (*)(void *)) FuncWrappers::pow_wrapper)
            prefun_expr = new PowExpr();
        else
            prefun_expr = new PrefunExpr();
    }
    else
    {
        prefun_expr = new PrefunExpr();
    }

	prefun_expr->num_args = num_args;
	prefun_expr->func_ptr = ( float ( * ) ( void* ) ) func_ptr;
	prefun_expr->expr_list = expr_list;
	return prefun_expr;
}

/* Creates a new tree expression */
TreeExpr::TreeExpr ( InfixOp * _infix_op, Expr * _gen_expr, Expr * _left, Expr * _right ) :
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
	TreeExprMult( Expr * _left, Expr * _right ) :
	 	TreeExpr( Eval::infix_mult, nullptr, (TreeExpr *)_left, (TreeExpr *)_right) {}		// TODO fix TreeExpr constructor to avoid bogus cast
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
			return new TreeExprMult( _left, _right);
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
		Expr::delete_expr(expr_list[i]);
	}
	free ( expr_list );
}

/* Frees a tree expression */
TreeExpr::~TreeExpr()
{

	/* free left tree */
	if ( left != NULL )
	{
		Expr::delete_expr(left);
	}

	/* free general expression object */
	if ( gen_expr != NULL )
	{
		Expr::delete_expr(gen_expr);
	}

	/* Note that infix operators are always
	   stored in memory unless the program
	   exits, so we don't remove them here */

	/* free right tree */
	if ( right != NULL )
	{
		Expr::delete_expr(right);
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

bool isConstantFn(float (* fn)(void*))
{
    return (float (*)(float *))fn != FuncWrappers::print_wrapper &&
           (float (*)(float *))fn != FuncWrappers::rand_wrapper;
}

Expr *PrefunExpr::_optimize()
{
	bool constant_args = true;
	for (int i=0 ; i < num_args ; i++)
	{
		Expr *orig = expr_list[i];
		expr_list[i] = orig->_optimize();
		if (orig != expr_list[i])
            Expr::delete_expr(orig);
		constant_args = constant_args && expr_list[i]->isConstant();
	}
	if (constant_args && isConstantFn(func_ptr))
    {
        return Expr::const_to_expr(eval(-1, -1));
    }
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


AssignExpr::AssignExpr(LValue *lhs_, Expr *rhs_) : Expr(ASSIGN), lhs(lhs_), rhs(rhs_) {}

AssignExpr::~AssignExpr()
{
    Expr::delete_expr(lhs);
    Expr::delete_expr(rhs);
}

Expr * AssignExpr::_optimize()
{
    Expr *t = rhs->_optimize();
    if (t != rhs)
        Expr::delete_expr(rhs);
    rhs = t;
    return this;
}

float AssignExpr::eval(int mesh_i, int mesh_j)
{
    float v = rhs->eval( mesh_i, mesh_j );
    lhs->set( v );
    return v;
}

std::ostream& AssignExpr::to_string(std::ostream &out)
{
	out << lhs << " = " << rhs;
	return out;
}

AssignMatrixExpr::AssignMatrixExpr(LValue *lhs_, Expr *rhs_) : AssignExpr(lhs_, rhs_) {}

float AssignMatrixExpr::eval(int mesh_i, int mesh_j)
{
	float v = rhs->eval( mesh_i, mesh_j );
	lhs->set_matrix( mesh_i, mesh_j, v );
	return v;
}

std::ostream& AssignMatrixExpr::to_string(std::ostream &out)
{
	out << lhs << "[i,j] = " << rhs;
	return out;
}





// TESTS


#include <TestRunner.hpp>

#ifndef NDEBUG

#define TEST(cond) if (!verify(#cond,cond)) return false


struct ExprTest : public Test
{
	ExprTest() : Test("ExprTest")
	{}

public:
    bool optimize_constant_expr()
    {
        TreeExpr *a = TreeExpr::create(nullptr, Expr::const_to_expr( 1.0 ), nullptr, nullptr);
        TreeExpr *b = TreeExpr::create(nullptr, Expr::const_to_expr( 2.0 ), nullptr, nullptr);
	    Expr *c = TreeExpr::create(Eval::infix_add, nullptr, a, b);
        //TEST(3.0f == c->eval(-1,-1));
        Expr *x = Expr::optimize(c);
        TEST(x != c);
		Expr::delete_expr(c);
        TEST(x->clazz == CONSTANT);
        TEST(3.0f == x->eval(-1,-1));
		Expr::delete_expr(x);

        Expr **expr_array = (Expr **)malloc(sizeof(Expr *));
        expr_array[0] = TreeExpr::create(nullptr, Expr::const_to_expr( (float)M_PI ), nullptr, nullptr);
        Expr *sin = Expr::prefun_to_expr((float (*)(void *))FuncWrappers::sin_wrapper, expr_array, 1);
        x = Expr::optimize(sin);
        TEST(x != sin);
        Expr::delete_expr( sin );
        TEST(x->clazz == CONSTANT);
        TEST(sinf( (float)M_PI ) == x->eval(-1,-10));
        Expr::delete_expr(x);

        // make sure rand() is not optimized away
        expr_array = (Expr **)malloc(sizeof(Expr *));
		expr_array[0] = TreeExpr::create(nullptr, Expr::const_to_expr( (float)M_PI ), nullptr, nullptr);
		Expr *rand = Expr::prefun_to_expr((float (*)(void *))FuncWrappers::rand_wrapper, expr_array, 1);
		x = Expr::optimize(rand);
		TEST(x == rand);
		TEST(x->clazz != CONSTANT);
		Expr::delete_expr(x);

        return true;
    }

    bool jit()
    {
		float bound, result;
		CValue iv, lb, ub;
		iv.float_val = 0; lb.float_val=-1000000; ub.float_val=1000000;

		Expr *jitExpr;
	    Expr *CONST = Expr::const_to_expr(3.0f);
		jitExpr = Expr::jit(CONST);
	    TEST(3.0f == jitExpr->eval(-1,-1));
	    delete jitExpr;

		bound = 4.0f;
	    Expr *PARAM = Param::create("test", P_TYPE_DOUBLE, P_FLAG_NONE, &bound, nullptr, iv,ub,lb);
		jitExpr = Expr::jit(PARAM);
	    TEST(4.0f == jitExpr->eval(-1,-1));
		delete jitExpr;

	    Expr *MULT = new TreeExprMult(CONST, PARAM);
		jitExpr = Expr::jit(MULT);
	    TEST(12.0f == jitExpr->eval(-1,-1));
		delete jitExpr;
    }

	bool test() override
	{
        Eval::init_infix_ops();
	    bool result = true;
	    result &= optimize_constant_expr();
	    result &= jit();
		return result;
	}
};

Test* Expr::test()
{
	return new ExprTest();
}

#else

Test* Expr::test()
{
    return null;
}

#endif




// JIT!

using namespace llvm;


class JitExpr : public Expr
{
	ExecutionEngine *engine;
	Expr *expr;
	float (*fn)(int,int);

public:
	JitExpr(ExecutionEngine *engine_, Expr *orig, float (*fn_)(int,int)) : Expr(OTHER), engine(engine_), expr(orig), fn(fn_)
	{

	}

	float eval(int mesh_i, int mesh_j) override
	{
		return fn(mesh_i, mesh_j);
	}

	~JitExpr() override
	{
		Expr::delete_expr(expr);
		delete engine;
	}
};


__attribute__((noinline)) float eval_thunk(Expr *e, int i, int j)
{
	return e->eval(i,j);
}

__attribute__((noinline)) void set_matrix_thunk(LValue *lvalue, int i, int j, float v)
{

    return lvalue->set_matrix(i, j, v);
}


LLVMContext *llvmGLobalContext;

LLVMContext& getGlobalContext()
{
	if (nullptr == llvmGLobalContext)
	{
		InitializeNativeTarget();
		LLVMInitializeX86TargetInfo();
		LLVMInitializeX86Target();
		LLVMInitializeX86TargetMC();
		LLVMInitializeNativeAsmPrinter();
		LLVMInitializeNativeAsmParser();

		llvmGLobalContext = new LLVMContext();
	}
	return *llvmGLobalContext;
}


Value * Expr::generate_eval_call(JitContext &jitx, Expr *expr)
{
	// turn this into "void *"
	ConstantInt * thisConstant = ConstantInt::get(IntegerType::getInt64Ty(jitx.context), (uint64_t)expr, false);

	// thunk_expr into float ()(void *,int, int)
	ConstantInt * thunkConstant = ConstantInt::get(IntegerType::getInt64Ty(jitx.context), (uint64_t)eval_thunk, false);
	// TODO create type once
	std::vector<Type*> exprEvalFunctionArgs;
	exprEvalFunctionArgs.push_back(IntegerType::getInt64Ty(jitx.context));    // Expr *
	exprEvalFunctionArgs.push_back(IntegerType::getInt32Ty(jitx.context));
	exprEvalFunctionArgs.push_back(IntegerType::getInt32Ty(jitx.context));
	auto evalFunctionType = FunctionType::get(jitx.floatType, exprEvalFunctionArgs, false);
	auto evalFunctionPtrType = PointerType::get(evalFunctionType,1);
	Value* thunkFunctionPtr = llvm::ConstantExpr::getIntToPtr(thunkConstant , evalFunctionPtrType);

	std::vector<Value *> args;
	args.push_back(thisConstant);
    args.push_back(jitx.mesh_i);
    args.push_back(jitx.mesh_j);
	Value *ret = jitx.builder.CreateCall(thunkFunctionPtr, args);
	return ret;
}


Expr *Expr::jit(Expr *root)
{
	LLVMContext &Context = getGlobalContext();

	// Create some module to put our function into it.
	std::unique_ptr<Module> module_ptr = make_unique<Module>("ExprEvalTest", Context);

	std::vector<Type *> arg_typess;
	arg_typess.push_back(IntegerType::get(Context,32));
	arg_typess.push_back(IntegerType::get(Context,32));
	Constant* c = module_ptr->getOrInsertFunction<Type*>("Expr_eval",
			Type::getFloatTy(Context),
			IntegerType::get(Context,32),
			IntegerType::get(Context,32));
	Function *expr_eval_fun = cast<Function>(c);
	BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", expr_eval_fun);
	IRBuilder<> builder(BB);
	builder.SetInsertPoint(BB);

	Argument *i = &expr_eval_fun->arg_begin()[0];
	Argument *j = &expr_eval_fun->arg_begin()[1];
	JitContext jitx( Context, builder, i, j);
    Value *retValue = root->_llvm( jitx );
    if (nullptr == retValue)
    	return nullptr;
	builder.CreateRet(retValue);

	// Now we create the JIT.
	ExecutionEngine* executionEngine = EngineBuilder(std::move(module_ptr)).create();

	outs() << "We just constructed this LLVM module:\n\n" << *module_ptr;
	outs() << "\n\nRunning foo: ";
	outs().flush();

	auto fn = (float (*)(int,int))executionEngine->getFunctionAddress("Expr_eval");
	auto v  = (*fn)(12,45);
	outs() << "Result: " << v << "\n";

	return new JitExpr(executionEngine, root, fn);
}

/*

  return Builder->CreateSelect(Builder->CreateICmp(Pred, A, B), A, B);

 */