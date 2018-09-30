# Unique Base Graphs and Edge Mask Filtering (5th April)

Generate a list of graphs that are unique up to isomorphism.
Each of these unique base graphs serves a start for the usual DFS.
This introduces a new parameter `u` (aside from `n`, `r`, `s`) which specifies the node count of these base graphs.

    u = 6 takes     0.2 seconds at the moment (156 unique graphs)
    u = 7 takes   ~13   seconds at the moment (1044 unique graphs)
    u = 8 takes ~1760   seconds at the moment (12346 unique graphs)

With that we now filter the edge masks per unique base graph before we start the DFS.
For each unique base graph retain only the complete and empty edge masks that can result in positive hits.
I.e. if the base graph already contains bits that cause `(uniqueBase & mask) != mask` (or uniqueBase | mask) != mask), filter them out.

Going forward the unique base graphs are a natural starting point for parallelization, since the DFS of each is independent of the others.

    R(4,4) >  14 takes   <1 s (Solaire) (Down from 130s, u = 6)
    R(4,4) >  15 takes  14  s (Solaire) (Formerly impossible, u = 7)
    R(3,5) <= 14 takes  22  s (Solaire) (Formerly impossible, u = 7)

# Cleanup and faster generation of unique graphs

    u = 7 takes  0.4 seconds
    u = 8 takes 54   seconds

Now:

    R(4,4) >  15 takes  0.4 s (Solaire) (u = 7)
    R(4,4) >  16 takes 74   s (Solaire) (Formerly impossible, u = 8)
    R(3,5) <= 14 takes  8   s (Solaire) (u = 7)

Output of R(4,4) > 16:

    Problem: R(4,4) <= 16 ?
    Number of complete subgraphs:      1,820   # n choose r
    Number of empty subgraphs:         1,820   # n choose s
    Edges:                               120   # n*(n-1)/2
    Edge colorings:                    2^120   # 2^e
    Unique base graphs node count:         8

    Create unique base graphs:                                 53.820 seconds
    Number of unique base graphs:                          12,346
    Max recursion depth                                        92
    Max recursion steps per unique base graph                2^92
    Max recursion steps                              12346 * 2^92
    Create subgraph edge masks:                                 0.006 seconds
    Filter, sort subgraph edge masks:                           min    0.000, avg    0.000, max    0.000, sum    0.258 seconds
    Check all colorings:                                        min    0.000, avg    0.002, max   20.215, sum   20.215 seconds
    Filter, Sort, Check colorings (real):                      20.477 seconds
    Number of recursion steps:                      1,116,369,299
    Number of colorings checked:                    1,116,370,268
    Number of edge mask checks:                    14,008,628,132
    Total time:                                                74.303 seconds

    R(4,4)  > 16 (FALSE)
    Counter example:
        010111011110000
         11001011101100
          0001011000011
           110010110011
            10010011100
             0001101010
              000110101
               11111111
                0000110
                 011010
                  01001
                   0101
                    100
                     00
                      1

# Slightly faster generation of unique graphs (11th April)

The normal operator< causes extensions of smaller unique graphs
to sometimes get new smaller representations.
E.g. extending 1 00 to 1 00 001, 1 00 001 is larger than 0 01 100.
Because of this, in order to find the canonical representation of an extension,
I had to find the smallest representation of it.

With the new operator<

    bool compareLessThan(const u64* v1, const u64* v2, s64 nodes) {
        for (s64 i = 0; i < elements(nodes); i += 1) {
            if (v1[i] != v2[i]) {
                return (bool) ((v1[i]>>(__builtin_ffsll(v1[i] ^ v2[i]) - 1))&0x1);
            }
        }
        return false;
    }

smallest representations are always extensions of smaller unique graphs.
Now, instead of finding the smallest representation for each extension,
I check whether the current representation is the smallest.
If there is a smaller one, I can abort immediately.

The performance improvement here is sadly just ~10% for u=7 and 8.
The comparison itself is slightly slower due to the check for equality.

This change alters the choice of unique base graphs, and thus influences when a counter example gets found.
I.e. some queries (as shown below) got slower.

    u = 6 takes    0.006 seconds
    u = 7 takes   .4     seconds
    u = 8 takes 47       seconds

    R(4,4) >  15 takes    8   s (u = 7) ( 0.4 before) (Solaire)
    R(4,4) >  16 takes 4460   s (u = 8) (74   before) (Solaire)
    R(3,5) <= 14 takes    8   s (u = 7) ( 8   before) (Solaire)

I also tried filtering unique base graphs according to the complete and empty edge masks.
The idea is that a unique base graph that contains a complete subgraph of size r or an empty subgraph of size s
does not need to be considered further since it is already a hit.
Thus the technique lowers the number of unique base graphs and speeds up generating them, too, or so I thought.
It does not speed up the main loop and DFS since they would discard these unique base graphs immediately, too.

The code ended up a bit ugly, because filtering got mixed into uniqueAdjacencyMatrices().
BUT it was not faster at all (or perhaps just a tiny bit for u=9),
although the number of unique graphs for e.g. 8 nodes went down to ~2000 from 12345.
After some investigation, I'm still not sure why it is not faster.
Since there was no improvement and it made the code uglier, I left it on branch filter-unique-base-graphs.

# Faster Generation of Unique Graphs (30th September 2018)

1)
Do not store all permuted graphs, and don't compute them during the traversal.
Instead, compute each permutation of the DFS leaf's graph one after another.
Each time see whether or not the permuted graph is smaller than the original.
If so the original graph is not canonical.
This cuts down the number of computed permuted graphs, since it stops as soon as we know the original graph is not canonical.

2)
Switch from node-based permutation representation to edge-based.
This speeds up applying permutations.

    u = 7 takes   .2 seconds
    u = 8 takes 24   seconds

u = 9 is still far out of reach.
30x DFS leaves and 10x the number of permutations -> somewhere around ???x to 300x the work.

# Possible Next Steps

- (++) Instead of exiting upon the first encountered counter example (Ramsey Graph r, s, n),
       collect all of them.
       Count them and verify that count against the known numbers, e.g. here:
       `https://users.cecs.anu.edu.au/~bdm/data/ramsey.html`
       Store them and make them readable/available for later algorithms.
       E.g. to use them as a starting point for extensions to higher node counts.
       (The performance comparison is also kind of bad right now, since it depends on when the first
       Ramsey Graph is found. That time changes when I fiddle with the details of the algorithms,
       order of the graphs etc. making fair comparison impossible)

- (++) Speed up generating unique base graphs:
       This is clearly possibly as `geng` (part of nauty) demonstrates.
       I already fiddled with that a bit but didn't make any quick progress.
       `https://github.com/rmanders/unlabeled-graphical-enumeration/blob/master/graphs/orderly.py`'s functions
       (especially unlabeled_complement) could serve as a basis.
       `geng`'s source code is hard to read.
       Perhaps I could copy and refactor it until I understand what it does.

- (+ ) Filter complete and empty masks in the main DFS as it progresses.
       The same is already done for each unique base graphs, but is then
       never repeated.
       Not sure how much this can speed things up, but having fewer masks to check in the DFS would probably be good.

- (++) (Vague) It would be great if I could put both edge mask checks into one loop with a unified check.
       Bit twiddling.

- (++) (Vague) It would be great if I could leverage symmetry and/or simple deduplication (isomorphism) somehow.
       E.g. via complement of the to-be-checked graph.

- (-) Parallelize generating unique base graphs
- (-) Parallelize the main loop
