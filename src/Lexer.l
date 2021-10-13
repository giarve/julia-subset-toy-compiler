%{
  #include "./Parser.h"
  #define RET_TEXT yylval.sval = malloc(strlen(yytext)); strncpy(yylval.sval, yytext, strlen(yytext));
%}

%option outfile="Lexer.c" header-file="Lexer.h"
%option warn nodefault
%option reentrant noyywrap never-interactive nounistd yylineno
%option bison-bridge bison-locations
%option nounput noinput 


%x COMMENT_MULTILINE
%x COMMENT_SINGLELINE

DIGIT [0-9]
IDENTIFIER [a-zA-Z][a-zA-Z0-9]* 
INTEGER {DIGIT}+
FLOAT {INTEGER}\.{INTEGER}
NEWLINE \n|\r\n

%%

[ \t\r]+    { /* ignore whitespaces and tabs */ }

"#"                           { BEGIN(COMMENT_SINGLELINE); }
<COMMENT_SINGLELINE>{NEWLINE} { BEGIN(INITIAL); }
<COMMENT_SINGLELINE>.         { }

"#="                          { BEGIN(COMMENT_MULTILINE); }
<COMMENT_MULTILINE>"=#"       { BEGIN(INITIAL); }
<COMMENT_MULTILINE>{NEWLINE}  { }
<COMMENT_MULTILINE>.          { }

{NEWLINE}     { return TOKEN_NEWLINE; }
{INTEGER}     { return TOKEN_INTEGER; }
{FLOAT}       { return TOKEN_FLOAT; }
{IDENTIFIER}  { return TOKEN_IDENTIFIER; /* Fix this, shouldn't allow __ in the middle and _ at the end  */ }


";"           { return TOKEN_SEMICOLON; }

"="           { return TOKEN_EQUALS_SIGN; }
"*"           { return TOKEN_STAR; }
"+"           { return TOKEN_PLUS; }
"^"           { return TOKEN_CARET; }
"/"           { return TOKEN_SLASH; }
"%"           { return TOKEN_PERCENT; }
"("           { return TOKEN_LPAREN; }
")"           { return TOKEN_RPAREN; }

"=="          { return TOKEN_DOUBLE_EQUAL; }
"!="          { return TOKEN_EXCLAMATION_EQUAL; }
">"           { return TOKEN_GREATER; }
"<"           { return TOKEN_LOWER; }
">="          { return TOKEN_GREATER_EQUAL; }
"<="          { return TOKEN_LOWER_EQUAL; }

"&&"          { return TOKEN_AND; }
"||"          { return TOKEN_OR; }
"!"           { return TOKEN_BANG; }

.             { fprintf(stderr, "Lexer - Unexpected: %s\n", yytext); }

%%
