#include "builtin.hh"

namespace builtin
{
    int div(const int lhs, const int rhs)
    {
        return lhs / rhs;
    }

    variant::operable_multiarray size(const variant::operable_multiarray &op)
    {
        variant::operable_multiarray new_op;
        new_op.values[0].resize(2);
        new_op.values[0][0] = variant::operable((int)op.values.size());
        new_op.values[0][1] = variant::operable((int)op.values[0].size());

        return new_op;
    }

    variant::operable_multiarray fill_with(variant::JuliaType withType, int with, int n_rows, int n_columns)
    {
        auto to_fill_with = [&]()
        {
            switch (withType)
            {
            case variant::JuliaType::Int64:
                return variant::operable(with);
                break;
            case variant::JuliaType::Float64:
                return variant::operable((float)with);
                break;
            case variant::JuliaType::String:
                return variant::operable("");
                break;
            case variant::JuliaType::Bool:
                return variant::operable((bool)with);
                break;
            }

            return variant::operable();
        };

        variant::operable_multiarray new_op;
        new_op.values.resize(n_rows);
        for (auto &&col : new_op.values)
            col.resize(n_columns, to_fill_with());

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
        variant::operable_multiarray new_op;

        size_t n_rows = op.values.size();
        size_t n_cols = op.values[0].size();

        new_op.values.resize(n_cols);
        for (auto &&cols : new_op.values)
            cols.resize(n_rows);

        for (size_t i = 0; i < n_rows; i++)
            for (size_t j = 0; j < n_cols; j++)
                new_op.values[j][i] = op.values[i][j];

        return new_op;
    }

}
