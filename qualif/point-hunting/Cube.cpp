#include "Cube.h"

#include <cassert>
#include <tuple>

namespace {

void rotate4(int& a, int& b, int& c, int& d) {
    std::tie(a, b, c, d) = std::make_tuple(b, c, d, a);
}

} // anonymous namespace

Cube::Cube(int side, int rotations) {
    assert(side >= 0 && side <= 5);
    assert(rotations >= 0 && rotations <= 3);
    switch (side) {
        case 0: break;
        case 1: rollDown(); break;
        case 2: rollRight(); break;
        case 3: rollLeft(); break;
        case 4: rollUp(); break;
        case 5: rollUp(); rollUp(); break;
    }

    for (int i = 0; i < rotations; ++i) {
        rotateCW();
    }
}

void Cube::rollUp() {
    rotate4(top, down, bottom, up);
}

void Cube::rollRight() {
    rotate4(top, left, bottom, right);
}

void Cube::rollDown() {
    rotate4(top, up, bottom, down);
}

void Cube::rollLeft() {
    rotate4(top, right, bottom, left);
}

void Cube::rotateCW() {
    rotate4(left, up, right, down);
}

void Cube::rotateCCW() {
    rotate4(right, up, left, down);
}
