#include "AST/ast.h"

std::map<std::string, llvm::AllocaInst *> CurrentFuncSymbolTable;
std::map<std::string, llvm::AllocaInst *> MainFuncSymbolTable;

llvm::Type *convertJuliaTypeToLLVMType(JuliaType jType, llvm::LLVMContext *ctx)
{
    if (jType == JuliaType::Int64)
        return llvm::Type::getInt64Ty(*ctx);
    else if (jType == JuliaType::Float64)
        return llvm::Type::getDoubleTy(*ctx);
    else if (jType == JuliaType::String)
        return llvm::Type::getInt8Ty(*ctx); // TODO: string, char...
    else if (jType == JuliaType::Bool)
        return llvm::Type::getInt1Ty(*ctx);

    return llvm::Type::getVoidTy(*ctx); // Nothing
}

namespace ast
{
    llvm::Value *NumberExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        return std::visit(overloaded{[&](double arg) -> llvm::Value *
                                     {
                                         return llvm::ConstantFP::get(*Context, llvm::APFloat((double)arg));
                                     },
                                     [&](long arg) -> llvm::Value *
                                     {
                                         return llvm::ConstantInt::get(*Context, llvm::APInt(64, arg));
                                     }},
                          Val);
    }

    llvm::Value *BinaryExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        auto lhs_v = lhs.get()->codeGen(Context, Builder);
        auto rhs_v = rhs.get()->codeGen(Context, Builder);

        if (!lhs_v || !rhs_v)
        {
            llvm::errs() << location + " nullptr on some operand in binary expr\n";
            return nullptr;
        }

        switch (op)
        {
        case OpType::Add:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return llvm::BinaryOperator::CreateFAdd(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fadd", Builder->GetInsertBlock());

            return llvm::BinaryOperator::CreateAdd(lhs_v, rhs_v, "add", Builder->GetInsertBlock());
            break;
        case OpType::Sub:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return llvm::BinaryOperator::CreateFSub(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fsub", Builder->GetInsertBlock());

            return llvm::BinaryOperator::CreateSub(lhs_v, rhs_v, "sub", Builder->GetInsertBlock());
            break;
        case OpType::Mul:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return llvm::BinaryOperator::CreateFMul(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fmul", Builder->GetInsertBlock());

            return llvm::BinaryOperator::CreateMul(lhs_v, rhs_v, "mul", Builder->GetInsertBlock());
            break;
        case OpType::FDiv:
            if (lhs_v->getType()->getTypeID() != llvm::Type::TypeID::DoubleTyID)
                lhs_v = Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP");
            if (rhs_v->getType()->getTypeID() != llvm::Type::TypeID::DoubleTyID)
                rhs_v = Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP");

            return llvm::BinaryOperator::CreateFDiv(lhs_v, rhs_v, "fdiv", Builder->GetInsertBlock());
            break;
        case OpType::Rem:
            return llvm::BinaryOperator::CreateSRem(lhs_v, rhs_v, "srem", Builder->GetInsertBlock());
            break;
        case OpType::Pow:
            if (rhs_v->getType()->getTypeID() != llvm::Type::TypeID::DoubleTyID)
            {
                llvm::errs() << location + " power is not available for int/long types as exponents yet\n";
                return nullptr;
            }
            lhs_v = Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP");
            return Builder->CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs_v, rhs_v, nullptr, "powi");
            break;
        case OpType::BAnd:
            return llvm::BinaryOperator::CreateAnd(lhs_v, rhs_v, "and", Builder->GetInsertBlock());
            break;
        case OpType::BOr:
            return llvm::BinaryOperator::CreateOr(lhs_v, rhs_v, "or", Builder->GetInsertBlock());
            break;
        default:
            break;
        }

        return nullptr;
    }
    llvm::Value *ReturnExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        if (expr.has_value())
        {
            return Builder->CreateRet(expr.value().get()->codeGen(Context, Builder));
        }
        else
            return Builder->CreateRetVoid();
    }

    llvm::Value *CallExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        if (this->callee == "print")
            return ast::PrintExprAST(location, this->args[0].get()).codeGen(Context, Builder);

        std::vector<llvm::Value *> argsToPass;
        for (unsigned i = 0, e = this->args.size(); i != e; ++i)
            argsToPass.push_back(this->args[i].get()->codeGen(Context, Builder));

        if (this->callee == "div") // signed division builtin, should I create a function? (to get type checking for free)
            return llvm::BinaryOperator::CreateSDiv(argsToPass[0], argsToPass[1], "sdiv", Builder->GetInsertBlock());

        auto module = Builder->GetInsertBlock()->getModule(); // quick hack
        llvm::Function *calleeFunction = module->getFunction(this->callee);

        if (!calleeFunction)
            llvm::errs() << location + " use of undeclared function\n";

        if (calleeFunction->arg_size() != args.size())
            llvm::errs() << location + " number of arguments mismatch\n";

        // TODO: Check if argsToPass are of the same type as calleeFunction is expecting (verification)

        if (calleeFunction->getReturnType()->isVoidTy())
            return Builder->CreateCall(calleeFunction, argsToPass);
        else
            return Builder->CreateCall(calleeFunction, argsToPass, this->callee + "_CALL");
    }

    llvm::Value *LiteralExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        // TODO: Merge code with VariableDeclExprAST when shape of variable is > 1

        auto insertBlock = Builder->GetInsertBlock();
        size_t arraySize = shape;

        auto firstVal = values[0].get()->codeGen(Context, Builder);
        llvm::Type *firstElemType;
        if (firstVal)
            firstElemType = firstVal->getType();
        else
            firstElemType = llvm::Type::getInt1Ty(*Context);

        auto arrayType = llvm::ArrayType::get(firstElemType, arraySize);
        llvm::AllocaInst *arrayPtr = new llvm::AllocaInst(arrayType, 0, "", insertBlock);
        auto zero = llvm::ConstantInt::get(*Context, llvm::APInt(64, 0, true));
        for (size_t i = 0; i < arraySize; i++)
        {
            llvm::ConstantInt *index = llvm::ConstantInt::get(*Context, llvm::APInt(64, i, true));
            auto ptr = llvm::GetElementPtrInst::Create(arrayType, arrayPtr, {zero, index}, "arrayaccess", insertBlock);
            new llvm::StoreInst(values[i].get()->codeGen(Context, Builder), ptr, false, insertBlock);
        }

        return arrayPtr;
    }

    llvm::Value *VariableExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        auto allocaInst = CurrentFuncSymbolTable[name];
        if (!allocaInst)
        {
            llvm::errs() << location + " use of undeclared identifier: " + name + "\n";
            return nullptr;
        }

        return Builder->CreateLoad(allocaInst->getAllocatedType(), allocaInst, name);
    }

    llvm::Value *PrintExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        auto module = Builder->GetInsertBlock()->getModule(); // quick hack
        llvm::Function *printf = module->getFunction("printf");
        if (!printf)
        {
            std::vector<llvm::Type *> argTypes;
            argTypes.push_back(llvm::Type::getInt8PtrTy(*Context));

            llvm::FunctionType *printf_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*Context), argTypes, true); // true=vararg

            printf = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                "printf",
                *module);
            printf->setCallingConv(llvm::CallingConv::C);

            // FIXME: prints the value two times in diff formats,
            llvm::StringRef myStr = "expr %d (decimal) or %f (double)\n";
            Builder->CreateGlobalString(myStr, "printf_format", 0, module);
        }

        llvm::Value *argVal = arg->codeGen(Context, Builder);
        llvm::Constant *Zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), 0);
        if (!argVal)
            argVal = Zero;

        auto printfFormatGlobal = module->getNamedValue("printf_format");
        llvm::Constant *Indices[] = {Zero, Zero};
        auto formatPtr = llvm::ConstantExpr::getInBoundsGetElementPtr(printfFormatGlobal->getValueType(), printfFormatGlobal,
                                                                      Indices);

        return Builder->CreateCall(printf, {formatPtr, argVal, argVal});
    }

    llvm::Value *BooleanExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        return llvm::ConstantInt::getBool(*Context, Val);
    }

    llvm::Value *VariableDeclExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        llvm::Value *InitVal = expr->codeGen(Context, Builder);
        if (!InitVal)
        {
            llvm::errs() << location + " invalid initial value\n";
            return nullptr;
        }

        auto TheFunction = Builder->GetInsertBlock()->getParent();

        if (CurrentFuncSymbolTable.contains(name))
            Builder->CreateStore(InitVal, CurrentFuncSymbolTable[name]);
        else
        {

            llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
            llvm::AllocaInst *Alloca = TmpB.CreateAlloca(InitVal->getType(), nullptr, name);
            Builder->CreateStore(InitVal, Alloca);

            CurrentFuncSymbolTable[name] = Alloca;
        }

        return InitVal;
    }

    llvm::Value *CondBranchExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        llvm::Value *conditional = Cond->codeGen(Context, Builder);
        if (!conditional)
        {
            llvm::errs() << location + " expected a valid conditional statement\n";
            return nullptr;
        }

        // is expression equal to zero (false)?
        if (conditional->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
            conditional = Builder->CreateFCmpONE(conditional, llvm::ConstantFP::get(*Context, llvm::APFloat(0.0)), "ifcond");
        else
            conditional = Builder->CreateICmpNE(conditional, llvm::ConstantInt::get(*Context, llvm::APInt(1, 0, true)), "ifcond");

        llvm::Function *TheFunction = Builder->GetInsertBlock()->getParent();

        // Create blocks and insert ThenBB at the end of the current function
        llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*Context, "ifthen", TheFunction);
        llvm::BasicBlock *ElseBB = nullptr;
        llvm::BasicBlock *JoinBlock = llvm::BasicBlock::Create(*Context, "ifjoin");

        if (Else.size() > 0)
        {
            ElseBB = llvm::BasicBlock::Create(*Context, "ifelse");
            Builder->CreateCondBr(conditional, ThenBB, ElseBB);
        }
        else
            Builder->CreateCondBr(conditional, ThenBB, JoinBlock);

        // Emit ThenBB instructions
        Builder->SetInsertPoint(ThenBB);

        if (Then.size() == 0)
        {
            llvm::errs() << location + " expected a non-empty if statement body\n";
            return nullptr;
        }

        // emit first then instruction that we will add later to the phi node
        llvm::Value *firstThenInstr = Then[0]->codeGen(Context, Builder);
        if (!firstThenInstr)
        {
            llvm::errs() << location + " invalid if statement body\n";
            return nullptr;
        }

        // now generate all the other instructions in the conditional body
        for (size_t i = 1; i < Then.size(); i++)
            Then[i]->codeGen(Context, Builder);

        Builder->CreateBr(JoinBlock);

        // Emit else block (if there are any statements)
        llvm::Value *ElseV = nullptr;
        if (Else.size() > 0)
        {
            TheFunction->getBasicBlockList().push_back(ElseBB);
            Builder->SetInsertPoint(ElseBB);

            ElseV = Else[0]->codeGen(Context, Builder);
            if (!ElseV)
                return nullptr;

            for (size_t i = 1; i < Else.size(); i++)
                Else[i]->codeGen(Context, Builder);

            Builder->CreateBr(JoinBlock);
        }

        // Emit merge block
        TheFunction->getBasicBlockList().push_back(JoinBlock);
        Builder->SetInsertPoint(JoinBlock);

        // FIXME: if statements currently print 0 instead of last statement value
        return llvm::Constant::getNullValue(llvm::Type::getInt1Ty(*Context));
    }

    llvm::Value *RelationalOpExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        auto lhs_v = lhs.get()->codeGen(Context, Builder);
        auto rhs_v = rhs.get()->codeGen(Context, Builder);

        if (!lhs_v || !rhs_v)
        {
            llvm::errs() << location + " nullptr on some operand in relational expr\n";
            return nullptr;
        }

        switch (op)
        {
        case OpType::Eq:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return Builder->CreateFCmpOEQ(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fcmpoeq");

            return Builder->CreateICmpEQ(lhs_v, rhs_v, "icmpeq");
            break;
        case OpType::NotEq:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return Builder->CreateFCmpONE(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fcmpone");

            return Builder->CreateICmpNE(lhs_v, rhs_v, "icmpne");
            break;
        case OpType::Lower:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return Builder->CreateFCmpOLT(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fcmpolt");

            return Builder->CreateICmpSLT(lhs_v, rhs_v, "icmpslt");
            break;
        case OpType::LowerEq:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return Builder->CreateFCmpOLE(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fcmpole");

            return Builder->CreateICmpSLE(lhs_v, rhs_v, "icmpsle");
            break;
        case OpType::Greater:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return Builder->CreateFCmpOGT(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fcmpogt");

            return Builder->CreateICmpSGT(lhs_v, rhs_v, "icmpsgt");
            break;
        case OpType::GreaterEq:
            if (lhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID || rhs_v->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return Builder->CreateFCmpOGE(
                    Builder->CreateSIToFP(lhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"),
                    Builder->CreateSIToFP(rhs_v, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fcmpoge");

            return Builder->CreateICmpSGE(lhs_v, rhs_v, "icmpsge");
            break;
        default:
            break;
        }

        return nullptr;
    }

    llvm::Value *UnaryExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        if (!expr)
        {
            llvm::errs() << location + " nullptr on some operand in relational expr\n";
            return nullptr;
        }

        auto exprV = expr.get()->codeGen(Context, Builder);

        switch (op)
        {
        case OpType::Not:
        {

            return Builder->CreateNot(exprV, "not");
            break;
        }
        case OpType::Neg:
        {
            if (exprV->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
                return llvm::BinaryOperator::CreateFSub(
                    llvm::ConstantFP::get(*Context, llvm::APFloat(0.0)),
                    Builder->CreateSIToFP(exprV, llvm::Type::getDoubleTy(*Context), "castSItoFP"), "fneg", Builder->GetInsertBlock());

            return llvm::BinaryOperator::CreateSub(llvm::ConstantInt::get(*Context, llvm::APInt(64, 0, true)), exprV, "neg", Builder->GetInsertBlock());
            break;
        }
        default:
            break;
        }

        return nullptr;
    }

    llvm::Value *WhileExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        llvm::Value *condV = Cond->codeGen(Context, Builder);
        if (!condV)
        {
            llvm::errs() << location + " expected a valid conditional statement\n";
            return nullptr;
        }

        llvm::Function *TheFunction = Builder->GetInsertBlock()->getParent();

        llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*Context, "while", TheFunction);
        llvm::BasicBlock *AfterLoopBB = llvm::BasicBlock::Create(*Context, "whilebreak");

        // Setup first conditional
        llvm::Value *conditional = nullptr;
        if (condV->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
            conditional = Builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*Context, llvm::APFloat(0.0)), "whilefcond");
        else
            conditional = Builder->CreateICmpNE(condV, llvm::ConstantInt::get(*Context, llvm::APInt(1, 0, true)), "whilecond");
        Builder->CreateCondBr(conditional, LoopBB, AfterLoopBB); // first branch

        Builder->SetInsertPoint(LoopBB);
        for (auto &&expr : Exprs)
            expr->codeGen(Context, Builder);

        // Setup iteration conditional
        condV = Cond->codeGen(Context, Builder);
        if (condV->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
            conditional = Builder->CreateFCmpONE(condV, llvm::ConstantFP::get(*Context, llvm::APFloat(0.0)), "whileitfcond");
        else
            conditional = Builder->CreateICmpNE(condV, llvm::ConstantInt::get(*Context, llvm::APInt(1, 0, true)), "whileitcond");
        Builder->CreateCondBr(conditional, LoopBB, AfterLoopBB); // iteration branch

        TheFunction->getBasicBlockList().push_back(AfterLoopBB);
        Builder->SetInsertPoint(AfterLoopBB);

        return llvm::Constant::getNullValue(llvm::Type::getInt1Ty(*Context));
    }

    llvm::Value *ForInExprAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        llvm::Function *TheFunction = Builder->GetInsertBlock()->getParent();

        llvm::Value *StartV = llvm::ConstantInt::get(*Context, llvm::APInt(64, Start));
        llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
        llvm::AllocaInst *Alloca = TmpB.CreateAlloca(StartV->getType(), nullptr, IndexVarName);
        Builder->CreateStore(StartV, Alloca);

        // Within the loop, the variable is used as an iterative integer.
        // Save the shadowed variable to be restored later at the outside of the loop.
        llvm::AllocaInst *OldVal = CurrentFuncSymbolTable[IndexVarName];
        CurrentFuncSymbolTable[IndexVarName] = Alloca;

        llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*Context, "loop", TheFunction);
        // Jump explicitly to the body of the loop
        Builder->CreateBr(LoopBB);

        // Prepare step value
        llvm::Value *StepV = llvm::ConstantInt::get(*Context, llvm::APInt(64, Step));

        // Emit the body of the loop
        Builder->SetInsertPoint(LoopBB);

        // load previous value so that user will modify old variable and we will overwrite
        // (Julia restores index val in next iteration)
        llvm::Value *CurVar = Builder->CreateLoad(Alloca->getAllocatedType(), Alloca, IndexVarName);

        for (auto &&expr : Body)
            expr->codeGen(Context, Builder);

        // Do not reload but increment/step
        llvm::Value *NextVar = Builder->CreateAdd(CurVar, StepV, "stepadd");
        Builder->CreateStore(NextVar, Alloca);

        // Create the "after loop" block and insert it.
        llvm::BasicBlock *AfterLoopBB = llvm::BasicBlock::Create(*Context, "afterloop", TheFunction);

        // Insert the conditional branch at the end of the loop
        llvm::Value *StopV = llvm::ConstantInt::get(*Context, llvm::APInt(64, Stop));
        llvm::Value *EndCondition = Builder->CreateICmpULT(CurVar, StopV, "forcond");
        Builder->CreateCondBr(EndCondition, LoopBB, AfterLoopBB);

        // Prepare for emitting code after the loop
        Builder->SetInsertPoint(AfterLoopBB);

        // Restore the unshadowed variable
        if (OldVal)
            CurrentFuncSymbolTable[IndexVarName] = OldVal;
        else
            CurrentFuncSymbolTable.erase(IndexVarName);

        return llvm::Constant::getNullValue(llvm::Type::getInt1Ty(*Context));
    }

    llvm::Value *FunctionDeclarationAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        auto module = Builder->GetInsertBlock()->getModule(); // quick hack
        llvm::Function *declaredFunction = module->getFunction(name);

        if (declaredFunction)
            llvm::errs() << "At function: " << name + " function name shadowing not allowed yet\n";

        std::vector<llvm::Type *> inputParamTypes;
        for (auto &&inputParam : args)
        {
            if (!inputParam.second.has_value())
                llvm::errs() << "At function: " << name + " unknown parameter type (there is no type inference)\n";

            auto paramType = inputParam.second.value();
            inputParamTypes.push_back(convertJuliaTypeToLLVMType(paramType, Context));
        }

        llvm::Type *retType = convertJuliaTypeToLLVMType(returnType.value_or(JuliaType::Nothing), Context);

        llvm::FunctionType *functionTy = llvm::FunctionType::get(retType, inputParamTypes, false);

        declaredFunction = llvm::Function::Create(functionTy, llvm::Function::ExternalLinkage, name, module);

        // Set names for all arguments.
        unsigned i = 0;
        for (auto &arg : declaredFunction->args())
            arg.setName(args[i++].first);

        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(*Context, "entry", declaredFunction);
        Builder->SetInsertPoint(basicBlock);

        MainFuncSymbolTable = std::move(CurrentFuncSymbolTable); // backup Main symbol table
        CurrentFuncSymbolTable.clear();

        // Add the function arguments to the symbol table so that the block will be able to access them
        llvm::IRBuilder<> TmpB(&declaredFunction->getEntryBlock(), declaredFunction->getEntryBlock().begin());

        for (llvm::Argument &arg : declaredFunction->args())
        {
            auto nameString = std::string(arg.getName());
            llvm::AllocaInst *Alloca = TmpB.CreateAlloca(arg.getType(), nullptr, nameString);
            Builder->CreateStore(&arg, Alloca);
            CurrentFuncSymbolTable[nameString] = Alloca;
        }

        return declaredFunction;
    }

    llvm::Value *FunctionDefinitionAST::codeGen(llvm::LLVMContext *Context, llvm::IRBuilder<> *Builder) const
    {
        proto->codeGen(Context, Builder);

        for (auto &&expr : body)
            expr->codeGen(Context, Builder);

        auto basicBlock = Builder->GetInsertBlock();

        // add return if there is not any at the end O(1)
        if (!llvm::isa<llvm::ReturnInst>(basicBlock->back()))
            Builder->CreateRetVoid();

        // Now start writing again inside main function
        auto module = Builder->GetInsertBlock()->getModule(); // quick hack
        llvm::Function *mainFunc = module->getFunction("main");
        Builder->SetInsertPoint(&mainFunc->back());
        CurrentFuncSymbolTable = std::move(MainFuncSymbolTable);

        return basicBlock->getParent();
    }

}