#include <iostream>
#include <vector>
#include <cctype>
#include <array>
#include <algorithm>
#include <utility>
#include <fstream>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <boost/container/static_vector.hpp>

#ifdef CACHE_STATS
int cache_miss = 0;
int cache_query = 0;
#endif

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

std::uint64_t EveryBits4(BrickBits bits);

auto memoizedEveryBits4 = memoize(EveryBits4);

std::uint64_t EveryBits4(BrickBits bits) {
    --bits.height;
    if (bits.height == 0) {
        return (bits.ab & bits.bc & bits.cd) != 0;
    }

    bits.Normalize();
    std::uint64_t result = 0;

    // 4
    for (int a = bits.threes_end; a < bits.fours_end; ++a) {
        auto bb = bits;
        bb.type_counts[a] -= 1;
        bb.ab = true;
        bb.bc = true;
        bb.cd = true;
        result += memoizedEveryBits4(bb);
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
    for (int a = bits.ones_end; a < bits.twos_end; ++a) {
        if (bits.type_counts[a] >= 2) {
            auto bb = bits;
            bb.type_counts[a] -= 2;
            bb.ab = true;
            bb.cd = true;
            result += memoizedEveryBits4(bb);
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
            result += 2 * memoizedEveryBits4(bb);
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
                    result += memoizedEveryBits4(bits);
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

std::uint64_t doTheThing(int height, Bricks bricks) {
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
#endif
    }
}
