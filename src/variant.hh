#ifndef VARIANT_HH
#define VARIANT_HH

#include <string>
#include <sstream>
#include <map>
#include <variant>
#include <optional>

template <class... Ts>
struct overloaded : Ts...
{
	using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace variant
{

	using operable_variant_t = std::variant<int, float, std::string>;

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

		friend operable operator*(operable lhs,
								  const operable &rhs)
		{

			lhs = std::visit(overloaded{

								 [&](std::string lhs_arg) -> operable_variant_t
								 {
									 return std::visit(overloaded{
														   [&](auto rhs_arg) -> operable_variant_t
														   {
															   throw std::runtime_error("cannot multiply string " + lhs_arg + " with: " + stringify_operable_variant(rhs_arg));
														   },
														   [&](std::string rhs_arg) -> operable_variant_t
														   {
															   return lhs_arg + rhs_arg;
														   },
													   },
													   rhs.value);
								 },
								 [&](auto lhs_arg) -> operable_variant_t
								 {
									 return std::visit(overloaded{
														   [&](auto rhs_arg) -> operable_variant_t
														   {
															   return lhs_arg * rhs_arg;
														   },
														   [&](std::string rhs_arg) -> operable_variant_t
														   {
															   throw std::runtime_error("cannot multiply string " + rhs_arg + " with: " + stringify_operable_variant(lhs_arg));
														   },
													   },
													   rhs.value);
								 }},
							 lhs.value);
			return lhs;
		}

		friend std::ostream &operator<<(std::ostream &stream, const operable &op)
		{
			std::visit([&](const auto &p)
					   {
						   std::ostringstream v;
						   v << p;
						   std::string s_param(v.str());
						   stream << s_param;
					   },
					   op.value);
			return stream;
		}
	};

	class array
	{
	};

}

#endif // ! VARIANT_HH
