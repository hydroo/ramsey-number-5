#include "prereqs.hpp"

#include "adjacencymatrix.hpp"

using std::cerr;
using std::endl;

void create_constexpr_triangular() {
    constexpr AdjacencyMatrix<  0> m0;
        STATIC_ASSERT(m0.compile_time() == true);
        STATIC_ASSERT(m0.nodes()        == 0);
        STATIC_ASSERT(m0.edges()        == 0);
        STATIC_ASSERT(m0.bits()         == 0);
        //STATIC_ASSERT(m0.elements()     == 0); // is 1
    constexpr AdjacencyMatrix<  1> m1;
        STATIC_ASSERT(m1.nodes()        == 1);
        STATIC_ASSERT(m1.edges()        == 0);
        STATIC_ASSERT(m1.bits()         == 0);
        // STATIC_ASSERT(m1.elements()     == 0); // is 1
    constexpr AdjacencyMatrix<  2> m2;
        STATIC_ASSERT(m2.nodes()        == 2);
        STATIC_ASSERT(m2.edges()        == 1);
        STATIC_ASSERT(m2.bits()         == 1);
        STATIC_ASSERT(m2.elements()     == 1);
    constexpr AdjacencyMatrix<  3> m3;
        STATIC_ASSERT(m3.nodes()        == 3);
        STATIC_ASSERT(m3.edges()        == 3);
        STATIC_ASSERT(m3.bits()         == 3);
        STATIC_ASSERT(m3.elements()     == 1);
    constexpr AdjacencyMatrix<  4> m4;
        STATIC_ASSERT(m4.nodes()        == 4);
        STATIC_ASSERT(m4.edges()        == 6);
        STATIC_ASSERT(m4.bits()         == 6);
        STATIC_ASSERT(m4.elements()     == 1);
    constexpr AdjacencyMatrix< 11> m11;
        STATIC_ASSERT(m11.nodes()        == 11);
        STATIC_ASSERT(m11.edges()        == 55);
        STATIC_ASSERT(m11.bits()         == 55);
        STATIC_ASSERT(m11.elements()     == 1);
    constexpr AdjacencyMatrix< 12> m12;
        STATIC_ASSERT(m12.elements()     == 2);
    constexpr AdjacencyMatrix< 16> m16;
        STATIC_ASSERT(m16.elements()     == 2);
    constexpr AdjacencyMatrix< 17> m17;
        STATIC_ASSERT(m17.elements()     == 3);

}

void create_constexpr_nontriangular() {
    constexpr AdjacencyMatrix<  0, false> m0;
        STATIC_ASSERT(m0.compile_time() == true);
    constexpr AdjacencyMatrix<  1, false> m1;
        STATIC_ASSERT(m1.bits()         == 1);
    constexpr AdjacencyMatrix<  2, false> m2;
        STATIC_ASSERT(m2.bits()         == 4);
    constexpr AdjacencyMatrix<  3, false> m3;
        STATIC_ASSERT(m3.bits()         == 9);
}

void create_nonconstexpr_triangular() {
    AdjacencyMatrix<-1> m4(4);
        ASSERT(m4.compile_time() == false);
        ASSERT(m4.nodes()        == 4);
        ASSERT(m4.edges()        == 6);
        ASSERT(m4.bits()         == 6);
}

void create_nonconstexpr_nontriangular() {
    AdjacencyMatrix< -1, false> m4(4);
        ASSERT(m4.compile_time() == false);
        ASSERT(m4.nodes()        == 4);
        ASSERT(m4.edges()        == 6);
        ASSERT(m4.bits()         == 16);
}

int main() {
    create_constexpr_triangular();
    create_constexpr_nontriangular();
    create_constexpr_nontriangular();
    create_nonconstexpr_triangular();
    return 0;
}
