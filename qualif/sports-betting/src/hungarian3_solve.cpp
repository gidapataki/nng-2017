#include "solve.hpp"
#include "Hungarian.h"

#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <algorithm>
#include <numeric>

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

std::vector<std::vector<Number>> invert(
		const std::vector<std::vector<Number>>& m) {
	std::vector<std::vector<Number>> result;
	result.reserve(m.size());

	for (const auto& row: m) {
		std::vector<Number> invertedRow;
		invertedRow.reserve(row.size());
		for (const auto& n: row) {
			if (n) {
				invertedRow.push_back(1ll/n);
			} else {
				invertedRow.push_back(Number{100000});
			}
		}
		result.push_back(std::move(invertedRow));
	}

	return result;
}

} // unnamed namespace

Number solve(const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits) {

	if (totalBetsAvailable(bookieLimits) < offers.size()) {
		// Cannot assign, not enough columns
		return Number{0};
	}

	const auto expandedMatrix = expandMatrix(offers, bookieLimits);
	auto invertedMatrix = invert(expandedMatrix);
	HungarianAlgorithm solver;
	std::vector<int> assignment;
	auto naiveCost = solver.Solve(invertedMatrix, assignment);

	std::vector<Number> chosenOdds;
	chosenOdds.reserve(assignment.size());
	for (std::size_t i = 0; i < assignment.size(); ++i) {
		chosenOdds.push_back(expandedMatrix[i][assignment[i]]);
	}

	if (std::find(chosenOdds.begin(), chosenOdds.end(), Number{0})
			!= chosenOdds.end()) {
		return Number{0};
	}

	if (naiveCost) {
		return 1ll/naiveCost;
	} else {
		return Number{0};
	}
}
