#include <stdio.h>

#include "driver.hh"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid params. Expected: ./minijuliac in.jl out.txt\n");
        return -4;
    }

    driver drv;
    drv.trace_parsing = false;
    drv.trace_scanning = true;
    
    if(!drv.parse(argv[1]))
        std::cout << "Error code: " << drv.result << '\n';

    printf("=== Finished successfully === \n");
    return 0;
}