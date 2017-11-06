#!/bin/bash
set -e

if test -z "$1"
then
	echo "usage: $0 <executable>"
	exit 1
fi

for infile in tests/test*.in
do
	echo "Testing ${infile}"
	outfile=tests/$(basename ${infile} .in).out
	diff "${outfile}" <("$1" < "${infile}")
done
