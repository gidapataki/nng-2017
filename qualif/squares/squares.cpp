#include <cstdint>
#include <iostream>
#include <vector>
#include <cassert>


using Numbers = std::vector<uint64_t>;

struct Digits {
	uint8_t v[10] = {};
};

Digits& operator+=(Digits& lhs, const Digits& rhs) {
	for (int i = 0; i < 10; ++i) {
		lhs.v[i] += rhs.v[i];
	}
	return lhs;
}

bool operator<=(const Digits& lhs, const Digits& rhs) {
	for (int i = 0; i < 10; ++i) {
		if (lhs.v[i] > rhs.v[i]) {
			return false;
		}
	}
	return true;
}

bool operator>=(const Digits& lhs, const Digits& rhs) {
	return rhs <= lhs;
}

bool operator<(const Digits& lhs, const Digits& rhs) {
	return !(rhs <= lhs);
}

bool operator>(const Digits& lhs, const Digits& rhs) {
	return !(lhs <= rhs);
}

std::ostream& operator<<(std::ostream& stream, const Digits& ds) {
	stream << "<Digits";
	for (int i = 0; i < 10; ++i) {
		stream << " " << int(ds.v[i]);
	}
	stream << ">";
	return stream;
}

constexpr uint64_t pow10(int k) {
	constexpr uint64_t value[10] = {
		1,           // 10^0
		10,          // 10^1
		100,         // 10^2
		1000,        // 10^3
		10000,       // 10^4
		100000,      // 10^5
		1000000,     // 10^6
		10000000,    // 10^7
		100000000,   // 10^8
		1000000000,  // 10^9
	};
	return value[k];
}

Digits digits(uint64_t n, int width) {
	Digits result;
	auto value = n;
	while (width-- > 0) {
		++result.v[value % 10];
		value /= 10;
	}
	return result;
}

Digits pow_digits(uint64_t n, int width) {
	Digits result;
	auto value = (n * n) % pow10(width);
	while (width-- > 0) {
		++result.v[value % 10];
		value /= 10;
	}
	return result;
}

Digits combined_digits(uint64_t n) {
	Digits result;
	auto value = n * n;
	while (value > 0) {
		++result.v[value % 10];
		value /= 10;
	}

	value = n;
	while (value > 0) {
		++result.v[value % 10];
		value /= 10;
	}

	return result;
}

void sieve(const Digits& ds, Numbers& vec, int width) {
	Numbers result;
	uint64_t mul = pow10(width - 1);

	// Note: nesting is important here, so output becomes sorted.
	for (int i = 0; i < 10; ++i) {
		for (auto p : vec) {
			auto px = p + i * mul;
			auto px_digits = digits(px, width);
			px_digits += pow_digits(px, width);
			if (px_digits <= ds) {
				result.push_back(px);
			}
		}
	}
	vec.swap(result);
}

bool accept(const Digits& provided, const Digits& required) {
	if (provided.v[0] < required.v[0]) {
		return false;
	}

	for (int i = 1; i < 10; ++i) {
		if (provided.v[i] != required.v[i]) {
			return false;
		}
	}

	return true;
}

uint64_t find_best(const Digits& ds, const Numbers& vec, int width) {
	uint64_t mul = pow10(width - 1);
	for (int i = 10; i-- > 0;) {
		for (auto it = vec.rbegin(), ie = vec.rend(); it != ie; ++it) {
			auto p = *it;
			auto px = p + i * mul;
			auto px_digits = combined_digits(px);
			if (accept(ds, px_digits)) {
				return px;
			}
		}
	}

	return 0;
}

void solve(const std::vector<int>& digits) {
	assert(digits.size() % 3 == 0);

	Digits ds;
	int k = digits.size() / 3;

	for (auto d : digits) {
		assert(d >= 0 && d < 10);
		++ds.v[d];
	}

	Numbers vec;
	vec.push_back(0);
	for (int i = 1; i < k; ++i) {
		sieve(ds, vec, i);
	}

	std::cout << find_best(ds, vec, k) << std::endl;
}



int main() {
    solve({0, 4, 2});
    solve({0, 0, 1, 0, 0, 1});
	solve({0, 6, 0, 9, 7, 8, 6, 3, 0, 6, 8, 7});
    solve({
        2, 4, 2, 4, 0, 3, 3, 2, 1, 0, 2, 3,
        1, 2, 4, 5, 4, 3, 1, 4, 4, 3, 5, 1});

	solve({
		7, 8, 9, 7, 7, 5, 3, 3, 3,
		6, 4, 4, 7, 3, 3, 5, 3, 9,
		6, 8, 9, 1, 3, 0, 7, 3, 4});

	return 0;
}
