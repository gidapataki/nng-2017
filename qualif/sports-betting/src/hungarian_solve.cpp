#include "solve.hpp"
#include "hungarian.hpp"

#include <numeric>
#include <boost/assert.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace {

std::vector<std::vector<Number>> expandBookies(
		const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits) {
	std::vector<std::vector<Number>> result;

	for (std::size_t racerIndex = 0; racerIndex < offers.size(); ++racerIndex) {
		std::vector<Number> racerOffers;
		for (std::size_t bookieIndex = 0; bookieIndex < bookieLimits.size();
				++bookieIndex) {
			auto offerForRacer = offers[racerIndex][bookieIndex];
			auto bookieMultiplicity = bookieLimits[bookieIndex];
			for (std::size_t i = 0; i < bookieMultiplicity; ++i) {
				racerOffers.push_back(offerForRacer);
			}
		}
		result.push_back(std::move(racerOffers));
	}

	return result;
}

void makeSquare(std::vector<std::vector<Number>>& offers) {
	auto rows = offers.size();
	auto cols = offers.front().size();

	if (cols > rows) {
		for (std::size_t i=0; i < (cols - rows); ++i) {
			offers.push_back(std::vector<Number>{cols, Number{}});
		}
		return;
	}
	if (rows > cols) {
		for (auto& row: offers) {
			for (std::size_t i=0; i < (rows - cols); ++i) {
				row.push_back(Number{});
			}
		}
		return;
	}
}

std::vector<std::vector<Number>> transpose(
		const std::vector<std::vector<Number>>& m) {
	std::vector<std::vector<Number>> result;

	result.resize(m.size(), std::vector<Number>{m.size(), Number{}});

	for (std::size_t i = 0; i < m.size(); ++i) {
		for (std::size_t j = 0; j < m.size(); ++j) {
			result[i][j] = m[j][i];
		}
	}

	return result;
}

std::vector<std::vector<int>> normalize(
		const std::vector<std::vector<Number>>& offers) {
	std::vector<std::vector<int>> result;
	for (const auto& racerOffers: offers) {
		std::vector<int> normalizedOffers;
		for (const auto& offer: racerOffers) {
			int lcm110 = 5*7*8*9; // lcm(1,...,10)
			auto normalizedOffer = offer * Number{lcm110};
			BOOST_ASSERT_MSG(normalizedOffer.getDenominator() == 1,
					"Could not get rid of denominator");
			normalizedOffers.push_back(normalizedOffer.getNumerator());
		}
		result.push_back(std::move(normalizedOffers));
	}
	return result;
}

} // unnamed namespace

Number solve(const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits) {
	auto expandedOffers = expandBookies(offers, bookieLimits);
	makeSquare(expandedOffers);
	std::cerr << "Offers matrix" << std::endl;
	// TODO: Check if we should _always_ transpose
	auto normalizedOffers = normalize(transpose(expandedOffers));
	for (const auto& row: normalizedOffers) {
		for (const auto& offer: row) {
			std::cerr << offer << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << std::endl;
	auto rows = normalizedOffers.size();
	auto columns = std::accumulate(bookieLimits.begin(), bookieLimits.end(), 0);
	Hungarian<int> problem{normalizedOffers,
			static_cast<int>(rows), static_cast<int>(columns),
			HUNGARIAN_MODE_MAXIMIZE_UTIL};
	problem.solve();
	::fprintf(stderr, "status\n");
	problem.print_status();

	auto assignment = problem.assignment();
	std::vector<Number> chosenOffers;
	for (std::size_t i=0; i<assignment.size(); ++i) {
		BOOST_ASSERT_MSG(assignment[i].size() == assignment.size(),
				"Assignment matrix not square");
		for (std::size_t j=0; j<assignment.size(); ++j) {
			if (assignment[i][j]) {
				auto offer = expandedOffers[i][j];
				chosenOffers.push_back(offer);
			}
		}
	}

	std::cerr << "Chosen offers: {" << std::endl;
	for (const auto& offer: chosenOffers) {
		std::cerr << offer << std::endl;
	}
	std::cerr << "}" << std::endl;

	auto zeroIt = std::find(chosenOffers.begin(), chosenOffers.end(), Number{});
	if (zeroIt != chosenOffers.end()) {
		// Check if it is far enough to the back to be considered a
		// square-expanson zero
		if (static_cast<std::size_t>(
				std::distance(chosenOffers.begin(), zeroIt)) < offers.size()) {
			return Number{};
		}
	}
	chosenOffers.erase(zeroIt, chosenOffers.end());

	auto wagers = chosenOffers | boost::adaptors::transformed(
			std::mem_fn(&Number::reciprocal));

	auto totalWagered = std::accumulate(wagers.begin(), wagers.end(), Number{});
	if (totalWagered == Number{}) {
		return Number{};
	}
	return totalWagered.reciprocal();
}
