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
        auto FunctionType = llvm::TypeBuilder<data64_t(data64_t*), false>::get(this->context);

        auto *myFn =
                llvm::cast<llvm::Function>(this->module->getOrInsertFunction("_MyFunction", FunctionType));

        llvm::BasicBlock * bb = llvm::BasicBlock::Create(this->context, "entry", myFn);
        builder.SetInsertPoint(bb);

        auto argiter = myFn->arg_begin();

        llvm::Value* val = expression.build(builder, &*argiter);
        llvm::Value* res = builder.CreateBitCast(val, builder.getInt64Ty());
        builder.CreateRet(res);
        auto moduleHandle = this->jit.addModule(std::move(this->module));
        void* F = this->jit.getPointerToFunction("MyFunction");
        this->fnPtr = reinterpret_cast<data64_t (*)(data64_t *)>(F);
}

/// Compile an expression.
data64_t ExpressionCompiler::run(data64_t* args) {
    data64_t result = this->fnPtr(args);
    return result;
}

/// Dump the llvm module.
void ExpressionCompiler::dump() {
    module->print(llvm::errs(), nullptr);
}
