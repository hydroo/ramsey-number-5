#! /usr/bin/env bash

set -e

args="
    4,4,9
    4,4,10
    4,4,11
    4,4,12
    4,4,13
    4,4,18

    3,6,11
    3,6,12
    3,6,13
    3,6,14
    3,6,18

    4,5,10

    5,5,9
    5,5,10
"

date=$(date "+%y%m%d")

for rsn in $args; do
    IFS=","; set -- $rsn
    r=$1
    s=$2
    n=$3
    printf "%1d %1d %2d" $r $s $n
    build_log=$(./build.sh $r $s $n 2>&1)
    log=$(./test-8 2>&1)
    sleep 0.4
    echo "$log" > "$date-r-$r-$s-$n.log"
    total_time="$(echo "$log" | tail -n 1 | sed "s/Total time: //g" | sed "s/ seconds//g")"
    printf " %7.3f s\n" $total_time
done
