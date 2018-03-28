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

TEST(AdjacencyMatrix, copyconstruct) {
    constexpr auto ct = []() -> auto {
        AdjacencyMatrix<5> m;
        m.setEdge(2, 0);
        m.setEdge(1, 3);
        return m;
    }();

    constexpr auto cn = []() -> auto {
        AdjacencyMatrix<5, false> m;
        m.setEdge(2, 0);
        m.setEdge(1, 3);
        return m;
    }();

    AdjacencyMatrix<5> nct;
    nct.setEdge(2, 0);
    nct.setEdge(1, 3);

    AdjacencyMatrix<5, false> ncn;
    ncn.setEdge(2, 0);
    ncn.setEdge(1, 3);

    AdjacencyMatrix<-1> rt(5);
    rt.setEdge(2, 0);
    rt.setEdge(1, 3);

    AdjacencyMatrix<-1, false> rn(5);
    rn.setEdge(2, 0);
    rn.setEdge(1, 3);

    // from compile-time triangular (ct)
    constexpr AdjacencyMatrix< 5>        ct_from_ct(ct);
        R5_STATIC_ASSERT(ct_from_ct.edge(2, 0) == true);
        R5_STATIC_ASSERT(ct_from_ct.edge(1, 3) == true);
    AdjacencyMatrix          < 5>        nct_from_ct(ct);
        ASSERT_EQ(nct_from_ct.edge(2, 0), true);
        ASSERT_EQ(nct_from_ct.edge(1, 3), true);
    AdjacencyMatrix          <-1>        rt_from_ct(ct);
        ASSERT_EQ(rt_from_ct.edge(2, 0), true);
        ASSERT_EQ(rt_from_ct.edge(1, 3), true);

    // from compile-time non-triangular (cn)
    constexpr AdjacencyMatrix< 5, false> cn_from_cn(cn);
        R5_STATIC_ASSERT(cn_from_cn.edge(2, 0) == true);
        R5_STATIC_ASSERT(cn_from_cn.edge(0, 2) == true);
        R5_STATIC_ASSERT(cn_from_cn.edge(3, 1) == true);
        R5_STATIC_ASSERT(cn_from_cn.edge(1, 3) == true);
    AdjacencyMatrix          < 5, false> ncn_from_cn(cn);
        ASSERT_EQ(ncn_from_cn.edge(2, 0), true);
        ASSERT_EQ(ncn_from_cn.edge(1, 3), true);
    AdjacencyMatrix          <-1, false> rn_from_cn(cn);
        ASSERT_EQ(rn_from_cn.edge(2, 0), true);
        ASSERT_EQ(rn_from_cn.edge(1, 3), true);

    // from non-compile-time triangular (nct)
    AdjacencyMatrix< 5> nct_from_nct(nct);
        ASSERT_EQ(nct_from_nct.edge(2, 0), true);
        ASSERT_EQ(nct_from_nct.edge(1, 3), true);
    AdjacencyMatrix<-1> rt_from_nct(nct);
        ASSERT_EQ(rt_from_nct.edge(2, 0), true);
        ASSERT_EQ(rt_from_nct.edge(1, 3), true);
    // constexpr AdjacencyMatrix<5> ct_from_nct(nct); // Compilation error: Cannot create constexpr from non-constexpr

    // from non-compile-time non-triangular (ncn)
    AdjacencyMatrix< 5, false> ncn_from_ncn(ncn);
        ASSERT_EQ(ncn_from_ncn.edge(2, 0), true);
        ASSERT_EQ(ncn_from_ncn.edge(1, 3), true);
    AdjacencyMatrix<-1, false> rn_from_ncn(ncn);
        ASSERT_EQ(rn_from_ncn.edge(2, 0), true);
        ASSERT_EQ(rn_from_ncn.edge(1, 3), true);
    // constexpr AdjacencyMatrix<5, false> ct_from_ncn(ncn); // Compilation error: Cannot create constexpr from non-constexpr

    // from runtime triangular (rt)
    AdjacencyMatrix<-1> rt_from_rt(rt);
        ASSERT_EQ(rt_from_rt.edge(2, 0), true);
        ASSERT_EQ(rt_from_rt.edge(1, 3), true);
    AdjacencyMatrix<5> nct_from_rt(rt);
        ASSERT_EQ(nct_from_rt.edge(2, 0), true);
        ASSERT_EQ(nct_from_rt.edge(1, 3), true);
    // constexpr AdjacencyMatrix<5> ct_from_rt(rt);   // Compilation error: Cannot create constexpr from runtime
    //           AdjacencyMatrix<4> nct2_from_rt(rt); // Runtime error: Wrong node count

    // from runtime non-triangular (rn)
    AdjacencyMatrix<-1, false> rn_from_rn(rn);
        ASSERT_EQ(rn_from_rn.edge(2, 0), true);
        ASSERT_EQ(rn_from_rn.edge(0, 2), true);
        ASSERT_EQ(rn_from_rn.edge(1, 3), true);
    AdjacencyMatrix<5, false> ncn_from_rn(rn);
        ASSERT_EQ(ncn_from_rn.edge(2, 0), true);
        ASSERT_EQ(ncn_from_rn.edge(1, 3), true);
    // constexpr AdjacencyMatrix<5, false> ct_from_rt(rt);   // Compilation error: Cannot create constexpr from runtime
    //           AdjacencyMatrix<4, false> ncn2_from_rt(rn); // Runtime error: Wrong node count
}

TEST(AdjacencyMatrix, assign) {
    constexpr auto ct = []() -> auto {
        AdjacencyMatrix<5> m;
        m.setEdge(1, 3);
        return m;
    }();

    constexpr auto cn = []() -> auto {
        AdjacencyMatrix<5, false> m;
        m.setEdge(1, 3);
        return m;
    }();

    AdjacencyMatrix<5> nct;
    nct.setEdge(1, 3);

    AdjacencyMatrix<5, false> ncn;
    ncn.setEdge(1, 3);

    AdjacencyMatrix<-1> rt(5);
    rt.setEdge(1, 3);

    AdjacencyMatrix<-1, false> rn(5);
    rn.setEdge(1, 3);

    AdjacencyMatrix< 5>        nct_from_ct    ;
    AdjacencyMatrix<-1>        rt_from_ct  (0);
    AdjacencyMatrix< 5, false> ncn_from_cn    ;
    AdjacencyMatrix<-1, false> rn_from_cn  (0);
    AdjacencyMatrix< 5>        nct_from_nct   ;
    AdjacencyMatrix<-1>        rt_from_nct (0);
    AdjacencyMatrix< 5, false> ncn_from_ncn   ;
    AdjacencyMatrix<-1, false> rn_from_ncn (0);
    AdjacencyMatrix<-1>        rt_from_rt  (0);
    AdjacencyMatrix< 5>        nct_from_rt    ;
    AdjacencyMatrix<-1, false> rn_from_rn  (0);
    AdjacencyMatrix< 5, false> ncn_from_rn    ;

    // from compile-time triangular (ct)
    constexpr AdjacencyMatrix<5> ct_from_ct = ct;
        R5_STATIC_ASSERT(ct_from_ct.edge(1, 3) == true);
    nct_from_ct = ct;
        ASSERT_EQ(nct_from_ct.edge(1, 3), true);
    rt_from_ct  = ct;
        ASSERT_EQ(rt_from_ct.edge(1, 3), true);

    // from compile-time non-triangular (cn)
    constexpr AdjacencyMatrix<5, false> cn_from_cn = cn;
        R5_STATIC_ASSERT(cn_from_cn.edge(3, 1) == true);
        R5_STATIC_ASSERT(cn_from_cn.edge(1, 3) == true);
    ncn_from_cn = cn;
        ASSERT_EQ(ncn_from_cn.edge(1, 3), true);
    rn_from_cn = cn;
        ASSERT_EQ(rn_from_cn.edge(1, 3), true);

    // from non-compile-time triangular (nct)
    nct_from_nct = nct;
        ASSERT_EQ(nct_from_nct.edge(1, 3), true);
    rt_from_nct = nct;
        ASSERT_EQ(rt_from_nct.edge(1, 3), true);

    // from non-compile-time non-triangular (ncn)
    ncn_from_ncn = ncn;
        ASSERT_EQ(ncn_from_ncn.edge(1, 3), true);
    rn_from_ncn = ncn;
        ASSERT_EQ(rn_from_ncn.edge(1, 3), true);

    // from runtime triangular (rt)
    rt_from_rt = rt;
        ASSERT_EQ(rt_from_rt.edge(1, 3), true);
    nct_from_rt = rt;
        ASSERT_EQ(nct_from_rt.edge(1, 3), true);
    // AdjacencyMatrix<4> nct2_from_rt; // Runtime error: Wrong node count
    // nct2_from_rt = rt;

    // from runtime non-triangular (rn)
    rn_from_rn = rn;
        ASSERT_EQ(rn_from_rn.edge(1, 3), true);
    ncn_from_rn = rn;
        ASSERT_EQ(ncn_from_rn.edge(1, 3), true);
    // AdjacencyMatrix<4, false> ncn2_from_rt; // Runtime error: Wrong node count
    // ncn2_from_rt = rn;
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
