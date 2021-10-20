%define api.pure
%define parse.error verbose

%locations
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }

%output  "Parser.c"
%defines "Parser.h"

%code top{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
}
%code requires {
  #include <stdbool.h>

  typedef void* yyscan_t;
}
%code {
  int yylex(YYSTYPE* yylvalp, YYLTYPE* yyllocp, yyscan_t scanner);
  void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const char* msg);
}

%union {
	char *string;
	char *identifier; // make it a struct
	int integer;
	float floaT;
	bool true_or_false;
}

%token <integer> TOKEN_INTEGER
%token <floaT> TOKEN_FLOAT
%token <string> TOKEN_STRING 
%token <identifier> TOKEN_IDENTIFIER
%token TOKEN_EQUALS_SIGN TOKEN_STAR TOKEN_PLUS TOKEN_MINUS
%token TOKEN_LPAREN TOKEN_RPAREN TOKEN_PERCENT TOKEN_CIRCUMFLEX TOKEN_SLASH TOKEN_SEMICOLON TOKEN_COMMA
%token TOKEN_AND TOKEN_OR TOKEN_BANG
%token TOKEN_DOUBLE_EQUAL TOKEN_BANG_EQUAL 
%token TOKEN_GREATER TOKEN_LOWER TOKEN_GREATER_EQUAL TOKEN_LOWER_EQUAL
%token <true_or_false> TOKEN_TRUE
%token <true_or_false> TOKEN_FALSE
%token TOKEN_RSQRBRKT TOKEN_LSQRBRKT
%token TOKEN_FUNC_TRANSPOSE
%token TOKEN_NEWLINE

%start program

%%

// reverse order program top to bottom

CONSTANT
	: TOKEN_INTEGER
	| TOKEN_FLOAT
	| TOKEN_STRING
	| TOKEN_TRUE
	| TOKEN_FALSE
	;

// We don't have to implement declarations

composite_element_list_initialization
	: composite_element_list_initialization CONSTANT
	| composite_element_list_initialization TOKEN_SEMICOLON
	| CONSTANT
	;

primary_expression
	: TOKEN_IDENTIFIER
	| CONSTANT
	| TOKEN_LPAREN arithmetic_boolean_expressions_sentence TOKEN_RPAREN
	| TOKEN_LSQRBRKT composite_element_list_initialization TOKEN_RSQRBRKT // array/vector
	;

postfix_expression
	: primary_expression
	| postfix_expression TOKEN_LSQRBRKT arithmetic_boolean_expressions_sentence TOKEN_RSQRBRKT // index access
	;

unary_expression
	: postfix_expression
	| unary_operator postfix_expression
	| TOKEN_FUNC_TRANSPOSE TOKEN_LPAREN TOKEN_IDENTIFIER TOKEN_RPAREN

unary_operator
	: TOKEN_PLUS
	| TOKEN_MINUS
	| TOKEN_BANG
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression TOKEN_STAR unary_expression
	| multiplicative_expression TOKEN_SLASH unary_expression
	| multiplicative_expression TOKEN_PERCENT unary_expression
	;

exponentiative_expression
	: multiplicative_expression
	| exponentiative_expression TOKEN_CIRCUMFLEX multiplicative_expression
	;

additive_expression
	: exponentiative_expression
	| additive_expression TOKEN_PLUS exponentiative_expression
	| additive_expression TOKEN_MINUS exponentiative_expression
	;

relational_expression
	: additive_expression
	| relational_expression TOKEN_LOWER additive_expression
	| relational_expression TOKEN_LOWER_EQUAL additive_expression
	| relational_expression TOKEN_GREATER additive_expression
	| relational_expression TOKEN_GREATER_EQUAL additive_expression
	;

equality_expression
	: relational_expression
	| equality_expression TOKEN_DOUBLE_EQUAL relational_expression
	| equality_expression TOKEN_BANG_EQUAL relational_expression
	;

logical_and_expression
	: equality_expression
	| logical_and_expression TOKEN_AND equality_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression TOKEN_OR logical_and_expression

// Problema, no pot distingir tal i com esta fet entre una i altra
// separar aritmetica i booleana? o fer IDENTIFIER_bool com diria a classe
arithmetic_boolean_expressions_sentence 
	: logical_or_expression
	;

assignment_expression
	: TOKEN_IDENTIFIER TOKEN_EQUALS_SIGN arithmetic_boolean_expressions_sentence
	;

expression
	: assignment_expression TOKEN_NEWLINE
	| arithmetic_boolean_expressions_sentence TOKEN_NEWLINE
	;

statement_list
	: statement_list expression
	| expression
	;

program
    : statement_list
    ;

%%

void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const char* msg) {
  fprintf(stderr, "[%d:%d]: %s\n", yyllocp->first_line, yyllocp->first_column, msg);
}
