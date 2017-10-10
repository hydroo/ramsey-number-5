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

R(3,4) <= 10 takes   5.1  s  (Summitdev),    3.6  (Solaire)
R(4,4) >  10 takes   0.78 s  (Summitdev),    0.54 (Solaire)
R(4,4) >  11 takes   56.8 s  (Summitdev),   32.8  (Solaire)
R(3,4) <= 12 takes                         454    (Solaire)
R(4,4) >  12 takes                        2768    (Solaire)

I tried removing the inner ifs from the bitmask testing loops in favor of a bool variable that is repeatedly |='ed and gets evaluated later.
But this is ~20% slower.
This will probably be necessary for parallelization of these loops.
But on the other hand the loops are fairly short. Most of them very short, some are around ~30 iterations.

Next step is probably to break apart the recursion by either flattening it or transforming it into something iterative.
