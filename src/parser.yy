%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.1"
%header

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include "AST/ast.h"

  class driver;
  enum class JuliaType;
}

// The parsing context.
%param { driver& drv }
%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
  #include <unordered_map>
  #include <variant>
  #include "driver.hh"
  #include "AST/ast.h"

  llvm::Type* convertJuliaTypeToLLVMType(JuliaType jType, llvm::LLVMContext *ctx);
  void createMainFunction(driver& drv);
} 


%define api.token.prefix {TOKEN_}

%token <long> INTEGER
%token <float> FLOAT
%token <std::string> STRING 
%token <std::string> IDENTIFIER
%token <bool> BOOLEAN
%token <JuliaType> TYPE
%token EQUALS_SIGN STAR PLUS MINUS
%token LPAREN RPAREN PERCENT CIRCUMFLEX SLASH SEMICOLON COMMA COLON
%token AND OR BANG
%token DOUBLE_EQUAL BANG_EQUAL 
%token GREATER LOWER GREATER_EQUAL LOWER_EQUAL
%token RSQRBRKT LSQRBRKT
%token NEWLINE
%token FUNCTION_START END DOUBLECOLON RETURN IF ELSE ELSEIF FOR IN WHILE

%nterm <std::unique_ptr<ast::ExprAST>> arith_bool_exprs logical_or_expression logical_and_expression equality_expression relational_expression
%nterm <std::unique_ptr<ast::ExprAST>> exponentiative_expression multiplicative_expression unary_expression postfix_expression
%nterm <std::unique_ptr<ast::ExprAST>> additive_expression primary_expression assignment_expression expression
%nterm <std::unique_ptr<ast::ExprAST>> CONSTANT statement conditional_branch_statement iterative_statement 
%nterm <std::unique_ptr<ast::FunctionDefinitionAST>> function

%nterm <std::optional<JuliaType>> type_declaration.opt
%nterm <std::pair<std::string, std::optional<JuliaType>>> parameter_declaration
%nterm <std::vector<std::pair<std::string, std::optional<JuliaType>>>> parameter_declaration_list.opt
%nterm <ast::ExprASTList> parameter_call_list.opt function_statement_list.opt statement_list

%nterm <std::pair<ast::ExprASTList, long>> composite_element_list_initialization

// %printer { yyo << $$; } <*>; // debugging print, change <*> for a <type> with "<<" overload

%%
%start program;

program
	: ENTRYPOINT declaration_list {
		llvm::Function *mainFunc = drv.RootModule->getFunction("main");
		drv.Builder->CreateRetVoid(); // main return, not checking if duplicated!
		llvm::verifyFunction(*mainFunc, &llvm::errs());
		drv.FunctionPassManager->run(*mainFunc);
	}
	| ENTRYPOINT // empty input file or completely commented
	;

ENTRYPOINT:
	%empty { createMainFunction(drv); }

declaration_list
	: declaration_list declaration
	| declaration
	;

declaration
	: function {
		auto s = $1.get();
		if(s) {
			auto funcV = llvm::dyn_cast<llvm::Function>(s->codeGen(drv.TheContext.get(), drv.Builder.get()));
			llvm::verifyFunction(*funcV, &llvm::errs());
        	drv.FunctionPassManager->run(*funcV);
		}
	}
	| statement {
		// They will be added to main() but could be global vars
		auto s = $1.get();
		if(s) {
			// ast::PrintExprAST(drv.getLoc(), s).codeGen(drv.TheContext.get(), drv.Builder.get());
			s->codeGen(drv.TheContext.get(), drv.Builder.get());
		}
	}
	;

function
	: FUNCTION_START IDENTIFIER LPAREN parameter_declaration_list.opt RPAREN type_declaration.opt NEWLINE function_statement_list.opt END NEWLINE {
		auto def = std::make_unique<ast::FunctionDeclarationAST>(std::move($2), std::move($4), std::move($6));
		$$ = std::make_unique<ast::FunctionDefinitionAST>(std::move(def), std::move($8));
	}
	;

parameter_declaration_list.opt:
	%empty {}
	| parameter_declaration_list.opt COMMA parameter_declaration { $$ = std::move($1); $$.push_back($3); }
	| parameter_declaration { $$.push_back($1); }
	;

parameter_declaration
	: IDENTIFIER type_declaration.opt {	$$.first = $1;	$$.second = $2;	}
	;

type_declaration.opt:
	%empty {}
	| DOUBLECOLON TYPE { $$ = $2; }
	;

function_statement_list.opt:
	%empty { }
	| statement_list { $$ = std::move($1); }
	;

statement_list
	: statement_list statement { $$ = std::move($1); $$.push_back(std::move($2));  }
	| statement { $$.push_back(std::move($1)); }
	;

statement
	: expression NEWLINE { $$ = std::move($1); }
	| conditional_branch_statement NEWLINE { $$ = std::move($1); }
	| iterative_statement NEWLINE { $$ = std::move($1); }
	;

conditional_branch_statement // empty body conditionals not supported
	: IF arith_bool_exprs NEWLINE statement_list END { $$ = std::make_unique<ast::CondBranchExprAST>(drv.getLoc(), std::move($2), std::move($4), ast::ExprASTList()); }
	| IF arith_bool_exprs NEWLINE statement_list ELSE NEWLINE statement_list END { $$ = std::make_unique<ast::CondBranchExprAST>(drv.getLoc(), std::move($2), std::move($4), std::move($7)); }
	| IF arith_bool_exprs NEWLINE statement_list cond_branch_stmt_alternative_list END {} // TODO
	| IF arith_bool_exprs NEWLINE statement_list cond_branch_stmt_alternative_list ELSE NEWLINE statement_list END {} // TODO
	;

cond_branch_stmt_alternative_list
    : cond_branch_stmt_alternative_list cond_branch_stmt_alternative
	| cond_branch_stmt_alternative
	;

cond_branch_stmt_alternative
	: ELSEIF arith_bool_exprs NEWLINE statement_list
	;

iterative_statement
	: WHILE arith_bool_exprs NEWLINE statement_list END { $$ = std::make_unique<ast::WhileExprAST>(drv.getLoc(), std::move($2), std::move($4)); }
	// step ranges
	| FOR IDENTIFIER IN INTEGER COLON INTEGER NEWLINE statement_list END { $$ = std::make_unique<ast::ForInExprAST>(drv.getLoc(), std::move($2), $4, 1, $6, std::move($8)); }
	| FOR IDENTIFIER IN INTEGER COLON INTEGER COLON INTEGER NEWLINE statement_list END { $$ = std::make_unique<ast::ForInExprAST>(drv.getLoc(), std::move($2), $4, $6, $8, std::move($10)); }
	;

expression
	/* TODO: MOVE RETURN TO statement AND TEST IT */
	: assignment_expression { $$ = std::move($1); }
	| arith_bool_exprs {  $$ = std::move($1); }
	| RETURN {  $$ = std::make_unique<ast::ReturnExprAST>(drv.getLoc(), std::optional<std::unique_ptr<ast::ExprAST>>()); }
	| RETURN arith_bool_exprs {  $$ = std::make_unique<ast::ReturnExprAST>(drv.getLoc(), std::move($2)); }
	;

assignment_expression
	: IDENTIFIER EQUALS_SIGN arith_bool_exprs { $$ = std::make_unique<ast::VariableDeclExprAST>(drv.getLoc(), std::move($1), std::move($3)); }
	;

arith_bool_exprs 
	: logical_or_expression { $$ = std::move($1); }
	;

logical_or_expression
	: logical_and_expression { $$ = std::move($1); }
	| logical_or_expression OR logical_and_expression { $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::BOr, std::move($1), std::move($3)); }
	;

logical_and_expression
	: equality_expression { $$ = std::move($1); }
	| logical_and_expression AND equality_expression { $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::BAnd, std::move($1), std::move($3)); }
	;

equality_expression
	: relational_expression { $$ = std::move($1); }
	| equality_expression DOUBLE_EQUAL relational_expression { $$ = std::make_unique<ast::RelationalOpExprAST>(drv.getLoc(), ast::RelationalOpExprAST::OpType::Eq, std::move($1), std::move($3)); }
	| equality_expression BANG_EQUAL relational_expression	 { $$ = std::make_unique<ast::RelationalOpExprAST>(drv.getLoc(), ast::RelationalOpExprAST::OpType::NotEq, std::move($1), std::move($3)); }
	;

relational_expression
	: additive_expression { $$ = std::move($1); }
	| relational_expression LOWER additive_expression			{ $$ = std::make_unique<ast::RelationalOpExprAST>(drv.getLoc(), ast::RelationalOpExprAST::OpType::Lower, std::move($1), std::move($3)); }
	| relational_expression LOWER_EQUAL additive_expression		{ $$ = std::make_unique<ast::RelationalOpExprAST>(drv.getLoc(), ast::RelationalOpExprAST::OpType::LowerEq, std::move($1), std::move($3)); }
	| relational_expression GREATER additive_expression			{ $$ = std::make_unique<ast::RelationalOpExprAST>(drv.getLoc(), ast::RelationalOpExprAST::OpType::Greater, std::move($1), std::move($3)); }
	| relational_expression GREATER_EQUAL additive_expression	{ $$ = std::make_unique<ast::RelationalOpExprAST>(drv.getLoc(), ast::RelationalOpExprAST::OpType::GreaterEq, std::move($1), std::move($3)); }
	;


additive_expression
	: multiplicative_expression { $$ = std::move($1); }
	| additive_expression PLUS multiplicative_expression  { $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::Add, std::move($1), std::move($3)); }
	| additive_expression MINUS multiplicative_expression { $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::Sub, std::move($1), std::move($3)); }
	;

multiplicative_expression
	: exponentiative_expression { $$ = std::move($1); }
	| multiplicative_expression STAR exponentiative_expression 		{ $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::Mul, std::move($1), std::move($3)); }
	| multiplicative_expression SLASH exponentiative_expression		{ $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::FDiv, std::move($1), std::move($3)); }
	| multiplicative_expression PERCENT exponentiative_expression	{ $$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::Rem, std::move($1), std::move($3)); }
	;

exponentiative_expression
	: unary_expression { $$ = std::move($1); }
	| exponentiative_expression CIRCUMFLEX unary_expression {
		$$ = std::make_unique<ast::BinaryExprAST>(drv.getLoc(), ast::BinaryExprAST::OpType::Pow, std::move($1), std::move($3));
	}
	;

unary_expression
	: postfix_expression { $$ = std::move($1); }
	| PLUS postfix_expression  { $$ = std::move($2);  }
	| MINUS postfix_expression { $$ = std::make_unique<ast::UnaryExprAST>(drv.getLoc(), ast::UnaryExprAST::OpType::Neg, std::move($2)); }
	| BANG postfix_expression  { $$ = std::make_unique<ast::UnaryExprAST>(drv.getLoc(), ast::UnaryExprAST::OpType::Not, std::move($2)); }
	;

postfix_expression
	: primary_expression { $$ = std::move($1); }
	//| postfix_expression LSQRBRKT arith_bool_exprs RSQRBRKT 
	;

primary_expression
	: IDENTIFIER { $$ = std::make_unique<ast::VariableExprAST>(drv.getLoc(), std::move($1)); }
	| CONSTANT	{ $$ = std::move($1); }
	| LPAREN arith_bool_exprs RPAREN { $$ = std::move($2); }
	| LSQRBRKT composite_element_list_initialization RSQRBRKT {
		$$ = std::make_unique<ast::LiteralExprAST>(drv.getLoc(), std::move($2.first), std::move($2.second));
	}
	| IDENTIFIER LPAREN parameter_call_list.opt RPAREN {
		$$ = std::make_unique<ast::CallExprAST>(drv.getLoc(), $1, std::move($3));
	}
	| IDENTIFIER LSQRBRKT arith_bool_exprs RSQRBRKT {
		/*
		auto id = std::make_unique<ast::VariableExprAST>(drv.getLoc(), std::move($1));
		
		// TODO: requires the use of pointer deref
		
		// AST array access expr?
		llvm::Value* allocaInstTypeAsValue = id->codeGen(drv.TheContext.get(), drv.Builder.get());
		llvm::AllocaInst* arrayPtr = llvm::dyn_cast<llvm::AllocaInst>(allocaInstTypeAsValue);
		if(!arrayPtr)
			throw yy::parser::syntax_error(drv.location, "invalid expression for array access");

		llvm::ArrayType* arrayPtrType = llvm::dyn_cast<llvm::ArrayType>(arrayPtr->getAllocatedType());
		if(!arrayPtrType)
			throw yy::parser::syntax_error(drv.location, "invalid expression for array access, invalid allocated type");
		
		auto elementType = arrayPtrType->getElementType();
		//auto arrayNumElems = arrayPtrType->getNumElements();

		llvm::Value* index = $3->codeGen(drv.TheContext.get(), drv.Builder.get());

		auto one = llvm::ConstantInt::get(*drv.TheContext, llvm::APInt(64, 1, true));
		auto index_minus_one = llvm::BinaryOperator::CreateSub(index, one, "arr_idx_offset", drv.Builder->GetInsertBlock());

		auto zero = llvm::ConstantInt::get(*drv.TheContext, llvm::APInt(64, 0, true));
		auto ptr = llvm::GetElementPtrInst::CreateInBounds(arrayPtrType, arrayPtr, { zero, index_minus_one }, "", drv.Builder->GetInsertBlock());
		auto load = new llvm::LoadInst(elementType, ptr, "loader", drv.Builder->GetInsertBlock());

		//$$ = std::make_unique<ast:: */
	}
	;

parameter_call_list.opt:
	%empty {}
	| parameter_call_list.opt COMMA arith_bool_exprs { $$ = std::move($1); $$.push_back(std::move($3)); }
	| arith_bool_exprs { $$.push_back(std::move($1)); }
	;

composite_element_list_initialization
	: composite_element_list_initialization CONSTANT {
		$$ = std::move($1);
		$$.second++;
		$$.first.push_back(std::move($2));
	}
	| composite_element_list_initialization SEMICOLON {
		$$ = std::move($1);
	}
	| CONSTANT {
		$$.second++;
		$$.first.push_back(std::move($1)); // append elem to vec
	}
	;

CONSTANT
	: INTEGER { $$ = std::make_unique<ast::NumberExprAST>(drv.getLoc(), (int)$1); }
	| FLOAT	  { $$ = std::make_unique<ast::NumberExprAST>(drv.getLoc(), (double)$1); }
	| STRING  { }
	| BOOLEAN { $$ = std::make_unique<ast::BooleanExprAST>(drv.getLoc(), $1); }
	;

%%

void yy::parser::error(const location_type &l, const std::string &m)
{
    std::ostringstream ss;
    ss << l;
    ss << ": ";
    ss << m;
    ss << std::endl;
    std::cout << ss.str();
}

// Instead of using global variables, we put anything outside any other function inside the "main()" function
void createMainFunction(driver& drv)
{
    llvm::FunctionType *functionTy = llvm::FunctionType::get(llvm::Type::getVoidTy(*drv.TheContext), {}, false);
    llvm::Function *mainF = llvm::Function::Create(functionTy, llvm::Function::ExternalLinkage, "main", drv.RootModule.get());
    llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(*drv.TheContext, "entry", mainF);
    drv.Builder->SetInsertPoint(basicBlock);
}