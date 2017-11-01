#include "solve.hpp"

#include <boost/range/adaptor/transformed.hpp>

#include <algorithm>
#include <functional>
#include <numeric>

namespace {

Number choiceValue(const std::vector<std::size_t>& choice,
		const std::vector<std::vector<Number>>& offers) {
	if (choice.empty()) {
		return Number{};
	}
	std::vector<Number> odds;
	odds.reserve(choice.size());
	for (std::size_t racerIndex = 0; racerIndex < choice.size(); ++racerIndex) {
		odds.push_back(offers[racerIndex][choice[racerIndex]]);
	}

	auto wagers = odds | boost::adaptors::transformed(
			std::mem_fn(&Number::reciprocal));

	auto totalWagered = std::accumulate(wagers.begin(), wagers.end(), Number{});
	return totalWagered.reciprocal();
}

std::vector<std::size_t> choose(
		const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits) {

	std::vector<std::size_t> result;

	if (static_cast<std::size_t>(
					std::count(bookieLimits.begin(), bookieLimits.end(), 0)) ==
			bookieLimits.size()) {
		// If no bets can be placed, we can't bet
		return result;
	}

	if (offers.empty()) {
		return result;
	}

	auto lastRacerOffers = offers.back();

	std::vector<std::size_t> availableOffersForLast;
	for (std::size_t bookieIndex = 0; bookieIndex < lastRacerOffers.size();
			++bookieIndex) {
		if (bookieLimits[bookieIndex] > 0 && lastRacerOffers[bookieIndex])
		{
			availableOffersForLast.push_back(bookieIndex);
		}
	}

	std::vector<std::vector<std::size_t>> totalOffers;
	for (const auto& offerIndex : availableOffersForLast) {
		auto remainingOffers = offers;
		remainingOffers.pop_back();
		auto remainingBookieLimits = bookieLimits;
		--remainingBookieLimits[offerIndex];
		auto choicesForOthers = choose(remainingOffers, remainingBookieLimits);
		if (choicesForOthers.empty() && !remainingOffers.empty()) {
			continue; // This choice won't work
		}
		choicesForOthers.push_back(offerIndex);
		totalOffers.push_back(std::move(choicesForOthers));
	}

	if (totalOffers.empty()) {
		// No possible choices here
		return result;
	}

	if (totalOffers.size() == 1) {
		return totalOffers.front();
	}

	auto bestChoice = *std::max_element(totalOffers.begin(), totalOffers.end(),
			[&offers](const std::vector<std::size_t>& l,
					const std::vector<std::size_t>& r) {
				BOOST_ASSERT(!l.empty());
				BOOST_ASSERT(!r.empty());
				BOOST_ASSERT(l.size() == offers.size());
				BOOST_ASSERT(r.size() == offers.size());
				std::vector<Number> lOdds;
				lOdds.reserve(l.size());
				for (std::size_t racerIndex = 0; racerIndex < l.size();
						++racerIndex) {
					lOdds.push_back(offers[racerIndex][l[racerIndex]]);
				}
				std::vector<Number> rOdds;
				rOdds.reserve(r.size());
				for (std::size_t racerIndex = 0; racerIndex < r.size();
						++racerIndex) {
					rOdds.push_back(offers[racerIndex][r[racerIndex]]);
				}

				return choiceValue(l, offers) < choiceValue(r, offers);
			});

	result = std::move(bestChoice);

	return result;

}

} // unnamed namespace

Number solve(const std::vector<std::vector<Number>>& offers,
		const std::vector<std::size_t>& bookieLimits) {
	auto choice = choose(offers, bookieLimits);

	return choiceValue(choice, offers);
}
