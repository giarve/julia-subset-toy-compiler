%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdbool.h>

  #include "Parser.h"
  #include "Lexer.h"

  extern int yylex();
  extern int yyerror();
%}

%code requires {
  typedef void* yyscan_t;
}

%output  "Parser.c"
%defines "Parser.h"

%define parse.error verbose

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }


%token TOKEN_INTEGER TOKEN_FLOAT TOKEN_IDENTIFIER
%token TOKEN_STAR TOKEN_PLUS TOKEN_LPAREN TOKEN_RPAREN TOKEN_PERCENT TOKEN_CARET
%token TOKEN_AND TOKEN_OR TOKEN_NOT
%token TOKEN_EQUALS TOKEN_NOT_EQUALS TOKEN_GREATER TOKEN_LOWER TOKEN_GREATER_EQUAL TOKEN_LOWER_EQUAL

%%

input
    : arithmetic_expression { }
    ;

arithmetic_expression
    : TOKEN_IDENTIFIER  {}
    | TOKEN_INTEGER      {}
    ;


%%