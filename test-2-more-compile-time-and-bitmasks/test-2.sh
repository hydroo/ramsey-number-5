#! /usr/bin/env bash

set -e

./build.sh 1 1 1 &>/dev/null && ./test-2 2>/dev/null
./build.sh 1 2 2 &>/dev/null && ./test-2 2>/dev/null
./build.sh 1 3 3 &>/dev/null && ./test-2 2>/dev/null
./build.sh 1 4 4 &>/dev/null && ./test-2 2>/dev/null

./build.sh 2 1 2 &>/dev/null && ./test-2 2>/dev/null
./build.sh 2 2 2 &>/dev/null && ./test-2 2>/dev/null
./build.sh 2 3 3 &>/dev/null && ./test-2 2>/dev/null
./build.sh 2 4 4 &>/dev/null && ./test-2 2>/dev/null

./build.sh 3 1 3 &>/dev/null && ./test-2 2>/dev/null
./build.sh 3 2 3 &>/dev/null && ./test-2 2>/dev/null
./build.sh 3 3 6 &>/dev/null && ./test-2 2>/dev/null
#./build.sh 3 4 9 &>/dev/null && ./test-2 2>/dev/null

./build.sh 4 1  4 &>/dev/null && ./test-2 2>/dev/null
./build.sh 4 2  4 &>/dev/null && ./test-2 2>/dev/null
#./build.sh 4 3  9 &>/dev/null && ./test-2 2>/dev/null
#./build.sh 4 4 18 &>/dev/null && ./test-2 2>/dev/null
