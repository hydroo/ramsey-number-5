#! /usr/bin/env bash

make clean && make T2_COLOR0=2 T2_COLOR1=2 T2_NODES=3 && ./test-2-*
