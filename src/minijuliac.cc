#include <stdio.h>

#include "llvm/IR/LegacyPassManager.h"

#include "driver.hh"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid params. Expected: ./minijuliac in.jl out.txt\n");
        return -4;
    }

    driver drv;
    if (argv[3])
    {
        if (*argv[3] == 'O')
        {
            drv.optimize = true;
        }
        else if (*argv[3] == 'D')
        {
            drv.trace_parsing = true;
            drv.trace_scanning = true;
        }
    }

    if (drv.parse(argv[1], argv[2]) != 0)
        ;
    // std::cout << "Error code: " << drv.result << std::endl;
    else
    {
        drv.RootModule->print(llvm::outs(), nullptr); // Dump IR to stdout
        std::cout << "=== Success ==" << std::endl;
    }

    return 0;
}