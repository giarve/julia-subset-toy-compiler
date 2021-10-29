#include "customlog.hh"

template <class T>
outlog &outlog::operator<<(const T &out)
{
    std::cout << out;
    logfile << out;

    return *this;
}

void outlog::cout(const std::string &out)
{
    std::cout << out;
    logfile << out;
}