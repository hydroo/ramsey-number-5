# Plan (5th April)

1)
Generate a list of graphs that are unique up to isomorphism first.
For each of these graphs start the usual DFS and complete the graph up until the edge count.

Since this is expensive we will limit this to very small graphs, say 6 nodes or we'll see how large.

The cool thing is that we can use this list as a basis for parallelization.
I.e. for each base graph, in parallel, independently (1) filter edge masks (2) execute the DFS.

2)
Filter the list of edge masks to check.
Remove all items that will never result in a hit because in the lower bits there are already mismatches.
I probably want to do this for each unique (up to isomorphism) base graph before kicking of the DFS.
