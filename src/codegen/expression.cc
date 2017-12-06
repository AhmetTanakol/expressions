#include "moderndbs/error.h"
#include "moderndbs/codegen/expression.h"
#include <llvm/IR/TypeBuilder.h>
#include <iostream>

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
    /// TODO compile a function for the expression
    throw NotImplementedException();
}

/// Compile an expression.
data64_t ExpressionCompiler::run(data64_t* args) {
    /// TODO run the function
    throw NotImplementedException();
}

/// Dump the llvm module.
void ExpressionCompiler::dump() {
    module->print(llvm::errs(), nullptr);
}
