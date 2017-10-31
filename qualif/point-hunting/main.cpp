#include <iostream>
#include <vector>
#include <algorithm>

#include "Cube.h"
#include "Field.h"

std::ostream& operator<<(std::ostream& os, const std::vector<bool>& v) {
    for (bool b : v) {
        os << (b ? '>' : '^');
    }
    return os;
}

struct History {
    Cube cube;
    int rotations;
    int side;
    std::vector<bool> decisions;
};

std::ostream& operator<<(std::ostream& os, const History& h) {
    os << "cube(" << h.side << ", " << h.rotations << "): " << h.cube << " (" << h.decisions << ")";
    return os;
}

int doSingleTest(
    Cube cube,
    int width, int height,
    const std::vector<bool>& decisions)
{
    Field field;
    field.cube = cube;

    field.width = width;
    field.height = height;

    field.start();

    for (bool b : decisions) {
        field.roll(b);
    }

    return field.score;
}

int bruteForceMaxScore(int width, int height) {
    std::vector<bool> decisions(width + height - 2);
    for (int i = 0; i < width - 1; ++i) { decisions[i] = true; }
    std::sort(decisions.begin(), decisions.end());

    int max = 0;
    std::vector<History> best_histories;
    for (int s = 4; s <= 6; ++s) {
        for (int r = 0; r < 4; ++r) {
            do {
                Cube cube{s, r};
                int score = doSingleTest(cube, width, height, decisions);
                if (score > max) {
                    max = score;
                    best_histories.clear();
                }
                if (score == max) {
                    History h;
                    h.cube = cube;
                    h.side = s;
                    h.rotations = r;
                    h.decisions = decisions;
                    best_histories.push_back(h);
                }
            } while (std::next_permutation(decisions.begin(), decisions.end()));
        }
    }

    // for (auto& h : best_histories) {
    //     std::cout << "(" << width << "," << height << ") " << h << std::endl;
    // }

    return max;
}

int main() {
    int size = 15;
    std::vector<std::vector<int>> scores(15, std::vector<int>{15});

    for (int y = 1; y <= 35; ++y) {
        for (int x = 1; x <= 35; ++x) {
            if (x + y > 25) {
                continue;
            }
            auto score = bruteForceMaxScore(x, y);
            // scores[x-1][y-1] = score;
            std::cout << score << " ";
        }
        std::cout << std::endl;
    }
    // std::cout << bruteForceMaxScore(1, 1) << std::endl;
    // std::cout << bruteForceMaxScore(2, 1) << std::endl;
    // std::cout << bruteForceMaxScore(2, 2) << std::endl;
    // std::cout << bruteForceMaxScore(10, 7) << std::endl;
}
