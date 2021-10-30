#include <cmath>
#include "variant.hh"

namespace variant
{
	/* =================== OPERABLE =================== */

	std::string stringify_operable_variant(operable_variant_t v)
	{
		return std::visit([](auto arg) -> std::string
						  {
							  std::stringstream ss;
							  ss << std::boolalpha << std::showpoint << arg;
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
														   throw SemanticException("cannot add string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw SemanticException("use * operator to add string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
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
														   throw SemanticException("cannot add string '" + stringify_operable_variant(lhs_arg) + "' with: " + rhs_arg);
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
														   throw SemanticException("cannot substract string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw SemanticException("cannot substract string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
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
														   throw SemanticException("cannot substract string '" + stringify_operable_variant(lhs_arg) + "' with: " + rhs_arg);
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
														   throw SemanticException("cannot multiply string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
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
														   throw SemanticException("cannot multiply string '" + stringify_operable_variant(lhs_arg) + "' with: " + rhs_arg);
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
														   throw SemanticException("cannot divide string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](std::string rhs_arg) -> operable_variant_t
													   {
														   throw SemanticException("cannot divide string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
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
														   throw SemanticException("cannot divide " + stringify_operable_variant(lhs_arg) + " with string: " + rhs_arg);
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
														   throw SemanticException("cannot modulus integer " + stringify_operable_variant(lhs_arg) + " with non-integer: " + stringify_operable_variant(rhs_arg));
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
														   throw SemanticException("cannot modulus non-integers " + stringify_operable_variant(lhs_arg) + " with: " + stringify_operable_variant(rhs_arg));
													   },
													   [&](int rhs_arg) -> operable_variant_t
													   {
														   throw SemanticException("cannot modulus non-integer " + stringify_operable_variant(lhs_arg) + " with integer: " + stringify_operable_variant(rhs_arg));
													   },
												   },
												   rhs.value);
							 }},
						 lhs.value);
		return lhs;
	}

	operable operator^(operable lhs, const operable &rhs)
	{
		lhs = std::visit(overloaded{[&](integral_or_floating_point auto lhs_arg) -> operable_variant_t
									{
										return std::visit(overloaded{
															  [&](auto rhs_arg) -> operable_variant_t
															  {
																  throw SemanticException("cannot exponentiate integer " + stringify_operable_variant(lhs_arg) + " with: " + stringify_operable_variant(rhs_arg));
															  },
															  [&](integral_or_floating_point auto rhs_arg) -> operable_variant_t
															  {
																  return (double)std::pow(lhs_arg, rhs_arg);
															  },
														  },
														  rhs.value);
									},
									[&](std::string lhs_arg) -> operable_variant_t
									{
										return std::visit(overloaded{[&](auto rhs_arg) -> operable_variant_t
																	 {
																		 throw SemanticException("cannot exponentiate string '" + lhs_arg + "' with: " + stringify_operable_variant(rhs_arg));
																	 },
																	 [&](int rhs_arg) -> operable_variant_t
																	 {
																		 std::ostringstream os;
																		 for (int i = 0; i < rhs_arg; i++)
																			 os << lhs_arg;
																		 return os.str();
																	 },
																	 [&](std::floating_point auto rhs_arg) -> operable_variant_t
																	 {
																		 throw SemanticException("cannot exponentiate string '" + lhs_arg + "' with floating point: " + stringify_operable_variant(rhs_arg));
																	 }

														  },
														  rhs.value);
									},
									[&](auto lhs_arg) -> operable_variant_t
									{
										return std::visit(overloaded{[&](auto rhs_arg) -> operable_variant_t
																	 {
																		 throw SemanticException("cannot exponentiate " + stringify_operable_variant(lhs_arg) + " with: " + stringify_operable_variant(rhs_arg));
																	 }},
														  rhs.value);
									}},
						 lhs.value);
		return lhs;
	}

	operable operable::operator+()
	{
		return std::visit(overloaded{[&](integral_or_floating_point auto val) -> operable
									 {
										 return *this; // + unary operator does nothing to arithmetic
									 },
									 [&](auto val) -> operable
									 {
										 throw SemanticException("invalid '+' unary expression for: " + stringify_operable_variant(val));
									 }},
						  this->value);
	}

	operable operable::operator-()
	{
		return std::visit(overloaded{[&](integral_or_floating_point auto val) -> operable
									 {
										 operable op(-val);
										 return op;
									 },
									 [&](auto val) -> operable
									 {
										 throw SemanticException("invalid '-' unary expression for: " + stringify_operable_variant(val));
									 }},
						  this->value);
	}

	operable operable::operator!()
	{
		operable op(std::visit(overloaded{[&](bool val) -> bool
										  {
											  return !val;
										  },
										  [&](auto val) -> bool
										  {
											  throw SemanticException("invalid '!' unary expression for: " + stringify_operable_variant(val));
										  }},
							   this->value));
		return op;
	}

	bool operator&&(const operable &lhs, const operable &rhs)
	{
		return std::visit(overloaded{[&](bool lhs_arg) -> bool
									 {
										 return std::visit(overloaded{
															   [&](auto rhs_arg) -> bool
															   {
																   throw SemanticException("invalid boolean operation: " + stringify_operable_variant(lhs_arg) + " && " + stringify_operable_variant(rhs_arg));
															   },
															   [&](bool rhs_arg) -> bool
															   {
																   return lhs_arg && rhs_arg;
															   },
														   },
														   rhs.value);
									 },
									 [&](auto lhs_arg) -> bool
									 {
										 return std::visit(overloaded{[&](auto rhs_arg) -> bool
																	  {
																		  throw SemanticException("invalid boolean operation: " + stringify_operable_variant(lhs_arg) + " && " + stringify_operable_variant(rhs_arg));
																	  }},
														   rhs.value);
									 }},
						  lhs.value);
	}

	bool operator||(const operable &lhs, const operable &rhs)
	{
		return std::visit(overloaded{[&](bool lhs_arg) -> bool
									 {
										 return std::visit(overloaded{
															   [&](auto rhs_arg) -> bool
															   {
																   throw SemanticException("invalid boolean operation: " + stringify_operable_variant(lhs_arg) + " || " + stringify_operable_variant(rhs_arg));
															   },
															   [&](bool rhs_arg) -> bool
															   {
																   return lhs_arg || rhs_arg;
															   },
														   },
														   rhs.value);
									 },
									 [&](auto lhs_arg) -> bool
									 {
										 return std::visit(overloaded{[&](auto rhs_arg) -> bool
																	  {
																		  throw SemanticException("invalid boolean operation: " + stringify_operable_variant(lhs_arg) + " || " + stringify_operable_variant(rhs_arg));
																	  }},
														   rhs.value);
									 }},
						  lhs.value);
	}

	/* =================== OPERABLE_MULTIARRAY =================== */

	std::ostream &operator<<(std::ostream &stream, const operable_multiarray &opm)
	{
		const operable *scalar_equivalent = opm.scalar_equivalent();
		if (scalar_equivalent)
		{
			stream << *scalar_equivalent;
			return stream;
		}

		stream << '[';
		for (size_t i = 0; i < opm.values.size(); i++)
		{
			for (size_t j = 0; j < opm.values[i].size(); j++)
			{
				stream << opm.values[i][j];

				if (j < opm.values[i].size() - 1)
					stream << ' ';
			}

			if (i < opm.values.size() - 1)
				stream << ';' << ' ';
		}
		stream << ']';

		return stream;
	}

	operable_multiarray operator+(operable_multiarray lhs, const operable_multiarray &rhs)
	{
		if (operable_multiarray::dimension_mismatch(lhs, rhs))
		{
			std::ostringstream ss;
			ss << "cannot operate due to dimension mismatch: ";
			ss << lhs;
			ss << " + ";
			ss << rhs;
			throw SemanticException(ss.str());
		}

		for (size_t i = 0; i < lhs.values.size(); i++)
		{
			for (size_t j = 0; j < lhs.values[i].size(); j++)
			{
				lhs.values[i][j] = lhs.values[i][j] + rhs.values[i][j];
			}
		}
		return lhs;
	}
	operable_multiarray operator-(operable_multiarray lhs, const operable_multiarray &rhs)
	{
		if (operable_multiarray::dimension_mismatch(lhs, rhs))
		{
			std::ostringstream ss;
			ss << "cannot operate due to dimension mismatch: ";
			ss << lhs;
			ss << " - ";
			ss << rhs;
			throw SemanticException(ss.str());
		}

		for (size_t i = 0; i < lhs.values.size(); i++)
		{
			for (size_t j = 0; j < lhs.values[i].size(); j++)
			{
				lhs.values[i][j] = lhs.values[i][j] - rhs.values[i][j];
			}
		}
		return lhs;
	}
	operable_multiarray operator*(operable_multiarray lhs, const operable_multiarray &rhs)
	{
		const operable *scalar_equivalent_lhs = lhs.scalar_equivalent();
		const operable *scalar_equivalent_rhs = rhs.scalar_equivalent();

		if (scalar_equivalent_lhs && scalar_equivalent_rhs)
		{
			return *scalar_equivalent_lhs * *scalar_equivalent_rhs;
		}
		else if (scalar_equivalent_lhs && !scalar_equivalent_rhs)
		{
			operable tmp = *scalar_equivalent_lhs;
			lhs.values = rhs.values;
			for (size_t i = 0; i < lhs.values.size(); i++)
			{
				for (size_t j = 0; j < lhs.values[i].size(); j++)
				{
					lhs.values[i][j] = tmp * rhs.values[i][j];
				}
			}
			return lhs;
		}
		else if (!scalar_equivalent_lhs && scalar_equivalent_rhs)
		{
			for (size_t i = 0; i < lhs.values.size(); i++)
			{
				for (size_t j = 0; j < lhs.values[i].size(); j++)
				{
					lhs.values[i][j] = lhs.values[i][j] * *scalar_equivalent_rhs;
				}
			}
			return lhs;
		}
		else if (operable_multiarray::mult_compatible(lhs, rhs))
		{
			size_t n_rows_lhs = lhs.values.size();
			size_t n_rows_rhs = rhs.values.size();
			size_t n_cols_rhs = rhs.values[0].size();

			operable_multiarray result;
			result.values.resize(n_rows_lhs);
			for (auto &&col : result.values)
				col.resize(n_cols_rhs);

			for (size_t i = 0; i < n_rows_lhs; ++i)
				for (size_t j = 0; j < n_cols_rhs; ++j)
					for (size_t k = 0; k < n_rows_rhs; ++k)
						result.values[i][j] = result.values[i][j] + lhs.values[i][k] * rhs.values[k][j];

			return result;
		}
		else
		{
			std::ostringstream ss;
			ss << "cannot operate: ";
			ss << lhs;
			ss << " * ";
			ss << rhs;
			throw SemanticException(ss.str());
		}
	}

	operable_multiarray operator/(operable_multiarray lhs, const operable_multiarray &rhs)
	{
		const operable *scalar_equivalent_lhs = lhs.scalar_equivalent();
		const operable *scalar_equivalent_rhs = rhs.scalar_equivalent();

		if (scalar_equivalent_lhs && scalar_equivalent_rhs)
			return *scalar_equivalent_lhs / *scalar_equivalent_rhs;
		else
		{
			std::ostringstream ss;
			ss << "cannot operate: ";
			ss << lhs;
			ss << " / ";
			ss << rhs;
			throw SemanticException(ss.str());
		}
	}
	operable_multiarray operator%(operable_multiarray lhs, const operable_multiarray &rhs)
	{
		const operable *scalar_equivalent_lhs = lhs.scalar_equivalent();
		const operable *scalar_equivalent_rhs = rhs.scalar_equivalent();

		if (scalar_equivalent_lhs && scalar_equivalent_rhs)
			return *scalar_equivalent_lhs % *scalar_equivalent_rhs;
		else
		{
			std::ostringstream ss;
			ss << "cannot operate: ";
			ss << lhs;
			ss << " % ";
			ss << rhs;
			throw SemanticException(ss.str());
		}
	}
	operable_multiarray operator^(operable_multiarray lhs, const operable_multiarray &rhs)
	{
		const operable *scalar_equivalent_lhs = lhs.scalar_equivalent();
		const operable *scalar_equivalent_rhs = rhs.scalar_equivalent();

		if (scalar_equivalent_lhs && scalar_equivalent_rhs)
			return *scalar_equivalent_lhs ^ *scalar_equivalent_rhs;
		else
		{
			std::ostringstream ss;
			ss << "cannot operate: ";
			ss << lhs;
			ss << " ^ ";
			ss << rhs;
			throw SemanticException(ss.str());
		}
	}

	operable_multiarray operable_multiarray::operator+()
	{
		// Broadcast
		for (auto &&row : values)
		{
			for (auto &&col : row)
			{
				col = +col;
			}
		}
		return *this;
	}

	operable_multiarray operable_multiarray::operator-()
	{
		// Broadcast
		for (auto &&row : values)
		{
			for (auto &&col : row)
			{
				col = -col;
			}
		}

		return *this;
	}

	operable_multiarray operable_multiarray::operator!()
	{
		// Broadcast
		for (auto &&row : values)
		{
			for (auto &&col : row)
			{
				col = !col;
			}
		}

		return *this;
	}

	bool operator==(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		return lhs.values == rhs.values;
	}
	bool operator!=(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		return lhs.values != rhs.values;
	}
	bool operator<(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		return lhs.values < rhs.values;
	}
	bool operator>(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		return lhs.values > rhs.values;
	}
	bool operator<=(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		return lhs.values <= rhs.values;
	}
	bool operator>=(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		return lhs.values >= rhs.values;
	}
	bool operator&&(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		const operable *scalar_equivalent_lhs = lhs.scalar_equivalent();
		const operable *scalar_equivalent_rhs = rhs.scalar_equivalent();

		if (scalar_equivalent_lhs && scalar_equivalent_rhs)
			return *scalar_equivalent_lhs && *scalar_equivalent_rhs;
		else
		{
			std::ostringstream ss;
			ss << "cannot operate: ";
			ss << lhs;
			ss << " && ";
			ss << rhs;
			throw SemanticException(ss.str());
		}
	}
	bool operator||(const operable_multiarray &lhs, const operable_multiarray &rhs)
	{
		const operable *scalar_equivalent_lhs = lhs.scalar_equivalent();
		const operable *scalar_equivalent_rhs = rhs.scalar_equivalent();

		if (scalar_equivalent_lhs && scalar_equivalent_rhs)
			return *scalar_equivalent_lhs || *scalar_equivalent_rhs;
		else
		{
			std::ostringstream ss;
			ss << "cannot operate: ";
			ss << lhs;
			ss << " || ";
			ss << rhs;
			throw SemanticException(ss.str());
		}
	}

}