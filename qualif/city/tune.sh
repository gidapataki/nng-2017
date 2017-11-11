#!/usr/bin/env bash

function run() {
	./bin/grandprix -exec build/city -seed 1337 -novis 2>/dev/null
}

best_result=7000000

for forward in $(seq 0.5 0.1 1.5); do
	export CITY_FORWARD_WEIGHT="$forward"
	for side in $(seq 0.5 0.1 1.5); do
		export CITY_SIDE_WEIGHT="$side"
		for rear in $(seq 0.5 0.1 1.5); do
			export CITY_BACK_WEIGHT="$rear"
			result=$(run)
			if [ "$result" -lt "$best_result" ]; then
				best_result="$result"
				best_forward="$CITY_FORWARD_WEIGHT"
				best_side="$CITY_SIDE_WEIGHT"
				best_rear="$CITY_BACK_WEIGHT"
				echo "New best result:" "$best_result" "$best_forward" "$best_side" "$best_rear" >&2
			fi
		done
	done
done
