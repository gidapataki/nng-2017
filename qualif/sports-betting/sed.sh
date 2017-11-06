#!/usr/bin/env bash
sed --in-place 's/assignment/a/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/distMatrixTemp/m/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/distMatrix/b/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/starMatrix/c/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/newStarMatrix/d/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/primeMatrix/e/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/coveredColumns/f/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/coveredRows/g/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/nOfRows/hs/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/nOfColumns/i/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/minDim/j/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/row/k/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/column/l/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/minValue/n/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place 's/value/o/g' src/Hungarian.cpp src/Hungarian.h

sed --in-place 's/HungarianAlgorithm/H/g' src/Hungarian.cpp src/Hungarian.h
sed --in-place '1s/^/#define H HungarianAlgorithm\n/' src/Hungarian.h

sed --in-place 's/for/F/g' src/Hungarian.cpp
sed --in-place '1s/^/#define F(...) for(__VA_ARGS__)\n/' src/Hungarian.h

sed --in-place 's/true/T/g' src/Hungarian.cpp
sed --in-place '1s/^/auto T=true;\n/' src/Hungarian.cpp

sed --in-place 's/false/G/g' src/Hungarian.cpp
sed --in-place '1s/^/auto G=true;\n/' src/Hungarian.cpp

sed --in-place 's/std:://g' src/Hungarian.cpp src/hungarian3_solve.cpp
sed --in-place '1s/^/using namespace std;\n/' src/Hungarian.h
