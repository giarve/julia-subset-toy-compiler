#ifndef VARIANT_HH
#define VARIANT_HH

#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <optional>
#include <concepts>

template <class... Ts>
struct overloaded : Ts...
{
	using Ts::operator()...;
};

template <typename T>
concept integral_or_floating_point = std::integral<T> || std::floating_point<T>;

namespace variant
{
	struct SemanticException : public std::exception
	{
		std::string message;

		template <typename T>
		SemanticException(T what_arg)
		{
			message = std::string("semantic error: ") + what_arg;
		}

		const char *what() const throw()
		{
			return message.c_str();
		}
	};

	enum JuliaType
	{
		Int32,
		Float64,
		String,
		Bool
	};

	using operable_variant_t = std::variant<int, double, std::string, bool>;

	std::string stringify_operable_variant(operable_variant_t v);

	class operable
	{
	public:
		operable_variant_t value;

		operable() {} // Used by bison for %nterm
		operable(operable_variant_t val)
		{
			value = val;
		}

		friend std::ostream &operator<<(std::ostream &stream, const operable &op)
		{
			stream << stringify_operable_variant(op.value);
			return stream;
		}

		friend operable operator+(operable lhs, const operable &rhs);
		friend operable operator-(operable lhs, const operable &rhs);
		friend operable operator*(operable lhs, const operable &rhs);
		friend operable operator/(operable lhs, const operable &rhs);
		friend operable operator%(operable lhs, const operable &rhs);
		friend operable operator^(operable lhs, const operable &rhs);

		operable operator+();
		operable operator-();
		operable operator!();

		friend bool operator==(const operable &lhs, const operable &rhs) { return lhs.value == rhs.value; }
		friend bool operator!=(const operable &lhs, const operable &rhs) { return lhs.value != rhs.value; }
		friend bool operator<(const operable &lhs, const operable &rhs) { return lhs.value < rhs.value; }
		friend bool operator>(const operable &lhs, const operable &rhs) { return lhs.value > rhs.value; }
		friend bool operator<=(const operable &lhs, const operable &rhs) { return lhs.value <= rhs.value; }
		friend bool operator>=(const operable &lhs, const operable &rhs) { return lhs.value >= rhs.value; }

		/* Overloading like this breaks short-circuit evaluation */
		friend bool operator&&(const operable &lhs, const operable &rhs);
		friend bool operator||(const operable &lhs, const operable &rhs);
	};

	class operable_multiarray
	{
	public:
		std::optional<std::string> identifier;

		bool is_tuple = false; // is it read only?

		// TODO: it is easier to use a single vector and implement matrix access properly
		std::vector<std::vector<operable>> values = {std::vector<operable>{}};

		operable_multiarray() {} // Used by bison for nterm
		operable_multiarray(operable scalar)
		{
			insert_row_element(scalar);
		}

		// Returns nullptr if not equivalent
		operable const *scalar_equivalent() const
		{
			if (values.size() == 1 && values[0].size() == 1)
				return &values[0][0];

			return nullptr;
		}

		operable_multiarray &operator=(operable_variant_t &&rhs)
		{
			values[0].emplace_back(rhs);
			return *this;
		}

		operable_multiarray &operator=(operable &&rhs)
		{
			values[0].emplace_back(rhs);
			return *this;
		}

		void insert_row_element(operable op)
		{
			values[values.size() - 1].emplace_back(op);
		}

		void insert_row_element_start(operable op)
		{
			auto&& current_row = values[values.size() - 1];
			current_row.emplace(current_row.begin(), op);
		}

		void add_new_row()
		{
			values.emplace_back();
		}

		static bool dimension_mismatch(const operable_multiarray &lhs, const operable_multiarray &rhs)
		{
			if (lhs.values.size() != rhs.values.size())
				return true;

			for (size_t i = 0; i < lhs.values.size(); i++)
			{
				if (lhs.values[i].size() != rhs.values[i].size())
					return true;
			}

			return false;
		}

		static bool mult_compatible(const operable_multiarray &lhs, const operable_multiarray &rhs)
		{
			size_t n_cols_lhs = lhs.values[0].size();
			size_t n_rows_rhs = rhs.values.size();

			return n_cols_lhs == n_rows_rhs;
		}

		friend std::ostream &operator<<(std::ostream &stream, const operable_multiarray &opm);

		friend operable_multiarray operator+(operable_multiarray lhs, const operable_multiarray &rhs);
		friend operable_multiarray operator-(operable_multiarray lhs, const operable_multiarray &rhs);
		friend operable_multiarray operator*(operable_multiarray lhs, const operable_multiarray &rhs);
		friend operable_multiarray operator/(operable_multiarray lhs, const operable_multiarray &rhs);
		friend operable_multiarray operator%(operable_multiarray lhs, const operable_multiarray &rhs);
		friend operable_multiarray operator^(operable_multiarray lhs, const operable_multiarray &rhs);
		operable_multiarray operator+();
		operable_multiarray operator-();
		operable_multiarray operator!();
		friend bool operator==(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator!=(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator<(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator>(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator<=(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator>=(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator&&(const operable_multiarray &lhs, const operable_multiarray &rhs);
		friend bool operator||(const operable_multiarray &lhs, const operable_multiarray &rhs);
	};
}

#endif // ! VARIANT_HH
