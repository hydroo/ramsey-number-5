#include "prereqs.hpp"

#include "check.hpp"
#include "unique.hpp"

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

TEST(Test7, uniqueAdjacencyMatrices) {

    auto v1 = uniqueAdjacencyMatrices<1>();
    ASSERT_EQ(v1.size(), 1);
    ASSERT_EQ(v1[0].nodes(), 1);
    ASSERT_EQ(v1[0].edges(), 0);


    AdjacencyMatrix<2> m0; m0.unsetEdge(0);
    AdjacencyMatrix<2> m1; m1.setEdge(0);

    auto v2 = uniqueAdjacencyMatrices<2>();
    ASSERT_EQ(v2.size(), 2);
    ASSERT_TRUE((v2[0] == m0 && v2[1] == m1) || (v2[0] == m1 && v2[1] == m0));

    auto v3 = uniqueAdjacencyMatrices<3>();
    ASSERT_EQ(v3.size(), 4);

    auto v4 = uniqueAdjacencyMatrices<4>();
    ASSERT_EQ(v4.size(), 11);

    auto v5 = uniqueAdjacencyMatrices<5>();
    ASSERT_EQ(v5.size(), 34);

    auto v6 = uniqueAdjacencyMatrices<6>();
    ASSERT_EQ(v6.size(), 156);

    auto v7 = uniqueAdjacencyMatrices<7>();
    ASSERT_EQ(v7.size(), 1044);
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
