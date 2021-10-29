#ifndef DRIVER_HH
#define DRIVER_HH

#include <string>
#include <map>

#include "customlog.hh"
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

  std::map<std::string, int> variables;

  int result;

  // Run the parser on file F.  Return 0 on success.
  int parse(const std::string &fin, const std::string &fout);

   // The name of the file being parsed.
  std::string path_file_in; 
  std::string path_file_out;

  // Whether to generate parser debug traces.
  bool trace_parsing;

  outlog *output;

  // Handling the scanner.
  void scan_begin();
  void scan_end();
  // Whether to generate scanner debug traces.
  bool trace_scanning;
  // The token's location used by the scanner.
  yy::location location;
};
#endif // ! DRIVER_HH
