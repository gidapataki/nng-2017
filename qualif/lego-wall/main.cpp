#include <iostream>
#include <vector>
#include <cctype>
#include <array>
#include <algorithm>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <boost/container/static_vector.hpp>

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

bool operator==(const Bricks& lhs, const Bricks& rhs) {
    return lhs.bricks == rhs.bricks;
}

bool operator!=(const Bricks& lhs, const Bricks& rhs) {
    return !(lhs == rhs);
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

template<>
struct hash<Bricks> {
    typedef Bricks argument_type;
    typedef std::size_t result_type;
    result_type operator()(const Bricks& b) const {
        std::size_t hash = 0;
        for (auto& bricks : b.bricks) {
            boost::hash_combine(hash, bricks.size());
            for (auto count : bricks) {
                boost::hash_combine(hash, count);
            }
        }
        return hash;
    }
};

} // namespace std

template<typename R, typename Arg>
auto memoize(R (*f)(const Arg& arg)) {
    std::unordered_map<Arg, R> table;
    return [f, table](const Arg& arg) mutable {
        auto it = table.find(arg);
        if (it == end(table)) {
            it = table.insert(std::make_pair(arg, f(arg))).first;
        }
        return it->second;
    };
}

std::uint64_t doTheThing(Bricks bricks) {
    bricks.Normalize();
    std::cout << bricks << std::endl;
    return 0;
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
        auto all_count = doTheThing(bricks);
        std::cout << all_count << std::endl;
    }
}
