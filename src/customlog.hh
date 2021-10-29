#ifndef CUSTOMLOG_HH
#define CUSTOMLOG_HH

#include <iostream>
#include <fstream>

class outlog
{
private:
    std::ofstream logfile;

public:
    outlog(const char *file)
    {
        logfile.open(file);

        if (!(logfile.is_open()))
        {
            std::cerr << "Error opening \"" << file << "\" as output file.\n";
            this->~outlog();
        }
    }

    ~outlog()
    {
        if (logfile.is_open())
            logfile.close();
    }

    template <class T>
    outlog &operator<<(const T &out);
    void cout(const std::string &out);
};

#endif // CUSTOMLOG_HH