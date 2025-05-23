# Note

Use `codesign --verbose --deep --force --sign - --options runtime --entitlements ../entitlements.plist test-8` to allow profiling the app on Mac

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

# Iteration Instead of Recursion for `isIsomorphic` - 24th Jan 2020

uniqueAdjacencyMatrices5()
Note, that I inlined `match()` and `isIsomorphic()`.
Might want to refactor that later.

Notably, hAvailableNodes moved to using a std::vector, from std::set.
That speed up things 2x.
With std::set, due to overhead in stack pushing and popping (3x the number of `recursionSteps` than the recursive implementation),
the iterative code was barely faster.

                   This    Previously    Previously2   Boost    Test-7(u=8)
    
    R(3,5) =  14     0.2      0.2          too long      0.3     5.7      seconds
    R(3,6) =? 10     3.6      7.2          long         13       3.8
    R(4,4) =?  8     0.4      0.6           0.6          1.4     3
    R(4,4) =?  9    16.6     33           120           82       3
    R(4,4) =? 10  1143     2300

## Next up

Fix the permutation for n if it has degree 0 or nodes-1, or a unique degree.
In these three cases, any assignment will yield the same result.
Our current DFS requires n to be iterated in order, which makes the above optimization weird.
I hope switching to a flexible set of possible n will make this not too slow.
I tried incorporating this optimization while keeping the current stack handling, but turned out awful.
   
# Customize Node Traversal Order And Fix Some Nodes - 26th Jan 2020

And preallocate the stack to avoid reallocation.
This saves ~15% time.

uniqueAdjacencyMatrices5()

Order:
 - Assign any node of degree 0 to any other of degree 0
 - Assign any node of degree nodes-1 to any other degree nodes-1
 - Assign nodes of a unique degree to the only possible option
 - Traverse the rest of the nodes by lowest degree multiplicity first.
   I.e. nodes with a degree that few other nodes have are traversed earlier than more common degrees.
   This slims down the traversal tree.
   Small fan-out first, bigger fan-out later.

    R(3,5) =  14    0.1  seconds
    R(3,6) =? 10    1.4
    R(4,4) =?  8    0.2
    R(4,4) =?  9    4.5
    R(4,4) =? 10  217.5

    Ramsey(4,4)-graphs with 10 vertices
      Smaller Ramsey graphs:                            14,701
      New edges to fill:                                     9
      Possible combinations:                         7,526,912 # 14,701 * 2^9
    
      Check all colorings:                                   0.118 seconds
      Number of recursion steps:                     7,298,326
      Number of colorings checked:                   8,443,496
      Number of edge mask checks:                  107,139,220
      Non-unique Ramsey graphs:                      1,145,170
    
      Average fixed nodes:                                   1.063
      Unique degree histograms:                            520
      Max graphs per degree histogram:                   4,674
      Graph combinations checked                 1,078,273,657
      Graph combinations requiring traversal       928,355,955 # !!
      Recursion steps                           12,329,634,075 # !! compare to above output
      Permutation checks                        10,083,114,223
    
      Uniquify Ramsey graphs:                              215.938 seconds
      Ramsey graphs:                                   103,706
    
    Ramsey(4,4)-graphs with 10 vertices: 103,706
    Total time: 220.167 seconds

# PProf Instead of Perf

Perf's output is not super-helpful right now.
The samples are scattered throughout the assembly - around 5% or less on a line.
And it does offer a proper call graph.
I don't know where some of the calls come from, and it doesn't say which asm lines correspond to which inlined function.
Pprof seems to do a better job.
But perhaps perf is that way, because it doesn't know for sure.

    LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so.0.4.18 CPUPROFILE=./pprof.out ./test-8

    google-pprof --text ./test-8 pprof.out

    google-pprof --gv ./test-8 pprof.out

    google-pprof --gv --lines ./test-8 pprof.out # very nice!

    google-pprof --callgrind ./test-8 pprof.out > test-8.callgrind
    kcachegrind test-8.callgrind

35% auto hAvailableNodes = std::get<1>(t) # Jesus!
    25% of which is std::array:array() -> std::vector::vector() copy constructing
    # This copying could be moved to after checking the definitely mapped nodes, which could save 10% of these 35%
    # You could try using a different data type, but I don't see anything too obvious here
    # Handling this custom could work, but I guess that's not worth just 35% speed, yet.
27% AdjacencyMatrix<nodes>::edge() of which 21% are in AdjacencyMatrixIndexer<nodes>::edge()
 6% erase+find (3.5%+2.0%) of hAvailableNodes
 5% edge testing loop header which belongs to the top 27%
 4% stack.emplace_back()
 3% stack.pop_back()


# Order Graphs Properly - 31st Jan 2020

This change properly orders graphs to make reasoning about extensions easier.

    1 11 111 -> 1 11 000 -> 0 10 100

Ordering by leftmost 1 first is good because extensions to earlier nodes are preferred over later ones.
E.g. 0 00 001 would smaller than 0 00 100, so it would come first in traversal and uniquification.
1 11 100 is more desirable to extend and check than 1 11 001.`

The degree histogram map messes with the ordering, and thus extensions and uniquification were not done in an intuitive order.

The goal is to skip some extensions to reduce the number of non-unique ramsey graphs before the uniquification step.
Another angle of attack would be to add more properties like the degree histogram to test fewer graphs for isomorphisms.

This sort decreases perf by 10% :(

# Don't Use Turan's Theorem

Update: This doesn't help, because traversing edge masks and checking them already takes care of this.
This check would not improve anything.
Maybe, for Test-7 on larger n it could save checking the edge masks and thus some time.

https://en.wikipedia.org/wiki/Tur%C3%A1n%27s_theorem

    e := (r-2)/(2*(r-1)) * n^2

A graph of size n with more than e edges has to contain a complete subgraph of size r.
In the ramsey graph searching world, the reverse (less than n*(n-1)/2 - e edges for empty graphs is also true).

Ramsey(4,4)-Graphs of size  8 have  7-21 edges. An extension would add 0-8.
Ramsey(4,4)-Graphs of size  9 have  9-27 edges. An extension would add 0-9.
Ramsey(4,4)-Graphs of size 10 have 12-33 edges.
So from 8 to 9 we can prune 7, 8, 28 and 29 edge graphs.
From 9 to 10 we can prune 9, 10, 11, 34, 35 and 36 edge graphs.
I don't know how much this can help.

The current stack-based extension enumeration is ill-suited for adding in this check to shortcut.
I'd need to redo it.
At the moment it wouldn't help much as this part contributes almost no time.
But in Test-7 for deeper searches it could be beneficial.

# Make hAvailableNodes Constant - 10th Nov 2022

This avoids copying/constructing/destroying this data structure and thus improves performance by 30-40%

    R(4,4) =?  9    2.7
    R(4,4) =? 10  129.5

# Specialize AdjacencyMatrix::edge() for One Element per AdjacencyMatrix - 11th Nov 2022

    R(4,4) =?  9    2.4
    R(4,4) =? 10  119.0
    R(4,4) =? 11 5276.1

# Add Triangle and Empty Triangle Count Histograms as Properties for Pre-Bucketing Potentially Isomorphic Graphs - 13th Nov 2022

                  i7-9750H   M2 Pro (9th Apr 2024)  (22nd Jun) (27th Jul) (9th Aug) (16th)  M4 Pro (13th Nov)
    R(4,4) =? 10     3.7           2.5                   2.7        0.9       0.9     0.9         0.6
    R(4,4) =? 11    30.5          24.2                  21.5        7.7       7.3     7.5         5.3
    R(4,4) =? 12   194.1         135.5                  89.6       38.5      36.7    37.8        26.4
    R(4,4) =? 13   410.9         264.2                 171.7       84.6      81.8    84.7        59.0
    R(4,4) == 18   473.9         300.2                 203.7      109.8     106.2   109.4        77.1

    R(3,6) =? 11                   1.6                   1.3        0.6       0.6     0.5         0.4
    R(3,6) =? 12                  16.2                   8.0        4.5       4.3     4.2         3.0
    R(3,6) =? 13                 118.1                  37.0       22.0      21.0    21.1        15.3
    R(3,6) =? 14                 280.9                  86.0       56.4      54.1    55.4        41.1
    R(3,6) == 18                 329.7                 111.2       76.8      74.6    75.3        57.6

    R(4,5) == 10                   52.2                  61.0      18.3      17.4    17.7        12.6
    R(4,5) == 11                                                 1644.4

    R(5,5) ==  9                   4.0                    4.5       1.4       1.2     1.2         0.9
    R(5,5) == 10                 245.8                  283.0      82.0      77.8    79.8        55.8

# Combine all Three Degree Types and Also Use Them to Inform Traversal - 22nd June 2024

Achieved a large reduction in key operations for uniqueness checking, via fewer collisions (more property buckets)
and up to 10x fewer traversal steps in the traversal stack.
Updated performance numbers in the previous section.

Unfortunately, the gNodesByDegree map in the value type of uniqueGraphs ate up a lot of potential upside (35%+ of current runtime).
Furthermore the large number of fixed nodes also incurs more permutation assignment upfront than before (20 of the above 35%+).
These two factors should account for most of the new downside.
Additionally, the map of uniqueGraphs itself costs 30%+ perf.

Apple Instruments is a bit finicky: ~20% of runtime is not accounted for, but likely burried in map-related functions (destruction of uniqueGraphs?).

So we basically traded a reduction of operations uniqueAdjacencyMatrices5 for "performance potential".
The good thing is, if these two maps are properly optimized the upside is 3-10x.
Hard to tell how much exactly.

# Better Containers - 27th July 2024

Introduced various improvements, including but not limited to boost containers, PackedUIntTuple, custom NodesByDegree.
Speedup is between ~30% (R(3,6)=18) and 3x for R(4,5) and R(5,5).
RAM savings are substantial, but unclear due to changes in calculation and lack of reliability of the printouts compared to Instruments Allocations.

R(4,5)!=11 is now possible in 1644s, yay! Before, we ran out of RAM on this one.

NodesByDegree.find() is the largest bottleneck with ~35%+ of execution time still. The search seems not vectorized.
We may want to pull out the multiplicity out of DegreeHistogramEntry as it seems to have unfortunate alignment and cost 12% of total time to extract.

In the future we have to address excessive RAM usage.
Since every extra node (I.e. going from R(4,5)!=10 to 11) costs ~100x in time and 10x+ in RAM usage, the current approach will not work any longer.
Bandaids would be to store some static/non-resident things on disk, but ultimately we have to partition the data further.
Pre-bucket graphs by edge count and process each bucket separately, etc. etc...
And then we likely have to create a depth-first hybrid that allocates a reasonable amount of RAM per level/nodecount and continues deeper with not-fully-deduplicated(isomorphism) sets of graphs.
Here, parallelization should come in finally.
This will trade off performance, unfortunately, but it seems unavoidable at this time. 

# Possible next steps - August 2024
- Fix NodesByDegree.dump()
- Fix permutation counting for logs by including the pre-testing?! Perhaps as a separate entry

- Maybe try to get rid of index array in NodesByDegree. See note in the code.
- Print size of complete and empty edge masks
- Look into reducing the pre-alloc for uniqueGraphs to never go over the RAM size.
  This leads to notable slowdowns (up to 8% total) as well as sometimes much higher RAM (3->4GB) usage.
  Still need to find out how to best do this.
  - A packed version of AdjacencyMatrixProperties could help speed up collision checks
- NodesByDegree is still the biggest bottleneck
  - SIMDify .find() - https://github.com/hydroo/ramsey-number-5/pull/4
- AdjacencyMatrixProperties could be denser if we packed the array. Would save some more RAM.
- Checkramseygraphcount.hpp: Find results for 5,5,n and confirm the 5,5,n additions from commit 25ceeae in test_test-8.cpp
- Could print out stats on graph bucketing distribution in uniqueGraphs (Verbose >= 2)
- Improve gProperties/gDegrees beyond edge and triangle degrees
  - Maybe entirely new properties like ?orbit lengths?
  - Maybe K_4+: Probably would use subGraphEdgeMasks() and some smarts to find the subgraphs, instead of the hardcoded degree+triangle+empty triangle stuff we do now
- Read, understand and implement the algorithm from geng - Applications of a technique for labelled enumeration (http://cs.anu.edu.au/~bdm/papers/LabelledEnumeration.pdf)
- Make the data type for Size flexible (smaller indexes could save storage/time)
  - In combination with SIMD-accelerated algorithms this becomes even more attractive
    Right now the hold-up here is that STL's handling of map.find() and tuple comparison are attrocious.
    But if you would improve that, it could enable benefiting from smaller a smaller Size-type / packing
- Make the magic value size max, instead of -1, so I can try unsigned ints for size
  - ?Throw out runtime-sized matrix code?
  - ?Throw out non-triangular matrix code?
- Add read/write/checkpointing facility in order to let this thing run day and night, and be able to continue working without starting from scratch
- SIMD-accelerated permutation application and comparison
- Parallelize on CPU
- Port to CUDA
- Eventually it will be necessary to combine graphs to form candidates for larger Ramsey graphs
- Maybe subGraphEdgeMasks can be used to construct counter examples. E.g. is it possible to generate a counter example for one of the unknowns in the Survey (page 4)