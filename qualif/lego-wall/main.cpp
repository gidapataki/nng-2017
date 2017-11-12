#include <iostream>
#include <vector>
#include <cctype>
#include <array>
#include <algorithm>
#include <utility>
#include <fstream>
#include <numeric>
#include <unordered_map>

#include <boost/rational.hpp>
#include <boost/functional/hash.hpp>
#include <boost/math/tools/polynomial.hpp>
#include <boost/container/static_vector.hpp>

#ifdef CACHE_STATS
int cache_miss = 0;
int cache_query = 0;
#endif

template <class T>
struct LinearAllocator {
    typedef T value_type;
    LinearAllocator()
        : data(new char[size])
        , current(data.get())
    {
    }
    template <class U> LinearAllocator(const LinearAllocator<U>&) noexcept
        : data(new char[size])
        , current(data.get())
    {
    }

    ~LinearAllocator() {
    }

    T* allocate(std::size_t n) {
        // align to 8
        current += (8 - (reinterpret_cast<uintptr_t>(current) & 7));
        void* ret = current;
        current += n * sizeof(T);
        return static_cast<T*>(ret);
    }
    void deallocate(T* p, std::size_t) noexcept { /* leak */ }

    const size_t size = (1 << 26);
    std::unique_ptr<char[]> data;
    char* current = nullptr;
};

template <class T, class U>
bool operator==(const LinearAllocator<T>&, const LinearAllocator<U>&) { return false; }
template <class T, class U>
bool operator!=(const LinearAllocator<T>&, const LinearAllocator<U>&) { return true; }

struct BrickBits {
    union {
        uint64_t all_bits;
        struct {
            unsigned char type_counts[5];

            unsigned char ones_end: 4;
            unsigned char twos_end: 4;
            unsigned char threes_end: 4;
            unsigned char fours_end: 4;
            unsigned char ab : 1;
            unsigned char bc : 1;
            unsigned char cd : 1;

            unsigned char height : 5;
        };
    };

    void CopyCounts(
        unsigned char from_start,
        unsigned char from_end,
        unsigned char to_start)
    {
        assert(from_end >= from_start);
        assert(from_end <= 5);
        assert(from_start <= 5);
        assert(to_start <= from_start);
        assert(to_start + (from_end - from_start) <= 5);

        for (int f = from_start, t = to_start; f < from_end; ++f, ++t) {
            type_counts[t] = type_counts[f];
        }
    }

    void Normalize();
};

void BrickBits::Normalize() {
    std::sort(type_counts + 0, type_counts + ones_end, std::greater<unsigned char>{});
    std::sort(type_counts + ones_end, type_counts + twos_end, std::greater<unsigned char>{});
    std::sort(type_counts + twos_end, type_counts + threes_end, std::greater<unsigned char>{});
    std::sort(type_counts + threes_end, type_counts + fours_end, std::greater<unsigned char>{});

    auto old_ones_end = ones_end;
    auto old_twos_end = twos_end;
    auto old_threes_end = threes_end;
    auto old_fours_end = fours_end;

    // remove zeros from ends
    for (int i = old_ones_end - 1; i >= 0; --i) { if (type_counts[i] == 0) { --ones_end; } }
    for (int i = old_twos_end - 1; i >= old_ones_end; --i) { if (type_counts[i] == 0) { --twos_end; } }
    for (int i = old_threes_end - 1; i >= old_twos_end; --i) { if (type_counts[i] == 0) { --threes_end; } }
    for (int i = old_fours_end - 1; i >= old_threes_end; --i) { if (type_counts[i] == 0) { --fours_end; } }

    CopyCounts(old_ones_end, twos_end, ones_end);
    twos_end -= old_ones_end - ones_end;

    CopyCounts(old_twos_end, threes_end, twos_end);
    threes_end -= old_twos_end - twos_end;

    CopyCounts(old_threes_end, fours_end, threes_end);
    fours_end -= old_threes_end - threes_end;

    auto remaining_area = height*4;
    // discard excess bricks
    for (int i = 0; i<ones_end; ++i) { type_counts[i] = std::min(static_cast<int>(type_counts[i]), remaining_area); };
    for (int i = ones_end; i<twos_end; ++i) { type_counts[i] = std::min(static_cast<int>(type_counts[i]), remaining_area/2); };
    for (int i = twos_end; i<threes_end; ++i) { type_counts[i] = std::min(static_cast<int>(type_counts[i]), remaining_area/3); };
    for (int i = threes_end; i<fours_end; ++i) { type_counts[i] = std::min(static_cast<int>(type_counts[i]), remaining_area/4); };
}

std::ostream& operator<<(std::ostream& os, const BrickBits& bb) {
    for (int i = 0; i < 5; ++i) { os << int(bb.type_counts[i]) << ' '; }
    os << '\n';

    std::array<char, 5> owners{{ 'x', 'x', 'x', 'x', 'x' }};

    for (int i = 0; i < bb.ones_end; ++i) { assert(owners[i] == 'x'); owners[i] = '1'; }
    for (int i = bb.ones_end; i < bb.twos_end; ++i) { assert(owners[i] == 'x'); owners[i] = '2'; }
    for (int i = bb.twos_end; i < bb.threes_end; ++i) { assert(owners[i] == 'x'); owners[i] = '3'; }
    for (int i = bb.threes_end; i < bb.fours_end; ++i) { assert(owners[i] == 'x'); owners[i] = '4'; }

    for (int i = 0; i < 5; ++i) { os << owners[i] << ' '; }
    os << '\n';

    return os;
}

bool operator==(const BrickBits& lhs, const BrickBits& rhs) {
    return lhs.all_bits == rhs.all_bits;
}

bool operator!=(const BrickBits& lhs, const BrickBits& rhs) {
    return !(lhs == rhs);
}

std::size_t hash_value(const BrickBits& bricks) {
    return bricks.all_bits;
}

using SingleBricks = boost::container::static_vector<int, 5>;

struct Bricks {
    std::array<SingleBricks, 4> bricks;
};

BrickBits BitsFromBricks(const Bricks& bricks) {
    BrickBits bits{};

    bits.ab = false;
    bits.bc = false;
    bits.cd = false;

    bits.ones_end = 0;
    for (int i = 0; i < bricks.bricks[0].size(); ++i) {
        bits.type_counts[bits.ones_end++] = bricks.bricks[0][i];
    }

    bits.twos_end = bits.ones_end;
    for (int i = 0; i < bricks.bricks[1].size(); ++i) {
        bits.type_counts[bits.twos_end++] = bricks.bricks[1][i];
    }

    bits.threes_end = bits.twos_end;
    for (int i = 0; i < bricks.bricks[2].size(); ++i) {
        bits.type_counts[bits.threes_end++] = bricks.bricks[2][i];
    }

    bits.fours_end = bits.threes_end;
    for (int i = 0; i < bricks.bricks[3].size(); ++i) {
        bits.type_counts[bits.fours_end++] = bricks.bricks[3][i];
    }

    return bits;
}

namespace std {

template<typename... Args>
struct hash<std::tuple<Args...>> {
    typedef std::tuple<Args...> argument_type;
    typedef std::size_t result_type;

    result_type operator()(const std::tuple<Args...>& t) const {
        return boost::hash_value(t);
    }
};

template<>
struct hash<BrickBits> {
    typedef BrickBits argument_type;
    typedef std::size_t result_type;
    result_type operator()(const BrickBits& b) const {
        return hash_value(b);
    }
};

} // namespace std

using BitsCache =
    std::unordered_map<
        BrickBits,
        std::uint64_t,
        std::hash<BrickBits>,
        std::equal_to<BrickBits>,
        LinearAllocator<std::pair<const BrickBits, std::uint64_t>>
    >;

std::unique_ptr<BitsCache> bits_cache;

auto memoize(std::uint64_t (*f)(BrickBits arg)) {
    return [f](BrickBits arg) mutable {
        arg.Normalize();
#ifdef CACHE_STATS
        ++cache_query;
#endif
        auto it = bits_cache->find(arg);
        if (it == end(*bits_cache)) {
            it = bits_cache->insert(std::make_pair(arg, f(arg))).first;
#ifdef CACHE_STATS
            ++cache_miss;
#endif
        }
        return it->second;
    };
}

std::uint64_t EveryBits4(BrickBits bits);

auto memoizedEveryBits4 = memoize(EveryBits4);

std::uint64_t EveryBits4(BrickBits bits) {
    --bits.height;
    if (bits.height == 0) {
        return (bits.ab & bits.bc & bits.cd) != 0;
    }

    int ones_end = bits.ones_end;
    int twos_end = bits.twos_end;
    int threes_end = bits.threes_end;
    int fours_end = bits.fours_end;

    // we only have ones, and still have a hole
    if (ones_end == fours_end) {
        if (!bits.ab || !bits.bc || !bits.cd) {
            return 0;
        }
    }

    // we only have twos, and still have a hole in the middle
    if (ones_end == 0 && twos_end == fours_end) {
        if (!bits.bc) {
            return 0;
        }
    }

    // we only have threes
    if (ones_end == 0 && twos_end == 0 && threes_end == fours_end) {
        return 0;
    }

    std::uint64_t result = 0;

    // 4
    for (int a = threes_end; a < fours_end; ++a) {
        auto bb = bits;
        bb.type_counts[a] -= 1;
        bb.ab = true;
        bb.bc = true;
        bb.cd = true;
        result += memoizedEveryBits4(bb);
    }

    // 1 - 3
    for (int a = 0; a < ones_end; ++a) {
        for (int b = twos_end; b < threes_end; ++b) {
            auto bb = bits;
            bb.type_counts[a] -= 1;
            bb.type_counts[b] -= 1;
            bb.bc = true;
            {
                auto bb2 = bb;
                bb2.ab = true;
                result += memoizedEveryBits4(bb2);
            }
            {
                auto bb2 = bb;
                bb2.cd = true;
                result += memoizedEveryBits4(bb2);
            }
        }
    }

    // 2a - 2a
    for (int a = ones_end; a < twos_end && bits.type_counts[a] >= 2; ++a) {
        auto bb = bits;
        bb.type_counts[a] -= 2;
        bb.ab = true;
        bb.cd = true;
        result += memoizedEveryBits4(bb);
    }

    // 2a - 2b
    for (int a = ones_end; a < twos_end; ++a) {
        for (int b = a+1; b < twos_end; ++b) {
            auto bb = bits;
            bb.type_counts[a] -= 1;
            bb.type_counts[b] -= 1;
            bb.ab = true;
            bb.cd = true;
            result += 2 * memoizedEveryBits4(bb);
        }
    }

    // 1a - 1a - 2b
    // 1a - 2b - 1a
    // 2b - 1a - 1a
    for (int a = 0; a < ones_end && bits.type_counts[a] >= 2; ++a) {
        for (int b = ones_end; b < twos_end; ++b) {
            auto bb = bits;
            bb.type_counts[a] -= 2;
            bb.type_counts[b] -= 1;
            {
                auto bb2 = bb;
                bb2.ab = true;
                result += memoizedEveryBits4(bb2);
            }
            {
                auto bb2 = bb;
                bb2.bc = true;
                result += memoizedEveryBits4(bb2);
            }
            {
                auto bb2 = bb;
                bb2.cd = true;
                result += memoizedEveryBits4(bb2);
            }
        }
    }

    // 1a - 1b - 2c
    // 1b - 1a - 2c
    // 1a - 2b - 1b
    // 1b - 2b - 1b
    // 2c - 1a - 1b
    // 2c - 1b - 1a
    for (int a = 0; a < ones_end; ++a) {
        for (int b = a+1; b < ones_end; ++b) {
            for (int c = ones_end; c < twos_end; ++c) {
                auto bb = bits;
                bb.type_counts[a] -= 1;
                bb.type_counts[b] -= 1;
                bb.type_counts[c] -= 1;
                {
                    auto bb2 = bb;
                    bb2.ab = true;
                    result += 2 * memoizedEveryBits4(bb2);
                }
                {
                    auto bb2 = bb;
                    bb2.bc = true;
                    result += 2 * memoizedEveryBits4(bb2);
                }
                {
                    auto bb2 = bb;
                    bb2.cd = true;
                    result += 2 * memoizedEveryBits4(bb2);
                }
            }
        }
    }

    for (int a = 0; a < ones_end; ++a) {
        auto bb1 = bits;
        bb1.type_counts[a] -= 1;
        for (int b = 0; b < ones_end; ++b) {
            if (bb1.type_counts[b] == 0) { continue; }
            auto bb2 = bb1;
            bb2.type_counts[b] -= 1;
            for (int c = 0; c < ones_end; ++c) {
                if (bb2.type_counts[c] == 0) { continue; }
                auto bb3 = bb2;
                bb3.type_counts[c] -= 1;
                for (int d = 0; d < ones_end; ++d) {
                    if (bb3.type_counts[d] == 0) { continue; }
                    auto bb4 = bb3;
                    bb4.type_counts[d] -= 1;
                    result += memoizedEveryBits4(bb4);
                }
            }
        }
    }
    return result;
}

using Rational = boost::rational<std::uint64_t>;
using Poly = boost::math::tools::polynomial<Rational>;

std::uint64_t factorial20(std::uint64_t n) {
    static std::uint64_t facts[21] = {
        1ull, 1ull, 2ull, 6ull, 24ull, 120ull, 720ull, 5040ull, 40320ull,
        362880ull, 3628800ull, 39916800ull, 479001600ull, 6227020800ull,
        87178291200ull, 1307674368000ull, 20922789888000ull, 355687428096000ull,
        6402373705728000ull, 121645100408832000ull, 2432902008176640000ull
    };
    assert(n <= 20);

    return facts[n];
}

// { 1/1 }
// { 1/1, 1/1 }
// { 1/1, 1/1, 1/2 }
// { 1/1, 1/1, 1/2, 1/6 }
// { 1/1, 1/1, 1/2, 1/6, 1/24 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200, 1/1307674368000 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200, 1/1307674368000, 1/20922789888000 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200, 1/1307674368000, 1/20922789888000, 1/355687428096000 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200, 1/1307674368000, 1/20922789888000, 1/355687428096000, 1/6402373705728000 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200, 1/1307674368000, 1/20922789888000, 1/355687428096000, 1/6402373705728000, 1/121645100408832000 }
// { 1/1, 1/1, 1/2, 1/6, 1/24, 1/120, 1/720, 1/5040, 1/40320, 1/362880, 1/3628800, 1/39916800, 1/479001600, 1/6227020800, 1/87178291200, 1/1307674368000, 1/20922789888000, 1/355687428096000, 1/6402373705728000, 1/121645100408832000, 1/2432902008176640000 }

Poly getFactPoly(std::uint64_t n) {
    std::vector<Rational> coeffs;
    coeffs.reserve(n);
    for (int i = 0; i <= n; ++i) {
        coeffs.push_back(Rational(1, factorial20(i)));
    }
    return Poly(coeffs.begin(), coeffs.end());
}

std::uint64_t getPossibleOrderings(int height, const boost::container::static_vector<int, 5>& fours) {
    using namespace boost::math::tools;
    using boost::rational;

    Poly p{1};
    for (auto n : fours) {
        p *= getFactPoly(n);
    }

    if (height >= p.size()) {
        return 0;
    }

    auto coeff = p[height];
    coeff *= factorial20(height);

    return coeff.numerator();
}

std::uint64_t doTheThing(int height, Bricks bricks) {
    // check if there is even enough bricks to make a wall tall enough
    int area = 0;
    for (int i = 0; i < bricks.bricks.size(); ++i) {
        for (int j = 0; j < bricks.bricks[i].size(); ++j) {
            area += (i+1) * bricks.bricks[i][j];
        }
    }

    if (area < 4*height) {
        return 0;
    }

    auto bits = BitsFromBricks(bricks);
    bits.height = height + 1;
    return memoizedEveryBits4(bits);
}

int main(int argc, char** argv) {
    static_assert(sizeof(BrickBits) == sizeof(uint64_t), "");
    auto* in_ptr = &std::cin;
    std::ifstream in_file;
    if (argc == 2) {
        in_file.open(argv[1]);
        in_ptr = &in_file;
    }
    auto& in = *in_ptr;

    int test_count;
    in >> test_count;

    for (int i = 0; i < test_count; ++i) {
        bits_cache = std::make_unique<BitsCache>();
        bits_cache->rehash(2000000);
        int height, brick_type_count;
        in >> height >> brick_type_count;
        Bricks bricks;
        for (int j = 0; j < brick_type_count; ++j) {
            int count, length;
            in >> count >> length;
            bricks.bricks[length - 1].push_back(count);
        }
        auto all_count = doTheThing(height, bricks);
        std::cout << all_count << std::endl;
#ifdef CACHE_STATS
        std::cerr << "Cache miss ratio: " << cache_miss << "/" << cache_query << " ("
            << double(cache_miss) / cache_query * 100 << "%)" << std::endl;
        cache_miss = 0;
        cache_query = 0;
        std::cout << "Map size = " << bits_cache->size() << std::endl;
        std::cout << "Bucket count = " << bits_cache->bucket_count() << std::endl;
#endif
    }
}
