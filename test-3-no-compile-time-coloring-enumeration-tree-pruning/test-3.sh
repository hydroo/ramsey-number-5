#! /usr/bin/env bash

set -e

./test-3 1
./test-3 1 2
./test-3 1 3
./test-3 1 4

./test-3 2 1
./test-3 3 1
./test-3 4 1


./test-3 2
./test-3 2 3
./test-3 2 4

./test-3 3 2
./test-3 4 2


./test-3 3
./test-3 3 4

./test-3 4 3


#./test-3 4 4
