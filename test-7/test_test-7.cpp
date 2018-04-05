#include "prereqs.hpp"

#include "check.hpp"

using std::cerr;
using std::endl;

using r5::AdjacencyMatrix;

TEST(Test7, hasCompleteOrEmptySubgraph) {

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
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 1, 1), true);
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 1, 2), true);
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 2, 1), true);
            }
            if (n >= 2) {
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 2, 2), true);
            }
            if (n >= 3) {
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 2, 3), true);
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 3, 2), true);
            }
            if (n >= 6) {
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 3, 3), true);
            }
            if (n >= 9) {
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 3, 4), true);
                ASSERT_EQ(hasCompleteOrEmptySubgraph(m, 4, 3), true);
            }
        }
    }
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
