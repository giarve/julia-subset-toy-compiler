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
}