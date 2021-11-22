#ifndef DRIVER_HH
#define DRIVER_HH

#include <string>
#include <map>

#include "parser.hh"

// Give Flex the prototype of yylex we want ...
#define YY_DECL \
  yy::parser::symbol_type yylex(driver &drv)
// ... and declare it for the parser's sake.
YY_DECL;

// Conducting the whole scanning and parsing.
class driver
{
public:
  driver();

  int result;

  // Run the parser on file F.  Return 0 on success.
  int parse(const std::string &fin, const std::string &fout);

  // The name of the file being parsed.
  std::string path_file_in;
  std::string path_file_out;

  // Whether to generate parser debug traces.
  bool trace_parsing = false;

  // Handling the scanner.
  void scan_begin();
  void scan_end();
  // Whether to generate scanner debug traces.
  bool trace_scanning = false;
  // The token's location used by the scanner.
  yy::location location;

  std::string getLoc();

  bool optimize = false;

  std::unique_ptr<llvm::LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
  std::unique_ptr<llvm::Module> RootModule = std::make_unique<llvm::Module>("root_module", *TheContext);
  std::unique_ptr<llvm::IRBuilder<>> Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
  std::unique_ptr<llvm::legacy::FunctionPassManager> FunctionPassManager = std::make_unique<llvm::legacy::FunctionPassManager>(RootModule.get());
};
#endif // ! DRIVER_HH
