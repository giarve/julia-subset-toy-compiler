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
  #include <unordered_map>

  std::unordered_map<std::string, variant::operable> symtab;
}


%define api.token.prefix {TOKEN_}

%token <int> INTEGER
%token <float> FLOAT
%token <std::string> STRING 
%token <std::string> IDENTIFIER
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

%nterm <variant::operable> arithmetic_boolean_expressions_sentence
%nterm <variant::operable> logical_or_expression logical_and_expression equality_expression relational_expression
%nterm <variant::operable> exponentiative_expression multiplicative_expression unary_expression postfix_expression
%nterm <variant::operable> additive_expression primary_expression assignment_expression CONSTANT

// %printer { yyo << $$.value.type().name(); } <variant::operable>;
// %printer { yyo << $$.name << ": " << $$.value.type().name() << " = "; /* << $$.value; */ } <variant::identifier>;
// %printer { yyo << $$; } <*>;


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
	: assignment_expression NEWLINE { 
		symtab[$1.identifier.value()] = $1;
		std::cout << $1.identifier.value() << " = " << $1 << std::endl;
	}
	| arithmetic_boolean_expressions_sentence NEWLINE	{} 
	// | boolean_expression
	// | arithmetic_expression
	;

assignment_expression
	: IDENTIFIER EQUALS_SIGN arithmetic_boolean_expressions_sentence
		{ 
			$$.identifier = $1;
			$$.value = $3.value;
		}
	;

// IDENTIFIER_bool or separate, look for examples
arithmetic_boolean_expressions_sentence 
	: logical_or_expression { $$ = $1;  }
	;

logical_or_expression
	: logical_and_expression { $$ = $1;  }
	| logical_or_expression OR logical_and_expression
	;

logical_and_expression
	: equality_expression { $$ = $1;  }
	| logical_and_expression AND equality_expression
	;

equality_expression
	: relational_expression { $$ = $1;  }
	| equality_expression DOUBLE_EQUAL relational_expression
	| equality_expression BANG_EQUAL relational_expression
	;

relational_expression
	: additive_expression { $$ = $1;  }
	| relational_expression LOWER additive_expression
	| relational_expression LOWER_EQUAL additive_expression
	| relational_expression GREATER additive_expression
	| relational_expression GREATER_EQUAL additive_expression
	;

additive_expression
	: exponentiative_expression { $$ = $1;  }
	| additive_expression PLUS exponentiative_expression { $$ = $1 + $3;  }
	| additive_expression MINUS exponentiative_expression { $$ = $1 - $3;  }
	;

exponentiative_expression
	: multiplicative_expression { $$ = $1;  }
	| exponentiative_expression CIRCUMFLEX multiplicative_expression
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression STAR unary_expression 		{ $$ = $1 * $3;  }
	| multiplicative_expression SLASH unary_expression		{ $$ = $1 / $3;  }
	| multiplicative_expression PERCENT unary_expression	{ $$ = $1 % $3;  }
	;

UNARY_OPERATOR
	: PLUS
	| MINUS
	| BANG
	;

unary_expression
	: postfix_expression { $$ = $1;   }
	| UNARY_OPERATOR postfix_expression { $$ = $2; }
	;

postfix_expression
	: primary_expression { $$ = $1;   }
	| postfix_expression LSQRBRKT arithmetic_boolean_expressions_sentence RSQRBRKT // index access
	;

// create a function expression?
// | FUNC_TRANSPOSE LPAREN IDENTIFIER RPAREN

primary_expression
	: IDENTIFIER	{
		if(!symtab.contains($1))
			throw yy::parser::syntax_error(drv.location, "use of undeclared identifier: " + $1);
		else
			$$ = symtab[$1];
		$$.identifier = $1;
		}
	| CONSTANT		{ $$ = $1;  }
	| LPAREN arithmetic_boolean_expressions_sentence RPAREN { $$ = $2; }
	| LSQRBRKT composite_element_list_initialization RSQRBRKT
	;

composite_element_list_initialization  // array/vector
	: composite_element_list_initialization CONSTANT
	| composite_element_list_initialization SEMICOLON
	| CONSTANT
	;

CONSTANT
	: INTEGER { $$ = $1; }
	| FLOAT	  { $$ = $1; }
	| STRING  { $$ = $1; }
	| TRUE	
	| FALSE	
	;

%%

void yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
