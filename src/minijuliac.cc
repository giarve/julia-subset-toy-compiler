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
    drv.trace_scanning = false;

    if(argv[3])
    {
        drv.trace_parsing = true;
        drv.trace_scanning = true;
    }
    
    if(drv.parse(argv[1], argv[2]) != 0)
        ;
        //std::cout << "Error code: " << drv.result << std::endl;
    else
        std::cout << "=== Success ==" << std::endl;

    return 0;
}