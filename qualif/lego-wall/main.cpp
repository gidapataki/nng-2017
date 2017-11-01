#include <iostream>
#include <vector>
#include <cctype>

struct Bricks {
    int count = 0;
    int length = 0;
};

std::uint64_t doTheThing(std::vector<Bricks> bricks_collection) {
    return 0;
}

int main() {
    int test_count;
    std::cin >> test_count;

    for (int i = 0; i < test_count; ++i) {
        int height, brick_type_count;
        std::cin >> height >> brick_type_count;
        std::vector<Bricks> bricks_collection;
        for (int j = 0; j < brick_type_count; ++j) {
            Bricks b;
            std::cin >> b.count >> b.length;
            bricks_collection.push_back(b);
        }
        auto all_count = doTheThing(bricks_collection);
        std::cout << all_count << std::endl;
    }
}
