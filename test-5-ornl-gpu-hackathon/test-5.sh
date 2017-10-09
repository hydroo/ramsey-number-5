#! /usr/bin/env bash

set -e

for rs in "1 1" "1 2" "1 3" "1 4" \
	"2 2" "2 3" "2 4" "2 5" \
	"3 3" "3 4"; do

	n=$(echo $rs | awk '{print $2;}')
	while true; do

		echo "R($rs) <= $n ?"

		./build.sh $rs $n &> /dev/null

		ret=$(./test-5 2>&1 || true)

		if echo $ret | grep -q TRUE; then
			break
		elif ! echo $ret | grep -q FALSE; then
			echo >&2 "ERROR"
			echo >&2 "$ret"
		fi

		n=$(($n+1))

	done

	echo "R($rs)  = $n !"

done
