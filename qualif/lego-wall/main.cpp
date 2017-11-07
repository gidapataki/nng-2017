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
    return memoizedEveryWall4[height](bricks, false, false, false);
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
