#ifndef NUMBER_HPP_
#define NUMBER_HPP_

#include <iosfwd>

#include <boost/operators.hpp>

class Number : boost::totally_ordered<Number>, boost::additive<Number> {
public:
	Number() = default;
	Number(const Number&) = default;
	Number(Number&&) = default;
	Number& operator=(const Number&) = default;
	Number& operator=(Number&&) = default;
	~Number() = default;

	explicit Number(int i);
	explicit Number(int p, int q);

	bool operator<(const Number& rhs) const;
	bool operator==(const Number& rhs) const;

	Number& operator+=(const Number& rhs);
	Number& operator-=(const Number& rhs);

	explicit operator double() const;

	friend std::ostream& operator<<(std::ostream& os, const Number& n);
	friend std::istream& operator>>(std::istream& is, Number& n);

private:
	int p = 0;
	unsigned q = 1;
};

#endif // NUMBER_HPP_
