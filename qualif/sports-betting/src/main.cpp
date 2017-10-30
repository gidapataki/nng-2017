#include "Number.hpp"

#include <iostream>

#include <boost/lexical_cast.hpp>

int main() {
	auto a = boost::lexical_cast<Number>("1/2");
	auto b = boost::lexical_cast<Number>("3/4");
	std::cerr << a+b << " " << a-b << std::endl;
}
