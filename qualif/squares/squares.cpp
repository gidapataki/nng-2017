#include <cstdint>
#include <iostream>
#include <vector>
#include <cassert>

#if DEBUG_MEMORY

#if __APPLE__
	#include <malloc/malloc.h>
#else
	#include <malloc.h>
#endif

size_t AllocSize(void* p) {
	if (!p) {
		return 0;
	}
#if __APPLE__
	return malloc_size(p) + 8;
#else
	return malloc_usable_size(p) + 8;
#endif
	return 0;
}

size_t g_AllocatedSize = 0;
size_t g_MaxAllocatedSize = 0;

void* Allocate(size_t size) {
	void* p = malloc(size);
	size = AllocSize(p);
        g_AllocatedSize += size;
        if (g_AllocatedSize > g_MaxAllocatedSize) {
            g_MaxAllocatedSize = g_AllocatedSize;
        }

	return p;
}

void Free(void* p) {
	size_t size = AllocSize(p);
        g_AllocatedSize -= size;
	free(p);
}

void* operator new(size_t size) throw (std::bad_alloc) {
	void* ptr = Allocate(size);
	if (!ptr) {
		throw std::bad_alloc();
	}
	return ptr;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept {
	return Allocate(size);
}

void operator delete(void* ptr) noexcept {
	Free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
	Free(ptr);
}

void* operator new[](size_t size) throw (std::bad_alloc) {
	return ::operator new(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept {
	return Allocate(size);
}

void operator delete[](void* ptr) noexcept {
	Free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
	Free(ptr);
}

#endif // DEBUG_MEMORY

struct Digits1 {
	uint8_t v[10] = {};
};

uint8_t get_digit(const Digits1& ds, int index) {
	return ds.v[index];
}

void inc_digit(Digits1& ds, int index) {
	++ds.v[index];
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

Digits2& operator+=(Digits2& lhs, const Digits2& rhs) {
	lhs.data[0] += rhs.data[0];
	lhs.data[1] += rhs.data[1];
	return lhs;
}

using Digits = Digits2;
using Numbers = std::vector<unsigned>;


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
			inc_digit(px_digits, i);
			inc_digit(px_digits, msd);

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
		inc_digit(ds, d);
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
#if DEBUG_MEMORY
        std::cerr << "MaxMem = " << g_MaxAllocatedSize / (1 << 20) << "MB" << std::endl;
#endif // DEBUG MEMORY
	return 0;
}
