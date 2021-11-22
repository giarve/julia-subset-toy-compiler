#include "driver.hh"
#include "parser.hh"

driver::driver()
{
}

std::string driver::getLoc()
{
    std::ostringstream ss;
    ss << location;
    return ss.str();
}

int driver::parse(const std::string &fin, const std::string &fout)
{
    path_file_in = fin;
    path_file_out = fout;

    RootModule->setSourceFileName(path_file_in);

    // idk if thats the best way but taken from kaleidoscope tutorial
    if (optimize)
    {
        // Promote allocas to registers.
        FunctionPassManager->add(llvm::createPromoteMemoryToRegisterPass());
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        FunctionPassManager->add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        FunctionPassManager->add(llvm::createReassociatePass());
        // Eliminate Common SubExpressions.
        FunctionPassManager->add(llvm::createGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        FunctionPassManager->add(llvm::createCFGSimplificationPass());
    }
    FunctionPassManager->doInitialization();

    location.initialize(&path_file_in);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();

    std::error_code EC;
    auto out = std::make_unique<llvm::raw_fd_ostream>(path_file_out, EC);
    RootModule->print(*out, nullptr);

    return res;
}
