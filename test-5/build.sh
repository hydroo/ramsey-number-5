#! /usr/bin/env bash

set -e

if [ $# -lt 3 ]; then
	echo "Usage: $1 <r> <s> <n>"
	echo "  where R(r, s) <= n will be answered"
	exit 0
fi

tmp_file=config.inc.new

cat << EOF >> $tmp_file
namespace config_inc {
   constexpr int r = $1;
   constexpr int s = $2;
   constexpr int n = $3;
}
EOF

if ! test -e config.inc || ! diff $tmp_file config.inc > /dev/null ; then
	echo "replaced config.inc"
	cp $tmp_file config.inc
else
	echo "did not replace config.inc"
fi

rm $tmp_file

echo "make"
make
