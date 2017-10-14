# initial test-5 commit (Oct 9th):

Same as test-3 except with bitsets instead of dynamic_bitsets,
(partially) arrays instead of vectors,
compile-time-fixing for r, s and n

R(3,4) =  9 takes  1.2 seconds (Down from 17 in test-3)
R(4,4) > 10 takes  1.5 seconds (Down from 33 in test-3)
R(4,4) > 11 takes 90   seconds

    $ ./build.sh 4 4 11  && perf stat -d ./test-5  
    did not replace config.inc
    make
    make: Nothing to be done for 'all'.
    Problem: R(4,4) <= 11 ? 
    Number of complete subgraphs:                       330   # n choose r
    Number of empty subgraphs:                          330   # n choose s
    Edges:                                               55   # n*(n-1)/2
    Edge colorings:                  36,028,797,018,963,968   # 2^e
    
    Timing: Create subgraph edge masks: 0.000667448 seconds
    Timing: Check all colorings:          90.6369 seconds
    Timing: Number of recursion steps:      1,693,082,835
    Timing: Number of colorings checked:      846,541,400
    Timing: Number edge mask size checks:  29,072,667,030
    R(4,4)  > 11 (FALSE)
    Counter example: 
        [1111111100
          111000011
           00110011
            0101011
             010111
              00111
               1011
                111
                 11
                  0]
    
    
     Performance counter stats for './test-5':
    
          90324,044350      task-clock (msec)         #    0,997 CPUs utilized          
                 8.353      context-switches          #    0,092 K/sec                  
                     5      cpu-migrations            #    0,000 K/sec                  
                   133      page-faults               #    0,001 K/sec                  
       248.120.361.644      cycles                    #    2,747 GHz                      (49,97%)
       452.030.902.081      instructions              #    1,82  insn per cycle           (62,46%)
        95.913.111.953      branches                  # 1061,878 M/sec                    (62,51%)
           333.765.524      branch-misses             #    0,35% of all branches          (62,53%)
       168.093.149.674      L1-dcache-loads           # 1861,001 M/sec                    (62,52%)
            18.673.369      L1-dcache-load-misses     #    0,01% of all L1-dcache hits    (62,51%)
             4.208.852      LLC-loads                 #    0,047 M/sec                    (49,98%)
             2.190.648      LLC-load-misses           #   52,05% of all LL-cache hits     (49,99%)


Comment: Sadly, I have to use vector<> for bitsets because the number of buckets for the maps is unknown at compiletime.
  Idea: Create the maps in a second program to be able to use std::arrays again in the main part.

# 0a1a5bff5a2e5b3c1dffabfde21afff4685d5a33 (replaced unordered_map with array, Oct 10th):

R(3,4) = 10 takes  6.0  seconds (Down from 9.1 ) (Summitdev)
R(4,4) > 10 takes  0.92 seconds (Down from 1.42) (Summitdev)
R(4,4) > 11 takes  60   seconds (Down from 75  ) (Summitdev)

# 6443f518f31cbeb19b9f308fa8b8e2a1ae7522e9 (small improvements, Oct 10th):

R(3,4) <= 10 takes 5.1  s  (Summitdev),    3.6  (Solaire)
R(4,4) >  10 takes 0.78 s  (Summitdev),    0.54 (Solaire)
R(4,4) >  11 takes 56.8 s  (Summitdev),   32.8  (Solaire)
R(3,4) <= 12 takes                       454    (Solaire)
R(4,4) >  12 takes                      2768    (Solaire)

I tried removing the inner ifs from the bitmask testing loops in favor of a bool variable that is repeatedly |='ed and gets evaluated later.
But this is ~20% slower.
This will probably be necessary for parallelization of these loops.
But on the other hand the loops are fairly short. Most of them very short, some are around ~30 iterations.

Next step is probably to break apart the recursion by either flattening it or transforming it into something iterative.

# switch to column major storing of adjacency matrices in bitsets (Oct 11th):

I noticed all of the edgeMasks*ByLast* maps have their first non-empty vector relatively late. As far back as 20 bits or more.
And this makes sense, considering we store graphs row-major.
With column-major a compact complete/empty n-graph can be in the first n bits, whereas it needs a lot more in column major.
This change brought a huge runtime performance improvement, because it prunes the tree that enumerates all graphs a lot earlier.

E.g.:

    Problem: R(3,4) <= 10 ? 
    Number of complete subgraphs:                       120   # n choose r
    Number of empty subgraphs:                          210   # n choose s
    Edges:                                               45   # n*(n-1)/2
    Edge colorings:                      35,184,372,088,832   # 2^e
    
    Timing: Create subgraph edge masks:              0.000206 seconds
    Complete edge masks by last 1 (last:vectorsize): {-1:0, 0:0, 1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0, 8:0, 9:1, 10:1, 11:1, 12:1, 13:1, 14:1, 15:1, 16:1, 17:2, 18:2, 19:2, 20:2, 21:2, 22:2, 23:2, 24:3, 25:3, 26:3, 27:3, 28:3, 29:3, 30:4, 31:4, 32:4, 33:4, 34:4, 35:5, 36:5, 37:5, 38:5, 39:6, 40:6, 41:6, 42:7, 43:7, 45:8}
    Empty edge masks by last 0    (last:vectorsize): {-1:0, 0:0, 1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0, 8:0, 9:0, 10:0, 11:0, 12:0, 13:0, 14:0, 15:0, 16:0, 17:1, 18:1, 19:1, 20:1, 21:1, 22:1, 23:1, 24:3, 25:3, 26:3, 27:3, 28:3, 29:3, 30:6, 31:6, 32:6, 33:6, 34:6, 35:10, 36:10, 37:10, 38:10, 39:15, 40:15, 41:15, 42:21, 43:21, 45:28}
    Timing: Check all colorings:          3.806234 seconds
    Timing: Number of recursion steps:       292,838,893
    Timing: Number of colorings checked:     146,419,447
    Timing: Number of edge mask checks:    2,561,364,122

is now:

    Problem: R(3,4) <= 10 ? 
    Number of complete subgraphs:                                        120   # n choose r
    Number of empty subgraphs:                                           210   # n choose s
    Edges:                                                                45   # n*(n-1)/2
    Edge colorings:                                       35,184,372,088,832   # 2^e
    
    Timing: Create subgraph edge masks:              0.000131 seconds
    Complete edge masks by last 1 (last:vectorsize): {-1:0, 0:0, 1:0, 2:1, 3:0, 4:1, 5:2, 6:0, 7:1, 8:2, 9:3, 10:0, 11:1, 12:2, 13:3, 14:4, 15:0, 16:1, 17:2, 18:3, 19:4, 20:5, 21:0, 22:1, 23:2, 24:3, 25:4, 26:5, 27:6, 28:0, 29:1, 30:2, 31:3, 32:4, 33:5, 34:6, 35:7, 36:0, 37:1, 38:2, 39:3, 40:4, 41:5, 42:6, 43:7, 45:8}
    Empty edge masks by last 0    (last:vectorsize): {-1:0, 0:0, 1:0, 2:0, 3:0, 4:0, 5:1, 6:0, 7:0, 8:1, 9:3, 10:0, 11:0, 12:1, 13:3, 14:6, 15:0, 16:0, 17:1, 18:3, 19:6, 20:10, 21:0, 22:0, 23:1, 24:3, 25:6, 26:10, 27:15, 28:0, 29:0, 30:1, 31:3, 32:6, 33:10, 34:15, 35:21, 36:0, 37:0, 38:1, 39:3, 40:6, 41:10, 42:15, 43:21, 45:28}
    Timing: Check all colorings:          0.034180 seconds
    Timing: Number of recursion steps:         2,540,751
    Timing: Number of colorings checked:       1,270,376
    Timing: Number of edge mask checks:       22,049,936

Notice:
- In the by last 0/1 maps the first non-zero entries are at lower keys, i.e. earlier in the recursion/tree.
- The number of recursion steps, colorings checked and edge masks checked is a lot lower.

R(3,4) <= 10 takes   0.04 s (Solaire)
R(4,4) >  13 takes   0.18 s (Solaire)
R(4,4) >  14 takes 312    s (Solaire)
R(3,5) >  13 takes   2.45 s (Solaire)
R(3,5) =  14 didn't finish in a few hours

One of the next task should be to verify some of these counter examples, because they are getting too large to just look at.

    R(4,4)  > 14 (FALSE)    R(3,5)  > 13 (FALSE)
    Counter example:        Counter example: 
        [1111110000000          [111100000000
          111001110000            00011100000
           00111101100             0010011000
            0111100011              001000110
             111011000               00010101
              00110110                0000110
               0011101                 010001
                001110                  11010
                 11001                   0000
                  0101                    101
                   011                     00
                    10                      1]
                     1]

# Possible Next Steps
- Verify some of the counter examples
- GPU
    - Transform recursive enumeration into iterative and find out howto best put it on a GPU.
- CPU
    - I read a bit of the assembly. It does not use simd, but does an interleaved and[0], cmp[0], ifneq, and[1], cmp[1], ifneq on the bitset. (The code itself does (a & b) == b per bitset/(128bit so far))
    - I'm reasonably sure I could write my own bitset and handcraft parts of the algorithm to speed it up considerably (2x or a lot more)
      https://software.intel.com/sites/landingpage/IntrinsicsGuide
      Perhaps I can do (a & b) == b in one instruction rather than two.
- What I didn't do is remove the inner ifs in the bitmask testing loops, because it gets slower on CPU. This should be done eventually for parallelism(ilp as well as tlp)
- If I could get rid of vector<> in std::array<std::vector<std::bitset<config::e>>, config::e + 1> edgeMasksCompleteByLastOne, that'd be great.
  Maybe template nextEdge in the local function, thus fixing the loop length.
  Might also be a terrible idea.
- Instead of having two loops for the search for complete and empty subgraphs, I might be able to put it into one big vector (and loop) by manipulating the masks cleverly. Maybe the operations need some tweaking then, too.

- The biggest speedups, of course, still lie in improving the enumeration / tree pruning / the overall algorithm.
  I.e. more pruning, and
  avoiding to check graphs that are isomorphic to something I checked or will definitely check later.
  Or perhaps replace the whole bitmask-based subgraph search which this test-5 is based on.
