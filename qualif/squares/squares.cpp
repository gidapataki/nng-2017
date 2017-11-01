#include <cstdint>
#include <iostream>
#include <vector>
#include <cassert>


using Numbers = std::vector<unsigned>;

struct Digits {
	uint8_t v[10] = {};
};

Digits& operator+=(Digits& lhs, const Digits& rhs) {
	for (int i = 0; i < 10; ++i) {
		lhs.v[i] += rhs.v[i];
	}
	return lhs;
}

std::ostream& operator<<(std::ostream& stream, const Digits& ds) {
	stream << "<Digits";
	for (int i = 0; i < 10; ++i) {
		stream << " " << int(ds.v[i]);
	}
	stream << ">";
	return stream;
}

constexpr unsigned pow10_values[9] = {
	1,           // 10^0
	10,          // 10^1
	100,         // 10^2
	1000,        // 10^3
	10000,       // 10^4
	100000,      // 10^5
	1000000,     // 10^6
	10000000,    // 10^7
	100000000,   // 10^8
};

constexpr unsigned pow10(int n) {
	return pow10_values[n];
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

bool accept(const Digits& provided, const Digits& required) {
	for (int i = 0; i < 10; ++i) {
		if (required.v[i] > provided.v[i]) {
			return false;
		}
	}
	return true;
}

void sieve(const Digits& ds, Numbers& vec, int width) {
	Numbers out[10];
	auto mul = pow10(width - 1);

	for (auto p : vec) {
		auto base_digits = digits(p, width - 1);

		auto p2 = uint64_t(p) * p;
		auto base_pow_digits = digits(p2, width - 1);
		auto base_msd = p2 / mul % 10;
		base_digits += base_pow_digits;

		for (int i = 0; i < 10; ++i) {
			auto px = p + i * mul;
			auto px_digits = base_digits;
			auto msd = (base_msd + 2 * i * p + (mul > 1 ? 0 : i*i)) % 10;
			++px_digits.v[i];
			++px_digits.v[msd];

			if (accept(ds, px_digits)) {
				out[i].push_back(px);
			}
		}
	}

	vec.clear();
	size_t s = 0;
	for (int i = 0; i < 10; ++i) {
		s += out[i].size();
	}
	vec.reserve(s);

	for (int i = 0; i < 10; ++i) {
		std::copy(out[i].begin(), out[i].end(), std::back_inserter(vec));
	}
}

bool accept_exact(const Digits& provided, const Digits& required) {
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
	auto mul = pow10(width - 1);
	for (int i = 10; i-- > 0;) {
		for (auto it = vec.rbegin(), ie = vec.rend(); it != ie; ++it) {
			auto p = *it;
			auto px = p + i * mul;
			auto px_digits = combined_digits(px);
			if (accept_exact(ds, px_digits)) {
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

void solve_example() {
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
}

void solve_input() {
	int t;
	std::cin >> t;
	for (int i = 0; i < t; ++i) {
		int n;
		std::cin >> n;
		std::vector<int> digits;
		for (int j = 0; j < n; ++j) {
			int d;
			std::cin >> d;
			digits.push_back(d);
		}
		solve(digits);
	}
}

int main() {
	// solve({
	// 	4, 5, 6, 9, 4, 0, 3, 8, 2, 2, 0, 8, 7, 9,
	// 	4, 5, 1, 2, 7, 0, 2, 3, 0, 5, 9, 2, 4});
	// solve_example();
	solve_input();
	return 0;
}
