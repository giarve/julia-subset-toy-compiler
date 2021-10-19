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
  #include <stdbool.h>
}
%code requires {
  typedef void* yyscan_t;
}
%code {
  int yylex(YYSTYPE* yylvalp, YYLTYPE* yyllocp, yyscan_t scanner);
  void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const char* msg);
}

%token TOKEN_INTEGER TOKEN_FLOAT TOKEN_STRING 
%token TOKEN_IDENTIFIER
%token TOKEN_EQUALS_SIGN TOKEN_STAR TOKEN_PLUS TOKEN_LPAREN TOKEN_RPAREN TOKEN_PERCENT TOKEN_CARET TOKEN_SLASH TOKEN_SEMICOLON
%token TOKEN_AND TOKEN_OR TOKEN_BANG
%token TOKEN_DOUBLE_EQUAL TOKEN_EXCLAMATION_EQUAL TOKEN_GREATER TOKEN_LOWER TOKEN_GREATER_EQUAL TOKEN_LOWER_EQUAL
%token TOKEN_TRUE TOKEN_FALSE
%token TOKEN_RSQRBRKT TOKEN_LSQRBRKT

%start program

%%

program
    : statement_list
    ;

statement_list
	: statement_list expression
	| expression
	;

expression
	: assignment_expression
	;

assignment_expression
	: TOKEN_IDENTIFIER ASSIGNMENT_OPERATOR var_definition { printf("statement\n"); }
	;

ASSIGNMENT_OPERATOR
	: TOKEN_EQUALS_SIGN
	| TOKEN_STAR
	;

var_definition
	: SIMPLE_TYPE
	| TOKEN_LSQRBRKT composite_element_list TOKEN_RSQRBRKT
	;

SIMPLE_TYPE
	: TOKEN_INTEGER
	| TOKEN_FLOAT
	| TOKEN_STRING
	| TOKEN_TRUE
	| TOKEN_FALSE
	;

composite_element_list
	: composite_element_list SIMPLE_TYPE
	| composite_element_list TOKEN_SEMICOLON
	| SIMPLE_TYPE
	;

%%

void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const char* msg) {
  fprintf(stderr, "[%d:%d]: %s\n", yyllocp->first_line, yyllocp->first_column, msg);
}
