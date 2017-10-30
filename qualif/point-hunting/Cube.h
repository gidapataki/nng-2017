#pragma once

class Cube {
public:
    void rotateUp();
    void rotateRight();

    void rotateDown();
    void rotateLeft();

    int bottom = 1;
    int top = 6;

    int left = 3;
    int right = 4;

    int up = 5;
    int down = 2;
};
