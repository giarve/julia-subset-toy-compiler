#ifndef BUILTIN_HH
#define BUILTIN_HH


#include "variant.hh"

namespace builtin
{
    int div(const int lhs, const int rhs);
    variant::operable_multiarray size(const variant::operable_multiarray &op);
    int length(const variant::operable_multiarray &op);
    variant::operable_multiarray transpose(const variant::operable_multiarray &op);
    variant::operable_multiarray fill_with(variant::JuliaType withType, int with, int n_rows, int n_columns);
}

#endif // ! BUILTIN_HH