#include "Number.hpp"
#include "solve.hpp"

#include <iostream>
#include <vector>

int main() {
	std::size_t numberOfTests = 0;
	std::cin >> numberOfTests;
	for (std::size_t test = 0; test < numberOfTests; ++test) {
		std::size_t numberOfRacers;
		std::size_t numberOfBookies;
		std::cin >> numberOfRacers >> numberOfBookies;
		std::vector<std::vector<Number>> racerOfferses;
		for (std::size_t racer = 0; racer < numberOfRacers; ++racer) {
			std::vector<Number> racerOffers;
			racerOffers.reserve(numberOfBookies);
			for (std::size_t bookie = 0; bookie < numberOfBookies; ++bookie) {
				Number offer;
				std::cin >> offer;
				racerOffers.push_back(offer);
			}
			racerOfferses.push_back(std::move(racerOffers));
		}
		std::vector<std::size_t> bookieLimits;
		bookieLimits.reserve(numberOfBookies);
		for (std::size_t bookie = 0; bookie < numberOfBookies; ++bookie) {
			std::size_t bookieLimit = 0;
			std::cin >> bookieLimit;
			bookieLimits.push_back(bookieLimit);
		}
		std::cout << solve(racerOfferses, bookieLimits) << std::endl;
	}
}
