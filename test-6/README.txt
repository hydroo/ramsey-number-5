# 2nd April 2018

Implemented my own adjacency matrix class and adapted Test 5 to use it.
It is overall a bit faster. Around 20%, maybe.
Compiling can take very very long, if you insist on constexpr creation of some of the matrices.
E.g. R(4,4) =? 14 takes 8 seconds, and R(4,4) =? 18 takes 27 seconds.
Removing constexpr'ness does not slow down the code, since sorting the matrices by last one/zero is done at runtime anyways (due to use of std::vector).

My hope was that Test 5 New is not slower than Test 5.
And it came true.
So that's nice.
It's not important to explain the performance difference.
I had a look the assembler of Test 5 New, and it looks reasonable.

Some timings (compare to ../test-5/README.txt):

R(3,4) <= 10 takes   0.03 s (Solaire)
R(4,4) >  13 takes   0.15 s (Solaire)
R(4,4) >  14 takes 259    s (Solaire)
R(3,5) >  13 takes   2.30 s (Solaire)
R(3,5)  = 14 probably still takes too long

Example output:
    """
    Problem: R(4,4) <= 14 ? 
    Number of complete subgraphs:                                      1,001   # n choose r
    Number of empty subgraphs:                                         1,001   # n choose s
    Edges:                                                                91   # n*(n-1)/2
    Edge colorings:                    2,475,880,078,570,760,549,798,248,448   # 2^e

    Timing: Create subgraph edge masks:  0.002697 seconds
    Complete edge masks by last 1 (last:vectorsize): {-1:0, 0:0, 1:0, 2:0, 3:0, 4:0, 5:1, 6:0, 7:0, 8:1, 9:3, 10:0, 11:0, 12:1, 13:3, 14:6, 15:0, 16:0, 17:1, 18:3, 19:6, 20:10, 21:0, 22:0, 23:1, 24:3, 25:6, 26:10, 27:15, 28:0, 29:0, 30:1, 31:3, 32:6, 33:10, 34:15, 35:21, 36:0, 37:0, 38:1, 39:3, 40:6, 41:10, 42:15, 43:21, 44:28, 45:0, 46:0, 47:1, 48:3, 49:6, 50:10, 51:15, 52:21, 53:28, 54:36, 55:0, 56:0, 57:1, 58:3, 59:6, 60:10, 61:15, 62:21, 63:28, 64:36, 65:45, 66:0, 67:0, 68:1, 69:3, 70:6, 71:10, 72:15, 73:21, 74:28, 75:36, 76:45, 77:55, 78:0, 79:0, 80:1, 81:3, 82:6, 83:10, 84:15, 85:21, 86:28, 87:36, 88:45, 89:55, 91:66}
    Empty edge masks by last 0    (last:vectorsize): {-1:0, 0:0, 1:0, 2:0, 3:0, 4:0, 5:1, 6:0, 7:0, 8:1, 9:3, 10:0, 11:0, 12:1, 13:3, 14:6, 15:0, 16:0, 17:1, 18:3, 19:6, 20:10, 21:0, 22:0, 23:1, 24:3, 25:6, 26:10, 27:15, 28:0, 29:0, 30:1, 31:3, 32:6, 33:10, 34:15, 35:21, 36:0, 37:0, 38:1, 39:3, 40:6, 41:10, 42:15, 43:21, 44:28, 45:0, 46:0, 47:1, 48:3, 49:6, 50:10, 51:15, 52:21, 53:28, 54:36, 55:0, 56:0, 57:1, 58:3, 59:6, 60:10, 61:15, 62:21, 63:28, 64:36, 65:45, 66:0, 67:0, 68:1, 69:3, 70:6, 71:10, 72:15, 73:21, 74:28, 75:36, 76:45, 77:55, 78:0, 79:0, 80:1, 81:3, 82:6, 83:10, 84:15, 85:21, 86:28, 87:36, 88:45, 89:55, 91:66}
    Timing: Check all colorings:         269.807218 seconds # That is with verbose
    Timing: Number of recursion steps:     6,276,522,242
    Timing: Number of colorings checked:   3,138,261,097
    Timing: Number of edge mask checks:  316,016,724,048
    R(4,4)  > 14 (FALSE)
    Counter example: 
        1111110000000
         111001110000
          00111101100
           0111100011
            111011000
             00110110
              0011101
               001110
                11001
                 0101
                  011
                   10
                    1
    """

Perf stat confirms the improved efficiency:

    Test 5:
             328362,461041      task-clock (msec)         #    1,000 CPUs utilized
                     2.718      context-switches          #    0,008 K/sec
                        25      cpu-migrations            #    0,000 K/sec
                       159      page-faults               #    0,000 K/sec
           901.076.395.711      cycles                    #    2,744 GHz
         3.328.606.805.191      instructions              #    3,69  insn per cycle
           559.155.836.338      branches                  # 1702,862 M/sec
             2.838.156.512      branch-misses             #    0,51% of all branches

             328,476394587 seconds time elapsed

    Test 5 New:
             264160,205289      task-clock (msec)         #    1,000 CPUs utilized
                     1.371      context-switches          #    0,005 K/sec
                        13      cpu-migrations            #    0,000 K/sec
                       156      page-faults               #    0,001 K/sec
           719.334.209.744      cycles                    #    2,723 GHz
         2.913.518.584.309      instructions              #    4,05  insn per cycle
           569.996.449.712      branches                  # 2157,768 M/sec
             1.941.439.852      branch-misses             #    0,34% of all branches

             264,246273495 seconds time elapsed


# Possible Next Steps (Copied from Test 5)
- Output counterexamples / graphs with wildcards/do-not-care terms (because most counter examples are overspecified, and this is also helpful for reducing memory requirements when storing counterexamples in future versions)
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
