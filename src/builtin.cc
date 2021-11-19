#include <cmath>

#include "builtin.hh"

namespace builtin
{
    int div(const int lhs, const int rhs)
    {
        if (rhs == 0)
            throw variant::SemanticException("division by zero");

        return lhs / rhs;
    }
}
