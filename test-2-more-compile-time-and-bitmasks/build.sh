#! /usr/bin/env bash

if [ $# -lt 3 ]; then
	echo "Usage: $1 <r> <s> <n>"
	echo "  where R(r, s) <= n will be answered"
fi

tmp_file=config.hpp.new

(
	echo "constexpr int color0SubgraphSize = $1;"
	echo "constexpr int color1SubgraphSize = $2;"
	echo "constexpr int nodes =              $3;"
)>$tmp_file

if ! diff $tmp_file config.hpp > /dev/null ; then
	echo "replaced config.hpp"
	mv $tmp_file config.hpp
else
	echo "did not replace config.hpp"
	rm $tmp_file
fi

echo "make"
make
