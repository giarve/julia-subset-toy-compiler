#ifndef VARIANT_HH
#define VARIANT_HH

#include <string>
#include <sstream>
#include <map>
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

	using operable_variant_t = std::variant<int, float, std::string, bool>;

	std::string stringify_operable_variant(operable_variant_t v);
	class operable
	{
	public:
		operable_variant_t value;
		std::optional<std::string> identifier;

		operable()
		{
			// Empty constructor used by bison for nterm grammars
		}

		operable &operator=(operable_variant_t &&rhs)
		{
			value = rhs;
			return *this;
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

	class array
	{
	};

}

#endif // ! VARIANT_HH
