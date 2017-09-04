- Create all colorings with a DFS instead of binary counting
    - This way we can prune the tree

- Go back to using arguments instead of compile time foo

In the first implementation without pruning R(3,4) > 8 takes 6.5 seconds up from 0.8 in test-2 :(.
Not sure whether it's because of dynamic_bitset or the DFS enumeration.
It should be one of the two or a mix.

# 088a631028a46fd6ef55cdfd52ab97d62d358988 (4th September)

With simple pruning R(3,4) > 8 takes 0.01 seconds

R(3,4) = 9 takes 21 seconds (Wohoo, we can calculate this!)
   The number of checked colorings, ~15mio is very low considering it's taking 21 seconds.
   Test-2 (R(3,4)<=9) with fixed width bitsets checks ~60mio per second.
   There could be a speedup of >10 in going to std::bitset again
   Todo: Use a profiler to see where time is spent.

   Performance counter stats for './test-3 3 4 9 9':
  
      57.407.015.955      cycles                                                        (10,73%)
       1.275.419.328      resource_stalls.any                                           (14,31%)
     183.904.693.026      instructions              #    3,20  insn per cycle           (17,90%)
           4.383.294      cache-references                                              (17,91%)
           1.554.578      cache-misses              #   35,466 % of all cache refs      (17,93%)
         493.442.404      bus-cycles                                                    (17,93%)
      41.781.735.308      branches                                                      (17,93%)
         198.767.180      branch-misses             #    0,48% of all branches          (14,34%)
           4.666.471      L1-dcache-load-misses     #    0,01% of all L1-dcache hits    (14,34%)
      57.538.392.688      L1-dcache-loads                                               (14,34%)
      19.920.707.053      L1-dcache-stores                                              (14,33%)
           2.193.442      L1-icache-load-misses                                         (14,33%)
             158.303      LLC-load-misses           #   33,88% of all LL-cache hits     (14,33%)
             467.181      LLC-loads                                                     (14,32%)
              30.947      LLC-store-misses                                              (7,14%)
              73.695      LLC-stores                                                    (7,13%)
         198.693.416      branch-load-misses                                            (10,74%)
      41.719.630.940      branch-loads                                                  (14,31%)
             202.385      dTLB-load-misses          #    0,00% of all dTLB cache hits   (14,31%)
      57.402.106.101      dTLB-loads                                                    (14,31%)
                 196      dTLB-store-misses                                             (14,31%)
      19.903.384.308      dTLB-stores                                                   (14,30%)
               2.475      iTLB-load-misses          #    1,24% of all iTLB cache hits   (14,30%)
             199.339      iTLB-loads                                                    (14,30%)
                   0      node-load-misses                                              (14,29%)
             218.559      node-loads                                                    (14,29%)
                   0      node-store-misses                                             (7,14%)
              20.705      node-stores                                                   (7,14%)

R(4,4) > 10 takes 44 seconds

Possible Next steps:
- Profile
- Go back to std::bitset
- Change the for loop over lastOne and lastZero to a table-based lookup for 'nextEdge'.
  This way we need to check for very few numbers, instead of iterating over all lastOnes and lastZeros
