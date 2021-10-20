%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdbool.h>

  #include "./Parser.h"
%}

%option outfile="Lexer.c" header-file="Lexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive yylineno
%option bison-bridge bison-locations
%option nounput noinput 


%x COMMENT_MULTILINE
%x COMMENT_SINGLELINE

DIGIT [0-9]
IDENTIFIER [a-zA-Z](_{0,1}[a-zA-Z0-9])*
INTEGER {DIGIT}+
FLOAT {INTEGER}\.{INTEGER}
NEWLINE \n|\r\n
STRING \".*\"

%%

[ \t]+    { /* ignore whitespaces, NEWLINEs and tabs */ }

"#"                            { BEGIN(COMMENT_SINGLELINE); }
<COMMENT_SINGLELINE>{NEWLINE}+ { BEGIN(INITIAL); }
<COMMENT_SINGLELINE>.          { }

"#="                              { BEGIN(COMMENT_MULTILINE); }
<COMMENT_MULTILINE>"=#"{NEWLINE}* { BEGIN(INITIAL); }
<COMMENT_MULTILINE>{NEWLINE}      { }
<COMMENT_MULTILINE>.              { }

";"           { return TOKEN_SEMICOLON; }
","           { return TOKEN_COMMA; }
"\["          { return TOKEN_LSQRBRKT; }
"\]"          { return TOKEN_RSQRBRKT; }

"="           { return TOKEN_EQUALS_SIGN; }
"*"           { return TOKEN_STAR; }
"+"           { return TOKEN_PLUS; }
"-"           { return TOKEN_MINUS; }
"^"           { return TOKEN_CIRCUMFLEX; }
"/"           { return TOKEN_SLASH; }
"%"           { return TOKEN_PERCENT; }
"("           { return TOKEN_LPAREN; }
")"           { return TOKEN_RPAREN; }

"=="          { return TOKEN_DOUBLE_EQUAL; }
"!="          { return TOKEN_BANG_EQUAL; }
">"           { return TOKEN_GREATER; }
"<"           { return TOKEN_LOWER; }
">="          { return TOKEN_GREATER_EQUAL; }
"<="          { return TOKEN_LOWER_EQUAL; }

"true"        { return TOKEN_TRUE; }
"false"       { return TOKEN_FALSE; }
"&&"          { return TOKEN_AND; }
"||"          { return TOKEN_OR; }
"!"           { return TOKEN_BANG; }

"transpose"   { return TOKEN_FUNC_TRANSPOSE; }

{NEWLINE}+    { return TOKEN_NEWLINE; }
{FLOAT}       { sscanf(yytext, "%f", &yylval->floaT);     return TOKEN_FLOAT; }
{INTEGER}     { sscanf(yytext, "%d", &yylval->integer);   return TOKEN_INTEGER; }
{STRING}      { yylval->string = strdup(yytext);          return TOKEN_STRING; }
{IDENTIFIER}  { yylval->identifier.name = strdup(yytext); return TOKEN_IDENTIFIER; }

.             { fprintf(stderr, "Lexical error - Unexpected: %s\n", yytext); }

%%
