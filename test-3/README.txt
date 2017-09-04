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

    perf stat -e cycles,resource_stalls.any,instructions,cache-references,cache-misses,bus-cycles,branches,branch-misses,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,L1-icache-load-misses,LLC-load-misses,LLC-loads,LLC-store-misses,LLC-stores,branch-load-misses,branch-loads,dTLB-load-misses,dTLB-loads,dTLB-store-misses,dTLB-stores,iTLB-load-misses,iTLB-loads,node-load-misses,node-loads,node-store-misses,node-stores ./test-3 3 4 9 9
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

    perf stat -d ./test-3 3 4 9 9
      Performance counter stats for './test-3 3 4 9 9':
           24261,692852      task-clock (msec)         #    1,000 CPUs utilized
                     56      context-switches          #    0,002 K/sec
                      1      cpu-migrations            #    0,000 K/sec
                    128      page-faults               #    0,005 K/sec
         65.829.174.891      cycles                    #    2,713 GHz                      (50,00%)
        192.816.534.975      instructions              #    2,93  insn per cycle           (62,51%)
         41.895.604.206      branches                  # 1726,821 M/sec                    (62,51%)
            194.007.085      branch-misses             #    0,46% of all branches          (62,51%)
         66.023.517.257      L1-dcache-loads           # 2721,307 M/sec                    (62,51%)
              9.463.290      L1-dcache-load-misses     #    0,01% of all L1-dcache hits    (62,51%)
              1.705.641      LLC-loads                 #    0,070 M/sec                    (49,99%)
                668.036      LLC-load-misses           #   39,17% of all LL-cache hits     (50,01%)

R(4,4) > 10 takes 44 seconds

Possible Next steps:
- Go back to std::bitset
- Change the for loop over lastOne and lastZero to a table-based lookup for 'nextEdge'.
  This way we need to check for very few numbers, instead of iterating over all lastOnes and lastZeros

# 36dcdb522eef666b6d08029f9339cb5592b53a26 (4th September)

Switch to look-up-table-based implementation of lastOne / lastZero / nextEdge check,
so that we don't loop over all subgraph edge masks all the time.
Number of edge mask size checks (the old for loops) decreased by a factor of ~10.

R(3,4) =  9 takes 17 seconds (-19%)
R(4,4) > 10 takes 33 seconds (-25%)

    perf stat -e cycles,resource_stalls.any,instructions,cache-references,cache-misses,bus-cycles,branches,branch-misses,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,L1-icache-load-misses,LLC-load-misses,LLC-loads,LLC-store-misses,LLC-stores,branch-load-misses,branch-loads,dTLB-load-misses,dTLB-loads,dTLB-store-misses,dTLB-stores,iTLB-load-misses,iTLB-loads,node-load-misses,node-loads,node-store-misses,node-stores ./test-3 3 4 9 9
      Performance counter stats for './test-3 3 4 9 9':

         45.563.469.148      cycles                                                        (10,73%)
            480.045.724      resource_stalls.any                                           (14,32%)
        138.164.442.540      instructions              #    3,03  insn per cycle           (17,91%)
             10.059.451      cache-references                                              (17,92%)
              3.802.571      cache-misses              #   37,801 % of all cache refs      (17,94%)
            397.074.554      bus-cycles                                                    (17,95%)
         33.729.435.882      branches                                                      (17,94%)
            105.306.584      branch-misses             #    0,31% of all branches          (14,35%)
             11.047.997      L1-dcache-load-misses     #    0,03% of all L1-dcache hits    (14,35%)
         35.906.811.365      L1-dcache-loads                                               (14,34%)
         20.181.506.658      L1-dcache-stores                                              (14,36%)
              2.603.890      L1-icache-load-misses                                         (14,36%)
                518.817      LLC-load-misses           #   41,45% of all LL-cache hits     (14,36%)
              1.251.718      LLC-loads                                                     (14,36%)
                 68.515      LLC-store-misses                                              (7,16%)
                232.712      LLC-stores                                                    (7,16%)
            106.074.523      branch-load-misses                                            (10,74%)
         33.977.941.006      branch-loads                                                  (14,32%)
                 15.775      dTLB-load-misses          #    0,00% of all dTLB cache hits   (14,31%)
         36.253.887.962      dTLB-loads                                                    (14,31%)
                    370      dTLB-store-misses                                             (14,31%)
         20.394.834.745      dTLB-stores                                                   (14,30%)
                  3.748      iTLB-load-misses          #    0,87% of all iTLB cache hits   (14,30%)
                431.951      iTLB-loads                                                    (14,30%)
                      0      node-load-misses                                              (14,28%)
                492.799      node-loads                                                    (14,25%)
                      0      node-store-misses                                             (7,10%)
                 88.582      node-stores                                                   (7,14%)

           16,900016868 seconds time elapsed

    perf stat -d ./test-3 3 4 9 9
     Performance counter stats for './test-3 3 4 9 9':

           17072,695325      task-clock (msec)         #    0,999 CPUs utilized
                    129      context-switches          #    0,008 K/sec
                      1      cpu-migrations            #    0,000 K/sec
                    135      page-faults               #    0,008 K/sec
         46.284.186.160      cycles                    #    2,711 GHz                      (49,97%)
        139.537.763.310      instructions              #    3,01  insn per cycle           (62,52%)
         33.930.993.037      branches                  # 1987,442 M/sec                    (62,53%)
            105.142.076      branch-misses             #    0,31% of all branches          (62,54%)
         36.065.832.601      L1-dcache-loads           # 2112,486 M/sec                    (62,56%)
             12.052.759      L1-dcache-load-misses     #    0,03% of all L1-dcache hits    (62,52%)
              1.434.377      LLC-loads                 #    0,084 M/sec                    (49,97%)
                610.949      LLC-load-misses           #   42,59% of all LL-cache hits     (49,95%)

           17,084335991 seconds time elapsed
