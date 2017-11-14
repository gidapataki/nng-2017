#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <cassert>
#include <algorithm>


struct Digits1 {
	uint8_t v[10] = {};
};

uint8_t get_digit(const Digits1& ds, int index) {
	return ds.v[index];
}

void inc_digit(Digits1& ds, int index) {
	++ds.v[index];
}

void dec_digit(Digits1& ds, int index) {
	--ds.v[index];
}

Digits1& operator+=(Digits1& lhs, const Digits1& rhs) {
	for (int i = 0; i < 10; ++i) {
		lhs.v[i] += rhs.v[i];
	}
	return lhs;
}

struct Digits2 {
	Digits2() {
		data[0] = 0;
		data[1] = 0;
	}

	union {
		uint64_t data[2];
		uint8_t digit[10];
	};
};

uint8_t get_digit(const Digits2& ds, int index) {
	return ds.digit[index];
	// return ds.data[index >> 3] >> ((index & 7) << 3);
}

void inc_digit(Digits2& ds, int index) {
	++ds.digit[index];
	// ds.data[index >> 3] += 1ull << ((index & 7) << 3);
}

void dec_digit(Digits2& ds, int index) {
	--ds.digit[index];
}

Digits2& operator+=(Digits2& lhs, const Digits2& rhs) {
	lhs.data[0] += rhs.data[0];
	lhs.data[1] += rhs.data[1];
	return lhs;
}

using Digits = Digits2;
using Numbers = std::vector<unsigned>;

struct Numbers10 {
	std::array<Numbers, 10> parts;
	std::array<std::vector<Digits>, 10> dparts;
};


std::ostream& operator<<(std::ostream& stream, const Digits& ds) {
	stream << "<Digits";
	for (int i = 0; i < 10; ++i) {
		stream << " " << int(get_digit(ds, i));
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
		inc_digit(result, value % 10);
		value /= 10;
	}
	return result;
}

Digits combined_digits(uint64_t n) {
	Digits result;
	auto value = n * n;
	while (value > 0) {
		inc_digit(result, value % 10);
		value /= 10;
	}

	value = n;
	while (value > 0) {
		inc_digit(result, value % 10);
		value /= 10;
	}

	return result;
}

bool accept(const Digits1& provided, const Digits1& required) {
	for (int i = 0; i < 10; ++i) {
		if (get_digit(required, i) > get_digit(provided, i)) {
			return false;
		}
	}
	return true;
}

bool accept(const Digits2& provided, const Digits2& required) {
	constexpr uint64_t mask = 0x8080808080808080ull;
	uint64_t x0 = provided.data[0] | mask;
	x0 -= required.data[0];
	if ((x0 & mask) != mask) {
		return false;
	}
	uint64_t x1 = provided.data[1] | mask;
	x1 -= required.data[1];
	if ((x1 & mask) != mask) {
		return false;
	}
	return true;
}

Digits2 substract(const Digits2& provided, const Digits2& required) {
	Digits2 result;
	result.data[0] = provided.data[0] - required.data[0];
	result.data[1] = provided.data[1] - required.data[1];
	return result;
}

void sieve(const Digits& ds, Numbers10& vec, int width) {
	Numbers10 out;
	auto mul = pow10(width - 1);

	for (auto& part : vec.parts) {
		for (auto p : part) {
			auto base_digits = digits(p, width - 1);

			auto p2 = uint64_t(p) * p;
			auto base_pow_digits = digits(p2, width - 1);
			auto base_msd = p2 / mul % 10;
			base_digits += base_pow_digits;

			for (int i = 0; i < 10; ++i) {
				auto px = p + i * mul;
				auto px_digits = base_digits;
				auto msd = (base_msd + 2 * i * p + (mul > 1 ? 0 : i*i)) % 10;
				inc_digit(px_digits, i);
				inc_digit(px_digits, msd);

				if (accept(ds, px_digits)) {
					out.parts[i].push_back(px);
					out.dparts[i].push_back(substract(ds, px_digits));
				}
			}
		}
	}

	vec.parts.swap(out.parts);
	vec.dparts.swap(out.dparts);
}

bool accept_exact(const Digits& provided, const Digits& required) {
	if (get_digit(provided, 0) < get_digit(required, 0)) {
		return false;
	}

	for (int i = 1; i < 10; ++i) {
		if (get_digit(provided, i) != get_digit(required, i)) {
			return false;
		}
	}

	return true;
}

uint64_t find_best(const Digits& ds, const Numbers10& vec, int sieved, int width) {
	auto mul = pow10(std::max(0, sieved - 1));
	for (int i = pow10(width - sieved + 1); i-- > 0;) {
		for (auto p_it = vec.parts.rbegin(); p_it != vec.parts.rend(); ++p_it) {
			for (auto it = p_it->rbegin(), ie = p_it->rend(); it != ie; ++it) {
				auto p = *it;
				auto px = p + i * mul;
				auto px_digits = combined_digits(px);
				if (accept_exact(ds, px_digits)) {
					return px;
				}
			}
		}
	}

	return 0;
}

int pow_prefix[100] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 0, 2, 2, 0, 3, 3,
	4, 4, 0, 5, 0, 6, 0, 7, 0, 8,
	9, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 0, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 0, 3, 3, 3, 3, 3,
	3, 3, 3, 0, 4, 4, 4, 4, 4, 4,
	0, 5, 5, 5, 5, 5, 5, 0, 6, 6,
	6, 6, 6, 0, 7, 7, 7, 7, 7, 0,
	8, 8, 8, 8, 0, 9, 9, 9, 9, 9
};

uint64_t find_best2(const Digits& ds, const Numbers10& vec, int sieved) {
	auto mul = pow10(std::max(0, sieved - 1));

	for (int d10 = 10; d10-- > 0;) {
		for (int d1 = 10; d1-- > 0;) {
			int x = d10 * 10 + d1;
			Digits req;
			auto pp = pow_prefix[x];
			inc_digit(req, d1);
			inc_digit(req, d10);
			if (pp) {
				inc_digit(req, pp);
			}

			for (int pt = 10; pt-- > 0;) {
				auto& part = vec.parts[pt];
				auto& dpart = vec.dparts[pt];
				for (int i = part.size(); i-- > 0;) {
					auto& dp = dpart[i];
					auto p = part[i];
					if (accept(dp, req)) {
						auto px = p + x * mul;
						auto px_digits = combined_digits(px);
						if (accept_exact(ds, px_digits)) {
							return px;
						}
					}
				}
			}
		}
	}

	return 0;
}

void solve(const std::vector<int>& digits, bool allow_best2=false) {
	assert(digits.size() % 3 == 0);

	Digits ds;

	for (auto d : digits) {
		assert(d >= 0 && d < 10);
		inc_digit(ds, d);
	}

	Numbers10 vec;
	int w = digits.size() / 3;
	int s = ((allow_best2 && w >= 6) ? w - 1 : w);

	vec.parts[0].push_back(0);
	vec.dparts[0].push_back(ds);
	for (int i = 1; i < s; ++i) {
		sieve(ds, vec, i);
	}

	if (s == w - 1) {
		std::cout << find_best2(ds, vec, s) << std::endl;
	} else {
		std::cout << find_best(ds, vec, s, w) << std::endl;
	}
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
	int max_n = 0;
	std::vector<std::vector<int>> testcases;

	std::cin >> t;
	testcases.resize(t);
	for (int i = 0; i < t; ++i) {
		int n;
		std::cin >> n;
		max_n = std::max(n, max_n);
		auto& digits = testcases[i];
		for (int j = 0; j < n; ++j) {
			int d;
			std::cin >> d;
			digits.push_back(d);
		}
	}

	for (auto& digits : testcases) {
		solve(digits, max_n == 27);
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
