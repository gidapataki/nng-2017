#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using Num = unsigned long long;

#if DEBUG
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
#endif

Num firstRow(Num width) {
    const int pattern[] = {6, 11, 13, 14};

    --width;

    return 14 * (width / 4) + pattern[width % 4];
}

Num secondRow(Num width) {
    const int pattern[] = {11, 15, 18, 20};

    --width;

    return 14 * (width / 4) + pattern[width % 4];
}

Num thirdRow(Num width) {
    const int pattern[] = {4, 7};

    width -= 3;

    return 18 + 7 * (width / 2) + pattern[width % 2];
}

Num fourthRow(Num width) {
    const int pattern[] = {4, 7};

    width -= 4;

    return 25 + 7 * (width / 2) + pattern[width % 2];
}

Num solveConstantTime(Num width, Num height) {
    if (height > width) {
        std::swap(width, height);
    }

    assert(width >= height);

    if (height == 1) {
        return firstRow(width);
    } else if (height == 2) {
        return secondRow(width);
    } else if (height == 3) {
        return thirdRow(width);
    } else if (height == 4) {
        return fourthRow(width);
    } else {
        int pattern[] = {4, 7};
        if (height % 2 == 0) {
            pattern[0] = 3;
        }

        return fourthRow(height) + 7 * ((width - 5) / 2) + pattern[(width - 5) % 2];
    }

    return -1;
}

int main() {
#if false && DEBUG
    int size = 15;
    std::vector<std::vector<int>> scores(15, std::vector<int>{15});

    for (int y = 1; y <= 11; ++y) {
        for (int x = 1; x <= 11; ++x) {
            auto bf = bruteForceMaxScore(x, y);
            auto ct = solveConstantTime(x, y);
            if (bf != ct) {
                std::cout << "(" << bruteForceMaxScore(x, y)
                    << ", " << solveConstantTime(x, y) << ") ";
            } else {
                std::cout << "(OK, OK) ";
            }
        }
        std::cout << std::endl;
    }
#else
    int c;
    std::cin >> c;

    for (int i = 0; i < c; ++i) {
        Num n, m;
        std::cin >> n >> m;

        std::cout << solveConstantTime(m, n) << std::endl;
    }

#endif
    // std::cout << bruteForceMaxScore(1, 1) << std::endl;
    // std::cout << bruteForceMaxScore(2, 1) << std::endl;
    // std::cout << bruteForceMaxScore(2, 2) << std::endl;
    // std::cout << bruteForceMaxScore(10, 7) << std::endl;
}
