#include <iostream>
#include <vector>
#include <algorithm>

#include "Cube.h"
#include "Field.h"

int doSingleTest(int width, int height, const std::vector<bool>& decisions) {
    Field field;

    field.width = width;
    field.height = height;

    for (bool b : decisions) {
        field.roll(b);
    }

    return field.score;
}

int main() {
    int width = 6;
    int height = 6;
    std::vector<bool> decisions(width + height - 2);
    for (int i = 0; i < width - 1; ++i) { decisions[i] = true; }
    std::sort(decisions.begin(), decisions.end());

    int max = 0;
    do {
        int score = doSingleTest(width, height, decisions);
        if (score > max) {
            max = score;
        }
    } while (std::next_permutation(decisions.begin(), decisions.end()));

    std::cout << max << std::endl;
}
