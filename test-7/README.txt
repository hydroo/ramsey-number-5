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
