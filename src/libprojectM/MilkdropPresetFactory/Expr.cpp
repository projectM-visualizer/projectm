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

#include "Eval.hpp"
#include "BuiltinFuncs.hpp"

// #ifdef USE_LLVM
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"


llvm::LLVMContext& getGlobalContext();

// Wrapper for one module which corresponds to one jit'd Expr
// TODO consider associating one JitContext with one Preset
struct JitContext
{
//    JitContext(llvm::LLVMContext &context_, llvm::Module *module_, llvm::IRBuilder<llvm::ConstantFolder,llvm::IRBuilderDefaultInserter> &builder_, llvm::Value *i, llvm::Value *j) :
//        context(context_), module(module_), builder(context_), mesh_i(i), mesh_j(j)
//    {
//        floatType = llvm::Type::getFloatTy(context);
//    }

    JitContext(std::string name="LLVMModule") :
            context(getGlobalContext()), builder(getGlobalContext())
    {
        floatType = llvm::Type::getFloatTy(context);
        module_ptr = llvm::make_unique<llvm::Module>(name, context);
        module = module_ptr.get();

//        module->setDataLayout(getTargetMachine().createDataLayout());

        // Create a new pass manager attached to it.
        fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(module);
        fpm->add(llvm::createInstructionCombiningPass());
        fpm->add(llvm::createReassociatePass());
        fpm->add(llvm::createGVNPass());
        fpm->add(llvm::createCFGSimplificationPass());
        fpm->doInitialization();
    }

    // NOTE: the module is either "owned" by module_ptr OR executionEngine
    llvm::LLVMContext &context;
    std::unique_ptr<llvm::Module> module_ptr;
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
    llvm::Module *module;
    llvm::IRBuilder<llvm::ConstantFolder,llvm::IRBuilderDefaultInserter> builder;
    llvm::Type *floatType;
    llvm::Value *mesh_i;
    llvm::Value *mesh_j;

    // helpers

    void OptimizePass()
    {
        auto end = module->end();
        for (auto it = module->begin(); it != end; ++it)
            fpm->run(*it);
    }

    llvm::Value *CreateConstant(float x)
    {
        return llvm::ConstantFP::get(floatType, x);
    }
    llvm::Value *CreateConstant(int i32)
    {
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), (uint64_t)(int64_t)i32);
    }
    llvm::Value *CallIntrinsic(llvm::Intrinsic::ID id, llvm::Value *value)
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(floatType);
        llvm::Function *sinFunction = llvm::Intrinsic::getDeclaration(module, id, arg_type);
        std::vector<llvm::Value *> arg;
        arg.push_back(value);
        return builder.CreateCall(sinFunction, arg);
    }
    llvm::Value *CallIntrinsic2(llvm::Intrinsic::ID id, llvm::Value *x, llvm::Value *y)
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(floatType);
        arg_type.push_back(floatType);
        llvm::Function *sinFunction = llvm::Intrinsic::getDeclaration(module, id, arg_type);
        std::vector<llvm::Value *> arg;
        arg.push_back(x);
        arg.push_back(y);
        return builder.CreateCall(sinFunction, arg);
    }

    llvm::Function *parent=nullptr;
    llvm::BasicBlock *then_block=nullptr;
    llvm::BasicBlock *else_block=nullptr;
    llvm::BasicBlock *merge_block=nullptr;

    void StartTernary(llvm::Value *condition)
    {
        parent = builder.GetInsertBlock()->getParent();
        then_block = llvm::BasicBlock::Create(context, "then", parent);
        else_block = llvm::BasicBlock::Create(context, "else");
        merge_block = llvm::BasicBlock::Create(context, "ifcont");
        builder.CreateCondBr(condition, then_block, else_block);
    }
    void withThen()
    {
        builder.SetInsertPoint(then_block);
    }
    void withElse()
    {
        // finish the withThen block
        builder.CreateBr(merge_block);
        parent->getBasicBlockList().push_back(else_block);
        builder.SetInsertPoint(else_block);
    }
    llvm::Value *FinishTernary(llvm::Value *thenValue, llvm::Value *elseValue)
    {
        // finish the withElse block
        builder.CreateBr(merge_block);
        parent->getBasicBlockList().push_back(merge_block);
        builder.SetInsertPoint(merge_block);
        llvm::PHINode *mergeValue = builder.CreatePHI(floatType, 2, "iftmp");
        mergeValue->addIncoming(thenValue, then_block);
        mergeValue->addIncoming(elseValue, else_block);
        parent = nullptr;
        then_block = else_block = merge_block = nullptr;
        return mergeValue;
    }
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

llvm::Value *PrefunExpr::_llvm(JitContext &jitx)
{
    //llvm::ArrayType *array_ty = llvm::ArrayType::get(jitx.floatType, num_args);
    //llvm::AllocaInst *array = jitx.builder.CreateAlloca(array_ty);
    llvm::AllocaInst *array = jitx.builder.CreateAlloca(jitx.floatType, jitx.CreateConstant(num_args));
    for ( unsigned i = 0; i < (unsigned)num_args; i++ )
    {
        llvm::Value *v = Expr::llvm(jitx,expr_list[i]);
        if (nullptr == v)
            return nullptr;
        llvm::ConstantInt *idx = llvm::ConstantInt::get(llvm::Type::getInt32Ty(jitx.context), i);
        jitx.builder.CreateStore(v, jitx.builder.CreateGEP(jitx.floatType, array, idx));
    }

    std::vector<llvm::Type *> arg_types;
    arg_types.push_back(llvm::PointerType::get(jitx.floatType,1)); // float *
    auto prefun_type = llvm::FunctionType::get(jitx.floatType, arg_types, false);
    auto prefun_ptr_type = llvm::PointerType::get(prefun_type,1);
    llvm::Constant *fn_const = llvm::ConstantInt::get(llvm::Type::getInt64Ty(jitx.context), (uint64_t)func_ptr);
    auto function_ptr = llvm::ConstantExpr::getIntToPtr(fn_const , prefun_ptr_type);

    std::vector<llvm::Value *> args;
    args.push_back(array);
    return jitx.builder.CreateCall(function_ptr, args);
}


class PrefunExprOne : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j )
    {
        float val = expr_list[0]->eval ( mesh_i, mesh_j );
        return (func_ptr)(&val);
    }
};

// short circuiting IF
// TODO consider IfAboveExpr, IfEqualExpr
class IfExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j )
    {
        // see if_wrapper()
        float val = expr_list[0]->eval ( mesh_i, mesh_j );
        if (val == 0)
            return expr_list[2]->eval ( mesh_i, mesh_j );
        return expr_list[1]->eval ( mesh_i, mesh_j );
    }
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *testValue = Expr::llvm(jitx, expr_list[0]);
        if (nullptr == testValue)
            return nullptr;
        llvm::Value *condition = jitx.builder.CreateFCmpUNE(testValue, jitx.CreateConstant(0.0f));
        jitx.StartTernary(condition);
        jitx.withThen();
        llvm::Value *thenValue = Expr::llvm(jitx, expr_list[1]);
        jitx.withElse();
        llvm::Value *elseValue = Expr::llvm(jitx, expr_list[2]);
        if (nullptr == thenValue || nullptr == elseValue)
            return nullptr;
        return jitx.FinishTernary(thenValue, elseValue);
        // CONSIDER use switch if left and right are both constant?
        //return jitx.builder.CreateSelect(jitx.builder.CreateICmpNEQ(zeroInt,expr0), expr1, expr2);
    }
};

class SinExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j ) override
    {
        float val = expr_list[0]->eval ( mesh_i, mesh_j );
        return sinf(val);
    }
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *val = Expr::llvm(jitx, expr_list[0]);
        if (nullptr == val)
            return nullptr;
        return jitx.CallIntrinsic(llvm::Intrinsic::sin, val);
    }
};

class CosExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j ) override
    {
        float val = expr_list[0]->eval ( mesh_i, mesh_j );
        return cosf(val);
    }
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *val = Expr::llvm(jitx, expr_list[0]);
        if (nullptr == val)
            return nullptr;
        return jitx.CallIntrinsic(llvm::Intrinsic::cos, val);
    }
};

class LogExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j ) override
    {
        float val = expr_list[0]->eval( mesh_i, mesh_j );
        return logf(val);
    }
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *val = Expr::llvm(jitx, expr_list[0]);
        if (nullptr == val)
            return nullptr;
        return jitx.CallIntrinsic(llvm::Intrinsic::log, val);
    }
};

class PowExpr : public PrefunExpr
{
    float eval ( int mesh_i, int mesh_j ) override
    {
        float x = expr_list[0]->eval( mesh_i, mesh_j );
        float y = expr_list[1]->eval( mesh_i, mesh_j );
        return powf(x, y);
    }
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *x = Expr::llvm(jitx, expr_list[0]);
        llvm::Value *y = Expr::llvm(jitx, expr_list[1]);
        if (nullptr == x || nullptr == y)
            return nullptr;
        return jitx.CallIntrinsic2(llvm::Intrinsic::pow, x, y);
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
        return jitx.CreateConstant(constant);
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

    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *avalue = Expr::llvm(jitx, a);
        llvm::Value *bvalue = Expr::llvm(jitx, b);
        llvm::Value *cvalue = Expr::llvm(jitx, c);
        if (nullptr == avalue || nullptr == bvalue || nullptr == cvalue)
            return nullptr;
        return jitx.builder.CreateFAdd(jitx.builder.CreateFMul(avalue,bvalue),cvalue);
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
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *value = Expr::llvm(jitx, expr);
        if (nullptr == value)
            return nullptr;
        return jitx.builder.CreateFMul(jitx.CreateConstant(c), value);
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
                return 0;
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
    llvm::Value *lhs = Expr::llvm(jitx, left);
    llvm::Value *rhs = Expr::llvm(jitx, right);
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
    {
        llvm::Type *int32_ty = llvm::IntegerType::get(jitx.context,32);
        llvm::Value *rhsInt = jitx.builder.CreateFPToSI(jitx.builder.CreateIntrinsic(llvm::Intrinsic::floor, rhs), int32_ty);
        // BUG??? Calling CreateICmpNE without the floor() (just FPToSI()) causes stack overflow, bug in llvm::ConstantFoldCompareInstruction()?
        llvm::Value *condNotZero = jitx.builder.CreateICmpNE(jitx.CreateConstant(0), rhsInt, "ifcond");
        jitx.StartTernary(condNotZero);
        jitx.withThen();
        llvm::Value *lhsInt = jitx.builder.CreateFPToSI(jitx.builder.CreateIntrinsic(llvm::Intrinsic::floor, lhs), int32_ty);
        // TODO check the semantics of C operator % (might not be the same as srem)
        llvm::Value *thenValue = jitx.builder.CreateSIToFP(jitx.builder.CreateSRem(lhsInt,rhsInt), jitx.floatType);
        jitx.withElse();
        llvm::Value *elseValue = jitx.CreateConstant(0.0f);
        return jitx.FinishTernary(thenValue, elseValue);
    }
    case INFIX_OR:
    {
        // bit-wise integer OR
        llvm::Type *int32_ty = llvm::IntegerType::get(jitx.context,32);
        llvm::Value *lhsInt = jitx.builder.CreateFPToSI(lhs, int32_ty);
        llvm::Value *rhsInt = jitx.builder.CreateFPToSI(rhs, int32_ty);
        llvm::Value *i = jitx.builder.CreateOr(lhsInt,rhsInt);
        return jitx.builder.CreateSIToFP(i, jitx.floatType);
    }
    case INFIX_AND:
    {
        // bit-wise integer AND
        llvm::Type *int32_ty = llvm::IntegerType::get(jitx.context, 32);
        llvm::Value *lhsInt = jitx.builder.CreateFPToSI(lhs, int32_ty);
        llvm::Value *rhsInt = jitx.builder.CreateFPToSI(rhs, int32_ty);
        llvm::Value *i = jitx.builder.CreateAnd(lhsInt, rhsInt);
        return jitx.builder.CreateSIToFP(i, jitx.floatType);
    }
    case INFIX_DIV:
    {
        llvm::Value *condNotZero = jitx.builder.CreateICmpNE(jitx.CreateConstant(0.0f), rhs, "ifcond");
        jitx.StartTernary(condNotZero);
        jitx.withThen();
        llvm::Value *thenValue = jitx.builder.CreateFDiv(lhs,rhs);
        jitx.withElse();
        llvm::Value *elseValue = jitx.CreateConstant((float)MAX_DOUBLE_SIZE);
        return jitx.FinishTernary(thenValue, elseValue);
    }
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
    else if (num_args == 3)
    {
        if (func_ptr == (float (*)(void *)) FuncWrappers::if_wrapper)
            prefun_expr = new IfExpr();
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
    TreeExprAdd( Expr * _left, Expr * _right ) :
         TreeExpr( Eval::infix_add, nullptr, _left, _right) {}
    float eval( int mesh_i, int mesh_j)
    {
        return left->eval(mesh_i, mesh_j) + right->eval(mesh_i, mesh_j);
    }
    llvm::Value *_llvm(JitContext &jitx) override
    {
        llvm::Value *l = Expr::llvm(jitx, left);
        llvm::Value *r = Expr::llvm(jitx, right);
        if (nullptr == l || nullptr == r)
            return nullptr;
        return jitx.builder.CreateFAdd(l,r);
    }
};

class TreeExprMinus : public TreeExpr
{
public:
    TreeExprMinus( Expr * _left, Expr * _right ) :
         TreeExpr( Eval::infix_minus, nullptr, _left, _right) {}
    float eval( int mesh_i, int mesh_j)
    {
        return left->eval(mesh_i, mesh_j) - right->eval(mesh_i, mesh_j);
    }
};

class TreeExprMult : public TreeExpr
{
public:
    TreeExprMult( Expr * _left, Expr * _right ) :
         TreeExpr( Eval::infix_mult, nullptr, (TreeExpr *)_left, (TreeExpr *)_right) {}        // TODO fix TreeExpr constructor to avoid bogus cast
    float eval( int mesh_i, int mesh_j)
    {
        return left->eval(mesh_i, mesh_j) * right->eval(mesh_i, mesh_j);
    }
};

// NOTE: Parser expects left and right to be TreeExpr, but other code paths don't require this
TreeExpr * TreeExpr::create( InfixOp * _infix_op, Expr * _gen_expr, TreeExpr * _left, TreeExpr * _right )
{
    if ( NULL != _infix_op )
    {
        // TODO move to TreeExpr::optimize()
        if ( _infix_op->type == INFIX_ADD )
            return new TreeExprAdd( _left, _right);
        if ( _infix_op->type == INFIX_MINUS )
            return new TreeExprMinus( _left, _right);
        if ( _infix_op->type == INFIX_MULT )
            return new TreeExprMult( _left, _right);
    }
    return new TreeExpr( _infix_op, _gen_expr, _left, _right );
}

TreeExpr * TreeExpr::create( InfixOp * _infix_op, Expr * _left, Expr * _right )
{
    assert( nullptr != _infix_op );
    // TODO move to TreeExpr::optimize()
    if ( _infix_op->type == INFIX_ADD )
        return new TreeExprAdd( _left, _right);
    if ( _infix_op->type == INFIX_MINUS )
        return new TreeExprMinus( _left, _right);
    if ( _infix_op->type == INFIX_MULT )
        return new TreeExprMult( _left, _right);
    return new TreeExpr( _infix_op, nullptr, _left, _right );
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

llvm::Value *AssignMatrixExpr::_llvm(JitContext &jitx)
{
    llvm::Value *value = Expr::llvm(jitx, rhs);
    if (nullptr == value)
        return nullptr;
    return Expr::generate_set_matrix_call(jitx, this, value);
}

/* caller only has to delete returned expression.  optimize() will delete unused nodes in original expr;
 * CONSIDER delete old expr for caller?
 */
Expr *Expr::optimize(Expr *expr)
{
    Expr *optimized = expr->_optimize();
    if (optimized != expr)
        Expr::delete_expr(expr);
    return optimized;
}


llvm::Value *ProgramExpr::_llvm(JitContext &jitx)
{
    llvm::Value *v = jitx.CreateConstant(0.0f);
    for (auto it=steps.begin() ; it<steps.end() ; it++)
        v = Expr::llvm(jitx, *it);
    return v;
}


// TESTS

#include <TestRunner.hpp>

#ifndef NDEBUG

#define TEST(cond) if (!verify(#cond,cond)) return false


struct ExprTest : public Test
{
    bool eq(float a, float b)
    {
        return std::abs(a-b) < (std::abs(a)+std::abs(b) + 1)/1000.0f;
    }

    Expr *constant(float f)
    {
        return Expr::const_to_expr(3.0f);
    }

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
		c = nullptr;
        TEST(x->clazz == CONSTANT);
        TEST(3.0f == x->eval(-1,-1));
        Expr::delete_expr(x);

        Expr **expr_array = (Expr **)malloc(sizeof(Expr *));
        expr_array[0] = TreeExpr::create(nullptr, Expr::const_to_expr( (float)M_PI ), nullptr, nullptr);
        Expr *sin = Expr::prefun_to_expr((float (*)(void *))FuncWrappers::sin_wrapper, expr_array, 1);
        x = Expr::optimize(sin);
        TEST(x != sin);
        sin = nullptr;
        TEST(x->clazz == CONSTANT);
        TEST(sinf( (float)M_PI ) == x->eval(-1,-10));
        Expr::delete_expr(x);

        // make sure rand() is not optimized away
        expr_array = (Expr **)malloc(sizeof(Expr *));
        expr_array[0] = TreeExpr::create(nullptr, Expr::const_to_expr( (float)M_PI ), nullptr, nullptr);
        Expr *rand = Expr::prefun_to_expr((float (*)(void *))FuncWrappers::rand_wrapper, expr_array, 1);
        x = Expr::optimize(rand);
        TEST(x == rand);
        rand = nullptr;
        TEST(x->clazz != CONSTANT);
        Expr::delete_expr(x);

        return true;
    }

//#ifdef USE_LLVM
    bool jit()
    {
        {
        Expr *CONST = Expr::const_to_expr(3.0f);
        Expr *jitExpr = Expr::jit(CONST);
        TEST(3.0f == jitExpr->eval(-1,-1));
        delete jitExpr;    // jitExpr now owns CONST, TODO make behavior consistent with optimize()
        }

        {
        Param *PARAM = Param::createUser("test");
        PARAM->set_param(4.0f);
        Expr *jitExpr = Expr::jit(PARAM);
        TEST(4.0f == jitExpr->eval(-1,-1));
        delete jitExpr;
        }

        {
        Expr *CONST = Expr::const_to_expr(3.0f);
        Param *PARAM = Param::createUser("test");
        PARAM->set_param(4.0f);
        Expr *MULT = new TreeExprMult(CONST, PARAM);
        Expr *jitExpr = Expr::jit(MULT);
        TEST(12.0f == jitExpr->eval(-1,-1));
        delete jitExpr;
        }

        {
        Expr *CONST = Expr::const_to_expr(3.0f);
        Param *PARAM = Param::createUser("test");
        PARAM->set_param(4.0f);
        Expr *ASSIGN = new AssignMatrixExpr(PARAM, CONST);
        Expr *jitExpr = Expr::jit(ASSIGN);
        TEST(3.0f == jitExpr->eval(-1,-1));
        TEST(3.0f == PARAM->eval(-1,-1));
        delete jitExpr;
        }

        {
        Expr *CONST = Expr::const_to_expr(3.0f);
        SinExpr *SIN = new SinExpr();    // TODO constructor
        SIN->num_args = 1;
        SIN->func_ptr = ( float ( * ) ( void* ) ) FuncWrappers::sin_wrapper;
        SIN->expr_list = (Expr **)malloc(1 * sizeof(Expr *));
        SIN->expr_list[0] = CONST;
        Expr *jitExpr = Expr::jit(SIN);
        TEST(sinf(3.0f) == jitExpr->eval(-1,-1));
        delete jitExpr;
        }

        // test_prefun_if:
        {
            Expr *CONST1 = Expr::const_to_expr(1.0f);
            Expr *CONST2 = Expr::const_to_expr(2.0f);
            Expr *CONST3 = Expr::const_to_expr(3.0f);
            PrefunExpr *IF = new PrefunExpr();    // TODO constructor
            IF->num_args = 3;
            IF->func_ptr = ( float ( * ) ( void* ) ) FuncWrappers::if_wrapper;
            IF->expr_list = (Expr **)malloc(3 * sizeof(Expr *));
            IF->expr_list[0] = CONST1;
            IF->expr_list[1] = CONST2;
            IF->expr_list[2] = CONST3;
            Expr *jitExpr = Expr::jit(IF);
            TEST(2.0f == jitExpr->eval(-1,-1));
            delete jitExpr;
        }

        //test_ternary_mod_1:
        {
            Expr *CONST2 = Expr::const_to_expr(2.0f);
            Expr *CONST3 = Expr::const_to_expr(3.0f);
            TreeExpr *MOD = TreeExpr::create(Eval::infix_mod, CONST3, CONST2);
            Expr *jitExpr = Expr::jit(MOD);
            TEST(1.0f == jitExpr->eval(-1,-1));
            delete jitExpr;
        }

        //test_ternary_mod_2:
        {
            Expr *CONST0 = Expr::const_to_expr(0.0f);
            Expr *CONST3 = Expr::const_to_expr(3.0f);
            TreeExpr *MOD = TreeExpr::create(Eval::infix_mod, CONST3, CONST0);
            Expr *jitExpr = Expr::jit(MOD);
            TEST(0.0f == jitExpr->eval(-1,-1));
            delete jitExpr;
        }


        return true;
    }
//#endif

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
    return nullptr;
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
    JitExpr(ExecutionEngine *engine_, Expr *orig, float (*fn_)(int,int)) : Expr(JIT), engine(engine_), expr(orig), fn(fn_)
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

    Value *_llvm(JitContext &jit) override
    {
        assert(false);
        return nullptr;
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
    // use eval_thunk
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


Value * Expr::generate_set_matrix_call(JitContext &jitx, Expr *expr, Value *value)
{
    // turn this into "void *"
    ConstantInt * thisConstant = ConstantInt::get(IntegerType::getInt64Ty(jitx.context), (uint64_t)expr, false);

    // thunk_expr into float ()(void *,int, int, float)
    // use eval_thunk
    ConstantInt * thunkConstant = ConstantInt::get(IntegerType::getInt64Ty(jitx.context), (uint64_t)eval_thunk, false);
    // TODO create type once
    std::vector<Type*> setMatrixFunctionArgs;
    setMatrixFunctionArgs.push_back(IntegerType::getInt64Ty(jitx.context));    // Expr *
    setMatrixFunctionArgs.push_back(IntegerType::getInt32Ty(jitx.context));
    setMatrixFunctionArgs.push_back(IntegerType::getInt32Ty(jitx.context));
    setMatrixFunctionArgs.push_back(jitx.floatType);
    auto evalFunctionType = FunctionType::get(jitx.floatType, setMatrixFunctionArgs, false);
    auto evalFunctionPtrType = PointerType::get(evalFunctionType,1);

    Value* thunkFunctionPtr = llvm::ConstantExpr::getIntToPtr(thunkConstant , evalFunctionPtrType);

    std::vector<Value *> args;
    args.push_back(thisConstant);
    args.push_back(jitx.mesh_i);
    args.push_back(jitx.mesh_j);
    args.push_back(value);
    Value *ret = jitx.builder.CreateCall(thunkFunctionPtr, args);
    return ret;
}


/* this is mostly a passthrough, but can be used to intercept calls to Param */
Value *Expr::llvm(JitContext &jitx, Expr *root)
{
    return root->_llvm(jitx);
}


Expr *Expr::jit(Expr *root)
{
    LLVMContext &Context = getGlobalContext();

    // Create some module to put our function into it.
    JitContext jitx;

    std::vector<Type *> arg_typess;
    arg_typess.push_back(IntegerType::get(Context,32));
    arg_typess.push_back(IntegerType::get(Context,32));
    Constant* c = jitx.module->getOrInsertFunction<Type*>("Expr_eval",
            Type::getFloatTy(Context),
            IntegerType::get(Context,32),
            IntegerType::get(Context,32));
    auto *expr_eval_fun = cast<Function>(c);
    BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", expr_eval_fun);
    jitx.builder.SetInsertPoint(BB);
    jitx.mesh_i = &expr_eval_fun->arg_begin()[0];
    jitx.mesh_j = &expr_eval_fun->arg_begin()[1];

    // Generate IR Code!
    Value *retValue = Expr::llvm(jitx, root);
    if (nullptr == retValue)
    {
        // module is still owned by module_ptr and should be cleaned up
        return nullptr;
    }
    jitx.builder.CreateRet(retValue);


#if 1
    outs() << "MODULE\n\n" << *jitx.module << "\n\n";
	outs().flush();
#endif

	// and JIT!
	jitx.OptimizePass();
#if 1
    outs() << "MODULE AFTER\n\n" << *jitx.module << "\n\n";
    outs().flush();
#endif
    ExecutionEngine* executionEngine = EngineBuilder(std::move(jitx.module_ptr)).create();

/*
// Create a function pass manager for this engine
    FunctionPassManager *FPM = new FunctionPassManager();
    // Set up the optimizer pipeline.  Start with registering info about how the
// target lays out data structures.
    FPM->addPass(new DataLayout(executionEngine->getDataLayout()));
// Provide basic AliasAnalysis support for GVN.
    FPM->addPass(createBasicAAWrapperPass());
// Promote allocas to registers.
    FPM->addPass(createPromoteMemoryToRegisterPass());
// Do simple "peephole" optimizations and bit-twiddling optzns.
    FPM->addPass(createInstructionCombiningPass());
// Reassociate expressions.
    FPM->addPass(createReassociatePass());
// Eliminate Common SubExpressions.
    FPM->addPass(createGVNPass());
// Simplify the control flow graph (deleting unreachable blocks, etc).
    FPM->addPass(createCFGSimplificationPass());
//    FPM->doInitialization();
    // For each function in the module
    FunctionAnalysisManager FAM(false);
    Module::iterator end = module->end();
#if 1
    outs() << "MODULE AFTER\n\n" << *module_ptr << "\n\n";
    outs().flush();
#endif
*/



    auto fn = (float (*)(int,int))executionEngine->getFunctionAddress("Expr_eval");

    return new JitExpr(executionEngine, root, fn);
}

/*

  return Builder->CreateSelect(Builder->CreateICmp(Pred, A, B), A, B);

 */