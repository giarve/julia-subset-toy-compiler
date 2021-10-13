#include <stdio.h>

#include "Parser.h"
#include "Lexer.h"

#define DEBUG_ENABLED 1

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid params. Expected: ./minijuliac in.jl out.txt\n");
        return -4;
    }

    FILE *fp_in = fopen(argv[1], "r");
    if (fp_in == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        return -3;
    }

    FILE *fp_out = fopen(argv[2], "w");
    if (fp_out == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", argv[2]);
        fclose(fp_in);
        return -5;
    }

    yyscan_t scanner;
    if (yylex_init(&scanner))
    {
        printf("Error initializing");
        return -2;
    }

    yyset_debug(DEBUG_ENABLED, scanner);
    yyset_in(fp_in, scanner);
    yyset_out(fp_out, scanner);

    if (yyparse(scanner))
    {
        printf("=== Error parsing ===\n");
        return -1;
    }

    yylex_destroy(scanner);

    fclose(fp_in);
    fclose(fp_out);

    printf("=== Finished successfully === \n");
    return 0;
}