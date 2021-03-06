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
  yy::parser::symbol_type make_INTEGER (const std::string &s, const yy::parser::location_type& loc);
  yy::parser::symbol_type make_FLOAT (const std::string &s, const yy::parser::location_type& loc);
  yy::parser::symbol_type make_IDENTIFIER (const std::string &s, const yy::parser::location_type& loc);
  yy::parser::symbol_type make_STRING (const std::string &s, const yy::parser::location_type& loc);
  yy::parser::symbol_type make_BOOLEAN (const std::string &s, const yy::parser::location_type& loc);
  yy::parser::symbol_type make_TYPE (const std::string &s, const yy::parser::location_type& loc);
%}

%x COMMENT_MULTILINE
%x COMMENT_SINGLELINE

DIGIT [0-9]
IDENTIFIER [a-zA-Z](_{0,1}[a-zA-Z0-9])*
FLOAT {DIGIT}*\.{DIGIT}+([eE][-+]?{DIGIT}+)?
INTEGER {DIGIT}+
NEWLINE \n
BLANK [ \t\r]
STRING \"([^\\\"]|\\.)*\"

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

{BLANK}+  loc.step ();

^{BLANK}*{NEWLINE} { loc.lines (1); loc.step(); }
^{NEWLINE}$ { loc.lines (1); loc.step(); }

{NEWLINE} { loc.lines (yyleng); loc.step(); return yy::parser::make_NEWLINE(loc); }

"#"                            BEGIN(COMMENT_SINGLELINE);
<COMMENT_SINGLELINE>{NEWLINE}  { loc.lines(1); loc.step(); BEGIN(INITIAL); }
<COMMENT_SINGLELINE>.          { }

"#="                              BEGIN(COMMENT_MULTILINE);
<COMMENT_MULTILINE>"=#"{NEWLINE}* { loc.lines(yyleng-2); loc.step(); BEGIN(INITIAL); }
<COMMENT_MULTILINE>{NEWLINE}      { loc.lines(yyleng); loc.step(); }
<COMMENT_MULTILINE>.              { }

";"           return yy::parser::make_SEMICOLON (loc);
":"           return yy::parser::make_COLON (loc);
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

"true"        return make_BOOLEAN(yytext, loc);
"false"       return make_BOOLEAN(yytext, loc);
"&&"          return yy::parser::make_AND   (loc);
"||"          return yy::parser::make_OR    (loc);
"!"           return yy::parser::make_BANG  (loc);


"pi"          return make_FLOAT("3.1415926535897", loc);

"function"     return yy::parser::make_FUNCTION_START(loc);
"end"          return yy::parser::make_END(loc);
"return"       return yy::parser::make_RETURN(loc);
"::"           return yy::parser::make_DOUBLECOLON(loc);
"if"           return yy::parser::make_IF(loc);
"else"         return yy::parser::make_ELSE(loc);
"elseif"       return yy::parser::make_ELSEIF(loc);
"while"        return yy::parser::make_WHILE(loc);
"for"          return yy::parser::make_FOR(loc);
"in"           return yy::parser::make_IN(loc);

"Int64"        return make_TYPE (yytext, loc);
"Float64"      return make_TYPE (yytext, loc);
"String"       return make_TYPE (yytext, loc);
"Bool"         return make_TYPE (yytext, loc);


{FLOAT}       return make_FLOAT(yytext, loc);
{INTEGER}     return make_INTEGER(yytext, loc);
{STRING}      return make_STRING(yytext, loc);
{IDENTIFIER}  return make_IDENTIFIER(yytext, loc);

.             throw yy::parser::syntax_error(loc, "Scanner - Invalid char: " + std::string(yytext));

<<EOF>>       return yy::parser::make_YYEOF (loc);

%%


yy::parser::symbol_type make_INTEGER(const std::string &s, const yy::parser::location_type &loc)
{
    errno = 0;
    long n = strtol(s.c_str(), NULL, 10);
    if (!(INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
        throw yy::parser::syntax_error(loc, "integer is out of range: " + s);
    return yy::parser::make_INTEGER(n, loc);
}


yy::parser::symbol_type make_FLOAT(const std::string &s, const yy::parser::location_type &loc)
{
    double n = strtod(s.c_str(), NULL);
    return yy::parser::make_FLOAT(n, loc);
}

yy::parser::symbol_type make_IDENTIFIER(const std::string &s, const yy::parser::location_type &loc)
{
    return yy::parser::make_IDENTIFIER(s, loc);
}

yy::parser::symbol_type make_STRING(const std::string &s, const yy::parser::location_type &loc)
{
    // We remove quotes from the start and end
    std::string without_quotes = std::string(s.begin() + 1, s.end()-1);
    return yy::parser::make_STRING(without_quotes, loc);
}

yy::parser::symbol_type make_BOOLEAN(const std::string &s, const yy::parser::location_type &loc)
{
    bool b = (s == "true");
    return yy::parser::make_BOOLEAN(b, loc);
}

yy::parser::symbol_type make_TYPE(const std::string &s, const yy::parser::location_type &loc)
{
    if(s == "Int64")
        return yy::parser::make_TYPE(JuliaType::Int64, loc);
    else if(s == "Float64")
        return yy::parser::make_TYPE(JuliaType::Float64, loc);
    else if(s == "String")
        return yy::parser::make_TYPE(JuliaType::String, loc);
    else
        return yy::parser::make_TYPE(JuliaType::Bool, loc);
}

void driver::scan_begin()
{
    yy_flex_debug = trace_scanning;

    if (path_file_in.empty() || path_file_in == "-")
        yyin = stdin;
    else if (!(yyin = fopen(path_file_in.c_str(), "r")))
    {
        std::cerr << "Cannot read input file `" << path_file_in << "`: " << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

void driver::scan_end()
{
    fclose(yyin);
}