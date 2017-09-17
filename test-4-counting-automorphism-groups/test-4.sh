#! /usr/bin/env bash

# geng is party of nauty http://pallini.di.uniroma1.it/
geng="./geng"

echo "Number of automorphism groups for graphs of size n"
echo ""
echo "n=nodes, e=edges (n*(n-1)/2), c=colorings (2^e), a=automorphism groups"
for n in $(seq 11); do
	e=$(echo "$n*($n-1)/2" | bc)
	c=$(echo "2^$e" | bc)

	a=$($geng -u $n 2>&1 | tail -n 1 | sed "s/^>Z //g" | sed "s/ graphs gen.*$//g")

	c_div_a=$(echo "$c / $a" | bc)

	printf "n %2s, e %2s, c %17s, a %11s, c/a %1s\n" $n $e $c $a $c_div_a
done
