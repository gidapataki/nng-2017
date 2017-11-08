#include <iostream>
#include <vector>
#include <cctype>
#include <array>
#include <algorithm>
#include <utility>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <boost/container/static_vector.hpp>

#ifdef CACHE_STATS
int cache_miss = 0;
int cache_query = 0;
#endif

uint64_t mask(uint64_t start_bit, uint64_t length) {
    return ((1 << length) - 1) << start_bit;
}

uint64_t getBits(uint64_t value, uint64_t start_bit, uint64_t length) {
    return (value & mask(start_bit, length)) >> start_bit;
}

uint64_t setBits(uint64_t value, uint64_t start_bit, uint64_t length, uint64_t new_bits) {
    assert(new_bits < (1 << length));
    value &= ~mask(start_bit, length);
    value |= new_bits << start_bit;
    return value;
}

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

        auto length = from_end - from_start;

        assert(to_start + length <= 5);

        unsigned char buffer[5];
        for (int i = from_start; i < from_end; ++i) {
            buffer[i - from_start] = type_counts[i];
        }
        for (int i = to_start; i < to_start + length; ++i) {
            type_counts[i] = buffer[i - to_start];
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

    void Normalize();
};

void Bricks::Normalize() {
    for (auto& b : bricks) {
        std::sort(b.begin(), b.end(), std::greater<int>{});
        while (!b.empty() && b.back() == 0) {
            b.pop_back();
        }
    }
}

bool operator<(const Bricks& lhs, const Bricks& rhs) {
    return lhs.bricks < rhs.bricks;
}

bool operator==(const Bricks& lhs, const Bricks& rhs) {
    return lhs.bricks == rhs.bricks;
}

bool operator!=(const Bricks& lhs, const Bricks& rhs) {
    return !(lhs == rhs);
}

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

std::size_t hash_value(const Bricks& bricks) {
    std::size_t hash = 0;
    for (auto& bricks : bricks.bricks) {
        boost::hash_combine(hash, bricks.size());
        for (auto count : bricks) {
            boost::hash_combine(hash, count);
        }
    }
    return hash;
}

std::ostream& operator<<(std::ostream& os, const Bricks& bricks) {
    for (int i = 0; i < bricks.bricks.size(); ++i) {
        for (auto c : bricks.bricks[i]) {
            os << (i+1) << ": " << c << ", ";
        }
    }
    return os;
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
struct hash<Bricks> {
    typedef Bricks argument_type;
    typedef std::size_t result_type;
    result_type operator()(const Bricks& b) const {
        return hash_value(b);
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

template<typename R, typename... Args>
auto memoize(R (*f)(Args... arg)) {
    std::unordered_map<std::tuple<Args...>, R> table;
    return [f, table](Args... args) mutable {
#ifdef CACHE_STATS
        ++cache_query;
#endif
        auto key = std::make_tuple(args...);
        auto it = table.find(key);
        if (it == end(table)) {
            it = table.insert(std::make_pair(key, f(args...))).first;
#ifdef CACHE_STATS
            ++cache_miss;
#endif
        }
        return it->second;
    };
}

template<typename R, typename Arg>
auto memoize(R (*f)(Arg arg)) {
    std::unordered_map<Arg, R> table;
    return [f, table](Arg arg) mutable {
#ifdef CACHE_STATS
        ++cache_query;
#endif
        auto it = table.find(arg);
        if (it == end(table)) {
            it = table.insert(std::make_pair(arg, f(arg))).first;
#ifdef CACHE_STATS
            ++cache_miss;
#endif
        }
        return it->second;
    };
}

template<int H>
struct EveryBits4 {
    static std::uint64_t apply(BrickBits bricks);
};

template<>
struct EveryBits4<0> {
    static std::uint64_t apply(BrickBits bricks) {
        return (bricks.ab & bricks.bc & bricks.cd) != 0;
    }
};

static std::function<std::uint64_t(BrickBits)> memoizedEveryBits4[] = {
    EveryBits4<0>::apply,
    memoize(EveryBits4<1>::apply),
    memoize(EveryBits4<2>::apply),
    memoize(EveryBits4<3>::apply),
    memoize(EveryBits4<4>::apply),
    memoize(EveryBits4<5>::apply),
    memoize(EveryBits4<6>::apply),
    memoize(EveryBits4<7>::apply),
    memoize(EveryBits4<8>::apply),
    memoize(EveryBits4<9>::apply),
    memoize(EveryBits4<10>::apply),
    memoize(EveryBits4<11>::apply),
    memoize(EveryBits4<12>::apply),
    memoize(EveryBits4<13>::apply),
    memoize(EveryBits4<14>::apply),
    memoize(EveryBits4<15>::apply),
    memoize(EveryBits4<16>::apply),
    memoize(EveryBits4<17>::apply),
    memoize(EveryBits4<18>::apply),
    memoize(EveryBits4<19>::apply),
    memoize(EveryBits4<20>::apply)
};

template<int H>
std::uint64_t EveryBits4<H>::apply(BrickBits bits) {
    bits.Normalize();
    std::uint64_t result = 0;

    // 4
    for (int a = bits.threes_end; a < bits.fours_end; ++a) {
        auto bb = bits;
        bb.type_counts[a] -= 1;
        bb.ab = true;
        bb.bc = true;
        bb.cd = true;
        result += memoizedEveryBits4[H-1](bb);
    }

    // 1 - 3
    for (int a = 0; a < bits.ones_end; ++a) {
        for (int b = bits.twos_end; b < bits.threes_end; ++b) {
            auto bb = bits;
            bb.type_counts[a] -= 1;
            bb.type_counts[b] -= 1;
            bb.bc = true;
            {
                auto bb2 = bb;
                bb2.ab = true;
                result += memoizedEveryBits4[H-1](bb2);
            }
            {
                auto bb2 = bb;
                bb2.cd = true;
                result += memoizedEveryBits4[H-1](bb2);
            }
        }
    }

    // 2a - 2a
    for (int a = bits.ones_end; a < bits.twos_end; ++a) {
        if (bits.type_counts[a] >= 2) {
            auto bb = bits;
            bb.type_counts[a] -= 2;
            bb.ab = true;
            bb.cd = true;
            result += memoizedEveryBits4[H-1](bb);
        }
    }

    // 2a - 2b
    for (int a = bits.ones_end; a < bits.twos_end; ++a) {
        for (int b = a+1; b < bits.twos_end; ++b) {
            auto bb = bits;
            bb.type_counts[a] -= 1;
            bb.type_counts[b] -= 1;
            bb.ab = true;
            bb.cd = true;
            result += 2 * memoizedEveryBits4[H-1](bb);
        }
    }

    // 1a - 1a - 2b
    // 1a - 2b - 1a
    // 2b - 1a - 1a
    for (int a = 0; a < bits.ones_end; ++a) {
        for (int b = bits.ones_end; b < bits.twos_end; ++b) {
            if (bits.type_counts[a] >= 2) {
                auto bb = bits;
                bb.type_counts[a] -= 2;
                bb.type_counts[b] -= 1;
                {
                    auto bb2 = bb;
                    bb2.ab = true;
                    result += memoizedEveryBits4[H-1](bb2);
                }
                {
                    auto bb2 = bb;
                    bb2.bc = true;
                    result += memoizedEveryBits4[H-1](bb2);
                }
                {
                    auto bb2 = bb;
                    bb2.cd = true;
                    result += memoizedEveryBits4[H-1](bb2);
                }
            }
        }
    }

    // 1a - 1b - 2c
    // 1b - 1a - 2c
    // 1a - 2b - 1b
    // 1b - 2b - 1b
    // 2c - 1a - 1b
    // 2c - 1b - 1a
    for (int a = 0; a < bits.ones_end; ++a) {
        for (int b = a+1; b < bits.ones_end; ++b) {
            for (int c = bits.ones_end; c < bits.twos_end; ++c) {
                auto bb = bits;
                bb.type_counts[a] -= 1;
                bb.type_counts[b] -= 1;
                bb.type_counts[c] -= 1;
                {
                    auto bb2 = bb;
                    bb2.ab = true;
                    result += 2 * memoizedEveryBits4[H-1](bb2);
                }
                {
                    auto bb2 = bb;
                    bb2.bc = true;
                    result += 2 * memoizedEveryBits4[H-1](bb2);
                }
                {
                    auto bb2 = bb;
                    bb2.cd = true;
                    result += 2 * memoizedEveryBits4[H-1](bb2);
                }
            }
        }
    }

    for (int a = 0; a < bits.ones_end; ++a) {
        bits.type_counts[a] -= 1;
        for (int b = 0; b < bits.ones_end; ++b) {
            if (bits.type_counts[b] == 0) { continue; }
            bits.type_counts[b] -= 1;
            for (int c = 0; c < bits.ones_end; ++c) {
                if (bits.type_counts[c] == 0) { continue; }
                bits.type_counts[c] -= 1;
                for (int d = 0; d < bits.ones_end; ++d) {
                    if (bits.type_counts[d] == 0) { continue; }
                    bits.type_counts[d] -= 1;
                    result += memoizedEveryBits4[H-1](bits);
                    bits.type_counts[d] += 1;
                }
                bits.type_counts[c] += 1;
            }
            bits.type_counts[b] += 1;
        }
        bits.type_counts[a] += 1;
    }
    return result;
}
template<int H>
struct EveryWall4 {
    static std::uint64_t apply(Bricks bricks, bool ab, bool bc, bool cd);
};

template<>
struct EveryWall4<0> {
    static std::uint64_t apply(Bricks bricks, bool ab, bool bc, bool cd) {
        return ab && bc && cd;
    }
};

static std::function<std::uint64_t(Bricks, bool, bool, bool)> memoizedEveryWall4[] = {
    EveryWall4<0>::apply,
    memoize(EveryWall4<1>::apply),
    memoize(EveryWall4<2>::apply),
    memoize(EveryWall4<3>::apply),
    memoize(EveryWall4<4>::apply),
    memoize(EveryWall4<5>::apply),
    memoize(EveryWall4<6>::apply),
    memoize(EveryWall4<7>::apply),
    memoize(EveryWall4<8>::apply),
    memoize(EveryWall4<9>::apply),
    memoize(EveryWall4<10>::apply),
    memoize(EveryWall4<11>::apply),
    memoize(EveryWall4<12>::apply),
    memoize(EveryWall4<13>::apply),
    memoize(EveryWall4<14>::apply),
    memoize(EveryWall4<15>::apply),
    memoize(EveryWall4<16>::apply),
    memoize(EveryWall4<17>::apply),
    memoize(EveryWall4<18>::apply),
    memoize(EveryWall4<19>::apply),
    memoize(EveryWall4<20>::apply)
};

template<int H>
std::uint64_t EveryWall4<H>::apply(Bricks bricks, bool ab, bool bc, bool cd) {
    bricks.Normalize();
    std::uint64_t result = 0;

    // 4
    for (int a = 0; a < bricks.bricks[4 - 1].size(); ++a) {
        bricks.bricks[4 - 1][a] -= 1;
        result += memoizedEveryWall4[H-1](bricks, true, true, true);
        bricks.bricks[4 - 1][a] += 1;
    }

    // 1 - 3
    for (int a = 0; a < bricks.bricks[1 - 1].size(); ++a) {
        for (int b = 0; b < bricks.bricks[3 - 1].size(); ++b) {
            bricks.bricks[1 - 1][a] -= 1;
            bricks.bricks[3 - 1][b] -= 1;
            result += memoizedEveryWall4[H-1](bricks, true, true, cd);
            result += memoizedEveryWall4[H-1](bricks, ab, true, true);
            bricks.bricks[1 - 1][a] += 1;
            bricks.bricks[3 - 1][b] += 1;
        }
    }

    // 2a - 2a
    for (int a = 0; a < bricks.bricks[2 - 1].size(); ++a) {
        if (bricks.bricks[2 - 1][a] >= 2) {
            bricks.bricks[2 - 1][a] -= 2;
            result += memoizedEveryWall4[H-1](bricks, true, bc, true);
            bricks.bricks[2 - 1][a] += 2;
        }
    }

    // 2a - 2b
    for (int a = 0; a < bricks.bricks[2 - 1].size(); ++a) {
        for (int b = a+1; b < bricks.bricks[2 - 1].size(); ++b) {
            bricks.bricks[2 - 1][a] -= 1;
            bricks.bricks[2 - 1][b] -= 1;
            result += 2 * memoizedEveryWall4[H-1](bricks, true, bc, true);
            bricks.bricks[2 - 1][a] += 1;
            bricks.bricks[2 - 1][b] += 1;
        }
    }

    // 1a - 1a - 2b
    // 1a - 2b - 1a
    // 2b - 1a - 1a
    for (int a = 0; a < bricks.bricks[1 - 1].size(); ++a) {
        for (int b = 0; b < bricks.bricks[2 - 1].size(); ++b) {
            if (bricks.bricks[1 - 1][a] >= 2) {
                bricks.bricks[1 - 1][a] -= 2;
                bricks.bricks[2 - 1][b] -= 1;
                result += memoizedEveryWall4[H-1](bricks, true, bc, cd);
                result += memoizedEveryWall4[H-1](bricks, ab, true, cd);
                result += memoizedEveryWall4[H-1](bricks, ab, bc, true);
                bricks.bricks[1 - 1][a] += 2;
                bricks.bricks[2 - 1][b] += 1;
            }
        }
    }

    // 1a - 1b - 2c
    // 1b - 1a - 2c
    // 1a - 2b - 1b
    // 1b - 2b - 1b
    // 2c - 1a - 1b
    // 2c - 1b - 1a
    for (int a = 0; a < bricks.bricks[1 - 1].size(); ++a) {
        for (int b = a+1; b < bricks.bricks[1 - 1].size(); ++b) {
            for (int c = 0; c < bricks.bricks[2 - 1].size(); ++c) {
                bricks.bricks[1 - 1][a] -= 1;
                bricks.bricks[1 - 1][b] -= 1;
                bricks.bricks[2 - 1][c] -= 1;
                result += 2 * memoizedEveryWall4[H-1](bricks, true, bc, cd);
                result += 2 * memoizedEveryWall4[H-1](bricks, ab, true, cd);
                result += 2 * memoizedEveryWall4[H-1](bricks, ab, bc, true);
                bricks.bricks[1 - 1][a] += 1;
                bricks.bricks[1 - 1][b] += 1;
                bricks.bricks[2 - 1][c] += 1;
            }
        }
    }

    for (int a = 0; a < bricks.bricks[1 - 1].size(); ++a) {
        bricks.bricks[1 - 1][a] -= 1;
        for (int b = 0; b < bricks.bricks[1 - 1].size(); ++b) {
            if (bricks.bricks[1 - 1][b] == 0) { continue; }
            bricks.bricks[1 - 1][b] -= 1;
            for (int c = 0; c < bricks.bricks[1 - 1].size(); ++c) {
                if (bricks.bricks[1 - 1][c] == 0) { continue; }
                bricks.bricks[1 - 1][c] -= 1;
                for (int d = 0; d < bricks.bricks[1 - 1].size(); ++d) {
                    if (bricks.bricks[1 - 1][d] == 0) { continue; }
                    bricks.bricks[1 - 1][d] -= 1;
                    result += memoizedEveryWall4[H-1](bricks, ab, bc, cd);
                    bricks.bricks[1 - 1][d] += 1;
                }
                bricks.bricks[1 - 1][c] += 1;
            }
            bricks.bricks[1 - 1][b] += 1;
        }
        bricks.bricks[1 - 1][a] += 1;
    }
    return result;
}

std::uint64_t doTheThing(int height, Bricks bricks) {
    auto bits = BitsFromBricks(bricks);
    auto new_result = memoizedEveryBits4[height](bits);
    // auto old_result = memoizedEveryWall4[height](bricks, false, false, false);
    // assert(new_result == old_result);
    return new_result;
}

int main() {
    int test_count;
    std::cin >> test_count;

    for (int i = 0; i < test_count; ++i) {
        int height, brick_type_count;
        std::cin >> height >> brick_type_count;
        Bricks bricks;
        for (int j = 0; j < brick_type_count; ++j) {
            int count, length;
            std::cin >> count >> length;
            bricks.bricks[length - 1].push_back(count);
        }
        auto all_count = doTheThing(height, bricks);
        std::cout << all_count << std::endl;
#ifdef CACHE_STATS
        std::cerr << "Cache miss ratio: " << cache_miss << "/" << cache_query << " ("
            << double(cache_miss) / cache_query * 100 << "%)" << std::endl;
        cache_miss = 0;
        cache_query = 0;
#endif
    }
}
