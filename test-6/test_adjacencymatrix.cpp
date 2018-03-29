#include "prereqs.hpp"

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

TEST(AdjacencyMatrix, setAllEdges) {

    constexpr s64 n = 12;

    constexpr auto uct = []() -> auto {
        AdjacencyMatrix<3> m;
        m.unsetAllEdges();
        return m;
    }();

    constexpr auto sct = []() -> auto {
        AdjacencyMatrix<3> m;
        m.setAllEdges();
        return m;
    }();

    constexpr auto ucn = []() -> auto {
        AdjacencyMatrix<3, false> m;
        m.unsetAllEdges();
        return m;
    }();

    constexpr auto scn = []() -> auto {
        AdjacencyMatrix<3, false> m;
        m.setAllEdges();
        return m;
    }();

    R5_STATIC_ASSERT(uct.edge(1, 0) == false);
    R5_STATIC_ASSERT(uct.edge(2, 0) == false);
    R5_STATIC_ASSERT(uct.edge(2, 1) == false);

    R5_STATIC_ASSERT(sct.edge(1, 0) == true);
    R5_STATIC_ASSERT(sct.edge(2, 0) == true);
    R5_STATIC_ASSERT(sct.edge(2, 1) == true);

    R5_STATIC_ASSERT(ucn.edge(1, 0) == false);
    R5_STATIC_ASSERT(ucn.edge(2, 0) == false);
    R5_STATIC_ASSERT(ucn.edge(2, 1) == false);

    R5_STATIC_ASSERT(scn.edge(1, 0) == true);
    R5_STATIC_ASSERT(scn.edge(2, 0) == true);
    R5_STATIC_ASSERT(scn.edge(2, 1) == true);

    AdjacencyMatrix<n>         unct;   unct.unsetAllEdges();
    AdjacencyMatrix<n>         snct;   snct.setAllEdges();
    AdjacencyMatrix<n, false>  uncn;   uncn.unsetAllEdges();
    AdjacencyMatrix<n, false>  sncn;   sncn.setAllEdges();
    AdjacencyMatrix<-1>        urt(n); urt.unsetAllEdges();
    AdjacencyMatrix<-1>        srt(n); srt.setAllEdges();
    AdjacencyMatrix<-1, false> urn(n); urn.unsetAllEdges();
    AdjacencyMatrix<-1, false> srn(n); srn.setAllEdges();

    auto testAllEdges = [n](auto m, bool test) {
        for (s64 c = 0; c < n; c += 1) {
            for (s64 r = 0; r < n; r += 1) {
                if (c == r) { continue; }
                ASSERT_EQ(m.edge(c, r), test);
            }
        }
    };

    testAllEdges(unct, false);
    testAllEdges(snct, true );
    testAllEdges(uncn, false);
    testAllEdges(sncn, true );
    testAllEdges(urt , false);
    testAllEdges(srt , true );
    testAllEdges(urn , false);
    testAllEdges(srn , true );
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
    ASSERT_THROW2(AdjacencyMatrix<4> nct2_from_rt(rt)); // Wrong node count

    // from runtime non-triangular (rn)
    AdjacencyMatrix<-1, false> rn_from_rn(rn);
        ASSERT_EQ(rn_from_rn.edge(2, 0), true);
        ASSERT_EQ(rn_from_rn.edge(0, 2), true);
        ASSERT_EQ(rn_from_rn.edge(1, 3), true);
    AdjacencyMatrix<5, false> ncn_from_rn(rn);
        ASSERT_EQ(ncn_from_rn.edge(2, 0), true);
        ASSERT_EQ(ncn_from_rn.edge(1, 3), true);
    // constexpr AdjacencyMatrix<5, false> ct_from_rt(rt);   // Compilation error: Cannot create constexpr from runtime
    using AdjacencyMatrix4false = AdjacencyMatrix<4, false>;
    ASSERT_THROW2(AdjacencyMatrix4false ncn2_from_rt(rn)); // Wrong node count
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
    AdjacencyMatrix<4> nct2_from_rt;
    ASSERT_THROW2(nct2_from_rt = rt); // Wrong node count

    // from runtime non-triangular (rn)
    rn_from_rn = rn;
        ASSERT_EQ(rn_from_rn.edge(1, 3), true);
    ncn_from_rn = rn;
        ASSERT_EQ(ncn_from_rn.edge(1, 3), true);
    AdjacencyMatrix<4, false> ncn2_from_rt;
    ASSERT_THROW2(ncn2_from_rt = rn); // Wrong node count
}

TEST(AdjacencyMatrix, bitwiseOr) {
    constexpr auto ct1 = []() -> auto {
        AdjacencyMatrix<6> m;
        m.unsetAllEdges();
        m.setEdge(1, 0);
        return m;
    }();
    constexpr auto ct2 = []() -> auto {
        AdjacencyMatrix<6> m;
        m.unsetAllEdges();
        m.setEdge(3, 5);
        return m;
    }();

    constexpr auto cn1 = []() -> auto {
        AdjacencyMatrix<6, false> m;
        m.unsetAllEdges();
        m.setEdge(1, 0);
        return m;
    }();

    constexpr auto cn2 = []() -> auto {
        AdjacencyMatrix<6, false> m;
        m.unsetAllEdges();
        m.setEdge(3, 5);
        return m;
    }();

    AdjacencyMatrix<6>         nct1;   nct1.unsetAllEdges(); nct1.setEdge(1, 0);
    AdjacencyMatrix<6>         nct2;   nct2.unsetAllEdges(); nct2.setEdge(3, 5);

    AdjacencyMatrix<6, false>  ncn1;   ncn1.unsetAllEdges(); ncn1.setEdge(1, 0);
    AdjacencyMatrix<6, false>  ncn2;   ncn2.unsetAllEdges(); ncn2.setEdge(3, 5);

    AdjacencyMatrix<-1>        rt1(6); rt1.unsetAllEdges(); rt1.setEdge(1, 0);
    AdjacencyMatrix<-1>        rt2(6); rt2.unsetAllEdges(); rt2.setEdge(3, 5);

    AdjacencyMatrix<-1, false> rn1(6); rn1.unsetAllEdges(); rn1.setEdge(1, 0);
    AdjacencyMatrix<-1, false> rn2(6); rn2.unsetAllEdges(); rn2.setEdge(3, 5);

    // ct | ct, ct | nct, ct | rt
    constexpr auto ct_or_ct = ct1 | ct2;
        R5_STATIC_ASSERT(ct_or_ct.edge(1, 0) == true);
        R5_STATIC_ASSERT(ct_or_ct.edge(5, 3) == true);
    // constexpr auto ct_or_nct2 = ct1 | nct2; // Compilation error
    auto ct_or_nct = ct1 | nct2;
        ASSERT_EQ(ct_or_nct.edge(1, 0), true);
        ASSERT_EQ(ct_or_nct.edge(5, 3), true);
    // constexpr auto ct_or_rt3 = ct1 | rt2; // Compilation error
    AdjacencyMatrix<6> ct_or_rt = ct1 | rt2;
        ASSERT_EQ(ct_or_rt.edge(1, 0), true);
        ASSERT_EQ(ct_or_rt.edge(5, 3), true);
    AdjacencyMatrix<-1> ct_or_rt2 = ct1 | rt2;
        ASSERT_EQ(ct_or_rt2.edge(1, 0), true);
        ASSERT_EQ(ct_or_rt2.edge(5, 3), true);

    // cn | cn, cn | ncn, cn | rn
    constexpr auto cn_or_cn = cn1 | cn2;
        R5_STATIC_ASSERT(cn_or_cn.edge(1, 0) == true);
        R5_STATIC_ASSERT(cn_or_cn.edge(5, 3) == true);
    // constexpr auto cn_or_ncn2 = cn1 | ncn2; // Compilation error
    auto cn_or_ncn = cn1 | ncn2;
        ASSERT_EQ(cn_or_ncn.edge(1, 0), true);
        ASSERT_EQ(cn_or_ncn.edge(5, 3), true);
    // constexpr auto cn_or_rn3 = cn1 | rn2; // Compilation error
    AdjacencyMatrix<6, false> cn_or_rn = cn1 | rn2;
        ASSERT_EQ(cn_or_rn.edge(1, 0), true);
        ASSERT_EQ(cn_or_rn.edge(5, 3), true);
    AdjacencyMatrix<-1, false> cn_or_rn2 = cn1 | rn2;
        ASSERT_EQ(cn_or_rn2.edge(1, 0), true);
        ASSERT_EQ(cn_or_rn2.edge(5, 3), true);

    // nct | ct, nct | nct, nct | rt
    // constexpr auto nct_or_ct   = nct1 | ct2;  // Compilation error
    // constexpr auto nct_or_nct2 = nct1 | nct2; // Compilation error
    auto nct_or_nct = nct1 | nct2;
        ASSERT_EQ(nct_or_nct.edge(1, 0), true);
        ASSERT_EQ(nct_or_nct.edge(5, 3), true);
    // constexpr auto nct_or_rt3 = nct1 | rt2; // Compilation error
    AdjacencyMatrix<6> nct_or_rt = nct1 | rt2;
        ASSERT_EQ(nct_or_rt.edge(1, 0), true);
        ASSERT_EQ(nct_or_rt.edge(5, 3), true);
    AdjacencyMatrix<-1> nct_or_rt2 = nct1 | rt2;
        ASSERT_EQ(nct_or_rt2.edge(1, 0), true);
        ASSERT_EQ(nct_or_rt2.edge(5, 3), true);

    // ncn | ct, ncn | ncn, ncn | rn
    // constexpr auto ncn_or_cn   = ncn1 | cn2;  // Compilation error
    // constexpr auto ncn_or_ncn2 = ncn1 | ncn2; // Compilation error
    auto ncn_or_ncn = ncn1 | ncn2;
        ASSERT_EQ(ncn_or_ncn.edge(1, 0), true);
        ASSERT_EQ(ncn_or_ncn.edge(5, 3), true);
    // constexpr auto ncn_or_rn3 = ncn1 | rn2; // Compilation error
    AdjacencyMatrix<6, false> ncn_or_rn = ncn1 | rn2;
        ASSERT_EQ(ncn_or_rn.edge(1, 0), true);
        ASSERT_EQ(ncn_or_rn.edge(5, 3), true);
    AdjacencyMatrix<-1, false> ncn_or_rn2 = ncn1 | rn2;
        ASSERT_EQ(ncn_or_rn2.edge(1, 0), true);
        ASSERT_EQ(ncn_or_rn2.edge(5, 3), true);

    // rt | ct, rt | nct, rt | rt
    // constexpr auto rt_or_ct   = rt1 | ct2;  // Compilation error
    // constexpr auto rt_or_nct3 = rt1 | nct2; // Compilation error
    AdjacencyMatrix<6> rt_or_nct = rt1 | nct2;
        ASSERT_EQ(rt_or_nct.edge(1, 0), true);
        ASSERT_EQ(rt_or_nct.edge(5, 3), true);
    AdjacencyMatrix<-1> rt_or_nct2 = rt1 | nct2;
        ASSERT_EQ(rt_or_nct2.edge(1, 0), true);
        ASSERT_EQ(rt_or_nct2.edge(5, 3), true);
    // constexpr auto rt_or_rt3 = rt1 | rt2; // Compilation error
    AdjacencyMatrix<6> rt_or_rt = rt1 | rt2;
        ASSERT_EQ(rt_or_rt.edge(1, 0), true);
        ASSERT_EQ(rt_or_rt.edge(5, 3), true);
    AdjacencyMatrix<-1> rt_or_rt2 = rt1 | rt2;
        ASSERT_EQ(rt_or_rt2.edge(1, 0), true);
        ASSERT_EQ(rt_or_rt2.edge(5, 3), true);

    // rn | cn, rn | ncn, rn | rn
    // constexpr auto rn_or_cn   = rn1 | cn2;  // Compilation error
    // constexpr auto rn_or_ncn3 = rn1 | ncn2; // Compilation error
    AdjacencyMatrix<6, false> rn_or_ncn = rn1 | ncn2;
        ASSERT_EQ(rn_or_ncn.edge(1, 0), true);
        ASSERT_EQ(rn_or_ncn.edge(5, 3), true);
    AdjacencyMatrix<-1, false> rn_or_ncn2 = rn1 | ncn2;
        ASSERT_EQ(rn_or_ncn2.edge(1, 0), true);
        ASSERT_EQ(rn_or_ncn2.edge(5, 3), true);
    // constexpr auto rn_or_rn3 = rn1 | rn2; // Compilation error
    AdjacencyMatrix<6, false> rn_or_rn = rn1 | rn2;
        ASSERT_EQ(rn_or_rn.edge(1, 0), true);
        ASSERT_EQ(rn_or_rn.edge(5, 3), true);
    AdjacencyMatrix<-1, false> rn_or_rn2 = rn1 | rn2;
        ASSERT_EQ(rn_or_rn2.edge(1, 0), true);
        ASSERT_EQ(rn_or_rn2.edge(5, 3), true);

    // *** make sure matrices of wrong node counts can't interact ***

    constexpr AdjacencyMatrix<1>        wct1;
    constexpr AdjacencyMatrix<2>        wct2;
    constexpr AdjacencyMatrix<1, false> wcn1;
    constexpr AdjacencyMatrix<2, false> wcn2;
    AdjacencyMatrix<1>                  wnct1;
    AdjacencyMatrix<2>                  wnct2;
    AdjacencyMatrix<1, false>           wncn1;
    AdjacencyMatrix<2, false>           wncn2;
    AdjacencyMatrix<-1>                 wrt1(1);
    AdjacencyMatrix<-1>                 wrt2(2);
    AdjacencyMatrix<-1, false>          wrn1(1);
    AdjacencyMatrix<-1, false>          wrn2(2);

    // auto wct_or_wct  = wct1 | wct2;  // Compilation error
    // auto wct_or_wnct = wct1 | wnct2; // Compilation error
    ASSERT_THROW2(wct1 | wrt2);

    // auto wcn_or_wcn  = wcn1 | wcn2;  // Compilation error
    // auto wcn_or_wncn = wcn1 | wncn2; // Compilation error
    ASSERT_THROW2(wcn1 | wrn2);

    // auto wnct_or_wct  = wnct1 | wct2;  // Compilation error
    // auto wnct_or_wnct = wnct1 | wnct2; // Compilation error
    ASSERT_THROW2(wnct1 | wrt2);

    // auto wncn_or_wcn  = wncn1 | wcn2;  // Compilation error
    // auto wncn_or_wncn = wncn1 | wncn2; // Compilation error
    ASSERT_THROW2(wncn1 | wrn2);

    ASSERT_THROW2(wrt1 | wct2);
    ASSERT_THROW2(wrt1 | wnct2);
    ASSERT_THROW2(wrt1 | wrt2);

    ASSERT_THROW2(wrn1 | wcn2);
    ASSERT_THROW2(wrn1 | wncn2);
    ASSERT_THROW2(wrn1 | wrn2);
}

// Pretty much a copy of binaryOr, except slight differences in the choosen edges and assertions
TEST(AdjacencyMatrix, bitwiseAnd) {
    constexpr auto ct1 = []() -> auto {
        AdjacencyMatrix<7> m;
        m.unsetAllEdges();
        m.setEdge(1, 0);
        m.setEdge(4, 5);
        return m;
    }();
    constexpr auto ct2 = []() -> auto {
        AdjacencyMatrix<7> m;
        m.unsetAllEdges();
        m.setEdge(4, 5);
        return m;
    }();

    constexpr auto cn1 = []() -> auto {
        AdjacencyMatrix<7, false> m;
        m.unsetAllEdges();
        m.setEdge(1, 0);
        m.setEdge(4, 5);
        return m;
    }();

    constexpr auto cn2 = []() -> auto {
        AdjacencyMatrix<7, false> m;
        m.unsetAllEdges();
        m.setEdge(4, 5);
        return m;
    }();

    AdjacencyMatrix<7>         nct1;   nct1.unsetAllEdges(); nct1.setEdge(4, 5); nct1.setEdge(1, 0);
    AdjacencyMatrix<7>         nct2;   nct2.unsetAllEdges(); nct2.setEdge(4, 5);

    AdjacencyMatrix<7, false>  ncn1;   ncn1.unsetAllEdges(); ncn1.setEdge(4, 5); ncn1.setEdge(1, 0);
    AdjacencyMatrix<7, false>  ncn2;   ncn2.unsetAllEdges(); ncn2.setEdge(4, 5);

    AdjacencyMatrix<-1>        rt1(7); rt1.unsetAllEdges();  rt1.setEdge(4, 5);  rt1.setEdge(1, 0);
    AdjacencyMatrix<-1>        rt2(7); rt2.unsetAllEdges();  rt2.setEdge(4, 5);

    AdjacencyMatrix<-1, false> rn1(7); rn1.unsetAllEdges();  rn1.setEdge(4, 5);  rn1.setEdge(1, 0);
    AdjacencyMatrix<-1, false> rn2(7); rn2.unsetAllEdges();  rn2.setEdge(4, 5);

    // ct & ct, ct & nct, ct & rt
    constexpr auto ct_and_ct = ct1 & ct2;
        R5_STATIC_ASSERT(ct_and_ct.edge(1, 0) == false);
        R5_STATIC_ASSERT(ct_and_ct.edge(5, 4) == true);
    // constexpr auto ct_and_nct2 = ct1 & nct2; // Compilation error
    auto ct_and_nct = ct1 & nct2;
        ASSERT_EQ(ct_and_nct.edge(1, 0), false);
        ASSERT_EQ(ct_and_nct.edge(5, 4), true);
    // constexpr auto ct_and_rt3 = ct1 & rt2; // Compilation error
    AdjacencyMatrix<7> ct_and_rt = ct1 & rt2;
        ASSERT_EQ(ct_and_rt.edge(1, 0), false);
        ASSERT_EQ(ct_and_rt.edge(5, 4), true);
    AdjacencyMatrix<-1> ct_and_rt2 = ct1 & rt2;
        ASSERT_EQ(ct_and_rt2.edge(1, 0), false);
        ASSERT_EQ(ct_and_rt2.edge(5, 4), true);

    // cn & cn, cn & ncn, cn & rn
    constexpr auto cn_and_cn = cn1 & cn2;
        R5_STATIC_ASSERT(cn_and_cn.edge(1, 0) == false);
        R5_STATIC_ASSERT(cn_and_cn.edge(5, 4) == true);
    // constexpr auto cn_and_ncn2 = cn1 & ncn2; // Compilation error
    auto cn_and_ncn = cn1 & ncn2;
        ASSERT_EQ(cn_and_ncn.edge(1, 0), false);
        ASSERT_EQ(cn_and_ncn.edge(5, 4), true);
    // constexpr auto cn_and_rn3 = cn1 & rn2; // Compilation error
    AdjacencyMatrix<7, false> cn_and_rn = cn1 & rn2;
        ASSERT_EQ(cn_and_rn.edge(1, 0), false);
        ASSERT_EQ(cn_and_rn.edge(5, 4), true);
    AdjacencyMatrix<-1, false> cn_and_rn2 = cn1 & rn2;
        ASSERT_EQ(cn_and_rn2.edge(1, 0), false);
        ASSERT_EQ(cn_and_rn2.edge(5, 4), true);

    // nct & ct, nct & nct, nct & rt
    // constexpr auto nct_and_ct   = nct1 & ct2;  // Compilation error
    // constexpr auto nct_and_nct2 = nct1 & nct2; // Compilation error
    auto nct_and_nct = nct1 & nct2;
        ASSERT_EQ(nct_and_nct.edge(1, 0), false);
        ASSERT_EQ(nct_and_nct.edge(5, 4), true);
    // constexpr auto nct_and_rt3 = nct1 & rt2; // Compilation error
    AdjacencyMatrix<7> nct_and_rt = nct1 & rt2;
        ASSERT_EQ(nct_and_rt.edge(1, 0), false);
        ASSERT_EQ(nct_and_rt.edge(5, 4), true);
    AdjacencyMatrix<-1> nct_and_rt2 = nct1 & rt2;
        ASSERT_EQ(nct_and_rt2.edge(1, 0), false);
        ASSERT_EQ(nct_and_rt2.edge(5, 4), true);

    // ncn & ct, ncn & ncn, ncn & rn
    // constexpr auto ncn_and_cn   = ncn1 & cn2;  // Compilation error
    // constexpr auto ncn_and_ncn2 = ncn1 & ncn2; // Compilation error
    auto ncn_and_ncn = ncn1 & ncn2;
        ASSERT_EQ(ncn_and_ncn.edge(1, 0), false);
        ASSERT_EQ(ncn_and_ncn.edge(5, 4), true);
    // constexpr auto ncn_and_rn3 = ncn1 & rn2; // Compilation error
    AdjacencyMatrix<7, false> ncn_and_rn = ncn1 & rn2;
        ASSERT_EQ(ncn_and_rn.edge(1, 0), false);
        ASSERT_EQ(ncn_and_rn.edge(5, 4), true);
    AdjacencyMatrix<-1, false> ncn_and_rn2 = ncn1 & rn2;
        ASSERT_EQ(ncn_and_rn2.edge(1, 0), false);
        ASSERT_EQ(ncn_and_rn2.edge(5, 4), true);

    // rt & ct, rt & nct, rt & rt
    // constexpr auto rt_and_ct   = rt1 & ct2;  // Compilation error
    // constexpr auto rt_and_nct3 = rt1 & nct2; // Compilation error
    AdjacencyMatrix<7> rt_and_nct = rt1 & nct2;
        ASSERT_EQ(rt_and_nct.edge(1, 0), false);
        ASSERT_EQ(rt_and_nct.edge(5, 4), true);
    AdjacencyMatrix<-1> rt_and_nct2 = rt1 & nct2;
        ASSERT_EQ(rt_and_nct2.edge(1, 0), false);
        ASSERT_EQ(rt_and_nct2.edge(5, 4), true);
    // constexpr auto rt_and_rt3 = rt1 & rt2; // Compilation error
    AdjacencyMatrix<7> rt_and_rt = rt1 & rt2;
        ASSERT_EQ(rt_and_rt.edge(1, 0), false);
        ASSERT_EQ(rt_and_rt.edge(5, 4), true);
    AdjacencyMatrix<-1> rt_and_rt2 = rt1 & rt2;
        ASSERT_EQ(rt_and_rt2.edge(1, 0), false);
        ASSERT_EQ(rt_and_rt2.edge(5, 4), true);

    // // rn & cn, rn & ncn, rn & rn
    // constexpr auto rn_and_cn   = rn1 & cn2;  // Compilation error
    // constexpr auto rn_and_ncn3 = rn1 & ncn2; // Compilation error
    AdjacencyMatrix<7, false> rn_and_ncn = rn1 & ncn2;
        ASSERT_EQ(rn_and_ncn.edge(1, 0), false);
        ASSERT_EQ(rn_and_ncn.edge(5, 4), true);
    AdjacencyMatrix<-1, false> rn_and_ncn2 = rn1 & ncn2;
        ASSERT_EQ(rn_and_ncn2.edge(1, 0), false);
        ASSERT_EQ(rn_and_ncn2.edge(5, 4), true);
    // constexpr auto rn_and_rn3 = rn1 & rn2; // Compilation error
    AdjacencyMatrix<7, false> rn_and_rn = rn1 & rn2;
        ASSERT_EQ(rn_and_rn.edge(1, 0), false);
        ASSERT_EQ(rn_and_rn.edge(5, 4), true);
    AdjacencyMatrix<-1, false> rn_and_rn2 = rn1 & rn2;
        ASSERT_EQ(rn_and_rn2.edge(1, 0), false);
        ASSERT_EQ(rn_and_rn2.edge(5, 4), true);

    // *** make sure matrices of wrong node counts can't interact ***

    constexpr AdjacencyMatrix<1>        wct1;
    constexpr AdjacencyMatrix<2>        wct2;
    constexpr AdjacencyMatrix<1, false> wcn1;
    constexpr AdjacencyMatrix<2, false> wcn2;
    AdjacencyMatrix<1>                  wnct1;
    AdjacencyMatrix<2>                  wnct2;
    AdjacencyMatrix<1, false>           wncn1;
    AdjacencyMatrix<2, false>           wncn2;
    AdjacencyMatrix<-1>                 wrt1(1);
    AdjacencyMatrix<-1>                 wrt2(2);
    AdjacencyMatrix<-1, false>          wrn1(1);
    AdjacencyMatrix<-1, false>          wrn2(2);

    // auto wct_and_wct  = wct1 & wct2;  // Compilation error
    // auto wct_and_wnct = wct1 & wnct2; // Compilation error
    ASSERT_THROW2(wct1 & wrt2);

    // auto wcn_and_wcn  = wcn1 & wcn2;  // Compilation error
    // auto wcn_and_wncn = wcn1 & wncn2; // Compilation error
    ASSERT_THROW2(wcn1 & wrn2);

    // auto wnct_and_wct  = wnct1 & wct2;  // Compilation error
    // auto wnct_and_wnct = wnct1 & wnct2; // Compilation error
    ASSERT_THROW2(wnct1 & wrt2);

    // auto wncn_and_wcn  = wncn1 & wcn2;  // Compilation error
    // auto wncn_and_wncn = wncn1 & wncn2; // Compilation error
    ASSERT_THROW2(wncn1 & wrn2);

    ASSERT_THROW2(wrt1 & wct2);
    ASSERT_THROW2(wrt1 & wnct2);
    ASSERT_THROW2(wrt1 & wrt2);

    ASSERT_THROW2(wrn1 & wcn2);
    ASSERT_THROW2(wrn1 & wncn2);
    ASSERT_THROW2(wrn1 & wrn2);
}

TEST(AdjacencyMatrix, bitwiseComplement) {
    constexpr s64 n = 17; // 3 (triangular) or 5 (non-triangular) elements

    constexpr auto ct = []() -> auto {
        AdjacencyMatrix<n> m;
        m.unsetAllEdges();
        m.setEdge(4, 14);
        return m;
    }();
    constexpr auto cn = []() -> auto {
        AdjacencyMatrix<n, false> m;
        m.unsetAllEdges();
        m.setEdge(4, 14);
        return m;
    }();

    AdjacencyMatrix<n>         nct;   nct.unsetAllEdges(); nct.setEdge(4, 14);
    AdjacencyMatrix<n, false>  ncn;   ncn.unsetAllEdges(); ncn.setEdge(4, 14);
    AdjacencyMatrix<-1>        rt(n); rt.unsetAllEdges();  rt.setEdge(4, 14);
    AdjacencyMatrix<-1, false> rn(n); rn.unsetAllEdges();  rn.setEdge(4, 14);

    auto testAllEdges = [n](auto m) {
        for (s64 c = 0; c < n; c += 1) {
            for (s64 r = 0; r < n; r += 1) {
                if (c == r) { continue; }
                if ((c == 4 && r == 14) || (c == 14 && r == 4)) {
                    ASSERT_EQ(m.edge(c, r), false);
                } else {
                    ASSERT_EQ(m.edge(c, r), true);
                }
            }
        }
    };

    R5_STATIC_ASSERT((~ct).edge(8, 5)  == true);
    R5_STATIC_ASSERT((~ct).edge(14, 4) == false);

    R5_STATIC_ASSERT((~cn).edge(8, 5)  == true);
    R5_STATIC_ASSERT((~cn).edge(14, 4) == false);

    testAllEdges(~nct);
    testAllEdges(~ncn);
    testAllEdges(~rt);
    testAllEdges(~rn);
}

TEST(AdjacencyMatrix, compareEqual) {
    constexpr auto ct1 = []() -> auto {
        AdjacencyMatrix<7> m;
        m.unsetAllEdges();
        m.setEdge(4, 6);
        return m;
    }();
    constexpr auto ct2 = []() -> auto {
        AdjacencyMatrix<7> m;
        m.unsetAllEdges();
        return m;
    }();

    constexpr auto cn1 = []() -> auto {
        AdjacencyMatrix<7, false> m;
        m.unsetAllEdges();
        m.setEdge(4, 6);
        return m;
    }();

    constexpr auto cn2 = []() -> auto {
        AdjacencyMatrix<7, false> m;
        m.unsetAllEdges();
        return m;
    }();

    AdjacencyMatrix<7>         nct1;   nct1.unsetAllEdges(); nct1.setEdge(4, 6);
    AdjacencyMatrix<7>         nct2;   nct2.unsetAllEdges();

    AdjacencyMatrix<7, false>  ncn1;   ncn1.unsetAllEdges(); ncn1.setEdge(4, 6);
    AdjacencyMatrix<7, false>  ncn2;   ncn2.unsetAllEdges();

    AdjacencyMatrix<-1>        rt1(7); rt1.unsetAllEdges();  rt1.setEdge(4, 6);
    AdjacencyMatrix<-1>        rt2(7); rt2.unsetAllEdges();

    AdjacencyMatrix<-1, false> rn1(7); rn1.unsetAllEdges();  rn1.setEdge(4, 6);
    AdjacencyMatrix<-1, false> rn2(7); rn2.unsetAllEdges();

    // ct
    R5_STATIC_ASSERT(ct1 == ct1);
    ASSERT_EQ(ct1, nct1);
    ASSERT_EQ(ct1, rt1);

    R5_STATIC_ASSERT(ct1 != ct2);
    ASSERT_NE(ct1, nct2);
    ASSERT_NE(ct1, rt2);

    // cn
    R5_STATIC_ASSERT(cn1 == cn1);
    ASSERT_EQ(cn1, ncn1);
    ASSERT_EQ(cn1, rn1);

    R5_STATIC_ASSERT(cn1 != cn2);
    ASSERT_NE(cn1, ncn2);
    ASSERT_NE(cn1, rn2);

    // nct
    ASSERT_EQ(nct1, ct1);
    ASSERT_EQ(nct1, nct1);
    ASSERT_EQ(nct1, rt1);

    ASSERT_NE(nct1, ct2);
    ASSERT_NE(nct1, nct2);
    ASSERT_NE(nct1, rt2);

    // ncn
    ASSERT_EQ(ncn1, cn1);
    ASSERT_EQ(ncn1, ncn1);
    ASSERT_EQ(ncn1, rn1);

    ASSERT_NE(ncn1, cn2);
    ASSERT_NE(ncn1, ncn2);
    ASSERT_NE(ncn1, rn2);


    // rt
    ASSERT_EQ(rt1, ct1);
    ASSERT_EQ(rt1, nct1);
    ASSERT_EQ(rt1, rt1);

    ASSERT_NE(rt1, ct2);
    ASSERT_NE(rt1, nct2);
    ASSERT_NE(rt1, rt2);


    // rn
    ASSERT_EQ(rn1, cn1);
    ASSERT_EQ(rn1, ncn1);
    ASSERT_EQ(rn1, rn1);

    ASSERT_NE(rn1, cn2);
    ASSERT_NE(rn1, ncn2);
    ASSERT_NE(rn1, rn2);

    // *** make sure matrices of wrong node counts can't interact ***
    // a copy of bitwiseAnd's test

    constexpr AdjacencyMatrix<1>        wct1;
    constexpr AdjacencyMatrix<2>        wct2;
    constexpr AdjacencyMatrix<1, false> wcn1;
    constexpr AdjacencyMatrix<2, false> wcn2;
    AdjacencyMatrix<1>                  wnct1;
    AdjacencyMatrix<2>                  wnct2;
    AdjacencyMatrix<1, false>           wncn1;
    AdjacencyMatrix<2, false>           wncn2;
    AdjacencyMatrix<-1>                 wrt1(1);
    AdjacencyMatrix<-1>                 wrt2(2);
    AdjacencyMatrix<-1, false>          wrn1(1);
    AdjacencyMatrix<-1, false>          wrn2(2);

    // auto wct_and_wct  = wct1 == wct2;  // Compilation error
    // auto wct_and_wnct = wct1 == wnct2; // Compilation error
    ASSERT_THROW2(wct1 == wrt2);

    // auto wcn_and_wcn  = wcn1 == wcn2;  // Compilation error
    // auto wcn_and_wncn = wcn1 == wncn2; // Compilation error
    ASSERT_THROW2(wcn1 == wrn2);

    // auto wnct_and_wct  = wnct1 == wct2;  // Compilation error
    // auto wnct_and_wnct = wnct1 == wnct2; // Compilation error
    ASSERT_THROW2(wnct1 == wrt2);

    // auto wncn_and_wcn  = wncn1 == wcn2;  // Compilation error
    // auto wncn_and_wncn = wncn1 == wncn2; // Compilation error
    ASSERT_THROW2(wncn1 == wrn2);

    ASSERT_THROW2(wrt1 == wct2);
    ASSERT_THROW2(wrt1 == wnct2);
    ASSERT_THROW2(wrt1 == wrt2);

    ASSERT_THROW2(wrn1 == wcn2);
    ASSERT_THROW2(wrn1 == wncn2);
    ASSERT_THROW2(wrn1 == wrn2);
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
