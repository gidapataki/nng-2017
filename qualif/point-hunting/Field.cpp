#include "Field.h"

#include <cassert>

void Field::roll(bool direction) {
    if (direction) {
        assert(px < width - 1);
        cube.rotateRight();
        ++px;
    } else {
        assert(py < height - 1);
        cube.rotateUp();
        ++py;
    }
    score += cube.bottom;
}
