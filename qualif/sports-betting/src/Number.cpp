#include "Number.hpp"

#include <iostream>

#include <boost/assert.hpp>

namespace {


unsigned gcd(unsigned m, unsigned n) {
	return n == 0 ? m : gcd(n, m % n);
}

} // unnamed namespace

Number::Number(int i) : p{i}, q{1} {
}

Number::Number(int p, int q) {
	BOOST_ASSERT_MSG(q > 0, "denominator of Number cannot be negative");
	this->p = p;
	this->q = static_cast<unsigned>(q);
}

bool Number::operator<(const Number& rhs) const {
	return static_cast<double>(*this) < static_cast<double>(rhs);
}

bool Number::operator==(const Number& rhs) const {
	return p == rhs.p && q == rhs.q;
}

Number& Number::operator+=(const Number& rhs) {
	auto denominator = q*rhs.q;
	int numerator = rhs.q*p + rhs.p*q;
	int commonFactor = gcd(std::abs(numerator), denominator);
	p = numerator / commonFactor;
	q = static_cast<unsigned>(denominator) / commonFactor;
	return *this;
}

Number& Number::operator-=(const Number& rhs) {
	auto denominator = q*rhs.q;
	int numerator = rhs.q*p - q*rhs.p;
	int commonFactor = gcd(std::abs(numerator), denominator);
	p = numerator / commonFactor;
	q = static_cast<unsigned>(denominator) / commonFactor;
	return *this;
}

Number::operator double() const {
	return static_cast<double>(p) / static_cast<double>(q);
}

Number::operator bool() const {
	return p != 0;
}

Number Number::reciprocal() const {
	BOOST_ASSERT_MSG(p > 0, "Cannot invert 0");
	return Number{static_cast<int>(q), p};
}

std::ostream& operator<<(std::ostream& os, const Number& n) {
	if (n.q == 1) {
		return os << n.p;
	}
	return os << n.p << '/' << n.q;
}

std::istream& operator>>(std::istream& is, Number& n) {
	is >> n.p;
	if (is.good() && is.peek() == '/') {
		is.get();
		is >> n.q;
		auto d = gcd(n.p, n.q);
		n.p /= d;
		n.q /= d;
	} else {
		n.q = 1;
	}
	return is;
}
