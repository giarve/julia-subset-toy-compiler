#ifndef BUILTIN_HH
#define BUILTIN_HH

#include <functional>

#include "variant.hh"

namespace builtin
{
    int div(const int lhs, const int rhs);
    variant::operable_multiarray size(const variant::operable_multiarray &op);
    int length(const variant::operable_multiarray &op);
    variant::operable_multiarray transpose(const variant::operable_multiarray &op);
    variant::operable_multiarray fill_with(variant::JuliaType withType, int with, int n_rows, int n_columns);

    variant::operable_multiarray sin(const variant::operable_multiarray &op);
    variant::operable_multiarray cos(const variant::operable_multiarray &op);
    variant::operable_multiarray tan(const variant::operable_multiarray &op);

    variant::operable_multiarray asin(const variant::operable_multiarray &op);
    variant::operable_multiarray acos(const variant::operable_multiarray &op);
    variant::operable_multiarray atan(const variant::operable_multiarray &op);
}

#endif // ! BUILTIN_HH