#include "Parser.h"
#include "Lexer.h"

#include <stdio.h>

int main(void)
{
    char expr[] = "41212"; // TODO: read input from file * .jl

    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner))
    {
        /* could not initialize */
        printf("Error initializing");
        return -2;
    }

    state = yy_scan_string(expr, scanner);

    if (yyparse(scanner))
    {
        /* error parsing */
        printf("Error parsing %s \n", expr);
        return -1;
    }

    yy_delete_buffer(state, scanner);

    yylex_destroy(scanner);

    printf("=== Scanned %s === \n", expr);
    return 0;
}