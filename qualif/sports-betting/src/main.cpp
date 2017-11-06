#include "Number.hpp"
#include "solve.hpp"

#include <iostream>
#include <vector>

std::istream& parseNumber(std::istream& is, Number& n) {
	Number::int_type numerator;
	is >> numerator;
	if (is.good() && is.peek() == '/') {
		is.get();
		Number::int_type denominator;
		is >> denominator;
		n = Number{numerator, denominator};
	} else {
		n = Number{numerator};
	}
	return is;
}

std::ostream& printNumber(std::ostream& os, const Number& n) {
	if (n.denominator() == Number::int_type{1}) {
		return os << n.numerator();
	} else {
		return os << n.numerator() << '/' << n.denominator();
	}
}

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
				parseNumber(std::cin, offer);
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
		printNumber(std::cout, solve(racerOfferses, bookieLimits));
		std::cout << std::endl;
	}
}
