#include "prereqs.hpp"

#include "adjacencymatrixindexer.hpp"

using std::cerr;
using std::endl;
using std::make_pair;

using r5::AdjacencyMatrixIndexer;
using r5::Size;

TEST(AdjacencyMatrixIndexer, index_constexpr_triangular) {
    using I3 = AdjacencyMatrixIndexer<3>;
    R5_STATIC_ASSERT(I3::indexChecked(0, 0) == -1);
    R5_STATIC_ASSERT(I3::indexChecked(0, 1) ==  0);
    R5_STATIC_ASSERT(I3::indexChecked(0, 2) ==  1);
    R5_STATIC_ASSERT(I3::indexChecked(1, 0) ==  0);
    R5_STATIC_ASSERT(I3::indexChecked(1, 1) == -1);
    R5_STATIC_ASSERT(I3::indexChecked(1, 2) ==  2);
    R5_STATIC_ASSERT(I3::indexChecked(2, 0) ==  1);
    R5_STATIC_ASSERT(I3::indexChecked(2, 1) ==  2);
    R5_STATIC_ASSERT(I3::indexChecked(2, 2) == -1);

    using I4 = AdjacencyMatrixIndexer<4>;
    R5_STATIC_ASSERT(I4::indexChecked(0, 0) == -1);
    R5_STATIC_ASSERT(I4::indexChecked(0, 1) ==  0);
    R5_STATIC_ASSERT(I4::indexChecked(0, 2) ==  1);
    R5_STATIC_ASSERT(I4::indexChecked(0, 3) ==  3);
    R5_STATIC_ASSERT(I4::indexChecked(1, 0) ==  0);
    R5_STATIC_ASSERT(I4::indexChecked(1, 1) == -1);
    R5_STATIC_ASSERT(I4::indexChecked(1, 2) ==  2);
    R5_STATIC_ASSERT(I4::indexChecked(1, 3) ==  4);
    R5_STATIC_ASSERT(I4::indexChecked(2, 0) ==  1);
    R5_STATIC_ASSERT(I4::indexChecked(2, 1) ==  2);
    R5_STATIC_ASSERT(I4::indexChecked(2, 2) == -1);
    R5_STATIC_ASSERT(I4::indexChecked(2, 3) ==  5);
    R5_STATIC_ASSERT(I4::indexChecked(3, 0) ==  3);
    R5_STATIC_ASSERT(I4::indexChecked(3, 1) ==  4);
    R5_STATIC_ASSERT(I4::indexChecked(3, 2) ==  5);
    R5_STATIC_ASSERT(I4::indexChecked(3, 3) == -1);
}

TEST(AdjacencyMatrixIndexer, index_constexpr_nontriangular) {
    using I3 = AdjacencyMatrixIndexer<3, false>;
    R5_STATIC_ASSERT(I3::indexChecked(0, 0) ==  0);
    R5_STATIC_ASSERT(I3::indexChecked(0, 1) ==  1);
    R5_STATIC_ASSERT(I3::indexChecked(0, 2) ==  2);
    R5_STATIC_ASSERT(I3::indexChecked(1, 0) ==  3);
    R5_STATIC_ASSERT(I3::indexChecked(1, 1) ==  4);
    R5_STATIC_ASSERT(I3::indexChecked(1, 2) ==  5);
    R5_STATIC_ASSERT(I3::indexChecked(2, 0) ==  6);
    R5_STATIC_ASSERT(I3::indexChecked(2, 1) ==  7);
    R5_STATIC_ASSERT(I3::indexChecked(2, 2) ==  8);

    using I4 = AdjacencyMatrixIndexer<4, false>;
    R5_STATIC_ASSERT(I4::indexChecked(0, 0) ==  0);
    R5_STATIC_ASSERT(I4::indexChecked(0, 1) ==  1);
    R5_STATIC_ASSERT(I4::indexChecked(0, 2) ==  2);
    R5_STATIC_ASSERT(I4::indexChecked(0, 3) ==  3);
    R5_STATIC_ASSERT(I4::indexChecked(1, 0) ==  4);
    R5_STATIC_ASSERT(I4::indexChecked(1, 1) ==  5);
    R5_STATIC_ASSERT(I4::indexChecked(1, 2) ==  6);
    R5_STATIC_ASSERT(I4::indexChecked(1, 3) ==  7);
    R5_STATIC_ASSERT(I4::indexChecked(2, 0) ==  8);
    R5_STATIC_ASSERT(I4::indexChecked(2, 1) ==  9);
    R5_STATIC_ASSERT(I4::indexChecked(2, 2) == 10);
    R5_STATIC_ASSERT(I4::indexChecked(2, 3) == 11);
    R5_STATIC_ASSERT(I4::indexChecked(3, 0) == 12);
    R5_STATIC_ASSERT(I4::indexChecked(3, 1) == 13);
    R5_STATIC_ASSERT(I4::indexChecked(3, 2) == 14);
    R5_STATIC_ASSERT(I4::indexChecked(3, 3) == 15);
}

TEST(AdjacencyMatrixIndexer, index_nonconstexpr_triangular) {
    using I = AdjacencyMatrixIndexer<-1>;
    ASSERT_EQ(I::indexChecked(0, 0, 3), -1);
    ASSERT_EQ(I::indexChecked(0, 1, 3),  0);
    ASSERT_EQ(I::indexChecked(0, 2, 3),  1);
    ASSERT_EQ(I::indexChecked(1, 0, 3),  0);
    ASSERT_EQ(I::indexChecked(1, 1, 3), -1);
    ASSERT_EQ(I::indexChecked(1, 2, 3),  2);
    ASSERT_EQ(I::indexChecked(2, 0, 3),  1);
    ASSERT_EQ(I::indexChecked(2, 1, 3),  2);
    ASSERT_EQ(I::indexChecked(2, 2, 3), -1);

    ASSERT_EQ(I::indexChecked(0, 0, 4), -1);
    ASSERT_EQ(I::indexChecked(0, 1, 4),  0);
    ASSERT_EQ(I::indexChecked(0, 2, 4),  1);
    ASSERT_EQ(I::indexChecked(0, 3, 4),  3);
    ASSERT_EQ(I::indexChecked(1, 0, 4),  0);
    ASSERT_EQ(I::indexChecked(1, 1, 4), -1);
    ASSERT_EQ(I::indexChecked(1, 2, 4),  2);
    ASSERT_EQ(I::indexChecked(1, 3, 4),  4);
    ASSERT_EQ(I::indexChecked(2, 0, 4),  1);
    ASSERT_EQ(I::indexChecked(2, 1, 4),  2);
    ASSERT_EQ(I::indexChecked(2, 2, 4), -1);
    ASSERT_EQ(I::indexChecked(2, 3, 4),  5);
    ASSERT_EQ(I::indexChecked(3, 0, 4),  3);
    ASSERT_EQ(I::indexChecked(3, 1, 4),  4);
    ASSERT_EQ(I::indexChecked(3, 2, 4),  5);
    ASSERT_EQ(I::indexChecked(3, 3, 4), -1);
}

TEST(AdjacencyMatrixIndexer, index_nonconstexpr_nontriangular) {
    using I = AdjacencyMatrixIndexer<-1, false>;
    ASSERT_EQ(I::indexChecked(0, 0, 3),  0);
    ASSERT_EQ(I::indexChecked(0, 1, 3),  1);
    ASSERT_EQ(I::indexChecked(0, 2, 3),  2);
    ASSERT_EQ(I::indexChecked(1, 0, 3),  3);
    ASSERT_EQ(I::indexChecked(1, 1, 3),  4);
    ASSERT_EQ(I::indexChecked(1, 2, 3),  5);
    ASSERT_EQ(I::indexChecked(2, 0, 3),  6);
    ASSERT_EQ(I::indexChecked(2, 1, 3),  7);
    ASSERT_EQ(I::indexChecked(2, 2, 3),  8);

    ASSERT_EQ(I::indexChecked(0, 0, 4),  0);
    ASSERT_EQ(I::indexChecked(0, 1, 4),  1);
    ASSERT_EQ(I::indexChecked(0, 2, 4),  2);
    ASSERT_EQ(I::indexChecked(0, 3, 4),  3);
    ASSERT_EQ(I::indexChecked(1, 0, 4),  4);
    ASSERT_EQ(I::indexChecked(1, 1, 4),  5);
    ASSERT_EQ(I::indexChecked(1, 2, 4),  6);
    ASSERT_EQ(I::indexChecked(1, 3, 4),  7);
    ASSERT_EQ(I::indexChecked(2, 0, 4),  8);
    ASSERT_EQ(I::indexChecked(2, 1, 4),  9);
    ASSERT_EQ(I::indexChecked(2, 2, 4), 10);
    ASSERT_EQ(I::indexChecked(2, 3, 4), 11);
    ASSERT_EQ(I::indexChecked(3, 0, 4), 12);
    ASSERT_EQ(I::indexChecked(3, 1, 4), 13);
    ASSERT_EQ(I::indexChecked(3, 2, 4), 14);
    ASSERT_EQ(I::indexChecked(3, 3, 4), 15);
}

//TEST(AdjacencyMatrixIndexer, reverse_constexpr_triangular) {
//    using I3 = AdjacencyMatrixIndexer<3>;
//    R5_STATIC_ASSERT((I3::reverseChecked(0) == make_pair<Size, Size>(1, 0)));
//    R5_STATIC_ASSERT((I3::reverseChecked(1) == make_pair<Size, Size>(2, 0)));
//    R5_STATIC_ASSERT((I3::reverseChecked(2) == make_pair<Size, Size>(2, 1)));
//
//    using I4 = AdjacencyMatrixIndexer<4>;
//    R5_STATIC_ASSERT((I4::reverseChecked(0) == make_pair<Size, Size>(1, 0)));
//    R5_STATIC_ASSERT((I4::reverseChecked(1) == make_pair<Size, Size>(2, 0)));
//    R5_STATIC_ASSERT((I4::reverseChecked(2) == make_pair<Size, Size>(2, 1)));
//    R5_STATIC_ASSERT((I4::reverseChecked(3) == make_pair<Size, Size>(3, 0)));
//    R5_STATIC_ASSERT((I4::reverseChecked(4) == make_pair<Size, Size>(3, 1)));
//    R5_STATIC_ASSERT((I4::reverseChecked(5) == make_pair<Size, Size>(3, 2)));
//}

TEST(AdjacencyMatrixIndexer, reverse_constexpr_nontriangular) {
    using I3 = AdjacencyMatrixIndexer<3, false>;
    R5_STATIC_ASSERT((I3::reverseChecked( 0) == make_pair<Size, Size>(0, 0)));
    R5_STATIC_ASSERT((I3::reverseChecked( 1) == make_pair<Size, Size>(0, 1)));
    R5_STATIC_ASSERT((I3::reverseChecked( 2) == make_pair<Size, Size>(0, 2)));
    R5_STATIC_ASSERT((I3::reverseChecked( 3) == make_pair<Size, Size>(1, 0)));
    R5_STATIC_ASSERT((I3::reverseChecked( 4) == make_pair<Size, Size>(1, 1)));
    R5_STATIC_ASSERT((I3::reverseChecked( 5) == make_pair<Size, Size>(1, 2)));
    R5_STATIC_ASSERT((I3::reverseChecked( 6) == make_pair<Size, Size>(2, 0)));
    R5_STATIC_ASSERT((I3::reverseChecked( 7) == make_pair<Size, Size>(2, 1)));
    R5_STATIC_ASSERT((I3::reverseChecked( 8) == make_pair<Size, Size>(2, 2)));

    using I4 = AdjacencyMatrixIndexer<4, false>;
    R5_STATIC_ASSERT((I4::reverseChecked( 0) == make_pair<Size, Size>(0, 0)));
    R5_STATIC_ASSERT((I4::reverseChecked( 1) == make_pair<Size, Size>(0, 1)));
    R5_STATIC_ASSERT((I4::reverseChecked( 2) == make_pair<Size, Size>(0, 2)));
    R5_STATIC_ASSERT((I4::reverseChecked( 3) == make_pair<Size, Size>(0, 3)));
    R5_STATIC_ASSERT((I4::reverseChecked( 4) == make_pair<Size, Size>(1, 0)));
    R5_STATIC_ASSERT((I4::reverseChecked( 5) == make_pair<Size, Size>(1, 1)));
    R5_STATIC_ASSERT((I4::reverseChecked( 6) == make_pair<Size, Size>(1, 2)));
    R5_STATIC_ASSERT((I4::reverseChecked( 7) == make_pair<Size, Size>(1, 3)));
    R5_STATIC_ASSERT((I4::reverseChecked( 8) == make_pair<Size, Size>(2, 0)));
    R5_STATIC_ASSERT((I4::reverseChecked( 9) == make_pair<Size, Size>(2, 1)));
    R5_STATIC_ASSERT((I4::reverseChecked(10) == make_pair<Size, Size>(2, 2)));
    R5_STATIC_ASSERT((I4::reverseChecked(11) == make_pair<Size, Size>(2, 3)));
    R5_STATIC_ASSERT((I4::reverseChecked(12) == make_pair<Size, Size>(3, 0)));
    R5_STATIC_ASSERT((I4::reverseChecked(13) == make_pair<Size, Size>(3, 1)));
    R5_STATIC_ASSERT((I4::reverseChecked(14) == make_pair<Size, Size>(3, 2)));
    R5_STATIC_ASSERT((I4::reverseChecked(15) == make_pair<Size, Size>(3, 3)));
}

TEST(AdjacencyMatrixIndexer, reverse_nonconstexpr_triangular) {
    using I = AdjacencyMatrixIndexer<-1>;
    ASSERT_EQ(I::reverseChecked(0, 3), (make_pair<Size, Size>(1, 0)));
    ASSERT_EQ(I::reverseChecked(1, 3), (make_pair<Size, Size>(2, 0)));
    ASSERT_EQ(I::reverseChecked(2, 3), (make_pair<Size, Size>(2, 1)));

    ASSERT_EQ(I::reverseChecked(0, 4), (make_pair<Size, Size>(1, 0)));
    ASSERT_EQ(I::reverseChecked(1, 4), (make_pair<Size, Size>(2, 0)));
    ASSERT_EQ(I::reverseChecked(2, 4), (make_pair<Size, Size>(2, 1)));
    ASSERT_EQ(I::reverseChecked(3, 4), (make_pair<Size, Size>(3, 0)));
    ASSERT_EQ(I::reverseChecked(4, 4), (make_pair<Size, Size>(3, 1)));
    ASSERT_EQ(I::reverseChecked(5, 4), (make_pair<Size, Size>(3, 2)));
}

TEST(AdjacencyMatrixIndexer, reverse_nonconstexpr_nontriangular) {
    using I = AdjacencyMatrixIndexer<-1, false>;
    ASSERT_EQ(I::reverseChecked( 0, 3), (make_pair<Size, Size>(0, 0)));
    ASSERT_EQ(I::reverseChecked( 1, 3), (make_pair<Size, Size>(0, 1)));
    ASSERT_EQ(I::reverseChecked( 2, 3), (make_pair<Size, Size>(0, 2)));
    ASSERT_EQ(I::reverseChecked( 3, 3), (make_pair<Size, Size>(1, 0)));
    ASSERT_EQ(I::reverseChecked( 4, 3), (make_pair<Size, Size>(1, 1)));
    ASSERT_EQ(I::reverseChecked( 5, 3), (make_pair<Size, Size>(1, 2)));
    ASSERT_EQ(I::reverseChecked( 6, 3), (make_pair<Size, Size>(2, 0)));
    ASSERT_EQ(I::reverseChecked( 7, 3), (make_pair<Size, Size>(2, 1)));
    ASSERT_EQ(I::reverseChecked( 8, 3), (make_pair<Size, Size>(2, 2)));

    ASSERT_EQ(I::reverseChecked( 0, 4), (make_pair<Size, Size>(0, 0)));
    ASSERT_EQ(I::reverseChecked( 1, 4), (make_pair<Size, Size>(0, 1)));
    ASSERT_EQ(I::reverseChecked( 2, 4), (make_pair<Size, Size>(0, 2)));
    ASSERT_EQ(I::reverseChecked( 3, 4), (make_pair<Size, Size>(0, 3)));
    ASSERT_EQ(I::reverseChecked( 4, 4), (make_pair<Size, Size>(1, 0)));
    ASSERT_EQ(I::reverseChecked( 5, 4), (make_pair<Size, Size>(1, 1)));
    ASSERT_EQ(I::reverseChecked( 6, 4), (make_pair<Size, Size>(1, 2)));
    ASSERT_EQ(I::reverseChecked( 7, 4), (make_pair<Size, Size>(1, 3)));
    ASSERT_EQ(I::reverseChecked( 8, 4), (make_pair<Size, Size>(2, 0)));
    ASSERT_EQ(I::reverseChecked( 9, 4), (make_pair<Size, Size>(2, 1)));
    ASSERT_EQ(I::reverseChecked(10, 4), (make_pair<Size, Size>(2, 2)));
    ASSERT_EQ(I::reverseChecked(11, 4), (make_pair<Size, Size>(2, 3)));
    ASSERT_EQ(I::reverseChecked(12, 4), (make_pair<Size, Size>(3, 0)));
    ASSERT_EQ(I::reverseChecked(13, 4), (make_pair<Size, Size>(3, 1)));
    ASSERT_EQ(I::reverseChecked(14, 4), (make_pair<Size, Size>(3, 2)));
    ASSERT_EQ(I::reverseChecked(15, 4), (make_pair<Size, Size>(3, 3)));
}

int main(int argc, char** args) {
    ::testing::InitGoogleTest(&argc, args);
    return RUN_ALL_TESTS();
}
