#ifndef MINIJULIA_DECLARATION_AST_H_
#define MINIJULIA_DECLARATION_AST_H_

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include <vector>

#include "expression.h"

namespace ast
{

  class FunctionDeclarationAST
  {
  public:
    std::string name;
    std::vector<std::pair<std::string, std::optional<JuliaType>>> args;
    std::optional<JuliaType> returnType;

    FunctionDeclarationAST(const std::string &name,
                           std::vector<std::pair<std::string, std::optional<JuliaType>>> args,
                           std::optional<JuliaType> returnType)
        : name(name), args(std::move(args)), returnType(std::move(returnType)) {}

    llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const;
  };

  class FunctionDefinitionAST
  {
  public:
    std::unique_ptr<FunctionDeclarationAST> proto;
    ExprASTList body;

    FunctionDefinitionAST(std::unique_ptr<FunctionDeclarationAST> proto,
                          ExprASTList body)
        : proto(std::move(proto)), body(std::move(body)) {}

    llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const;
  };

} // namespace ast

#endif // MINIJULIA_DECLARATION_AST_H_
