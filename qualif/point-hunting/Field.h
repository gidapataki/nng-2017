#pragma once

#include "Cube.h"

class Field {
public:
    Cube cube;

    // cube's position
    int px = 0;
    int py = 0;

    int width = 6;
    int height = 6;

    int score = 0;

    void start(); // init score
    void roll(bool direction); // true right, false up
};
