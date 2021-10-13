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

%token TOKEN_INTEGER TOKEN_FLOAT
%token TOKEN_IDENTIFIER
%token TOKEN_EQUALS_SIGN TOKEN_STAR TOKEN_PLUS TOKEN_LPAREN TOKEN_RPAREN TOKEN_PERCENT TOKEN_CARET
%token TOKEN_AND TOKEN_OR TOKEN_NOT
%token TOKEN_DOUBLE_EQUAL TOKEN_EXCLAMATION_EQUAL TOKEN_GREATER TOKEN_LOWER TOKEN_GREATER_EQUAL TOKEN_LOWER_EQUAL

%%

input
    : arithmetic_expression { }
    ;

arithmetic_expression
    : TOKEN_IDENTIFIER  { }
    | TOKEN_INTEGER      {}
    ;


%%

void yyerror(YYLTYPE* yyllocp, yyscan_t unused, const char* msg) {
  fprintf(stderr, "[%d:%d]: %s\n", yyllocp->first_line, yyllocp->first_column, msg);
}