#include "variant.hh"

namespace variant
{
	std::string stringify_operable_variant(operable_variant_t v)
	{
		return std::visit([](auto arg) -> std::string
						  {
							  std::stringstream ss;
							  ss << arg;
							  return ss.str();
						  },
						  v);
	}

	operable operator+(operable lhs, const operable &rhs)
	{
		lhs = std::visit(overloaded{

							 [&](std::string lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot add string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("use * operator to add string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
												   },
												   rhs.value);
							 },
							 [&](auto lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   return lhs_arg + rhs_arg;
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot add string '" + rhs_arg + "' with: " + stringify_operable_variant(lhs_arg));
													   },
												   },
												   rhs.value);
							 }},
						 lhs.value);
		return lhs;
	}

	operable operator-(operable lhs, const operable &rhs)
	{
		lhs = std::visit(overloaded{

							 [&](std::string lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot substract string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot substract string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
												   },
												   rhs.value);
							 },
							 [&](auto lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   return lhs_arg - rhs_arg;
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot substract string '" + rhs_arg + "' with: " + stringify_operable_variant(lhs_arg));
													   },
												   },
												   rhs.value);
							 }},
						 lhs.value);
		return lhs;
	}

	operable operator*(operable lhs, const operable &rhs)
	{
		lhs = std::visit(overloaded{

							 [&](std::string lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot multiply string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
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
														   throw std::runtime_error("cannot multiply string '" + rhs_arg + "' with: " + stringify_operable_variant(lhs_arg));
													   },
												   },
												   rhs.value);
							 }},
						 lhs.value);
		return lhs;
	}

	operable operator/(operable lhs, const operable &rhs)
	{
		lhs = std::visit(overloaded{

							 [&](std::string lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot divide string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot divide string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
												   },
												   rhs.value);
							 },
							 [&](auto lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   return lhs_arg / rhs_arg;
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot divide string '" + rhs_arg + "' with: " + stringify_operable_variant(lhs_arg));
													   },
												   },
												   rhs.value);
							 }},
						 lhs.value);
		return lhs;
	}

	operable operator%(operable lhs, const operable &rhs)
	{
		lhs = std::visit(overloaded{

							 [&](int lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot modulus integer '" + stringify_operable_variant(lhs_arg) + "' with non-integer: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](int rhs_arg) -> operable_variant_t
													   {
														   return lhs_arg % rhs_arg;
													   },
												   },
												   rhs.value);
							 },
							 [&](auto lhs_arg) -> operable_variant_t
							 {
								 return std::visit(overloaded{
													   [&](auto rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot modulus non-integers '" + stringify_operable_variant(rhs_arg) + "' with: " + stringify_operable_variant(lhs_arg));
													   },
													   [&](int rhs_arg) -> operable_variant_t
													   {
														   throw std::runtime_error("cannot modulus non-integer '" + stringify_operable_variant(rhs_arg) + "' with integer: " + stringify_operable_variant(lhs_arg));
													   },
												   },
												   rhs.value);
							 }},
						 lhs.value);
		return lhs;
	}
}