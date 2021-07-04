//
// Created by matthew on 1/26/19.
//

#ifndef PROJECTM_JITCONTEXT_H
#define PROJECTM_JITCONTEXT_H

#if HAVE_LLVM
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
struct Symbol
{
    llvm::Value *value = nullptr;
    llvm::Value *assigned_value = nullptr;
};

struct JitContext
{
    // NOTE: the module is either "owned" by module_ptr OR executionEngine
    llvm::LLVMContext &context;
    std::unique_ptr<llvm::Module> module_ptr;
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm;
    llvm::Module *module;
    llvm::IRBuilder<llvm::ConstantFolder,llvm::IRBuilderDefaultInserter> builder;
    llvm::Type *floatType;
    llvm::Value *mesh_i;
    llvm::Value *mesh_j;
    std::map<Param *,Symbol *> symbols;


    JitContext(std::string name="LLVMModule") :
            context(getGlobalContext()), builder(getGlobalContext())
    {
        floatType = llvm::Type::getFloatTy(context);
        module_ptr = std::make_unique<llvm::Module>(name, context);
        module = module_ptr.get();

        llvm::FastMathFlags fmf;
        fmf.set();
        builder.setFastMathFlags(fmf);

//        module->setDataLayout(getTargetMachine().createDataLayout());

        // Create a new pass manager attached to it.
        fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module);
        fpm->add(llvm::createInstructionCombiningPass());
        fpm->add(llvm::createReassociatePass());
        fpm->add(llvm::createGVNPass());
        fpm->add(llvm::createCFGSimplificationPass());
        fpm->doInitialization();
    }

    ~JitContext()
    {
        traverse<TraverseFunctors::Delete<Symbol> >(symbols);
    }

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
    llvm::Constant *CreateFloatPtr(float *p)
    {
        llvm::ConstantInt *pv = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), (uint64_t)p);
        return llvm::ConstantExpr::getIntToPtr(pv , llvm::PointerType::get(floatType, 1));
    }
    llvm::Value *CallIntrinsic(llvm::Intrinsic::ID id, llvm::Value *value)
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(floatType);
        llvm::Function *function = llvm::Intrinsic::getDeclaration(module, id, arg_type);
        std::vector<llvm::Value *> arg;
        arg.push_back(value);
        return builder.CreateCall(function, arg);
    }
    llvm::Value *CallIntrinsic2(llvm::Intrinsic::ID id, llvm::Value *x, llvm::Value *y)
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(floatType);
        arg_type.push_back(floatType);
        llvm::Function *function = llvm::Intrinsic::getDeclaration(module, id, arg_type);
        std::vector<llvm::Value *> arg;
        arg.push_back(x);
        arg.push_back(y);
        return builder.CreateCall(function, arg);
    }
    llvm::Value *CallIntrinsic3(llvm::Intrinsic::ID id, llvm::Value *x, llvm::Value *y, llvm::Value *z)
    {
        std::vector<llvm::Type *> arg_type;
        arg_type.push_back(floatType);
        arg_type.push_back(floatType);
        arg_type.push_back(floatType);
        llvm::Function *function = llvm::Intrinsic::getDeclaration(module, id, arg_type);
        std::vector<llvm::Value *> arg;
        arg.push_back(x);
        arg.push_back(y);
        arg.push_back(z);
        return builder.CreateCall(function, arg);
    }

    std::vector<llvm::Function *> parent;
    std::vector<llvm::BasicBlock *> then_block;
    std::vector<llvm::BasicBlock *> else_block;
    std::vector<llvm::BasicBlock *> merge_block;

    void StartTernary(llvm::Value *condition)
    {
        parent.push_back(builder.GetInsertBlock()->getParent());
        then_block.push_back(llvm::BasicBlock::Create(context, "then", parent.back()));
        else_block.push_back(llvm::BasicBlock::Create(context, "else"));
        merge_block.push_back(llvm::BasicBlock::Create(context, "fi"));
        builder.CreateCondBr(condition, then_block.back(), else_block.back());
    }
    void withThen()
    {
        builder.SetInsertPoint(then_block.back());
    }
    void withElse()
    {
        // finish the withThen block, remember the last block of the THEN (may have changed)
        llvm::BasicBlock *lastBlockOfThen = &parent.back()->getBasicBlockList().back();
        then_block.back() = lastBlockOfThen;
        builder.CreateBr(merge_block.back());
        parent.back()->getBasicBlockList().push_back(else_block.back());
        builder.SetInsertPoint(else_block.back());
    }
    llvm::Value *FinishTernary(llvm::Value *thenValue, llvm::Value *elseValue)
    {
        // finish the withElse block, remember the last block of the ELSE
        llvm::BasicBlock *lastBlockOfElse = &parent.back()->getBasicBlockList().back();
        else_block.back() = lastBlockOfElse;
        builder.CreateBr(merge_block.back());
        parent.back()->getBasicBlockList().push_back(merge_block.back());
        builder.SetInsertPoint(merge_block.back());
        llvm::PHINode *mergeValue = builder.CreatePHI(floatType, 2, "iftmp");
        mergeValue->addIncoming(thenValue, then_block.back());
        mergeValue->addIncoming(elseValue, else_block.back());
        parent.pop_back();
        then_block.pop_back();
        else_block.pop_back();
        merge_block.pop_back();
        return mergeValue;
    }

    llvm::Value *getSymbolValue(Param *p)
    {
        auto it = symbols.find(p);
        Symbol *sym = (it == symbols.end()) ? nullptr : it->second;
        if (sym && sym->value)
            return sym->value;
        llvm::Value *v = p->_llvm(*this);
        // don't remember this value if we are in a conditional
        if (!merge_block.empty())
            return v;
        if (nullptr == sym)
        {
            sym = new Symbol();
            symbols.insert(std::make_pair(p, sym));
        }
        sym->value = v;
        return v;
    }
    // TODO: we optimize READ of parameters, but not WRITE
    // It would help to delay writing to parameters until the last write (or end of function)
    // NOTE: be careful of the fact that milkdrop allows assignments inside conditionals
    // I don't think the projectM parser handles this yet (?) but just be aware
    void assignSymbolValue(Param *p, llvm::Value *v)
    {
        v->setName(p->name);
        auto it = symbols.find(p);
        Symbol *sym = (it == symbols.end()) ? nullptr : it->second;
        if (nullptr == sym)
        {
            sym = new Symbol();
            symbols.insert(std::make_pair(p, sym));
        }
        sym->value = v;
        sym->assigned_value = v;
    }
};
#endif

#endif //PROJECTM_JITCONTEXT_H
