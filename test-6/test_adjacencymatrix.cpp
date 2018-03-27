#include "prereqs.hpp"

#include <gtest/gtest.h>

#include "adjacencymatrix.hpp"

using std::cerr;
using std::endl;

using r5::AdjacencyMatrix;

TEST(AdjacencyMatrix, create_constexpr_triangular) {
    constexpr AdjacencyMatrix<  0> m0;
        R5_STATIC_ASSERT(m0.nodes()        == 0);
        R5_STATIC_ASSERT(m0.edges()        == 0);
        R5_STATIC_ASSERT(m0.bits()         == 0);
        //R5_STATIC_ASSERT(m0.elements()     == 0); // is 1
    constexpr AdjacencyMatrix<  1> m1;
        R5_STATIC_ASSERT(m1.nodes()        == 1);
        R5_STATIC_ASSERT(m1.edges()        == 0);
        R5_STATIC_ASSERT(m1.bits()         == 0);
        // R5_STATIC_ASSERT(m1.elements()     == 0); // is 1
    constexpr AdjacencyMatrix<  2> m2;
        R5_STATIC_ASSERT(m2.nodes()        == 2);
        R5_STATIC_ASSERT(m2.edges()        == 1);
        R5_STATIC_ASSERT(m2.bits()         == 1);
        R5_STATIC_ASSERT(m2.elements()     == 1);
    constexpr AdjacencyMatrix<  3> m3;
        R5_STATIC_ASSERT(m3.nodes()        == 3);
        R5_STATIC_ASSERT(m3.edges()        == 3);
        R5_STATIC_ASSERT(m3.bits()         == 3);
        R5_STATIC_ASSERT(m3.elements()     == 1);
    constexpr AdjacencyMatrix<  4> m4;
        R5_STATIC_ASSERT(m4.nodes()        == 4);
        R5_STATIC_ASSERT(m4.edges()        == 6);
        R5_STATIC_ASSERT(m4.bits()         == 6);
        R5_STATIC_ASSERT(m4.elements()     == 1);
    constexpr AdjacencyMatrix< 11> m11;
        R5_STATIC_ASSERT(m11.nodes()        == 11);
        R5_STATIC_ASSERT(m11.edges()        == 55);
        R5_STATIC_ASSERT(m11.bits()         == 55);
        R5_STATIC_ASSERT(m11.elements()     == 1);
    constexpr AdjacencyMatrix< 12> m12;
        R5_STATIC_ASSERT(m12.elements()     == 2);
    constexpr AdjacencyMatrix< 16> m16;
        R5_STATIC_ASSERT(m16.elements()     == 2);
    constexpr AdjacencyMatrix< 17> m17;
        R5_STATIC_ASSERT(m17.elements()     == 3);
}

TEST(AdjacencyMatrix, create_constexpr_nontriangular) {
    constexpr AdjacencyMatrix<  1, false> m1;
        R5_STATIC_ASSERT(m1.bits()         == 1);
    constexpr AdjacencyMatrix<  2, false> m2;
        R5_STATIC_ASSERT(m2.bits()         == 4);
    constexpr AdjacencyMatrix<  3, false> m3;
        R5_STATIC_ASSERT(m3.bits()         == 9);
}

TEST(AdjacencyMatrix, create_nonconstexpr_triangular) {
    AdjacencyMatrix<-1> m4(4);
        ASSERT_EQ(m4.nodes()       , 4);
        ASSERT_EQ(m4.edges()       , 6);
        ASSERT_EQ(m4.bits()        , 6);
}

TEST(AdjacencyMatrix, create_nonconstexpr_nontriangular) {
    AdjacencyMatrix< -1, false> m4(4);
        ASSERT_EQ(m4.nodes()       , 4);
        ASSERT_EQ(m4.edges()       , 6);
        ASSERT_EQ(m4.bits()        , 16);
}

TEST(AdjacencyMatrix, setedge_constexpr) {
    constexpr AdjacencyMatrix<4> m1;
    R5_STATIC_ASSERT(m1.edge(3, 2) == false);

    constexpr auto m2 = []() -> AdjacencyMatrix<4> {
        AdjacencyMatrix<4> m;
        m.unsetEdge(3, 2);
        m.setEdge(3, 2);
        return m;
    }();
    R5_STATIC_ASSERT(m2.edge(3, 2) == true);

    constexpr AdjacencyMatrix<4, false> m3;
    R5_STATIC_ASSERT(m3.edge(3, 2) == false);

    constexpr auto m4 = []() -> AdjacencyMatrix<4, false> {
        AdjacencyMatrix<4, false> m;
        m.unsetEdge(3, 2);
        m.setEdge(3, 2);
        return m;
    }();
    R5_STATIC_ASSERT(m4.edge(3, 2) == true);
}

TEST(AdjacencyMatrix, setedge_nonconstexpr_triangular) {
    constexpr s64 nodes = 4;
    AdjacencyMatrix< -1> m(nodes);

    for (s64 c = 0; c < nodes; c += 1) {
        for (s64 r = 0; r < nodes; r += 1) {
            if (c == r) { continue; }
            m.unsetEdge(c, r);
            ASSERT_EQ(m.edge(c, r), false);
            ASSERT_EQ(m.edge(r, c), false);
            m.setEdge(c, r);

            // all previously set edges and this one ought to be set
            for (s64 c2 = 0; c2 <= c; c2 += 1) {
                for (s64 r2 = 0; r2 <= r; r2 += 1) {
                    if (c2 == r2) { continue; }
                    ASSERT_EQ(m.edge(c2, r2), true);
                    ASSERT_EQ(m.edge(r2, c2), true);
                }
            }

            // cerr << " c " << c << ", r " << r << ", " << m.print() << endl << m.print(true, "  ") << endl;
        }
    }
}

TEST(AdjacencyMatrix, setedge_nonconstexpr_nontriangular) {
    constexpr s64 nodes = 4;
    AdjacencyMatrix< -1, false> m(nodes);

    for (s64 c = 0; c < nodes; c += 1) {
        for (s64 r = 0; r < nodes; r += 1) {
            if (c == r) { continue; }
            m.unsetEdge(c, r);
            ASSERT_EQ(m.edge(c, r), false);
            ASSERT_EQ(m.edge(r, c), false);
            m.setEdge(c, r);

            // all previously set edges and this one ought to be set
            for (s64 c2 = 0; c2 <= c; c2 += 1) {
                for (s64 r2 = 0; r2 <= r; r2 += 1) {
                    if (c2 == r2) { continue; }
                    ASSERT_EQ(m.edge(c2, r2), true);
                    ASSERT_EQ(m.edge(r2, c2), true);
                }
            }

            // cerr << " c " << c << ", r " << r << ", " << m.print() << endl << m.print(true, "  ") << endl;
        }
    }
}

// // print() can't be constexpr because std::ostringstream and std::string
// TEST(AdjacencyMatrix, print_constexpr) {
//     constexpr AdjacencyMatrix<3> m1;
//     constexpr auto s1 = m1.print();
// 
//     constexpr AdjacencyMatrix<3, false> m2;
//     constexpr auto s2 = m2.print();
// }

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
