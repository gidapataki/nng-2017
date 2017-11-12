#!/bin/bash
set -e


city_bin="$1"
if test -z "${city_bin}" || ! test -e "${city_bin}"
then
	echo "usage: $0 <executable>"
	exit 1
fi

sum_score=0
tab=$'\t'

for map in maps/*.in
do
	output=$(
		(time bin/grandprix -exec "$1" -map "${map}" -novis 2>/dev/null) 2> >(grep real)
	)
	score=$(head -n1 <<< "${output}")
	elapsed=$(tail -n1 <<< "${output}")
	sum_score=$((sum_score + score))
	printf "%8d\t${map}\t${elapsed}\n" "${score}"
done

printf "%8d\ttotal\n" "${sum_score}"
