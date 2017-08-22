#! /usr/bin/env sh

./test-1 2 2 2>/dev/null
./test-1 2 3 2>/dev/null
./test-1 3 2 2>/dev/null
./test-1 3 3 2>/dev/null
./test-1 3 4 2>/dev/null # takes multiple minutes
./test-1 4 3 2>/dev/null # takes multiple minutes
