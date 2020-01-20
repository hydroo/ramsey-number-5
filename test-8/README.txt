# Overhaul 20th Jan 2020

Create Ramsey graphs directly, instead of the test-7 method.

Recap: Test 7 created a set of unique base graphs for efficiency.
Then enumerated all possible edge coloring extensions via DFS for each new node added.
To find a counter example it iterates through possible complete and empty subgraphs
until the to-be-tested graph ANDed by the mask equals the mask.
Some efficiency is gained by only checking complete/empty subgraphs that regard the currently added edge
and does only test edges in the unique base graph, which would've been already checked.
This approach only yields one counter example, which was not a great idea, but it's reasonable if you only care about
whether or not R(r,s)=n is true or not.
With all it's optimizations we got results up to n=16.

Test 8 recursively creates all Ramsey graphs of size n-1, extends them by one node,
goes through all possible new edge colorings of that extension,
and ANDs complete/empty subgraphs to them.
Afterwards the graphs are reduced to unique-up-to-isomorphism.

So far it's slow, and only goes up to 10 nodes total, because uniquification is very expensive.
The number of permutations checked to establish canonicity is way too high.
I marked some possible improvements as TODOs.

Expected number of Ramsey graphs is taken from https://users.cecs.anu.edu.au/~bdm/data/ramsey.html and https://en.wikipedia.org/wiki/Ramsey%27s_theorem .

## Next
 - Clean up
 - Maybe rename variables and functions to make everything more homogeneous (maybe rename adjancencymatrix to graph)
 - Add a checking table to the results to not only rely on test_test-8 for correctness
 - Speed up uniquification
 - Start porting to CUDA
