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
 - Add a checking table to the results to not only rely on test_test-8 for correctness
 - Speed up uniquification
 - Start porting to CUDA

# Trying to Speed up Uniquification 21st Jan 2020

You cannot only permute the last node with another.
All permutations seem to be needed (empirical).

Uniquification is a bit strange right now.
The adjacency matrix comparison operator's description is a bit wrong.
It might not make sense to continue using it instead of a simple < comparison.

Uniquification is faster than I thought.
For each extension we check whether or not the representation is canonical by checking all permutations
for each extension.
You could instead test isomorphism of the extensions, but for that you'd need to check all permutations
for all pairs of extensions.
That is slower (uniqueAdjacencyMatrices2).
If you could construct the isomorphism it might be faster.

I realized that uniqueAdjacencyMatrices() in Test 8 is very similar to uniqueAdjacencyMatrices() in Test 7.
In Test 7 it generates unique base graphs disregarding the R, S configuration.
In Test 8 it also filters complete/empty subgraphs and is thus faster.
u=8 went from 3.6 to 0.6 seconds, and u=9 from 2700 (old laptop) to 128 seconds.
So by integrating this change into Test 7 we could reach slightly better results, but it's not good enough by a long long shot.

And of course Test 7 remains faster than Test 8, because doing something with all permutations for n nodes is very slow.
11! = 40mio, 12! = 480mio permutations.

I need:
 ++ smart selection/generation of permutations
 o  faster generation of permutations than std::next_permutation
 +  faster application of permutations onto adjaceny matrices
