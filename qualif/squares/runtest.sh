#!/bin/bash
set -e

for infile in tests/test*.in
do
	echo "Testing ${infile}"
	outfile=tests/$(basename ${infile} .in).out
	time diff "${outfile}" <($(sugoi target -b)/squares < "${infile}")
done
