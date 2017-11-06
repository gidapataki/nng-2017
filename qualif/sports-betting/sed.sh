#!/usr/bin/env bash

SED="sed"
if which gsed > /dev/null; then
    SED="gsed"
fi


$SED --in-place 's/assignment/a/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/distMatrixTemp/m/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/distMatrix/b/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/starMatrix/c/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/newStarMatrix/d/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/primeMatrix/e/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/coveredColumns/f/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/coveredRows/g/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/nOfRows/hs/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/nOfColumns/i/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/minDim/j/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/row/k/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/column/l/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/minValue/n/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place 's/value/o/g' src/Hungarian.cpp src/Hungarian.h

$SED --in-place 's/HungarianAlgorithm/H/g' src/Hungarian.cpp src/Hungarian.h
$SED --in-place '1s/^/#define H HungarianAlgorithm\n/' src/Hungarian.h

$SED --in-place 's/for/Q/g' src/Hungarian.cpp
$SED --in-place '1s/^/#define Q(...) for(__VA_ARGS__)\n/' src/Hungarian.h

$SED --in-place 's/true/T/g' src/Hungarian.cpp
$SED --in-place '1s/^/auto T=true;\n/' src/Hungarian.cpp

$SED --in-place 's/false/G/g' src/Hungarian.cpp
$SED --in-place '1s/^/auto G=false;\n/' src/Hungarian.cpp

$SED --in-place 's/std:://g' src/Hungarian.cpp src/hungarian3_solve.cpp
$SED --in-place '1s/^/using namespace std;\n/' src/Hungarian.h
