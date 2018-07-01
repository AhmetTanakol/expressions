#ifndef INCLUDE_MODERNDBS_CODEGEN_EXPRESSION_H
#define INCLUDE_MODERNDBS_CODEGEN_EXPRESSION_H

#include <memory>
#include "moderndbs/codegen/jit.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <iostream>

namespace moderndbs {
    /// A value (can either be a signed (!) 64 bit integer or a double).
    using data64_t = uint64_t;

    struct Expression {

        /// A value type.
        enum class ValueType {
            INT64,
            DOUBLE
        };

        /// The constant type.
        ValueType type;

        /// Constructor.
        explicit Expression(ValueType type): type(type) {}

        /// Get the expression type.
        ValueType getType() { return type; }
        /// Evaluate the expression.
        virtual data64_t evaluate(const data64_t* args);
        /// Build the expression code.
        virtual llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args);
    };

    struct Constant: public Expression {
        /// The constant value.
        data64_t value;

        /// Constructor.
        explicit Constant(int64_t value)
            : Expression(ValueType::INT64), value(*reinterpret_cast<data64_t*>(&value)) {}
        /// Constructor.
        explicit Constant(double value)
            : Expression(ValueType::DOUBLE), value(*reinterpret_cast<data64_t*>(&value)) {}


        data64_t evaluate(const data64_t* args) override {
            return this->value;
        }

        llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args) override {
            if (this->getType() == ValueType::INT64) {
                return llvm::ConstantInt::get(builder.getInt64Ty(), this->value);
            } else {
                return llvm::ConstantFP::get(builder.getDoubleTy(), *reinterpret_cast<double *>(&this->value));
            }
        }
    };

    struct Argument: public Expression {
        /// The argument index.
        uint64_t index;

        /// Constructor.
        Argument(uint64_t index, ValueType type)
            : Expression(type), index(index) {}

        data64_t evaluate(const data64_t* args) override {
            return *(args + this->index);
        }

        llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args) override {
            llvm::Value *arg1_a = builder.CreateGEP(args, llvm::ConstantInt::get(builder.getInt64Ty(), this->index), "ptr");
            llvm::Value *data = builder.CreateLoad(arg1_a, "a");
            return builder.CreateZExt(data, builder.getInt64Ty(), "i");
        }
    };

    struct Cast: public Expression {
        /// The child.
        Expression& child;
        /// The child type.
        ValueType childType;

        /// Constructor.
        Cast(Expression& child, ValueType type)
            : Expression(type), child(child) {
            childType = child.getType();
        }

        /// TODO(students) implement evaluate and build
    };

    struct BinaryExpression: public Expression {
        /// The left child.
        Expression& left;
        /// The right child.
        Expression& right;

        /// Constructor.
        BinaryExpression(Expression& left, Expression& right)
            : Expression(ValueType::INT64), left(left), right(right) {
            assert(left.getType() == right.getType() && "the left and right type must equal");
            type = left.getType();
        }
    };

    struct AddExpression: public BinaryExpression {
        /// Constructor
        AddExpression(Expression& left, Expression& right)
            : BinaryExpression(left, right) {}

        data64_t evaluate(const data64_t* args) override {
            if (this->getType() == ValueType::INT64) {
                return this->left.evaluate(args) + this->right.evaluate(args);
            } else {
                auto l = this->left.evaluate(args);
                auto r = this->right.evaluate(args);
                double res = *reinterpret_cast<double*>(&l) + *reinterpret_cast<double*>(&r);
                return *reinterpret_cast<data64_t*>(&res);
            }
        }

        llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args) override {
            if (this->getType() == ValueType::INT64) {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Add = builder.CreateAdd(val1, val2);
                return Add;
            } else {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Add = builder.CreateFAdd(val1, val2);
                return Add;
            }
        }
    };

    struct SubExpression: public BinaryExpression {
        /// Constructor
        SubExpression(Expression& left, Expression& right)
            : BinaryExpression(left, right) {}

        data64_t evaluate(const data64_t* args) override {
            if (this->getType() == ValueType::INT64) {
                return this->left.evaluate(args) - this->right.evaluate(args);
            } else {
                auto l = this->left.evaluate(args);
                auto r = this->right.evaluate(args);
                double res = *reinterpret_cast<double*>(&l) - *reinterpret_cast<double*>(&r);
                return *reinterpret_cast<data64_t*>(&res);
            }
        }

        llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args) override {
            if (this->getType() == ValueType::INT64) {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Sub = builder.CreateSub(val1, val2);
                return Sub;
            } else {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Sub = builder.CreateFSub(val1, val2);
                return Sub;
            }
        }
    };

    struct MulExpression: public BinaryExpression {
        /// Constructor
        MulExpression(Expression& left, Expression& right)
            : BinaryExpression(left, right) {}

        data64_t evaluate(const data64_t* args) override {
            if (this->getType() == ValueType::INT64) {
                return this->left.evaluate(args) * this->right.evaluate(args);
            } else {
                auto l = this->left.evaluate(args);
                auto r = this->right.evaluate(args);
                double res = *reinterpret_cast<double*>(&l) * *reinterpret_cast<double*>(&r);
                return *reinterpret_cast<data64_t*>(&res);
            }
        }

        llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args) override {
            if (this->getType() == ValueType::INT64) {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Mul = builder.CreateMul(val1, val2);
                return Mul;
            } else {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Mul = builder.CreateFMul(val1, val2);
                return Mul;
            }
        }
    };

    struct DivExpression: public BinaryExpression {
        /// Constructor
        DivExpression(Expression& left, Expression& right)
            : BinaryExpression(left, right) {}

        data64_t evaluate(const data64_t* args) override {
            if (this->getType() == ValueType::INT64) {
                return this->left.evaluate(args) / this->right.evaluate(args);
            } else {
                auto l = this->left.evaluate(args);
                auto r = this->right.evaluate(args);
                double res = *reinterpret_cast<double*>(&l) / *reinterpret_cast<double*>(&r);
                return *reinterpret_cast<data64_t*>(&res);
            }
        }

        llvm::Value* build(llvm::IRBuilder<>& builder, llvm::Value* args) override {
            if (this->getType() == ValueType::INT64) {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Div = builder.CreateSDiv(val1, val2);
                return Div;
            } else {
                llvm::Value* val1 = this->left.build(builder, args);
                llvm::Value* val2 = this->right.build(builder, args);
                llvm::Value *Div = builder.CreateFDiv(val1, val2);
                return Div;
            }
        }
    };

    struct ExpressionCompiler {
        /// The llvm context.
        llvm::LLVMContext& context;
        /// The llvm module.
        std::shared_ptr<llvm::Module> module;
        /// The jit.
        JIT jit;
        /// The compiled function.
        data64_t (*fnPtr)(data64_t* args);

        /// Constructor.
        explicit ExpressionCompiler(llvm::LLVMContext& context);

        /// Compile an expression.
        void compile(Expression& expression);
        /// Run a previously compiled expression
        data64_t run(data64_t* args);

        /// Dump the llvm module
        void dump();
    };

}  // namespace moderndbs

#endif
