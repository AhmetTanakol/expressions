#include "moderndbs/error.h"
#include "moderndbs/codegen/expression.h"
#include <llvm/IR/TypeBuilder.h>
#include <iostream>
#include <moderndbs/codegen/expression.h>

using JIT = moderndbs::JIT;
using Expression = moderndbs::Expression;
using ExpressionCompiler = moderndbs::ExpressionCompiler;
using NotImplementedException = moderndbs::NotImplementedException;
using data64_t = moderndbs::data64_t;

/// Evaluate the expresion.
data64_t Expression::evaluate(const data64_t* args) {
    throw NotImplementedException();
}

/// Build the expression code.
llvm::Value* Expression::build(llvm::IRBuilder<>& builder, llvm::Value* args) {
    throw NotImplementedException();
}

/// Constructor.
ExpressionCompiler::ExpressionCompiler(llvm::LLVMContext& context)
    : context(context), module(std::make_shared<llvm::Module>("meaningful_module_name", context)), jit(), fnPtr(nullptr) {}

/// Compile an expression.
void ExpressionCompiler::compile(Expression& expression) {
        llvm::IRBuilder<> builder(this->context);
        /*llvm::FunctionType* FunctionType;
        if (expression.getType() == Expression::ValueType::INT64) {
            FunctionType = llvm::TypeBuilder<data64_t(data64_t*), false>::get(this->context);
        } else {
            FunctionType = llvm::TypeBuilder<data64_t(data64_t*), false>::get(this->context);
        }*/
        auto FunctionType = llvm::TypeBuilder<data64_t(data64_t*), false>::get(this->context);

        llvm::Function *myFn =
                llvm::cast<llvm::Function>(this->module->getOrInsertFunction("_MyFunction", FunctionType));

        llvm::BasicBlock * bb = llvm::BasicBlock::Create(this->context, "entry", myFn);
        builder.SetInsertPoint(bb);

        auto argiter = myFn->arg_begin();
        llvm::Value *arg1 = &*argiter;

        llvm::Value* val = expression.build(builder, argiter);
        builder.CreateRet(val);
        this->dump();
}

/// Compile an expression.
data64_t ExpressionCompiler::run(data64_t* args) {
    auto moduleHandle = this->jit.addModule(std::move(this->module));
    void* F = this->jit.getPointerToFunction("MyFunction");
    this->fnPtr = (data64_t (*)(data64_t *))F;
    return this->fnPtr(args);
}

/// Dump the llvm module.
void ExpressionCompiler::dump() {
    module->print(llvm::errs(), nullptr);
}
