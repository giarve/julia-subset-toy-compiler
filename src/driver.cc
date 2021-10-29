#include "driver.hh"
#include "parser.hh"

driver::driver()
    : trace_parsing(false), trace_scanning(false)
{
    variables["one"] = 1;
    variables["two"] = 2;
}

int driver::parse(const std::string &fin, const std::string &fout)
{
    path_file_in = fin;
    path_file_out = fout;

    output = new outlog(path_file_out.c_str());

    location.initialize(&path_file_in);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();

    delete output;

    return res;
}
