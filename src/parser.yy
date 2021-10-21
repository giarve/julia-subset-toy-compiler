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
}


%define api.token.prefix {TOKEN_}

%token <int> INTEGER
%token <float> FLOAT
%token <std::string> STRING 
%token <variant::identifier> IDENTIFIER
%token EQUALS_SIGN STAR PLUS MINUS
%token LPAREN RPAREN PERCENT CIRCUMFLEX SLASH SEMICOLON COMMA
%token AND OR BANG
%token DOUBLE_EQUAL BANG_EQUAL 
%token GREATER LOWER GREATER_EQUAL LOWER_EQUAL
%token TRUE
%token FALSE
%token RSQRBRKT LSQRBRKT
%token FUNC_TRANSPOSE
%token NEWLINE

%printer { yyo << $$.name << ": " << $$.type << " = " << $$.value; } <variant::identifier>;
%printer { yyo << $$; } <*>;


%%
%start program;

program
    : statement_list
    ;

statement_list
	: statement_list expression
	| expression
	;

expression
	: assignment_expression NEWLINE					{ /* printf("%s", $1); */ }
	| arithmetic_boolean_expressions_sentence NEWLINE	{ /* printf("%s", $1); */ }
	// | boolean_expression
	// | arithmetic_expression
	;

assignment_expression
	: IDENTIFIER EQUALS_SIGN arithmetic_boolean_expressions_sentence {/* $$ = $1 + '=' concat $2 */}
	;

// IDENTIFIER_bool or separate, look for examples
arithmetic_boolean_expressions_sentence 
	: logical_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR logical_and_expression
	;

logical_and_expression
	: equality_expression
	| logical_and_expression AND equality_expression
	;

equality_expression
	: relational_expression
	| equality_expression DOUBLE_EQUAL relational_expression
	| equality_expression BANG_EQUAL relational_expression
	;

relational_expression
	: additive_expression
	| relational_expression LOWER additive_expression
	| relational_expression LOWER_EQUAL additive_expression
	| relational_expression GREATER additive_expression
	| relational_expression GREATER_EQUAL additive_expression
	;

additive_expression
	: exponentiative_expression
	| additive_expression PLUS exponentiative_expression
	| additive_expression MINUS exponentiative_expression
	;

exponentiative_expression
	: multiplicative_expression
	| exponentiative_expression CIRCUMFLEX multiplicative_expression
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression STAR unary_expression {  }
	| multiplicative_expression SLASH unary_expression
	| multiplicative_expression PERCENT unary_expression
	;

UNARY_OPERATOR
	: PLUS
	| MINUS
	| BANG
	;

unary_expression
	: postfix_expression
	| UNARY_OPERATOR postfix_expression
	| FUNC_TRANSPOSE LPAREN IDENTIFIER RPAREN
	;

postfix_expression
	: primary_expression
	| postfix_expression LSQRBRKT arithmetic_boolean_expressions_sentence RSQRBRKT // index access
	;

primary_expression
	: IDENTIFIER
	| CONSTANT
	| LPAREN arithmetic_boolean_expressions_sentence RPAREN		{ /* $$ = $2 */ }
	| LSQRBRKT composite_element_list_initialization RSQRBRKT	{  /* $$ = $2 */ }
	;

composite_element_list_initialization  // array/vector
	: composite_element_list_initialization CONSTANT
	| composite_element_list_initialization SEMICOLON
	| CONSTANT
	;

CONSTANT
	: INTEGER
	| FLOAT
	| STRING
	| TRUE
	| FALSE
	;

%%

void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
