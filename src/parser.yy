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

  std::unordered_map<std::string, variant::operable_multiarray> symtab;
}


%define api.token.prefix {TOKEN_}

%token <int> INTEGER
%token <float> FLOAT
%token <std::string> STRING 
%token <std::string> IDENTIFIER
%token <bool> BOOLEAN
%token EQUALS_SIGN STAR PLUS MINUS
%token LPAREN RPAREN PERCENT CIRCUMFLEX SLASH SEMICOLON COMMA
%token AND OR BANG
%token DOUBLE_EQUAL BANG_EQUAL 
%token GREATER LOWER GREATER_EQUAL LOWER_EQUAL
%token RSQRBRKT LSQRBRKT
%token NEWLINE
%token FUNC_DIV FUNC_LENGTH FUNC_SIZE FUNC_TRANSPOSE

%nterm <variant::operable_multiarray> arith_bool_exprs
%nterm <variant::operable_multiarray> logical_or_expression logical_and_expression equality_expression relational_expression
%nterm <variant::operable_multiarray> exponentiative_expression multiplicative_expression unary_expression postfix_expression
%nterm <variant::operable_multiarray> additive_expression primary_expression assignment_expression
%nterm <variant::operable_multiarray> composite_element_list_initialization
%nterm <variant::operable> CONSTANT
%nterm <std::pair<int,int>> composite_element_list_access
%nterm <int> arith_bool_expr_to_integer
%nterm <variant::operable_multiarray> function

// %printer { yyo << $$; } <*>; // debugging print

%%
%start program;

// QUICK FIX: Remove NEWLINE token in `program` grammar if the input reader appends a NEWLINE
// at the end and removes newlines before the first statement
program
    : NEWLINE statement_list
	| statement_list
    ;

statement_list
	: statement_list expression
	| expression
	;

expression
	: assignment_expression NEWLINE { 
		symtab[$1.identifier.value()] = $1;
		std::cout << $1.identifier.value() << " = " << $1 << std::endl;
	}
	| arith_bool_exprs NEWLINE {
		std::cout << $1 << std::endl;
	}
	;

assignment_expression
	: IDENTIFIER EQUALS_SIGN arith_bool_exprs
		{ 
			$$ = $3;
			$$.identifier = $1;
		}
	;

arith_bool_exprs 
	: logical_or_expression { $$ = $1; }
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR logical_and_expression { $$ = $1 || $3; }
	;

logical_and_expression
	: equality_expression { $$ = $1; }
	| logical_and_expression AND equality_expression	{ $$ = $1 && $3; }
	;

equality_expression
	: relational_expression { $$ = $1; }
	| equality_expression DOUBLE_EQUAL relational_expression { $$ = $1 == $3; }
	| equality_expression BANG_EQUAL relational_expression	 { $$ = $1 != $3; }
	;

relational_expression
	: additive_expression { $$ = $1; }
	| relational_expression LOWER additive_expression			{ $$ = $1 < $3; }
	| relational_expression LOWER_EQUAL additive_expression		{ $$ = $1 <= $3; }
	| relational_expression GREATER additive_expression			{ $$ = $1 > $3; }
	| relational_expression GREATER_EQUAL additive_expression	{ $$ = $1 >= $3; }
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression PLUS multiplicative_expression { $$ = $1 + $3; }
	| additive_expression MINUS multiplicative_expression { $$ = $1 - $3; }
	;

multiplicative_expression
	: exponentiative_expression { $$ = $1; }
	| multiplicative_expression STAR exponentiative_expression 		{ $$ = $1 * $3; }
	| multiplicative_expression SLASH exponentiative_expression		{ $$ = $1 / $3; }
	| multiplicative_expression PERCENT exponentiative_expression	{ $$ = $1 % $3; }
	;

exponentiative_expression
	: unary_expression { $$ = $1; }
	| exponentiative_expression CIRCUMFLEX unary_expression { $$ = $1 ^ $3; }
	;

unary_expression
	: postfix_expression { $$ = $1;  }
	| PLUS postfix_expression  { $$ = +$2; }
	| MINUS postfix_expression { $$ = -$2; }
	| BANG postfix_expression  { $$ = !$2; }
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
	: arith_bool_expr_to_integer COMMA arith_bool_expr_to_integer { $$.first = $1-1; $$.second = $3-1; } // [x,y]
	| arith_bool_expr_to_integer { $$.first = $1-1; } // [x]
	;

arith_bool_expr_to_integer
	: arith_bool_exprs {
		const variant::operable *scalar_equivalent = $1.scalar_equivalent();
		try {
			$$ = std::get<int>(scalar_equivalent->value);
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
			$$ = symtab[$1];
		}
	| CONSTANT		{ $$.insert_row_element($1); }
	| LPAREN arith_bool_exprs RPAREN { $$ = $2; }
	| LSQRBRKT composite_element_list_initialization RSQRBRKT { $$ = $2; }
	| function { $$ = $1; }
	;

composite_element_list_initialization
	: composite_element_list_initialization CONSTANT { $1.insert_row_element($2); $$ = $1; }
	| composite_element_list_initialization SEMICOLON { $$ = $1; $$.add_new_row(); }
	| CONSTANT { $$.insert_row_element($1); }
	;

function
	: FUNC_DIV 		 arith_bool_exprs COMMA arith_bool_exprs RPAREN { $$ = builtin::div($2, $4); }
	| FUNC_LENGTH	 arith_bool_exprs RPAREN { $$ = builtin::length($2); }
	| FUNC_SIZE		 arith_bool_exprs RPAREN { $$ = builtin::size($2); }
	| FUNC_TRANSPOSE arith_bool_exprs RPAREN { $$ = builtin::transpose($2); }
	;

CONSTANT
	: INTEGER { $$.value = $1; }
	| FLOAT	  { $$.value = $1; }
	| STRING  { $$.value = $1; }
	| BOOLEAN { $$.value = $1; }
	;

%%

void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
