#ifndef SOLVE_HPP_
#define SOLVE_HPP_

#include "Number.hpp"

#include <vector>

Number solve(const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits);

#endif // SOLVE_HPP_
