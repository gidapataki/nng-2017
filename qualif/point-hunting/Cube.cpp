#include "Cube.h"

#include <tuple>

namespace {

void rotate4(int& a, int& b, int& c, int& d) {
    std::tie(a, b, c, d) = std::make_tuple(b, c, d, a);
}

} // anonymous namespace

void Cube::rotateUp() {
    rotate4(top, down, bottom, up);
}

void Cube::rotateRight() {
    rotate4(top, left, bottom, right);
}

void Cube::rotateDown() {
    rotate4(top, up, bottom, down);
}

void Cube::rotateLeft() {
    rotate4(top, right, bottom, left);
}
