#ifndef VARIANT_HH
#define VARIANT_HH

#include <string>
#include <map>
#include <variant>
#include <optional>

namespace variant
{
	class operable
	{
	public:
		using operable_variant_t = std::variant<int, float, std::string>;
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

		friend std::ostream& operator<< (std::ostream& stream, const operable& op) {
			// TODO: visit op value and return a string
            return stream;
        }
	};

	class array
	{
	};

}

#endif // ! VARIANT_HH
