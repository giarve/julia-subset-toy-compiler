#ifndef BUILTIN_HH
#define BUILTIN_HH


#include "variant.hh"

namespace builtin
{
    variant::operable_multiarray div(const variant::operable_multiarray &lhs, const variant::operable_multiarray &rhs);
    variant::operable_multiarray size(const variant::operable_multiarray &op);
    int length(const variant::operable_multiarray &op);
    variant::operable_multiarray transpose(const variant::operable_multiarray &op);
}

#endif // ! BUILTIN_HH