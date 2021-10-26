#include "builtin.hh"

namespace builtin
{
    variant::operable_multiarray div(const variant::operable_multiarray &lhs, const variant::operable_multiarray &rhs)
    {
        return lhs;
    }

    variant::operable_multiarray size(const variant::operable_multiarray &op)
    {
        variant::operable_multiarray new_op;
        new_op.values = op.values;

        for (auto &&new_row : new_op.values)
            for (auto &&new_col : new_row)
                new_col = std::holds_alternative<int>(new_col.value) ? variant::operable(0) : variant::operable(0.f);

        return new_op;
    }

    int length(const variant::operable_multiarray &op)
    {
        const variant::operable *scalar_equivalent = op.scalar_equivalent();
        if (scalar_equivalent)
            return std::visit(overloaded{[&](std::string arg) -> int
                                         {
                                             return arg.size();
                                         },
                                         [&](integral_or_floating_point auto arg) -> int
                                         {
                                             return 1;
                                         },
                                         [&](auto arg) -> int
                                         {
                                             throw std::runtime_error("invalid type for length() builtin call");
                                         }},
                              scalar_equivalent->value);

        int sum = 0;
        for (auto &&i : op.values)
            sum += i.size();

        return sum;
    }

    variant::operable_multiarray transpose(const variant::operable_multiarray &op)
    {
        return op;
    }
}
