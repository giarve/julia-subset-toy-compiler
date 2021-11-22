#ifndef MINIJULIA_EXPRESSION_AST_H_
#define MINIJULIA_EXPRESSION_AST_H_

// helper type for the visitor
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

enum class JuliaType
{
    Int64,
    Float64,
    String,
    Bool,
    Nothing
};

namespace ast
{
    class ExprAST
    {
    public:
        enum ExprASTKind
        {
            Expr_Return,
            Expr_Num,
            Expr_Bool,
            Expr_Literal,
            Expr_VarDecl,
            Expr_Var,
            Expr_BinOp,
            Expr_UnaryOp,
            Expr_RelOp,
            Expr_Call,
            Expr_Print,
            Expr_CondBranch
        };

        ExprAST(ExprASTKind kind, Location location)
            : kind(kind), location(location) {}
        virtual ~ExprAST() = default;

        const ExprASTKind kind;
        const Location location;

        virtual llvm::Value *codeGen(llvm::LLVMContext *TheContext, llvm::IRBuilder<> *Builder) const { return nullptr; };
    };

    using ExprASTList = std::vector<std::unique_ptr<ExprAST>>;

    class NumberExprAST : public ExprAST
    {
    public:
        std::variant<long, double> Val;

        NumberExprAST(Location loc, std::variant<long, double> val) : ExprAST(Expr_Num, loc), Val(val) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Num; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class BooleanExprAST : public ExprAST
    {
    public:
        bool Val;

        BooleanExprAST(Location loc, bool val) : ExprAST(Expr_Num, loc), Val(val) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Bool; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class LiteralExprAST : public ExprAST
    {
    public:
        ExprASTList values;
        // std::vector<long> shape; // dimensions, could create a N-dimensional array
        long shape = 0;

        LiteralExprAST(Location loc, ExprASTList values,
                       long shape)
            : ExprAST(Expr_Literal, loc), values(std::move(values)),
              shape(std::move(shape)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Literal; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class VariableDeclExprAST : public ExprAST
    {

    public:
        std::string name;
        std::unique_ptr<ExprAST> expr;

        VariableDeclExprAST(Location loc, llvm::StringRef name, std::unique_ptr<ExprAST> expr)
            : ExprAST(Expr_Var, loc), name(name), expr(std::move(expr)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_VarDecl; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class VariableExprAST : public ExprAST
    {

    public:
        std::string name;
        VariableExprAST(Location loc, llvm::StringRef name)
            : ExprAST(Expr_Var, loc), name(name) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Var; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class ReturnExprAST : public ExprAST
    {
    public:
        std::optional<std::unique_ptr<ExprAST>> expr;

        ReturnExprAST(Location loc, std::optional<std::unique_ptr<ExprAST>> expr)
            : ExprAST(Expr_Return, loc), expr(std::move(expr)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Return; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class BinaryExprAST : public ExprAST
    {
    public:
        enum OpType
        {
            Add,
            Sub,
            Mul,
            FDiv,
            Rem,
            Pow,
            BAnd,
            BOr
        } op;
        std::unique_ptr<ExprAST> lhs, rhs;

        BinaryExprAST(Location loc, OpType Op, std::unique_ptr<ExprAST> lhs,
                      std::unique_ptr<ExprAST> rhs)
            : ExprAST(Expr_BinOp, loc), op(Op), lhs(std::move(lhs)),
              rhs(std::move(rhs)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_BinOp; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class CallExprAST : public ExprAST
    {
    public:
        std::string callee;
        ExprASTList args;

        CallExprAST(Location loc, const std::string &callee,
                    ExprASTList args)
            : ExprAST(Expr_Call, loc), callee(callee), args(std::move(args)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Call; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class PrintExprAST : public ExprAST
    {
        ExprAST *arg;

    public:
        PrintExprAST(Location loc, ExprAST *arg)
            : ExprAST(Expr_Print, loc), arg(arg) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_Print; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class CondBranchExprAST : public ExprAST
    {
        std::unique_ptr<ExprAST> Cond;
        ExprASTList Then, Else;
        // std::vector<std::pair<std::unique_ptr<ExprAST>, ExprASTList>> ElseIf;

    public:
        CondBranchExprAST(Location loc, std::unique_ptr<ExprAST> Cond, ExprASTList Then, ExprASTList Else)
            : ExprAST(Expr_Print, loc), Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_CondBranch; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class RelationalOpExprAST : public ExprAST
    {
    public:
        enum OpType
        {
            Eq,
            NotEq,
            Lower,
            LowerEq,
            Greater,
            GreaterEq
        } op;
        std::unique_ptr<ExprAST> lhs, rhs;

        RelationalOpExprAST(Location loc, OpType Op, std::unique_ptr<ExprAST> lhs,
                            std::unique_ptr<ExprAST> rhs)
            : ExprAST(Expr_BinOp, loc), op(Op), lhs(std::move(lhs)),
              rhs(std::move(rhs)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_RelOp; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class UnaryExprAST : public ExprAST
    {
    public:
        enum OpType
        {
            Not,
            Neg
        } op;
        std::unique_ptr<ExprAST> expr;

        UnaryExprAST(Location loc, OpType Op, std::unique_ptr<ExprAST> expr)
            : ExprAST(Expr_BinOp, loc), op(Op), expr(std::move(expr)) {}

        static bool classof(const ExprAST *c) { return c->kind == Expr_UnaryOp; }

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class WhileExprAST : public ExprAST
    {
        std::unique_ptr<ExprAST> Cond;
        ExprASTList Exprs;

    public:
        WhileExprAST(Location loc, std::unique_ptr<ExprAST> Cond, ExprASTList Exprs)
            : ExprAST(Expr_Print, loc), Cond(std::move(Cond)), Exprs(std::move(Exprs)) {}

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

    class ForInExprAST : public ExprAST
    {
        std::string IndexVarName;
        long Start;
        long Step;
        long Stop;
        ExprASTList Body;

    public:
        ForInExprAST(Location loc, std::string IndexVarName, long Start, long Step, long Stop, ExprASTList Body)
            : ExprAST(Expr_Print, loc), IndexVarName(std::move(IndexVarName)), Start(Start), Step(Step), Stop(Stop), Body(std::move(Body)) {}

        llvm::Value *codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const override;
    };

}

#endif // MINIJULIA_EXPRESSION_AST_H_
