%{
  #include "./Parser.h"
  #define RET_TEXT yylval.sval = malloc(strlen(yytext)); strncpy(yylval.sval, yytext, strlen(yytext));
%}

%option outfile="Lexer.c" header-file="Lexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive nounistd yylineno
%option bison-bridge
%option nounput noinput

DIGIT [0-9]
IDENTIFIER [a-zA-Z][a-zA-Z0-9]* 
INTEGER {DIGIT}+
FLOAT {INTEGER}\.{INTEGER}

%%

[ \t\r\n]+    { continue; /* ignore whitespaces, tabs and newlines */ }

{INTEGER}     { return TOKEN_INTEGER; }
{FLOAT}       { return TOKEN_FLOAT; }
{IDENTIFIER}  { return TOKEN_IDENTIFIER; /* Fix this, shouldn't allow __ in the middle and _ at the end  */ }

"*"           { return TOKEN_STAR; }
"+"           { return TOKEN_PLUS; }
"^"           { return TOKEN_CARET; }
"%"           { return TOKEN_PERCENT; }
"("           { return TOKEN_LPAREN; }
")"           { return TOKEN_RPAREN; }

"=="          { return TOKEN_EQUALS; }
"!="          { return TOKEN_NOT_EQUALS; }
">"           { return TOKEN_GREATER; }
"<"           { return TOKEN_LOWER; }
">="          { return TOKEN_GREATER_EQUAL; }
"<="          { return TOKEN_LOWER_EQUAL; }

"&&"          { return TOKEN_AND; }
"||"          { return TOKEN_OR; }
"!"           { return TOKEN_NOT; }

.             { continue; /* Ignore unexpected characters. */}

%%

int yyerror(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}