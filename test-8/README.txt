# Overhaul - 20th Jan 2020

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

# Trying to Speed up Uniquification - 21st Jan 2020

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

# Boost::graph Isomorphism Testing - 22nd Jan 2020

uniqueAdjacencyMatrices3()

Since this does not rely on exhaustively checking permutations, but searches for an isomorphism, we can succesfully compute R(3,5) !

              Previously   Boost    Test-7(u=8)

R(3,5) =  14   too long     0.3!    5.7      seconds
R(3,6) =? 10   long        13       3.8
R(4,4) =?  8    0.6         1.4     3
R(4,4) =?  9  120          82       3

According to https://www.boost.org/doc/libs/1_72_0/libs/graph/doc/isomorphism.html
(notice link https://www.boost.org/doc/libs/1_72_0/libs/graph/doc/isomorphism-impl.pdf)
the implementation here is slow.
It also references a better algorithm.
I need to understand one of these algorithms and adopt it.

I hope my adjacency matrix representation is OK.
I don't want to implement a second representation - edge lists?
Full adjaceny matrices (non-triangular) might already work, if needed.

# My Own Isomorphism Testing - 23rd Jan 2020

uniqueAdjacencyMatrices4()

               This    Previously   Boost    Test-7(u=8)

R(3,5) =  14    0.2    too long     0.3     5.7      seconds
R(3,6) =? 10    7.2    long        13       3.8
R(4,4) =?  8    0.6     0.6         1.4     3
R(4,4) =?  9    33    120          82       3
R(4,4) =? 10  2300

Log of R(4,4) =? 10

    Ramsey(4,4)-graphs with 10 vertices
      Smaller Ramsey graphs:                            14,701
      New edges to fill:                                     9
      Possible combinations:                         7,526,912 # 14,701 * 2^9

      Check all colorings:                                   0.125 seconds # this can be improved
      Number of recursion steps:                     7,298,326
      Number of colorings checked:                   8,443,496
      Number of edge mask checks:                  107,139,220
      Non-unique Ramsey graphs:                      1,145,170

      Unique degree histograms:                            520
      Max graphs per degree histogram:                   4,674
      Graph combinations checked                 1,078,273,657
      Recursion steps                           19,272,400,811
      Permutation checks                        54,545,819,071

      Uniquify Ramsey graphs:                            2,296.312 seconds
      Ramsey graphs:                                   103,706

    Ramsey(4,4)-graphs with 10 vertices: 103,706
    Total time: 2,329.797 seconds

## Possible Optimizations

 - Transform recursion in isIsomorphic() into a stack-based iteration implementation
   Perf report looks very confusing with all the std container ?copying?.
   I tried passing permutation and hAvailableNodes as modifyable and avoid copying,
   but it somehow made the code slower.
   So I scrapped it for now.
   I would think though, that by removing the recursion and never copying the datastructures this should get somewhat faster. 2x?
 - Algorithmic improvements
   - Better pruning. Can you add more / use better properties than simple node degree comparisons?
   - Select higher likelyhood m's to map n to, earlier (But how)
