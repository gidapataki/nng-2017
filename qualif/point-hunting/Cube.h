#pragma once

class Cube {
public:
    Cube() = default;

    // side from [0, 5]
    // rotation from [0, 3]
    Cube(int side, int rotations);

    void rollUp();
    void rollRight();

    void rollDown();
    void rollLeft();

    void rotateCW();
    void rotateCCW();

    int bottom = 1;
    int top = 6;

    int left = 3;
    int right = 4;

    int up = 5;
    int down = 2;
};
