#include <stdio.h>

#include "driver.hh"

#define DEBUG_ENABLED true

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid params. Expected: ./minijuliac in.jl out.txt\n");
        return -4;
    }

    driver drv;
    drv.trace_parsing = DEBUG_ENABLED;
    drv.trace_scanning = DEBUG_ENABLED;
    drv.parse(argv[1]);
    std::cout << drv.result << '\n';

    printf("=== Finished successfully === \n");
    return 0;
}