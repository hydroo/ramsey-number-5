#! /usr/bin/env bash

set -e

if [ $# -lt 3 ]; then
    echo "Usage: $1 <r> <s> <n>"
    echo "  where R(r, s) <= n will be answered"
    exit 0
fi

source_dir=".."
config_inc=$source_dir/config.inc
tmp_file=$config_inc.new

cat << EOF >> $tmp_file
namespace config_inc {
   constexpr s64 r = $1;
   constexpr s64 s = $2;
   constexpr s64 n = $3;
}
EOF

if ! test -e $config_inc || ! diff $tmp_file $config_inc > /dev/null ; then
    echo "replaced $config_inc"
    cp $tmp_file $config_inc
else
    echo "did not replace config.inc"
fi

rm $tmp_file

echo "make test-7"
make test-7
