#include "Field.h"

#include <cassert>

void Field::start() {
    assert(score == 0);
    score = cube.bottom;
}

void Field::roll(bool direction) {
    if (direction) {
        assert(px < width - 1);
        cube.rollRight();
        ++px;
    } else {
        assert(py < height - 1);
        cube.rollUp();
        ++py;
    }
    score += cube.bottom;
}
