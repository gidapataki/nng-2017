#include <iostream>
#include <vector>
#include <algorithm>

#include "Cube.h"
#include "Field.h"

int doSingleTest(
    int side, int rotations,
    int width, int height,
    const std::vector<bool>& decisions)
{
    Field field;
    field.cube = Cube{side, rotations};

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
    int best_r = -1;
    int best_s = -1;
    std::vector<bool> best_decisions;
    for (int s = 0; s < 6; ++s) {
        for (int r = 0; r < 4; ++r) {
            do {
                int score = doSingleTest(s, r, width, height, decisions);
                if (score > max) {
                    max = score;
                    best_r = r;
                    best_s = s;
                    best_decisions = decisions;
                }
            } while (std::next_permutation(decisions.begin(), decisions.end()));
        }
    }

    std::cout << best_s << ", " << best_r << std::endl;

    return max;
}

int main() {
    std::cout << bruteForceMaxScore(1, 1) << std::endl;
    std::cout << bruteForceMaxScore(2, 1) << std::endl;
    std::cout << bruteForceMaxScore(2, 2) << std::endl;
    std::cout << bruteForceMaxScore(10, 7) << std::endl;
}
