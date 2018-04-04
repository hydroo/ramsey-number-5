#include "test-5-check.hpp"

#include <iostream>
#include <vector>

using std::cerr;
using std::endl;

using r5::AdjacencyMatrix;

s64 expectedResult(s64 r, s64 s) {
    R5_ASSERT(r >= 1);
    R5_ASSERT(s >= 1);

    s64 r_ = r;
    s64 s_ = s;
    r = std::min(r_, s_);
    s = std::max(r_, s_);

    if (r == 1) {
        return 1;
    } else if (r == 2) {
        return s;
    }

    if (r == 3) {
        const std::vector<s64> results{-1, -1, -1, 6, 9, 14, 18, 23, 28, 36};
        if (s >= s64(results.size()) - 1) {
            std::cerr << "I don't know R(" << r << "," << s << ")" << std::endl;
            return -1;
        } else {
            return results[s];
        }
    }

    if (r == 4) {
        const std::vector<s64> results{-1, -1, -1, -1, 18, 25};
        if (s >= s64(results.size()) - 1) {
            std::cerr << "I don't know R(" << r << "," << s << ")" << std::endl;
            return -1;
        } else {
            return results[s];
        }
    }

    std::cerr << "I don't know whether R(" << r << "," << s << ")" << std::endl;
    return -1;
}

void test_hasCompleteOrEmptySubgraph() {

    srand(time(NULL));

    for (int n = 1; n <= 9; n += 1) {
        AdjacencyMatrix<-1> m(n);

        for (int i = 0; i < 100; i += 1) {

            for (int e = 0; e < m.edges(); e += 1) {
                if (((bool) (rand() % 2)) == true) { m.setEdge(e);   }
                else                               { m.unsetEdge(e); }
            }

            // cerr << m.print() << endl;

            if (n >= 1) {
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 1, 1) == true);
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 1, 2) == true);
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 2, 1) == true);
            }
            if (n >= 2) {
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 2, 2) == true);
            }
            if (n >= 3) {
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 2, 3) == true);
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 3, 2) == true);
            }
            if (n >= 6) {
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 3, 3) == true);
            }
            if (n >= 9) {
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 3, 4) == true);
                R5_ASSERT(hasCompleteOrEmptySubgraph(m, 4, 3) == true);
            }
        }
    }
}
