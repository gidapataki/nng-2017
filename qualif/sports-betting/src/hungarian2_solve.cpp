#include "solve.hpp"
#include "Hungarian2.hpp"

#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>

#include <boost/assert.hpp>

namespace {

std::vector<std::vector<Number>> expandMatrix(
		const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t> bookieLimits) {
	std::vector<std::vector<Number>> result;
	result.reserve(offers.size());

	for (const auto& racerOffers: offers) {
		auto begin = boost::make_zip_iterator(
				boost::make_tuple(racerOffers.begin(), bookieLimits.begin()));
		auto end = boost::make_zip_iterator(
				boost::make_tuple(racerOffers.end(), bookieLimits.end()));
		auto range = boost::make_iterator_range(begin, end);

		std::vector<Number> expandedRacerOffers;
		for (const auto& pair: range) {
			Number odds = pair.get<0>();
			std::size_t multiplicity = pair.get<1>();
			for (std::size_t i=0; i<multiplicity; ++i) {
				expandedRacerOffers.push_back(odds);
			}
		}
		result.push_back(std::move(expandedRacerOffers));
	}

	return result;
}

std::size_t totalBetsAvailable(const std::vector<std::size_t>& bookieLimits) {
	return std::accumulate(bookieLimits.begin(), bookieLimits.end(), 0);
}

} // unnamed namespace

Number solve(const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits) {
	if (totalBetsAvailable(bookieLimits) < offers.size()) {
		// Cannot assign, not enough columns
		return Number{0};
	}

	auto expandedMatrix = expandMatrix(offers, bookieLimits);
	BOOST_ASSERT_MSG(expandedMatrix.size() <= expandedMatrix.front().size(),
			"More racers than available bets");
	while (expandedMatrix.size() < expandedMatrix.front().size()) {
		expandedMatrix.push_back(std::vector<Number>(expandedMatrix.front().size(), Number{}));
	}


	std::cerr <<"foo" << std::endl;
	Hungarian2<Number> problem{std::move(expandedMatrix)};
	auto costs = problem.getCosts();
	for (const auto& cost: costs) {
		std::cerr << cost << std::endl;
	}

	return Number{0};
}
