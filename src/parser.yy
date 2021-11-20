%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.1"
%header

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>

  #include "variant.hh"
  #include "builtin.hh"
  class driver;
}

// The parsing context.
%param { driver& drv }
%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
  #include "driver.hh"
  #include <unordered_map>


  // Prologue and epilogue semantic action
  // Throw an error in the epilogue to stop compilation
  #define PRLG try { 
  #define EPLG } catch (variant::SemanticException const &e) { yy::parser::error(drv.location, e.what());  /* throw yy::parser::syntax_error(drv.location, e.what()); */ }

  std::unordered_map<std::string, variant::operable_multiarray> symtab;
}


%define api.token.prefix {TOKEN_}

%token <long> INTEGER
%token <float> FLOAT
%token <std::string> STRING 
%token <std::string> IDENTIFIER
%token <bool> BOOLEAN
%token <variant::JuliaType> TYPE
%token EQUALS_SIGN STAR PLUS MINUS
%token LPAREN RPAREN PERCENT CIRCUMFLEX SLASH SEMICOLON COMMA
%token AND OR BANG
%token DOUBLE_EQUAL BANG_EQUAL 
%token GREATER LOWER GREATER_EQUAL LOWER_EQUAL
%token RSQRBRKT LSQRBRKT
%token NEWLINE
%token FUNCTION_START FUNCTION_END DOUBLECOLON RETURN

%nterm <variant::operable_multiarray> arith_bool_exprs
// %nterm <variant::operable_multiarray> logical_or_expression logical_and_expression equality_expression relational_expression
%nterm <variant::operable_multiarray> exponentiative_expression multiplicative_expression unary_expression postfix_expression
%nterm <variant::operable_multiarray> additive_expression primary_expression assignment_expression
%nterm <variant::operable_multiarray> composite_element_list_initialization composite_element_tuple_initialization
%nterm <variant::operable> CONSTANT
%nterm <std::pair<long,long>> composite_element_list_access
%nterm <long> arith_bool_expr_to_long
//%nterm <variant::operable_multiarray> function_definition

// %printer { yyo << $$; } <*>; // debugging print

%%
%start program;

program
	: declaration_list
	;

declaration_list
	: declaration_list declaration
	| declaration
	;

declaration
	: function_definition
	| statement
	;

function_definition
	: FUNCTION_START IDENTIFIER LPAREN parameter_declaration_list.opt RPAREN type_declaration.opt NEWLINE function_statement_list.opt FUNCTION_END NEWLINE
	;

parameter_declaration_list.opt:
	%empty
	| parameter_declaration_list.opt COMMA parameter_declaration
	| parameter_declaration
	;

parameter_declaration
	: IDENTIFIER type_declaration.opt
	;

type_declaration.opt:
	%empty
	| DOUBLECOLON TYPE
	;

function_statement_list.opt:
	%empty
	| statement_list
	;

statement_list
	: statement_list statement
	| statement
	;

statement
	: assignment_expression NEWLINE { 
		symtab[$1.identifier.value()] = $1;
		std::ostringstream ss;
		ss << $1.identifier.value() << " = " << $1 << std::endl;
		drv.output->cout(ss.str());
	}
	| arith_bool_exprs NEWLINE {
		std::ostringstream ss;
		ss << $1 << std::endl;
		drv.output->cout(ss.str());
	}
	| RETURN NEWLINE
	| RETURN statement
	;

assignment_expression
	: IDENTIFIER EQUALS_SIGN arith_bool_exprs
		{
			PRLG
			$$ = $3;
			$$.identifier = $1;
			EPLG
		}
	;

arith_bool_exprs 
	: additive_expression { $$ = $1; } // TODO: boolean ops are now disabled, replace with logical_or_expression to enabled them again
	;
/*
logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR logical_and_expression { PRLG $$ = $1 || $3; EPLG }
	;

logical_and_expression
	: equality_expression { $$ = $1; }
	| logical_and_expression AND equality_expression	{ PRLG $$ = $1 && $3; EPLG }
	;

equality_expression
	: relational_expression { $$ = $1; }
	| equality_expression DOUBLE_EQUAL relational_expression { PRLG $$ = $1 == $3; EPLG }
	| equality_expression BANG_EQUAL relational_expression	 { PRLG $$ = $1 != $3; EPLG }
	;

relational_expression
	: additive_expression { $$ = $1; }
	| relational_expression LOWER additive_expression			{ PRLG $$ = $1 < $3;  EPLG }
	| relational_expression LOWER_EQUAL additive_expression		{ PRLG $$ = $1 <= $3; EPLG  }
	| relational_expression GREATER additive_expression			{ PRLG $$ = $1 > $3;  EPLG }
	| relational_expression GREATER_EQUAL additive_expression	{ PRLG $$ = $1 >= $3; EPLG  }
	;
*/

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression PLUS multiplicative_expression  { PRLG $$ = $1 + $3;  EPLG}
	| additive_expression MINUS multiplicative_expression { PRLG $$ = $1 - $3; EPLG }
	;

multiplicative_expression
	: exponentiative_expression { $$ = $1; }
	| multiplicative_expression STAR exponentiative_expression 		{ PRLG $$ = $1 * $3; EPLG }
	| multiplicative_expression SLASH exponentiative_expression		{ PRLG $$ = $1 / $3; EPLG }
	| multiplicative_expression PERCENT exponentiative_expression	{ PRLG $$ = $1 % $3; EPLG }
	;

exponentiative_expression
	: unary_expression { $$ = $1; }
	| exponentiative_expression CIRCUMFLEX unary_expression { PRLG $$ = $1 ^ $3; EPLG }
	;

unary_expression
	: postfix_expression { $$ = $1;  }
	| PLUS postfix_expression  { PRLG $$ = +$2; EPLG }
	| MINUS postfix_expression { PRLG $$ = -$2; EPLG }
	| BANG postfix_expression  { PRLG $$ = !$2; EPLG }
	;

postfix_expression
	: primary_expression { $$ = $1;  }
	| postfix_expression LSQRBRKT composite_element_list_access RSQRBRKT {
		// Index access
		$$ = $1.values.at($3.first).at($3.second);
	}
	;

composite_element_list_access
	// Substract 1 to each index because we use 0 index arrays internally
	: arith_bool_expr_to_long COMMA arith_bool_expr_to_long { $$.first = $1-1; $$.second = $3-1; } // [x,y]
	| arith_bool_expr_to_long { $$.first = $1-1; } // [x]
	;

arith_bool_expr_to_long
	: arith_bool_exprs {
		const variant::operable *scalar_equivalent = $1.scalar_equivalent();
		try {
			$$ = std::get<long>(scalar_equivalent->value);
		}
		catch (const std::bad_variant_access& ex) {
			throw yy::parser::syntax_error(drv.location, ex.what());
		}
	}
	;

primary_expression
	: IDENTIFIER	{
		if(!symtab.contains($1))
			throw yy::parser::syntax_error(drv.location, "use of undeclared identifier: " + $1);
		else
			$$ = symtab[$1]; // TODO: after this moment, we cannot compile-time arithmetically operate anymore
		}
	| CONSTANT		{ $$.insert_row_element($1); }
	| LPAREN arith_bool_exprs RPAREN { $$ = $2; }
	| LSQRBRKT composite_element_list_initialization RSQRBRKT { $$ = $2; }
	| LSQRBRKT RSQRBRKT { } // empty array declaration
	| LPAREN RPAREN { $$.is_tuple = true; } // empty tuple declaration
	| LPAREN CONSTANT composite_element_tuple_initialization RPAREN { $$ = $3; $$.insert_row_element_start($2); }
	| IDENTIFIER LPAREN parameter_call_list.opt RPAREN { /* TODO */ }
	;

parameter_call_list.opt:
	%empty
	| parameter_call_list.opt COMMA arith_bool_exprs
	| arith_bool_exprs
	;

// change this if getting reduce conflicts to behave like tuple grammar (although this is faster when using vectors)
composite_element_list_initialization 
	: composite_element_list_initialization CONSTANT { $1.insert_row_element($2); $$ = $1; }
	| composite_element_list_initialization SEMICOLON { $$ = $1; $$.add_new_row(); }
	| composite_element_list_initialization COMMA { $$ = $1; $$.add_new_row(); }
	| CONSTANT { $$.insert_row_element($1); }
	;

composite_element_tuple_initialization
	: composite_element_tuple_initialization COMMA CONSTANT {  $$ = $1; $$.insert_row_element($3); $$.is_tuple = true; }
	| COMMA CONSTANT { $$.insert_row_element($2); $$.is_tuple = true; }
	| COMMA { $$.is_tuple = true; }
	;

	// TODO: Add this function to functions' symbol table
	// FUNC_DIV 		 arith_bool_expr_to_long COMMA arith_bool_expr_to_long RPAREN { PRLG $$ = builtin::div($2, $4); EPLG }

CONSTANT
	: INTEGER { $$.value = $1; }
	| FLOAT	  { $$.value = $1; }
	| STRING  { $$.value = $1; }
	| BOOLEAN { $$.value = $1; }
	;

%%

void yy::parser::error (const location_type& l, const std::string& m)
{
   std::ostringstream ss;
   ss << l; ss << ": "; ss << m; ss << std::endl;
   drv.output->cout(ss.str());
}
