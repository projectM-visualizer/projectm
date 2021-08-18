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

#include "config.h"
#include "CValue.hpp"
#include "Func.hpp"
#include <iostream>
#include <vector>

class Test;
class Param;
class LValue;
class JitContext;

#ifdef HAVE_LLVM
namespace llvm {
  class Value;
}
#endif

#define CONST_STACK_ELEMENT 0
#define EXPR_STACK_ELEMENT 1

#define EVAL_ERROR -1

/* Infix Operator Function */
class InfixOp
{
public:
  int type;
  int precedence;

  InfixOp( int type, int precedence );
};

/** Term */
class Term
{
public:
  float constant; /* static variable */
  Param *param; /* pointer to a changing variable */

  Term() { this->constant = 0; this->param = 0; }
};
 

enum ExprClass
{
  TREE, CONSTANT, PARAMETER, FUNCTION, ASSIGN, PROGRAM, JIT, OTHER
};

class Expr
{
public:
  ExprClass clazz;
  explicit Expr(ExprClass c) : clazz(c) {};
  virtual ~Expr() = default;

  virtual bool isConstant() { return false; };
  virtual float eval(int mesh_i, int mesh_j) = 0;
  virtual std::ostream& to_string(std::ostream &out)
  {
      std::cout << "nyi"; return out;
  }

  static Test *test();
  static Expr *const_to_expr( float val );
  static Expr *param_to_expr( Param *param );
  static Expr *prefun_to_expr( Func *func, Expr **expr_list );
  static Expr *create_assignment(class LValue *lhs, Expr *rhs);
  static Expr *create_matrix_assignment(class LValue *lhs, Expr *rhs);
  // TODO eventually the ownSteps param needs to go away (but for now the individual expressions might be held by the preset)
  static Expr *create_program_expr(std::vector<Expr*> &steps_, bool ownSteps);

  static void delete_expr(Expr *expr) { if (nullptr != expr) expr->_delete_from_tree(); }
  static Expr *optimize(Expr *root);
  static Expr *jit(Expr *root, std::string name="Expr::jit");

public: // but don't call these from outside Expr.cpp

  virtual Expr *_optimize() { return this; };
#if HAVE_LLVM
  static  llvm::Value *llvm(JitContext &jit, Expr *);
  virtual llvm::Value *_llvm(JitContext &jit) = 0;  //ONLY called by llvm()
  static llvm::Value *generate_eval_call(JitContext &jit, Expr *expr, const char *name=nullptr);
  static llvm::Value *generate_set_call(JitContext &jitx, Expr *expr, llvm::Value *value);
  static llvm::Value *generate_set_matrix_call(JitContext &jitx, Expr *expr, llvm::Value *value);
#endif

  // override if this expr is not 'owned' by the containg expression tree
  virtual void _delete_from_tree()
  {
    delete this;
  }
};


inline std::ostream& operator<<(std::ostream& out, Expr *expr)
{
  if (nullptr == expr)
      out << "NULL";
  else 
      expr->to_string(out);
  return out;
}


/* A binary expression tree ordered by operator precedence */
class TreeExpr : public Expr
{
protected:
  TreeExpr( InfixOp *infix_op, Expr *gen_expr, Expr *left, Expr *right );
public:
  static TreeExpr *create( InfixOp *infix_op, Expr *gen_expr, TreeExpr *left, TreeExpr *right );
  static TreeExpr *create( InfixOp *infix_op, Expr *left, Expr *right );

  InfixOp * infix_op; /* null if leaf */
  Expr * gen_expr;
  // NOTE: before optimize() left and right will always be TreeExpr
  Expr *left, *right;
  // these are for type-safe access in Parser.cpp
  TreeExpr *leftTree()  { return dynamic_cast<TreeExpr *>(left); }
  TreeExpr *rightTree() { return dynamic_cast<TreeExpr *>(right); }

  ~TreeExpr() override;
  
  Expr *_optimize() override;
  float eval(int mesh_i, int mesh_j) override;
#if HAVE_LLVM
  llvm::Value *_llvm(JitContext &jitx) override;
#endif
  std::ostream& to_string(std::ostream &out) override;
};


class LValue : public Expr
{
public:
    explicit LValue(ExprClass c) : Expr(c) {};
    virtual void set(float value) = 0;
    virtual void set_matrix(int mesh_i, int mesh_j, float value) = 0;
#if HAVE_LLVM
    virtual llvm::Value *_llvm_set_matrix(JitContext &jitx, llvm::Value *rhs)
    {
        Expr::generate_set_matrix_call(jitx, this, rhs);
        return rhs;
    }
#endif
};



#endif /** _EXPR_H */
