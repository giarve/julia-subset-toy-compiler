%{
  #include <cerrno>
  #include <climits>
  #include <cstdlib>
  #include <cstring> // strerror
  #include <string>

  #include "driver.hh"
  #include "parser.hh"
%}

%option noyywrap nounput noinput batch debug

%{
  // A number symbol corresponding to the value in S.
  yy::parser::symbol_type make_INTEGER (const std::string &s, const yy::parser::location_type& loc);
%}

%x COMMENT_MULTILINE
%x COMMENT_SINGLELINE

DIGIT [0-9]
IDENTIFIER [a-zA-Z](_{0,1}[a-zA-Z0-9])*
INTEGER {DIGIT}+
FLOAT {INTEGER}\.{INTEGER}
NEWLINE \n|\r\n

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}

[ \t]+    { /* ignore whitespaces, NEWLINEs and tabs */ }

"#"                            BEGIN(COMMENT_SINGLELINE);
<COMMENT_SINGLELINE>{NEWLINE}+ BEGIN(INITIAL);
<COMMENT_SINGLELINE>.          { }

"#="                              BEGIN(COMMENT_MULTILINE);
<COMMENT_MULTILINE>"=#"{NEWLINE}* BEGIN(INITIAL);
<COMMENT_MULTILINE>{NEWLINE}      { }
<COMMENT_MULTILINE>.              { }

";"           return yy::parser::make_SEMICOLON (loc);
","           return yy::parser::make_COMMA     (loc);
"\["          return yy::parser::make_LSQRBRKT  (loc);
"\]"          return yy::parser::make_RSQRBRKT  (loc);

"="           return yy::parser::make_EQUALS_SIGN (loc);
"*"           return yy::parser::make_STAR        (loc);
"+"           return yy::parser::make_PLUS        (loc);
"-"           return yy::parser::make_MINUS       (loc);
"^"           return yy::parser::make_CIRCUMFLEX  (loc);
"/"           return yy::parser::make_SLASH       (loc);
"%"           return yy::parser::make_PERCENT     (loc);
"("           return yy::parser::make_LPAREN      (loc);
")"           return yy::parser::make_RPAREN      (loc);

"=="          return yy::parser::make_DOUBLE_EQUAL  (loc);
"!="          return yy::parser::make_BANG_EQUAL    (loc);
">"           return yy::parser::make_GREATER       (loc);
"<"           return yy::parser::make_LOWER         (loc);
">="          return yy::parser::make_GREATER_EQUAL (loc);
"<="          return yy::parser::make_LOWER_EQUAL   (loc);

"true"        return yy::parser::make_TRUE  (loc);
"false"       return yy::parser::make_FALSE (loc);
"&&"          return yy::parser::make_AND   (loc);
"||"          return yy::parser::make_OR    (loc);
"!"           return yy::parser::make_BANG  (loc);

"transpose"   return yy::parser::make_FUNC_TRANSPOSE (loc);

{NEWLINE}+    return yy::parser::make_NEWLINE    (loc);
{FLOAT}       return yy::parser::make_FLOAT      (loc);
{INTEGER}     return make_INTEGER(yytext, loc);
\".*\"        return yy::parser::make_STRING     (loc);
{IDENTIFIER}  return yy::parser::make_IDENTIFIER (loc);

.             throw yy::parser::syntax_error(loc, "invalid character: " + std::string(yytext));

<<EOF>>       return yy::parser::make_YYEOF (loc);

%%


yy::parser::symbol_type make_INTEGER(const std::string &s, const yy::parser::location_type &loc)
{
    errno = 0;
    long n = strtol(s.c_str(), NULL, 10);
    if (!(INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
        throw yy::parser::syntax_error(loc, "integer is out of range: " + s);
    return yy::parser::make_INTEGER(1, loc);
}

void driver::scan_begin()
{
    yy_flex_debug = trace_scanning;
    if (file.empty() || file == "-")
        yyin = stdin;
    else if (!(yyin = fopen(file.c_str(), "r")))
    {
        std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

void driver::scan_end()
{
    fclose(yyin);
}