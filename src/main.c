#include "Parser.h"
#include "Lexer.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(stderr, "Invalid params\n");
        return -4;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        return -3;
    }

    yyscan_t scanner;
    if (yylex_init(&scanner))
    {
        printf("Error initializing");
        return -2;
    }
    
    yyset_in(fp, scanner);

    if (yyparse(scanner))
    {
        printf("Error parsing\n");
        return -1;
    }

    yylex_destroy(scanner);
    
    fclose(fp);

    printf("=== Scanned === \n");
    return 0;
}