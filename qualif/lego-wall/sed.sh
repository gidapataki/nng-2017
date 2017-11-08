#!/usr/bin/env bash

SED="sed"
SED_ARGS=("--in-place")
if which gsed > /dev/null; then
    SED="gsed"
    SED_ARGS=("-i" "''")
fi

function substitute() {
  pattern="$1"
  "${SED}" --in-place -e "${pattern}" main.cpp
}

substitute 's/unsigned char/uc/g'
substitute '11iusing uc = unsigned char;\n'

substitute 's/type_counts/t/g'
substitute 's/ones_end/o/g'
substitute 's/twos_end/T/g'
substitute 's/threes_end/r/g'
substitute 's/fours_end/f/g'
substitute 's/BrickBits/B/g'
substitute 's/old/O/g'
substitute 's/result/R/g'
substitute 's/apply/A/g'
substitute 's/memoizedEveryBits4/M/g'
substitute 's/memoized/m/g'
substitute 's/from_start/fs/g'
substitute 's/from_end/fe/g'
substitute 's/to_start/ts/g'
substitute 's/buffer/F/g'
substitute 's/length/l/g'
substitute 's/bricks/BS/g'
