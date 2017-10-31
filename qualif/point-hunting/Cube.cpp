#include "Cube.h"

#include <cassert>
#include <tuple>

namespace {

void rotate4(int& a, int& b, int& c, int& d) {
    std::tie(a, b, c, d) = std::make_tuple(b, c, d, a);
}

} // anonymous namespace

Cube::Cube(int side, int rotations) {
    assert(side >= 1 && side <= 6);
    assert(rotations >= 0 && rotations <= 3);
    switch (side) {
        case 1: break;
        case 2: rollDown(); break;
        case 3: rollLeft(); break;
        case 4: rollRight(); break;
        case 5: rollUp(); break;
        case 6: rollUp(); rollUp(); break;
    }

    assert(bottom == side);

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

std::ostream& operator<<(std::ostream& os, const Cube& c) {
    os << "b: " << c.bottom << ", ";
    os << "t: " << c.top << ", ";
    os << "l: " << c.left << ", ";
    os << "r: " << c.right << ", ";
    os << "u: " << c.up << ", ";
    os << "d: " << c.down;
    return os;
}
