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
